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

            virtual void MusicChanged() = 0;

            virtual void PlaybackChanged(bool isPlaying) = 0;
            virtual void DemoTimeChanged(float time) = 0;
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
