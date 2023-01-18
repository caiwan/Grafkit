#pragma once
#include "animation/animation.h"
#include "utils/ViewModule.h"

class QImage;

namespace Idogep
{
    class CurvePointItem;

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

        void HideAnimationCurves() { m_displayCurve = false; }
        void ShowAnimationCurves() { m_displayCurve = true; }

        // TODO hide audiogram
        // TODO show audiogram

        virtual void ClearCurvePoints() = 0;
        virtual void AddCurvePoint(CurvePointItem* points) = 0;

        Event<size_t> onPointSelected;
        Event<> onPointDeSelected;


    protected:

        bool m_displayWaveform;
        bool m_displayCurve;

    };
}
