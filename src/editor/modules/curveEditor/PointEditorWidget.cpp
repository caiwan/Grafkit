#include "PointEditorWidget.h"
#include "ui_PointEditorWidget.h"

#include <QRegExpValidator>

using namespace Idogep;
using namespace Grafkit;

PointEditorWidget::PointEditorWidget(QWidget *parent) :
    QWidget(parent), 
    PointEditorView(), ui(new Ui::PointEditorWidget)
{
    ui->setupUi(this);

    SetupPointTypes();

    QRegExpValidator* rxv = new QRegExpValidator(QRegExp("[+-]?\\d*[\\.,]?\\d+"), this);

    ui->edit_key->setValidator(rxv);
    ui->edit_value->setValidator(rxv);
    ui->edit_angle->setValidator(rxv);
    ui->edit_radius->setValidator(rxv);
    connect(ui->edit_key, SIGNAL(returnPressed()), this, SLOT(keyReturnPressedSlot()));
    connect(ui->edit_value, SIGNAL(returnPressed()), this, SLOT(valueReturnPressedSlot()));
    connect(ui->edit_radius, SIGNAL(returnPressed()), this, SLOT(radiusReturnPressedSlot()));
    connect(ui->edit_angle, SIGNAL(returnPressed()), this, SLOT(angleReturnPressedSlot()));
    connect(ui->edit_knotType, SIGNAL(currentIndexChanged(int)), this, SLOT(knotTypeChangedSlot(int)));

    PointEditorWidget::UpdatePointEditor(false);

}

PointEditorWidget::~PointEditorWidget()
{
    delete ui;
}

void PointEditorWidget::UpdatePointEditor(bool isSet)
{
    // 
    if (!isSet)
    {
        this->ui->edit_key->setDisabled(true);
        this->ui->edit_value->setDisabled(true);
        this->ui->edit_angle->setDisabled(true);
        this->ui->edit_radius->setDisabled(true);
        this->ui->edit_knotType->setDisabled(true);
        return;
    }
    Animation::Key key = GetPointKey();

    this->ui->edit_key->setText(QString::number(key.m_time));
    this->ui->edit_value->setText(QString::number(key.m_value));
    this->ui->edit_angle->setText(QString::number(key.m_angle));
    this->ui->edit_radius->setText(QString::number(key.m_radius));
    this->ui->edit_knotType->setCurrentIndex(key.m_type);

    // TODO: type

    this->ui->edit_key->setDisabled(false);
    this->ui->edit_value->setDisabled(false);
    this->ui->edit_angle->setDisabled(false);
    this->ui->edit_radius->setDisabled(false);
    this->ui->edit_knotType->setDisabled(false);
}

void PointEditorWidget::RefreshView(bool force)
{
    UpdatePointEditor(ui->edit_key->isEnabled());
}

void PointEditorWidget::keyReturnPressedSlot()
{
    // start edit 
    float key = ui->edit_key->text().toFloat();
    m_pointKey.m_time = key;
    onEditKey(m_pointId, m_pointKey);
    onCommitEdit(m_pointId, m_pointKey);
}

void PointEditorWidget::valueReturnPressedSlot()
{
    // start edit 
    float value = ui->edit_value->text().toFloat();
    m_pointKey.m_value = value;
    onEditKey(m_pointId, m_pointKey);
    onCommitEdit(m_pointId, m_pointKey);
}

void PointEditorWidget::radiusReturnPressedSlot()
{
    // start edit 
    float radius = ui->edit_radius->text().toFloat();
    m_pointKey.m_radius= radius;
    onEditKey(m_pointId, m_pointKey);
    onCommitEdit(m_pointId, m_pointKey);
}

void PointEditorWidget::angleReturnPressedSlot()
{
    // start edit 
    float angle = ui->edit_angle->text().toFloat();
    m_pointKey.m_angle = angle;
    onEditKey(m_pointId, m_pointKey);
    onCommitEdit(m_pointId, m_pointKey);
}

void PointEditorWidget::knotTypeChangedSlot(int idex)
{
    // start edit 
    int knot_type = ui->edit_knotType->currentIndex();
    m_pointKey.m_type = static_cast<Animation::KeyInterpolation_e>(knot_type);
    onEditKey(m_pointId, m_pointKey);
    onCommitEdit(m_pointId, m_pointKey);
}

void PointEditorWidget::SetupPointTypes()
{
    // QApplication::translate("")
    ui->edit_knotType->addItem("KI_step");
    ui->edit_knotType->addItem("KI_linear");
    ui->edit_knotType->addItem("KI_ramp");
    ui->edit_knotType->addItem("KI_smooth");
    ui->edit_knotType->addItem("KI_hermite");
}