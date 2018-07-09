#pragma once
#include "animation/animation.h"
#include "utils/ViewModule.h"

class QImage;

namespace Idogep
{
    class CurvePointItem;
    class TimelineArea;

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

        virtual void Invalidate();

    protected:

        bool m_isDisplayWaveform;
        bool m_isDisplayCurve;

        Grafkit::Animation::ChannelRef m_channel;
    };
}
