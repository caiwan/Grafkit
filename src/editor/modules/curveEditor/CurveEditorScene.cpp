#include "CurveEditorScene.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <QDebug>

#include "animation/animation.h"

#include "CurvePointItem.h"
#include "CurvePointEditor.h"
#include "CurveEditor.h"
#include <QKeyEvent>

using namespace Idogep;
using namespace Grafkit;

namespace
{
    const QColor grey = QColor(192, 192, 192);
    const QColor red = QColor(255, 128, 128);
    const QColor blue = QColor(128, 128, 255);
    const QColor green = QColor(128, 255, 128);
    const QColor purple = QColor(255, 128, 255);
}

CurveEditorScene::CurveEditorScene(QObject* parent) : QGraphicsScene(parent)
, CurveEditorView()
, m_area(nullptr)
, m_cursorItem(nullptr)
, m_modifyDemoTime(false)
, m_altPressed(false)
, m_ctrlPressed(false)
, m_shiftPressed(false)
, m_midButton(false)
, m_rightButton(false)
, m_isValidAudiogram(false)
, m_curvePixmap()
, m_audiogramImage(nullptr)
{
    m_area = new TimelineArea();
    m_cursorItem = new CursorItem();
    m_cursorItem->SetArea(m_area);

    setBackgroundBrush(QColor(48, 48, 48));

    connect(this, SIGNAL(selectionChanged()), this, SLOT(SelectionChangedSlot()));
}

CurveEditorScene::~CurveEditorScene()
{
    delete m_area;
    delete m_audiogramImage;
}

void CurveEditorScene::RefreshView(const bool force)
{
    if (force)
    {
        Invalidate();
        UpdateAudiogram();
    }

    m_area->SetSceneRect(sceneRect());

    update();
}

bool CurveEditorScene::event(QEvent* event)
{
    if (event->type() == QEvent::Resize)
        RequestRefreshView(true);

    return QGraphicsScene::event(event);
}

void CurveEditorScene::PlaybackChangedEvent(bool isPlaying) {
}

void CurveEditorScene::DemoTimeChangedEvent(const float& time)
{
    m_cursorItem->SetPosition(time);
    update();
}

void CurveEditorScene::ClearCurvePoints() { clear(); }

void CurveEditorScene::AddCurvePoint(CurvePointItem* pointItem) { addItem(pointItem); }

// ---------------------------------------------------------------------
// DRAW STUFF 

void CurveEditorScene::drawBackground(QPainter* painter, const QRectF& r)
{
    QGraphicsScene::drawBackground(painter, r);
    painter->setBrush(Qt::NoBrush);

    setSceneRect(views().at(0)->geometry());

    if (m_isDisplayWaveform)
    {
        if (!m_audiogramImage || !m_isValidAudiogram) { UpdateAudiogram(); }

        if (m_audiogramImage) { painter->drawImage(r, *m_audiogramImage); }
    }
    m_area->DrawGrid(painter, r);

    if (m_isDisplayCurve)
        DrawCurve(painter, r);

    m_cursorItem->DrawCursor(painter, r);
}

void CurveEditorScene::DrawCurve(QPainter* painter, const QRectF& r)
{
    if (m_channel.Invalid() || m_channel->GetKeyCount() == 0)
        return;

    onRecalculateCurve(m_area);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(grey, 1.5f));

    // boundaries
    const QPointF pMin = m_area->GetMin();
    const int idmin = std::max(m_channel->FindKeyIndex(pMin.x()), 0);

    const QPointF pMax = m_area->GetMax();
    int idmax = m_channel->FindKeyIndex(pMax.x()) + 1;

    // line -inf -> fist point
    auto firstKey = m_channel->GetKey(0);
    QPointF firstPoint = m_area->Point2Screen({ firstKey.m_time, firstKey.m_value });
    if (idmin == 0 && firstPoint.x() > r.x()) { painter->drawLine({ 0, firstPoint.y() }, firstPoint); }

    // line last point -> +inf
    auto lastKey = m_channel->GetKey(m_channel->GetKeyCount() - 1);
    QPointF lastKeyPoint = m_area->Point2Screen({ lastKey.m_time, lastKey.m_value });
    if (lastKeyPoint.x() < r.x() + r.width()) { painter->drawLine(lastKeyPoint, { r.x() + r.width(), lastKeyPoint.y() }); }

    if (idmax >= m_channel->GetKeyCount() - 1)
    {
        idmax = m_channel->GetKeyCount() - 1; // avoid max+1 index
    }

    // qnd fix for offset

    for (int i = idmin; i < idmax; i++)
    {
        const auto k0 = m_channel->GetKey(i);
        const auto k1 = m_channel->GetKey(i + 1);

        // there should be a nice way to optimize this 
        const int stepCount = 32;
        const double stepWidth = (k1.m_time - k0.m_time) / stepCount;

        for (int j = 0; j < stepCount; j++)
        {
            const double t = k0.m_time + j * stepWidth;
            double v = m_channel->GetValue(t);

            QPointF p1(t, m_channel->GetValue(t));
            p1 = this->m_area->Point2Screen(p1);

            QPointF p2(t + stepWidth, m_channel->GetValue(t + stepWidth));
            p2 = this->m_area->Point2Screen(p2);

            painter->drawLine(p1, p2);
        }
    }
}

void CurveEditorScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (items().empty())
        return;

    QPointF pos = event->scenePos();

    const QTransform transformation = QTransform();
    CurvePointItem* pointItem = dynamic_cast<CurvePointItem*>(itemAt(pos, transformation));
    CurvePointItem* curvePointItem = pointItem;
    const CurvePointItem* itemAtCrsr = curvePointItem;

    if (!itemAtCrsr)
    {
        QPointF unitp = m_area->Screen2Point(pos);
        onCommitAddPointEvent(unitp.x(), unitp.y());
        update();
    }
    else
    {
        onCommitRemovePointEvent(itemAtCrsr->GetId());
        update();
    }

    QGraphicsScene::mouseDoubleClickEvent(event);
}

void CurveEditorScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QTransform transformation = QTransform();
    CurvePointItem* itemAtCrsr = dynamic_cast<CurvePointItem*>(itemAt(event->scenePos(), transformation));

    if (!itemAtCrsr)
    {
        switch (event->button())
        {
        case Qt::LeftButton:
        {
            const float time = m_area->Screen2Point(event->scenePos()).x();
            onDemoTimeChanged(time);
            m_cursorItem->SetPosition(time);
            m_modifyDemoTime = true;
            break;
        }
        case Qt::MidButton:
            m_midButton = true;
            break;
        case Qt::RightButton:
            m_rightButton = true;

            break;
        default:
            break;
        }
    }

    bool resetView = m_rightButton && m_ctrlPressed && m_altPressed;

    if (resetView) { m_area->ResetView(); }

    QGraphicsScene::mousePressEvent(event);
}

void CurveEditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    m_modifyDemoTime = false;
    m_midButton = false;
    m_rightButton = false;

    if (m_isValidAudiogram)
        UpdateAudiogram();

    m_isValidAudiogram = false;

    update();
    QGraphicsScene::mouseReleaseEvent(event);
}

void CurveEditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    bool modifyOfs = m_midButton && !m_ctrlPressed;
    bool modifyScale = m_midButton && m_ctrlPressed;

    bool resetView = m_rightButton && m_ctrlPressed && m_altPressed;

    QPointF delta = event->scenePos() - event->lastScenePos();
    if (m_modifyDemoTime)
    {
        const double time = m_area->Screen2Point(event->scenePos()).x();
        onDemoTimeChanged(time);
        m_cursorItem->SetPosition(time);
    }
    if (resetView) { m_area->ResetView(); }
    else if (modifyOfs)
    {
        m_area->Pan(delta);
        m_isValidAudiogram = false;
    }
    else if (modifyScale)
    {
        m_area->Zoom(delta);
        m_isValidAudiogram = false;
    }

    update();

    QGraphicsScene::mouseMoveEvent(event);
}


void CurveEditorScene::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    const float ZOOM_WHEEL_FACTOR = .03125;
    const float delta = static_cast<float>(event->delta()) * ZOOM_WHEEL_FACTOR;

    QPointF p = { delta, delta };
    m_area->Zoom(p);

    m_isValidAudiogram = false;

    update();
}

void CurveEditorScene::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Alt:
        m_altPressed = true;
        break;
    case Qt::Key_Control:
        m_ctrlPressed = true;
        break;
    case Qt::Key_Shift:
        m_shiftPressed = true;
        break;
    default:
        break;
    }

    QGraphicsScene::keyPressEvent(event);
}

void CurveEditorScene::keyReleaseEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Alt:
        m_altPressed = false;
        break;
    case Qt::Key_Control:
        m_ctrlPressed = false;
        break;
    case Qt::Key_Shift:
        m_shiftPressed = false;
        break;
    default:
        break;
    }

    QGraphicsScene::keyReleaseEvent(event);
}

void CurveEditorScene::SelectionChangedSlot()
{
    QList<QGraphicsItem*> items = selectedItems();
    if (!items.empty())
    {
        CurvePointItem* item = dynamic_cast<CurvePointItem*>(items.last());
        if (item)
        {
            onPointSelected(item->GetId());
            item->setFocus(Qt::ActiveWindowFocusReason); // to make key events work
            // or not 
        }
    }
    else
    {
        onPointDeSelected();
        setFocus(Qt::ActiveWindowFocusReason); // to make key events work
        // or not 
    }
}

void CurveEditorScene::UpdateAudiogram()
{
    assert(m_area);

    QPointF min = m_area->GetMin();
    QPointF max = m_area->GetMax();

    const float leftTime = min.x();
    const float rightTime = max.x();

    if (leftTime < 0.0f || rightTime < 0.0f || leftTime >= rightTime)
        return;

    QImage* img = nullptr;

    onRequestAudiogram(&img, leftTime, rightTime, int(sceneRect().width()), int(sceneRect().height()));

    if (!img)
        return;

    delete m_audiogramImage;
    m_audiogramImage = img;

    m_isValidAudiogram = true;
}


// ------------------------------------------------------------------
TimelineArea::TimelineArea() : m_gridImage(0, 0)
, m_gridValid(false)
{
    m_scale = QSizeF(64, 64);
    m_offset = QPointF(0, 0);
}

QPointF TimelineArea::Point2Screen(QPointF point) const { return point * m_transfrom; }

QPointF TimelineArea::Screen2Point(QPointF point) const { return point * m_inverse; }

QPointF TimelineArea::GetMin() const { return Screen2Point({ m_sceneRect.topLeft() }); }

QPointF TimelineArea::GetMax() const { return Screen2Point({ m_sceneRect.bottomRight() }); }

void TimelineArea::DrawGrid(QPainter* const & painter, const QRectF& r)
{
    if (m_gridImage.size() != r.size().toSize())
    {
        if (m_gridImage.isNull())
            m_gridImage = QPixmap(r.width(), r.height());
        else
            m_gridImage.scaled(r.width(), r.height());
        m_gridValid = false;
    }

    if (!m_gridValid)
    {
        m_gridImage.fill(Qt::transparent);
        QPainter picturePainter(&m_gridImage);

        DrawGridToPicture(&picturePainter, r);
        picturePainter.end();
        m_gridValid = true;
    }

    painter->drawPixmap(r.topLeft(), m_gridImage);
}

void TimelineArea::Pan(const QPointF& p)
{
    m_offset += p;
    if (m_offset.x() > 0.)
        m_offset.setX(0.);

    Invalidate();
}

void TimelineArea::Zoom(const QPointF& z)
{
    QSizeF scale = m_scale;
    m_scale += QSizeF(z.x(), z.y());
    if (m_scale.width() < 8.0f)
        m_scale.setWidth(8.0f);

    if (m_scale.width() > 750.)
        m_scale.setWidth(750.);

    if (m_scale.height() < 8.0f)
        m_scale.setHeight(8.0f);

    // Zoom to center
    const QSizeF deltaScale = m_scale - scale;
    const QPointF center = (GetMax() + GetMin()) * .5; // unit
    const QPointF delta = {
        -center.x() * deltaScale.width(),
        -center.y() * deltaScale.height()
    };

    Pan(delta);

    Invalidate();
}

void TimelineArea::ResetView()
{
    m_scale = QSizeF(64, 64);
    m_offset = QPointF(0, 0);
    Invalidate();
}

void TimelineArea::Invalidate()
{
    m_gridValid = false;

    // recalc transformation
    m_transfrom = QTransform();
    m_transfrom.translate(
        m_offset.x() + m_sceneRect.topLeft().x(),
        m_offset.y() + m_sceneRect.topLeft().y() + m_sceneRect.height() * .5
    );
    m_transfrom.scale(m_scale.width(), -m_scale.height());

    m_inverse = m_transfrom.inverted();
}

void TimelineArea::DrawGridToPicture(QPainter* const& painter, const QRectF& r) const
{
    float sc = 0;
    const float offsetX = m_offset.x();
    const float offsetY = m_offset.y();

    const float halfHeight = r.height() / 2;

    const float rectWidth = m_sceneRect.width();
    const float rectHeight = m_sceneRect.height();

    // Vertical thin
    sc = m_scale.width() / 4.0f;
    for (float f = fmod(offsetX, sc); f <= rectWidth + fmod(offsetX, sc); f += sc)
    {
        painter->setPen(QPen(QColor(56, 56, 56)));
        painter->drawLine(f, 0.0f, f, rectHeight);
    }

    // Vertical thick
    sc = m_scale.width();
    for (float f = fmod(offsetX, sc); f <= rectWidth + fmod(offsetX, sc); f += sc)
    {
        painter->setPen(QPen(QColor(64, 64, 64)));
        painter->drawLine(f, 0.0f, f, rectHeight);
    }

    // Horizontal thin
    sc = m_scale.height() / 4.0f;

    for (float f = fmod(offsetY - halfHeight, sc); f <= rectHeight + fmod(offsetY + halfHeight, sc); f += sc)
    {
        painter->setPen(QPen(QColor(56, 56, 56)));
        painter->drawLine(0.0f, f, rectWidth, f);
    }

    sc = m_scale.height();
    for (float f = fmod(offsetY - halfHeight, sc); f <= rectHeight + fmod(offsetY + halfHeight, sc); f += sc)
    {
        painter->setPen(QPen(QColor(64, 64, 64)));
        painter->drawLine(0.0f, f, rectWidth, f);
    }

    painter->setPen(QPen(QColor(144, 144, 144)));
    painter->drawLine(offsetX, 0.0f, offsetX, rectHeight);
    painter->setPen(QPen(QColor(144, 144, 144)));
    painter->drawLine(0.0f, offsetY + halfHeight, rectWidth, offsetY + halfHeight);
}

CursorItem::CursorItem() : m_position(0)
, m_area(nullptr) {
}

void CursorItem::DrawCursor(QPainter* const & painter, const QRectF& r)
{
    assert(m_area);

    // Out of screen 
    const QPointF minP = m_area->GetMin();
    const QPointF maxP = m_area->GetMax();

    if (m_position < minP.x() || m_position > maxP.x())
        return;

    // Time as string
    QString strTime;

    int minutes = int(m_position) / 60;

    if (minutes < 10)
        strTime.append("0");
    strTime.append(QString::number(minutes)).append(":");

    if (fmod(m_position, 60.0f) < 10.0f)
        strTime.append("0");

    strTime.append(QString::number(fmod(m_position, 60.0f), 'f', 3));

    // darw stuff, calc
    float screenPos = m_area->Point2Screen({ m_position, 0 }).x();

    float barOffset = 0.0f;
    // this will stop the label moving at the corners
    //if (screenpos)
    //    barOffset = -56.0f;

    // psh

    // draw crsr handler
    painter->setPen(QPen(QColor(48, 224, 48, 96), 2.0, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawLine(screenPos, r.top(), screenPos, r.bottom());

    // draw label
    painter->setFont(QFont(QString("Sans"), 8));
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(48, 224, 48, 96));
    painter->drawRect(QRectF(QPointF(screenPos, r.bottom() - 16.0f), QSizeF(56.0f, 16.0f)));

    // draw label text
    painter->setPen(QColor(255, 255, 255, 96));
    painter->setBrush(QColor(48, 224, 48, 96));
    painter->drawText(QRectF(QPointF(screenPos + barOffset, r.bottom() - 16.0f), QSizeF(56.0f, 16.0f)), strTime, QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
}
