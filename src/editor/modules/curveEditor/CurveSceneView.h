#pragma once
#include "utils/ViewModule.h"

namespace Idogep
{
	namespace Roles
	{

		class TimelineSceneViewRole
		{
		public:
			Event<float*&, size_t&, size_t&, size_t&> onRequestWaveform; // ptr, samplecount, channelcount, samplePerSec

			virtual void MusicChanged() = 0;

			virtual void PlaybackChanged(bool isPlaying) = 0;
			virtual void DemoTimeChanged(float time) = 0;
		};

		class CurveEditorSceneViewRole
		{

		};
	}
    
	class CurveSceneView : public View, public Roles::TimelineSceneViewRole, public Roles::CurveEditorSceneViewRole
	{

	};
}
