#pragma once

#include "../stdafx.h"

//#include "../GKSDK_config.h"
//#include "Player.h"
//#include "../utilites/io/package.h"

#include "Music.h"

#include "fmodex/fmod.hpp"

namespace Grafkit {

	class MusicFMOD : public Music /*, public AudioVisPRovider*/
	{
		protected:
			FMOD::System *system;
			FMOD::Sound *sound;
			FMOD::Channel *channel;

			Resource*res;

		protected:
			/*
			void init(const void* data, unsigned int len);
			void init(Resource* resource);
			void init(const char* filename);
			*/

		private:
			void init();

		public:
			//MusicFMOD(void);
			MusicFMOD(const char* filename);
			MusicFMOD(const void* data, unsigned int len);	///@todo azt hiszem, hogy also hango itt csak a resource-ra van igazan szukseg
			MusicFMOD(Resource *resource);

			virtual ~MusicFMOD(void);

			virtual void play();
			virtual void stop();
			virtual void pause(bool e);

			virtual unsigned int getTimeSample();
			virtual void setTimeSample(unsigned int t);
			virtual void setLoop(bool e);
			virtual int isPlaying();
	};
}
