#pragma once
#include "utils/ViewModule.h"

class QImage;

namespace Idogep
{
	namespace Roles
	{
		class TimelineSceneViewRole
		{
		public:

			Event<QImage*, float, float, int, int> onRequestAudiogram; //audiogramImgage, leftTime, rightTime, width, height

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
