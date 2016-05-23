#pragma once

#include "../stdafx.h"
#include "../utils/resource.h"

/*
Music player interface
*/

namespace Grafkit {

	class Music : virtual public Referencable{
	public:
		//-- virtualis metodusok, amiket implementalni kell --//
		Music();
		virtual ~Music();

		virtual void Play() = 0;	///< play stuff
		virtual void Stop() = 0;	///< stop stuff 
		virtual void Pause(bool e) = 0; ///< set pause flag e == true = paused

		virtual unsigned int GetTimeSample() = 0;	///< idot visszaadja sampleben
		virtual void SetTimeSample(unsigned int t) = 0;	///< kurzort samplera allitja
		virtual void SetLoop(bool e) = 0;	//< loop ki/be
		virtual int IsPlaying() = 0;	///< @return 1: playing 0:stopped/paused

		/* --- */
		double GetTime() { return (double)GetTimeSample() / (double)this->m_samplePerSec; }	///< idot visszaadja sec-ban
		double GetTimems() { return 1000.*GetTime(); }										///< idot visszaadja msec-ban
		double GetTimeBeat() { return (double)GetTimeSample() / (double)this->m_beatLenSample; } ///< idot visszaadja beat-terben

		void SetTime(double t) { SetTimeSample((int)(t*m_samplePerSec)); }		///< kurzort allitja
		void SetTimems(double t) { SetTimeSample((int)(t*m_samplePerSec / 1000.)); } ///< kurzort allitja
		void SetTimeBeat(double t) { SetTimeSample((int)(t*m_beatLenSample)); } ///< kurzort allitja

		float GetBPM() { return m_bpm; }
		void SetBPM(float f) { m_bpm = f; this->m_beatLenSample = (int)((m_bpm / 60.) * (double)m_samplePerSec); }

		double GetLength() { return (double)this->m_length / (double)this->m_samplePerSec; }
		double GetLengthms() { return 1000.*(double)this->m_length / (double)this->m_samplePerSec; }

	protected:
		float m_bpm;			///<beat per minute
		unsigned int m_length;///< hossz sampleben
		int m_samplePerSec;	///< savszelesseg
		int m_beatLenSample;	///< egy beat hossza szampleben
	};

	typedef Ref<Music> MusicRef;
	typedef Resource<MusicRef> MusicRes;
	typedef Ref<MusicRes> MusicResRef;

}
