#pragma once

#include "utils/ViewModule.h"
#include "models/Curve.h"

class QImage;

namespace Idogep
{
    class TimelineArea;

    class CurvePointItem;
    class CurvePointEditor;
    class CurveEditorView;

    namespace Roles {

        // Delegate audiogram rendering functions 
        class ManageCurveAudiogramRole {
        public:
            ManageCurveAudiogramRole();
            virtual ~ManageCurveAudiogramRole();

            void GetAudiogram(QImage** image, float startTime, float endTime, int rectWidth, int rectHeight);
            void ClearAudiogram();

            Event<float*&, size_t&, size_t&, size_t&> onRequestWaveform;
            //virtual bool RequestWaveform(float*& p, size_t& sampleCount, size_t& channelCount, size_t& samplePerSec) = 0;

        private:
            float* m_agBuffer;
            size_t m_agSampleCount;

            size_t m_agChannelCount;
            size_t m_agSamplePerSec;
        };

    }
    // ============================================================

    class CurveEditor : public Controller, public EmitsCommandRole
    {
    public:
        explicit CurveEditor();

        ~CurveEditor() override;

        void Initialize(Grafkit::IResourceManager* const& resourceManager) override;

        void ChannelSelectedEvent(Grafkit::Animation::TrackRef& track, const size_t& trackid, Grafkit::Animation::ChannelRef& channel);
        void ChannelDeselectedEvent();

        void UpdateChannel();

    private:
        void AddCurveToScene() const;

        Ref<CurveEditorView> m_myView;

        Roles::ManageCurveAudiogramRole* m_manageAudiogram;
        Ref<CurvePointEditor> m_pointEditor;
    };


    //=================================================================================================================================

    namespace Roles
    {
        class TimelineSceneViewRole
        {
        public:

            Event<QImage**, float, float, int, int> onRequestAudiogram; //audiogramImgage, leftTime, rightTime, width, height

            virtual void MusicChangedEvent() = 0;

            virtual void PlaybackChangedEvent(bool isPlaying) = 0;
            virtual void DemoTimeChangedEvent(const float &time) = 0;

            Event<double> onDemoTimeChanged;
        };
    }

    class CurveEditorView : public View, public Roles::TimelineSceneViewRole
    {
    public:

        CurveEditorView();

        void HideAnimationCurves() { m_isDisplayCurve = false; }
        void ShowAnimationCurves() { m_isDisplayCurve = true; }

        // TODO hide audiogram
        // TODO show audiogram

        virtual void ClearCurvePoints() = 0;
        virtual void AddCurvePoint(CurvePointItem* points) = 0;

        Event<size_t> onPointSelected;
        Event<> onPointDeSelected;

        Event<const float&, const float&> onCommitAddPointEvent;
        Event<const size_t&> onCommitRemovePointEvent;

        Event<TimelineArea* const &> onRecalculateCurve;

        Grafkit::Animation::ChannelRef GetChannel() const { return m_channel; }
        void SetChannel(const Grafkit::Animation::ChannelRef& channel) { m_channel = channel; }

        virtual void Invalidate() = 0;

    protected:

        bool m_isDisplayWaveform;
        bool m_isDisplayCurve;

        Grafkit::Animation::ChannelRef m_channel;
    };
}

