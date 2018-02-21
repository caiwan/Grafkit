#pragma once

#include "../stdafx.h"

#include "timer.h"

#include "../utils/asset.h"
#include "../utils/resource.h"

/*
Music player interface
*/

namespace Grafkit {

	class Audiogram  : virtual public Referencable {
	public:
		Audiogram(){}
		virtual ~Audiogram(){}

		virtual bool GetFFT(float* ptr, int segcount) { return false; }
		virtual bool GetWaveform(float* ptr, double begin, double end) { return false; }
	};

	class Music : virtual public Referencable, public Timer, public Audiogram
	{
	public:
		Music() : Timer(), Audiogram()
		{
		}

		virtual ~Music() {
		}

		virtual void Initialize(IAssetRef asset) = 0;
		virtual void Shutdown() = 0;

		// Timer
		//virtual void Play() = 0;
		//virtual void Stop() = 0;
		//virtual void Pause(int e) = 0;
		//virtual void Update() = 0;

		//virtual uint64_t GetTimeSample() = 0;
		//virtual void SetTimeSample(uint64_t t) = 0;
		//virtual void SetLoop(int e) = 0;
		//virtual int IsPlaying() = 0;

		// Audiogram
		//virtual bool GetFFT(float* ptr, int segcount) = 0;
		//virtual bool GetWaveform(float* ptr, double startMs, double stopMs) = 0;

		virtual void ToggleMute() {}

	protected:
	};

	typedef Ref<Music> MusicRef;
	typedef Resource<Music> MusicRes;
	typedef Ref<MusicRes> MusicResRef;

}
