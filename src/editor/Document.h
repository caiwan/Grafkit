#pragma once 

#include <qobject.h>

#include "render/renderer.h"
#include "render/animation.h"
#include "core/Music.h"

//#include "utils/ResourceManager.h"

#include "Event.h"

namespace Grafkit {
	class IResourceManager;
}

namespace Idogep {

	class EditorDocument;
	class CurveDocument;

	class MusicProxy : public Grafkit::Timer
	{
	public:
		virtual void Play();
		virtual void Stop();
		virtual void Pause(int e);
		virtual void Update();

		virtual uint64_t GetSampleCount();
		virtual uint64_t GetSampleCountPerSec();

		virtual uint64_t GetTimeSample();
		virtual void SetTimeSample(uint64_t t);
		virtual void SetLoop(int e);
		virtual int IsPlaying();

		void GetWaveform(float *&ptr, size_t &length, size_t &channelCount, size_t &samplePerSec);

		Event<> onMusicChanged;

	protected:
		Grafkit::MusicResRef m_music;
	};

	class EditorDocument : public QObject, public MusicProxy
	{
		Q_OBJECT
	public:
		EditorDocument();

		Ref<Grafkit::Animation::FloatTrack> track() { return m_testAnimation; }

		void beforePreload(Grafkit::IResourceManager* const & resman);
		void afterPreload(Grafkit::Renderer & renderer);

		void setDirty() { m_isDirty = true; }
		bool dirty() { return m_isDirty; }

	private:

		bool m_isDirty;

		Ref<Grafkit::Animation::FloatTrack> m_testAnimation;

	};

}
