#include "Editor.h"
#include "proxies/MusicProxy.h"

#include "CurveEditor.h"
#include "CurveEditorView.h"

// impl
#include "AnimationEditorWidget.h"
#include "CurveEditorScene.h"
#include "CurvePointEditor.h"
#include "CurvePointItem.h"

using namespace Idogep;
using namespace Grafkit;


Roles::ManageCurveAudiogramRole::ManageCurveAudiogramRole()
    : m_audiogramBuffer(nullptr)
    , m_audiogramSampleCount(0)
    , m_audiogramChannelCount(0) {
}

Roles::ManageCurveAudiogramRole::~ManageCurveAudiogramRole() { delete m_audiogramBuffer; }


void Roles::ManageCurveAudiogramRole::GetAudiogram(
    QImage** image, float startTime, float endTime, int rectWidth,
    int rectHeight)
{
    *image = nullptr;

    // todo: update when audiogramChanged 
    if (!m_audiogramBuffer)
    {
        onRequestWaveform(m_audiogramBuffer, m_audiogramSampleCount, m_audiogramChannelCount, m_audiogramSamplePerSec);
    }

    if (!m_audiogramBuffer || !m_audiogramSampleCount)
        return;

    const uint32_t offset = uint32_t(startTime * m_audiogramSamplePerSec) * m_audiogramChannelCount;
    const uint32_t length = uint32_t((endTime - startTime) * m_audiogramSamplePerSec) * m_audiogramChannelCount;

    float* buffer = &m_audiogramBuffer[offset];

    QImage* img = new QImage(QSize(rectWidth, rectHeight), QImage::Format_ARGB32);
    img->fill(0xFF303030);

    const uint32_t readFrames = m_audiogramSampleCount;
    const uint32_t valueHop = (readFrames / rectWidth) + ((readFrames % rectWidth) % 2);

    const float invChannelCount = 1. / m_audiogramChannelCount;

    uint32_t x = 0;
    for (uint32_t i = 0; i < readFrames * 2; i += valueHop * 2)
    {
        if (x >= rectWidth)
            break;

        float vv = 0.0f;
        for (uint32_t k = 0; k < valueHop * 2; k += 2)
        {
            float sum = 0.0f;
            if ((i + (k / 2) + 1) >= length) { sum = 0.; }
            else
            {
                float* p = &buffer[i + (k / 2)];
                for (uint32_t l = 0; l < m_audiogramChannelCount; l++) { sum += fabs(p[l]); }
            }
        }

        vv /= float(valueHop) / invChannelCount;

        for (uint32_t y = 0; y < rectHeight; y++)
        {
            const float cv = fabs((float(y) * 2.0f) / float(rectHeight - 1) - 1.0f);
            if (vv >= cv)
                img->setPixel(x, y, 0xFF3c3c3c);
        }

        x++;
    }

    *image = img;
}

void Roles::ManageCurveAudiogramRole::ClearAudiogram()
{
    delete m_audiogramBuffer;
    m_audiogramBuffer = nullptr;
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
    m_manageAudiogram->onRequestWaveform += Delegate(music, &Audiogram::GetWaveform);

    // 
    MusicProxy* musicProxy = editor->GetMusicProxy();
    assert(musicProxy);
    musicProxy->onMusicChanged += Delegate(m_manageAudiogram, &Roles::ManageCurveAudiogramRole::ClearAudiogram);
    musicProxy->onMusicChanged += Delegate(ces, &Roles::TimelineSceneViewRole::MusicChanged);

    ces->onRequestAudiogram += Delegate(m_manageAudiogram, &Roles::ManageCurveAudiogramRole::GetAudiogram);

    m_manageAudiogram->onRequestWaveform += Delegate(musicProxy, &Audiogram::GetWaveform);

    m_myView = ces;
    SetView(m_myView);
}

void CurveEditor::ChannelSelectedEvent(Animation::ChannelRef channel) const
{
    // show if was previously hidden
    if (channel == m_pointEditor->GetChannel())
    {
        m_myView->ShowAnimationCurves();
        m_pointEditor->ShowPoints();
        m_myView->RequestRefreshView(false);
        return;
    }

    // hide all if none selected
    m_myView->HideAnimationCurves();
    if (!channel) {
        m_pointEditor->HidePoints();
        m_myView->RequestRefreshView(false);
        return;
    }

    // throw and build up biew if different curve was selected
    m_myView->ClearCurvePoints();
    m_pointEditor->SetChannel(channel);

    AddCurveToScene();

    m_myView->ShowAnimationCurves();
    m_myView->RequestRefreshView(true);
}

void CurveEditor::Recalculate(TimelineArea* const area) const {
    m_pointEditor->Recalculate(area);
}

Animation::ChannelRef CurveEditor::GetChannel() const {
    return m_pointEditor->GetChannel();
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
