#pragma once 

#include "utils/Event.h"

namespace Idogep {

	namespace Roles {

		class HasWaveformRole {
		public:
			virtual void GetWaveform(float *&ptr, size_t &length, size_t &channelCount, size_t &samplePerSec) = 0;
		};
	}

	class Music : public Grafkit::Timer, public Roles::HasWaveformRole {
	public:
		Event<> onMusicChanged;
	};

}
