#include "math/fbm.h"

#include "Document.h"

#include "ui/curve/curvedoc.h"

using namespace Idogep;
using namespace Grafkit;

void Idogep::MusicProxy::Play()
{
	if (m_music.Valid() && m_music->Valid())
		(*m_music)->Play();
}

void Idogep::MusicProxy::Stop()
{
	if (m_music.Valid() && m_music->Valid())
		(*m_music)->Stop();

}

void Idogep::MusicProxy::Pause(int e)
{
	if (m_music.Valid() && m_music->Valid())
		(*m_music)->Pause(e);

}

void Idogep::MusicProxy::Update()
{
	if (m_music.Valid() && m_music->Valid())
		(*m_music)->Update();
}

uint64_t Idogep::MusicProxy::GetSampleCount()
{
	if (m_music.Valid() && m_music->Valid())
		return (*m_music)->GetSampleCount();
	else
		return 0;
}

uint64_t Idogep::MusicProxy::GetSampleCountPerSec()
{
	if (m_music.Valid() && m_music->Valid())
		return (*m_music)->GetSampleCountPerSec();
	else
		return 0;
}

uint64_t Idogep::MusicProxy::GetTimeSample()
{
	if (m_music.Valid() && m_music->Valid())
		return (*m_music)->GetTimeSample();
	else
		return 0;
}

void Idogep::MusicProxy::SetTimeSample(uint64_t t)
{
	if (m_music.Valid() && m_music->Valid())
		(*m_music)->SetTimeSample(t);
}

void Idogep::MusicProxy::SetLoop(int e)
{
	return;
}

int Idogep::MusicProxy::IsPlaying()
{
	if (m_music.Valid() && m_music->Valid())
		return (*m_music)->IsPlaying();
	else
		return 0;
}

// ===================================================================================================================


void Idogep::MusicProxy::getWaveform(float *& ptr, size_t & length, size_t & channelCount, size_t & samplePerSec)
{
	if (m_music.Valid() && m_music->Valid())
		(*m_music)->GetWaveform(ptr, length, channelCount, samplePerSec);
}

Idogep::EditorDocument::EditorDocument()
{
	m_testAnimation = new Animation::FloatTrack();
	for (int i = 0; i < 35; i++) {
		m_testAnimation->AddKey(Animation::FloatKey(i, PseudoRandom::Random()));
	}

	m_testCurveDocument = new CurveDocument(m_testAnimation);
	m_testCurveDocument->onRequestWaveform += Delegate(this, &MusicProxy::getWaveform);
}

void Idogep::EditorDocument::beforePreload(Grafkit::IResourceManager * const & resman)
{
	 // ... 
}

void Idogep::EditorDocument::afterPreload(Grafkit::Renderer & renderer)
{
	// ...
}
