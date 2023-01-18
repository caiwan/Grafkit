#pragma once

//#include "CurvePointEditor.h"

#include "animation/animation.h"
#include "utils/ViewModule.h"

namespace Idogep
{
    class TimelineArea;

    class CurvePointEditor;
    class CurveEditorView;

    typedef Ref<Grafkit::Animation::Channel> ChannelRef;
    typedef Grafkit::Animation::Key AnimationKey;

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
			float* m_audiogramBuffer;
			size_t m_audiogramSampleCount;

			size_t m_audiogramChannelCount;
			size_t m_audiogramSamplePerSec;
		};

	}
    // ============================================================

	class CurveEditor : public Controller, public EmitsCommandRole 
	{
	public:
	    explicit CurveEditor(const Ref<Controller>&& parent);

	    ~CurveEditor() override;
	    void Initialize() override;

		void ChannelSelectedEvent(Ref<Grafkit::Animation::Channel> chanel);
		void ClearChannels();

	    void Recalculate(TimelineArea* const area) const;
        ChannelRef GetChannel() const;

	private:
		Ref<CurveEditorView> m_myView;
		
	    Roles::ManageCurveAudiogramRole* m_manageAudiogram;
		CurvePointEditor *m_pointEditor;
	};
}
