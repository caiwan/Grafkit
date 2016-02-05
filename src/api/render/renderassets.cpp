#include "renderassets.h"

#include "easyloggingpp.h"

#include "exceptions.h"

using namespace FWrender;
using namespace FWdebugExceptions;

FWrender::IRenderAsset::IRenderAsset(IRenderAssetManager * assman)
	: m_assman(assman)
{
	if (!m_assman) 
	{
		LOG(WARNING) << "No given ass man. Operating w/o it.";
	}
	else {
		GuidGenerator gen;
		this->m_guid = gen.newGuid();
		this->m_assman->AddObject(this);
	}
}

FWrender::IRenderAsset::~IRenderAsset()
{
	if (m_assman != nullptr) {
		m_assman->RemoveObject(this);
	}
}

void FWrender::IRenderAsset::SetName(std::string name)
{
	std::string oldname = m_name;
	m_name = name;

	if (m_assman != nullptr) {
		m_assman->ChangeName(this, oldname);
	}
}

Guid FWrender::IRenderAsset::GenerateGUID()
{
	Guid old_uuid = m_guid;
	
	GuidGenerator gen;
	this->m_guid = gen.newGuid();

	if (m_assman != nullptr) {
		this->m_assman->ChangeUUID(this, old_uuid);
	}

	return this->m_guid;
}

// ==================================================================================================================================== 
FWrender::IRenderAssetManager::IRenderAssetManager()
{
	// ...
}

FWrender::IRenderAssetManager::~IRenderAssetManager()
{
	// ... 
}

void FWrender::IRenderAssetManager::AddObject(IRenderAsset * obj)
{
	this->m_mapID[obj->GetUUID()] = obj;
	this->m_mapNames[obj->GetName()] = obj;

	LOG(TRACE) << "Object added to assman" << obj->GetName << obj->GetUUID().toString_c_str();
}

void FWrender::IRenderAssetManager::RemoveObject(IRenderAsset * obj)
{
	IRenderAssetManager::id_map_t::iterator it_id = this->GetIDIterator(obj->GetUUID());
	IRenderAssetManager::name_map_t::iterator it_name = this->GetNameItertator(obj->GetName());

	if (it_id != m_mapID.end() && it_name != m_mapNames.end()) 
	{
		m_mapID.erase(it_id);
		m_mapNames.erase(it_name);
		LOG(TRACE) << "Object removed from assman" << obj->GetName << obj->GetUUID().toString_c_str();
	}
	else {
		LOG(WARNING) << "No object to remove from assman" << obj->GetName << obj->GetUUID().toString_c_str();
	}
}

void FWrender::IRenderAssetManager::ChangeName(IRenderAsset * obj, std::string oldname)
{
	// foyltkov
	throw EX(NotImplementedMethodException);
}

void FWrender::IRenderAssetManager::ChangeUUID(IRenderAsset * obj, Guid olduuid)
{
	// foyltkov
	throw EX(NotImplementedMethodException);
}

IRenderAsset * FWrender::IRenderAssetManager::GetObjectByUUID(Guid uuid)
{
	IRenderAssetManager::id_map_t::iterator it = GetIDIterator(uuid);
	return it == m_mapID.end?nullptr:it->second;
}

IRenderAsset * FWrender::IRenderAssetManager::GetObjectByName(std::string name)
{
	IRenderAssetManager::name_map_t::iterator it = GetNameItertator(name);
	return it == m_mapID.end ? nullptr : it->second;
}

inline FWrender::IRenderAssetManager::name_map_t::iterator FWrender::IRenderAssetManager::GetNameItertator(std::string name)
{
	return this->m_mapNames.find(name);
}

inline FWrender::IRenderAssetManager::id_map_t::iterator FWrender::IRenderAssetManager::GetIDIterator(Guid uuid)
{
	return this->m_mapID.find(uuid);
}

// ==================================================================================================================================== 

// bazd az anyad tetejebe 
