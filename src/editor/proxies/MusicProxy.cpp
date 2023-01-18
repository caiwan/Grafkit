#include "MusicProxy.h"

void Idogep::MusicProxy::Play()
{
	if (HasMusic())
		(*m_music)->Play();
}

void Idogep::MusicProxy::Stop()
{
	if (HasMusic())
		(*m_music)->Stop();
}

void Idogep::MusicProxy::Pause(int e)
{
	if (HasMusic())
		(*m_music)->Pause(e);
}

void Idogep::MusicProxy::Update()
{
	if (HasMusic())
		(*m_music)->Update();
}

void Idogep::MusicProxy::ToggleMute()
{
	if (HasMusic())
		(*m_music)->ToggleMute();
}

uint64_t Idogep::MusicProxy::GetSampleCount()
{
	if (HasMusic())
		return (*m_music)->GetSampleCount();
	return 0;
}

uint64_t Idogep::MusicProxy::GetSampleCountPerSec()
{
	if (HasMusic())
		return (*m_music)->GetSampleCountPerSec();
	return 1;
}

uint64_t Idogep::MusicProxy::GetTimeSample()
{
	if (HasMusic())
		return (*m_music)->GetTimeSample();
	return 0;
}

void Idogep::MusicProxy::SetTimeSample(uint64_t t)
{
	if (HasMusic())
		(*m_music)->SetTimeSample(t);
}

void Idogep::MusicProxy::SetLoop(int e)
{
}

int Idogep::MusicProxy::IsPlaying()
{
	if (HasMusic())
		return (*m_music)->IsPlaying();
	return 0;
}

// ===================================================================================================================


void Idogep::MusicProxy::GetFFT(float* ptr, int segcount)
{
	if (HasMusic())
		(*m_music)->GetFFT(ptr, segcount);
}

void Idogep::MusicProxy::GetWaveform(float*& ptr, size_t& length, size_t& channelCount, size_t& samplePerSec)
{
	if (HasMusic())
		(*m_music)->GetWaveform(ptr, length, channelCount, samplePerSec);
}

bool Idogep::MusicProxy::HasMusic() const { return m_music.Valid() && m_music->Valid(); }
