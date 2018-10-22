#pragma once

#include "../stdafx.h"

#include "timer.h"

#include "core/asset.h"
#include "core/resource.h"

/*
Music player interface
*/

namespace Grafkit {

	class Audiogram : virtual public Referencable {
	public:
		Audiogram() {}
		virtual ~Audiogram() {}

		virtual void GetFFT(float* ptr, int segcount) {}
		virtual void GetWaveform(float *&ptr, size_t &length, size_t &channelCount, size_t &samplePerSec) { ptr = nullptr; length = 0; }
	};

	class Music : virtual public Referencable, public Timer, public Audiogram
	{
	public:
		Music() : Timer(), Audiogram(){}

		virtual ~Music() {}

		virtual void Initialize(StreamRef& asset) = 0;
		virtual void Shutdown() = 0;

		virtual void ToggleMute() {}
	};

	typedef Ref<Music> MusicRef;
	typedef Resource<Music> MusicRes;
	typedef Ref<MusicRes> MusicResRef;

}
