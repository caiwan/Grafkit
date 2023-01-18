#pragma once

#include "utils/ViewModule.h"

#include "utils/Command.h"

namespace  Idogep
{
	class CurveEditorView;
	class CurveSceneModule;

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

	// ========================================================================================================

	class CurveEditorModule : public Module, public EmitsCommandRole
	{
	public:
		explicit CurveEditorModule(Ref<Module> parent);
		~CurveEditorModule() override;

		void Initialize() override;

		Event<float*&, size_t&, size_t&, size_t&> onRequestWaveform; // ptr, samplecount, channelcount, samplePerSec

	private:
		Ref<CurveEditorView> m_myView;
		Ref<CurveSceneModule> m_curveScene;
        
	    Roles::ManageCurveAudiogramRole* m_manageAudiogram;
	};

}
