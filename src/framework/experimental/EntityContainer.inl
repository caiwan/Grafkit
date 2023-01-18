#pragma once 
#include "render/Actor.h"

inline void Grafkit::EntityContainer::AddEntiy(ActorRef node, Entity3DRef entity)
{
	if (node.Invalid() || entity.Invalid())
		return;
	entity_t record(node, entity, m_entities.size());
	m_entities.push_back(record);
	m_entityMap[node->GetName()] = record;
}

inline Grafkit::ActorRef Grafkit::EntityContainer::GetActor(std::string name)
{
	auto it = m_entityMap.find(name);
	if (it != m_entityMap.end())
		return it->second.m_actor;
	else
		return nullptr;
}

inline Grafkit::Entity3DRef Grafkit::EntityContainer::GetEntity(std::string name)
{
	auto it = m_entityMap.find(name);
	if (it != m_entityMap.end())
		return it->second.m_entity;
	else
		return nullptr;
}

inline size_t Grafkit::EntityContainer::GetId(std::string name)
{
	auto it = m_entityMap.find(name);
	if (it != m_entityMap.end())
		return it->second.m_id;
	else
		return 0;
}

inline void Grafkit::EntityContainer::Calculate(Renderer & render, size_t id)
{
	auto &record = m_entities[id];
	if (record.m_actor.Invalid() || record.m_entity.Invalid())
		return;
	record.m_entity->Calculate(render, record.m_actor);
}

inline void Grafkit::EntityContainer::CalculateAll(Renderer & render)
{
	for (auto it = m_entities.begin(); it != m_entities.end(); it++) {

		if (it->m_actor.Invalid() || it->m_entity.Invalid())
			continue;
		it->m_entity->Calculate(render, it->m_actor);
	}
}

