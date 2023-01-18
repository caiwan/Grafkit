#include "Editor.h"
#include "proxies/MusicProxy.h"

#include "CurveEditor.h"

// impl
#include "AnimationEditorWidget.h"
#include "CurveEditorScene.h"
#include "CurvePointEditor.h"
#include "CurvePointItem.h"

using namespace Idogep;
using namespace Grafkit;

Roles::ManageCurveAudiogramRole::ManageCurveAudiogramRole()
    : m_agBuffer(nullptr)
    , m_agSampleCount(0)
    , m_agChannelCount(0)
    , m_agSamplePerSec(0) {
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

CurveEditor::CurveEditor() {
    m_manageAudiogram = new Roles::ManageCurveAudiogramRole();
    //m_pointEditor = new CurvePointEditor();
}

CurveEditor::~CurveEditor()
{
    delete m_manageAudiogram;
    //delete m_pointEditor;
}

void CurveEditor::Initialize(IResourceManager* const& resourceManager) {
    m_myView = dynamic_cast<CurveEditorScene*>(View::SafeGetView(resourceManager, "CurveEditorView").Get());
    assert(m_myView.Valid());

    Editor* editor = dynamic_cast<Editor*>(SafeGetController(resourceManager, "Editor").Get());
    assert(editor);

    Audiogram* music = editor->GetMusicProxy();
    assert(music);

    MusicProxy* musicProxy = editor->GetMusicProxy();
    assert(musicProxy);
    musicProxy->onMusicChanged += Delegate(m_manageAudiogram, &Roles::ManageCurveAudiogramRole::ClearAudiogram);
    musicProxy->onMusicChanged += Delegate(m_myView.Get(), &Roles::TimelineSceneViewRole::MusicChangedEvent);

    // -- timeline 
    m_myView->onDemoTimeChanged += Delegate(musicProxy, &MusicProxy::SetTime);
    editor->onDemoTimeChanged += Delegate(m_myView.Get(), &Roles::TimelineSceneViewRole::DemoTimeChangedEvent);
    
    // -- audiogram 
    m_myView->onRequestAudiogram += Delegate(m_manageAudiogram, &Roles::ManageCurveAudiogramRole::GetAudiogram);
    m_manageAudiogram->onRequestWaveform += Delegate(musicProxy, &Audiogram::GetWaveform);

    // -- point editor
    // is it safe to be here?
    m_pointEditor = dynamic_cast<CurvePointEditor*>(SafeGetController(resourceManager, "CurvePointEditor").Get());
    assert(m_pointEditor);

    m_pointEditor->onInvlaidateCurve += Delegate(m_myView.Get(), &CurveEditorView::Invalidate);
    m_pointEditor->onUpdateChannel += Delegate(this, &CurveEditor::UpdateChannel);
    m_myView->onRecalculateCurve += Delegate(m_pointEditor.Get(), &CurvePointEditor::Recalculate);
}

// ReSharper disable CppMemberFunctionMayBeConst
void CurveEditor::ChannelSelectedEvent(Animation::TrackRef &track, const size_t &trackid, Animation::ChannelRef &channel)
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

    // throw and build up biew if different curve was selected
    m_myView->ClearCurvePoints();
    m_pointEditor->SetChannel(track, trackid, channel);
    m_myView->SetChannel(channel);

    AddCurveToScene();

    m_myView->ShowAnimationCurves();
    m_myView->RequestRefreshView(true);
}

void CurveEditor::ChannelDeselectedEvent()
{
    m_myView->HideAnimationCurves();
    m_pointEditor->HidePoints();
    m_myView->RequestRefreshView(false);
}

void CurveEditor::UpdateChannel()
{
    Animation::ChannelRef channel = m_pointEditor->GetChannel();
    m_myView->HideAnimationCurves();
    m_myView->ClearCurvePoints();
    m_myView->SetChannel(channel);

    AddCurveToScene();

    m_myView->ShowAnimationCurves();
    m_myView->RequestRefreshView(true);
}
// ReSharper restore CppMemberFunctionMayBeConst

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

// ==========================================================================================

CurveEditorView::CurveEditorView() : View()
, m_isDisplayWaveform(true)
, m_isDisplayCurve(true)
{
    // ...
}
