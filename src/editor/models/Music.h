#pragma once 

#include "utils/Events"

namespace Idogep {
	
	namespace Roles{
	
	class HasWaveformRole {
		public:
		virtual void GetWaveform(float *&ptr, size_t &length, size_t &channelCount, size_t &samplePerSec) = 0;
	};
	}
	
	class Music : public Grafkit::Timer, public HasWaveformRole{
	public:
		
	
		Event<> onMusicChanged;
	};
	
}
