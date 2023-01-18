#pragma once

#include "utils/ViewModule.h"

#include "models/Curve.h"

namespace Idogep
{
    class TimelineArea;

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
}