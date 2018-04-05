#include "stdafx.h"
#include "animation.h"

using namespace Grafkit;

/* ============================================================================================== */

void Grafkit::Animation::_serialize(Archive & ar)
{
	PERSIST_STRING(ar, m_name);
}

// Serialize??? 

Grafkit::Animation::Track::Track(const char * name, const std::vector<std::string> channelNames)
	: m_name(name)
{
	if (!channelNames.empty()) {
		for (size_t a = 0; a < channelNames.size(); ++a) {
			CreateChannel(channelNames.at(a));
		}

	}
}

Grafkit::Animation::Track::Track(const char * name, const char * initials)
	: m_name(name)
{
	if (initials) {
		size_t i = 0;
		while (initials[i]) {
			CreateChannel(std::string(1, initials[i]));
			i++;
		}
	}
}

size_t Grafkit::Animation::Track::CreateChannel(const char * name)
{
	m_channels.push_back(new Channel(name));
	return m_channels.size() - 1;

}

size_t Grafkit::Animation::Track::CreateChannel(std::string name)
{
	m_channels.push_back(new Channel(name));
	return m_channels.size() - 1;
}

float3 Grafkit::Animation::Track::GetFloat3(float t)
{
	if (m_channels.size() < 3)
		return float3();

	return float3(
		m_channels[0]->GetValue(t),
		m_channels[1]->GetValue(t),
		m_channels[2]->GetValue(t)
	);
}

float4 Grafkit::Animation::Track::GetFloat4(float t)
{
	if (m_channels.size() < 3)
		return float4();

	return float4(
		m_channels[0]->GetValue(t),
		m_channels[1]->GetValue(t),
		m_channels[2]->GetValue(t),
		m_channels[3]->GetValue(t)
	);
}

void Grafkit::Animation::Track::SetFloat3(size_t id, float3 v)
{
	m_channels[0]->SetValue(id, v.x);
	m_channels[1]->SetValue(id, v.y);
	m_channels[2]->SetValue(id, v.z);
}

void Grafkit::Animation::Track::SetFloat4(size_t id, float4 v)
{
	m_channels[0]->SetValue(id, v.x);
	m_channels[1]->SetValue(id, v.y);
	m_channels[2]->SetValue(id, v.z);
	m_channels[3]->SetValue(id, v.w);
}

