#include "stdafx.h"
#include "animation.h"

PERSISTENT_IMPL(Grafkit::Animation::Track);
PERSISTENT_IMPL(Grafkit::Animation::Channel);

using namespace Grafkit;

/* ============================================================================================== */

Animation::Animation()
//: m_duration(0) 
{
}

Animation::~Animation() {
}

/* ============================================================================================== */


void Animation::Channel::InsertKey(const size_t& beforeId, const Key& key)
{
    const auto it = m_keys.begin() + beforeId;
    m_keys.insert(it, key);
}

void Animation::Channel::DeleteKey(const size_t& id)
{
    const auto it = m_keys.begin() + id;
    m_keys.erase(it);
}

void Animation::Channel::Serialize(Archive& ar) {
    assert(0);
}

/* ============================================================================================== */

Animation::Track::Track(const char* name, const std::vector<std::string> channelNames)
    : m_name(name) { if (!channelNames.empty()) { for (size_t a = 0; a < channelNames.size(); ++a) { CreateChannel(channelNames.at(a)); } } }


Animation::Track::Track(const char* name, const char* initials)
    : m_name(name)
{
    if (initials)
    {
        size_t i = 0;
        while (initials[i])
        {
            CreateChannel(std::string(1, initials[i]));
            i++;
        }
    }
}

size_t Animation::Track::CreateChannel(const char* name)
{
    m_channels.push_back(new Channel(name));
    return m_channels.size() - 1;
}

size_t Animation::Track::CreateChannel(std::string name)
{
    m_channels.push_back(new Channel(name));
    return m_channels.size() - 1;
}

float3 Animation::Track::GetFloat3(float t) const
{
    if (m_channels.size() < 3)
        return float3();

    return float3(
        m_channels[0]->GetValue(t),
        m_channels[1]->GetValue(t),
        m_channels[2]->GetValue(t)
    );
}

float4 Animation::Track::GetFloat4(float t) const
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

void Animation::Track::SetFloat3(size_t id, float3 v)
{
    m_channels[0]->SetValue(id, v.x);
    m_channels[1]->SetValue(id, v.y);
    m_channels[2]->SetValue(id, v.z);
}

void Animation::Track::SetFloat4(size_t id, float4 v)
{
    m_channels[0]->SetValue(id, v.x);
    m_channels[1]->SetValue(id, v.y);
    m_channels[2]->SetValue(id, v.z);
    m_channels[3]->SetValue(id, v.w);
}

void Animation::Track::Serialize(Archive& ar)
{
    assert(0);
}

/* ============================================================================================== */

void Animation::_Serialize(Archive& ar)
{
    PERSIST_STRING(ar, m_name);
}
