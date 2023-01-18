#include "Editor.h"
#include "proxies/MusicProxy.h"

#include "CurveEditor.h"
#include "CurveEditorView.h"

// impl
#include "AnimationEditorWidget.h"
#include "CurveEditorScene.h"
#include "CurvePointEditor.h"
#include "CurvePointItem.h"
#include "PointEditorWidget.h"

using namespace Idogep;
using namespace Grafkit;


Roles::ManageCurveAudiogramRole::ManageCurveAudiogramRole()
    : m_agBuffer(nullptr)
    , m_agSampleCount(0)
    , m_agChannelCount(0) {
}

Roles::ManageCurveAudiogramRole::~ManageCurveAudiogramRole() { delete m_agBuffer; }

void Roles::ManageCurveAudiogramRole::GetAudiogram(
    QImage** image, float startTime, float endTime, int rectWidth,
    int rectHeight)
{
    *image = nullptr;

    if (!m_agBuffer)
    {
        onRequestWaveform(m_agBuffer, m_agSampleCount, m_agChannelCount, m_agSamplePerSec);
    }

    if (!m_agBuffer || !m_agSampleCount)
        return;

    const size_t offset = size_t(startTime * m_agSamplePerSec);
    const size_t end = size_t(endTime * m_agSamplePerSec);
    const size_t readFrames = end - offset;

    QImage* img = new QImage(QSize(rectWidth, rectHeight), QImage::Format_ARGB32);
    img->fill(0xFF303030);

    const size_t valueHop = (readFrames / rectWidth) +((readFrames % rectWidth) % m_agChannelCount);
    const float invFactor = 1. / (m_agChannelCount * valueHop);
    const float* b = &m_agBuffer[offset * m_agChannelCount];
    
    size_t x = 0;
    for (size_t i = 0; i < readFrames; i += valueHop)
    {
        if (x >= rectWidth)
            break;

        float vSum = 0.0f;
        const float* p = &b[i * m_agChannelCount];

        for (size_t k = 0; k < valueHop * m_agChannelCount; k++)
        {
            if (offset + i + (k / m_agChannelCount) >= m_agSampleCount)
                break;
            vSum += fabs(p[k]) * invFactor;
        }

        for (size_t y = 0; y < rectHeight; y++)
        {
            const float cv = fabs((float(y) * 2.0f) / float(rectHeight - 1) - 1.0f);
            if (vSum >= cv)
                img->setPixel(x, y, 0xFF3c3c3c);
        }

        x++;
    }

    *image = img;
}

void Roles::ManageCurveAudiogramRole::ClearAudiogram()
{
    delete m_agBuffer;
    m_agBuffer = nullptr;
}


// ============================================================================================

CurveEditor::CurveEditor(const Ref<Controller>&& parent) : Controller(parent), m_manageAudiogram(nullptr)
{
    m_manageAudiogram = new Roles::ManageCurveAudiogramRole();
    m_pointEditor = new CurvePointEditor(this);
}

CurveEditor::~CurveEditor()
{
    delete m_manageAudiogram;
    delete m_pointEditor;
}

void CurveEditor::Initialize()
{
    assert(m_parent);
    assert(m_parent->GetView());

    AnimationEditorWidget* parentWidget = dynamic_cast<AnimationEditorWidget*>(m_parent->GetView().Get());
    assert(parentWidget);

    CurveEditorScene *ces = new CurveEditorScene();
    parentWidget->SetGraphicsScene(ces);


    // manage playback role 
    Editor* editor = dynamic_cast<Editor*>(GetRootModule().Get());
    assert(editor);

    // ... 
    Audiogram* music = editor->GetMusicProxy();
    assert(music);
    //m_manageAudiogram->onRequestWaveform += Delegate(music, &Audiogram::GetWaveform);

    // 
    MusicProxy* musicProxy = editor->GetMusicProxy();
    assert(musicProxy);
    musicProxy->onMusicChanged += Delegate(m_manageAudiogram, &Roles::ManageCurveAudiogramRole::ClearAudiogram);
    musicProxy->onMusicChanged += Delegate(ces, &Roles::TimelineSceneViewRole::MusicChangedEvent);
    
    ces->onDemoTimeChanged += Delegate(musicProxy, &MusicProxy::SetTime);

    ces->onRequestAudiogram += Delegate(m_manageAudiogram, &Roles::ManageCurveAudiogramRole::GetAudiogram);
    editor->onDemoTimeChanged += Delegate(ces, &Roles::TimelineSceneViewRole::DemoTimeChangedEvent);

    m_manageAudiogram->onRequestWaveform += Delegate(musicProxy, &Audiogram::GetWaveform);

    // 
    PointEditorWidget *pew = new PointEditorWidget();
    m_pointEditor->Initialize(pew);
    parentWidget->SetPointEditorWidget(pew);

    ces->onPointDeSelected += Delegate(m_pointEditor, &CurvePointEditor::PointDeSelectedEvent);
    ces->onPointSelected += Delegate(m_pointEditor, &CurvePointEditor::PointSelectedEvent);

    ces->onCommitAddPointEvent += Delegate(m_pointEditor, &CurvePointEditor::CommitAddPointEvent);
    ces->onCommitRemovePointEvent += Delegate(m_pointEditor, &CurvePointEditor::CommitRemovePointEvent);

    m_myView = ces;
    SetView(m_myView);
}

void CurveEditor::ChannelSelectedEvent(Animation::TrackRef &track, const size_t &trackid, Animation::ChannelRef &channel) const
{
    // show if was previously hidden
    if (channel == m_pointEditor->GetChannel())
    {
        m_myView->ShowAnimationCurves();
        m_pointEditor->ShowPoints();
        m_myView->RequestRefreshView(false);
        return;
    }

    m_myView->HideAnimationCurves();
    /*if (!channel) {
        m_pointEditor->HidePoints();
        m_myView->RequestRefreshView(false);
        return;
    }*/

    // throw and build up biew if different curve was selected
    m_myView->ClearCurvePoints();
    m_pointEditor->SetChannel(track, trackid, channel);

    AddCurveToScene();

    m_myView->ShowAnimationCurves();
    m_myView->RequestRefreshView(true);
}

void CurveEditor::ChannelDeselectedEvent() const
{
    m_myView->HideAnimationCurves();
    m_pointEditor->HidePoints();
    m_myView->RequestRefreshView(false);
}

void CurveEditor::Recalculate(TimelineArea* const area) const {
    m_pointEditor->Recalculate(area);
}

Animation::ChannelRef CurveEditor::GetChannel() const {
    return m_pointEditor->GetChannel();
}

bool CurveEditor::GetAndClearIsRedrawFlag() const {
    return m_pointEditor->GetAndClearIsCurveChangedFlag();
}

void CurveEditor::AddCurveToScene() const
{
    if (!m_pointEditor->HasCurvePoints())
        return;

    for (size_t i = 0; i < m_pointEditor->GetPointCount(); ++i)
    {
        CurvePointItem* point = m_pointEditor->GetPoint(i);
        m_myView->AddCurvePoint(point);
    }
    m_myView->RequestRefreshView(true);
}
