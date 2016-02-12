#include "renderassets.h"

#include "easyloggingpp.h"

#include "exceptions.h"

//using namespace FWrender;
using namespace FWassets;
using namespace FWdebugExceptions;

FWassets::IRenderAsset::IRenderAsset() :
	m_assman(nullptr)
{
	GuidGenerator gen;
	this->m_guid = gen.newGuid();
}

FWassets::IRenderAsset::~IRenderAsset()
{
	if (m_assman != nullptr) {
		m_assman->RemoveObject(this);
	}
}

void FWassets::IRenderAsset::SetName(std::string name)
{
	std::string oldname = m_name;
	m_name = name;

	if (m_assman != nullptr) {
		m_assman->ChangeName(this, oldname);
	}
}

Guid FWassets::IRenderAsset::GenerateGUID()
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
FWassets::IRenderAssetManager::IRenderAssetManager()
{
	// ...
}

FWassets::IRenderAssetManager::~IRenderAssetManager()
{
	// ... 
}

void FWassets::IRenderAssetManager::AddObject(IRenderAsset * obj)
{
	if (obj->m_assman == nullptr) obj->m_assman = this;
	this->m_mapID[obj->GetUUID()] = obj;
	this->m_mapNames[obj->GetName()] = obj;

	LOG(TRACE) << "Object added to assman" << obj->GetName() << obj->GetUUID().toString_c_str();
}

void FWassets::IRenderAssetManager::RemoveObject(IRenderAsset * obj)
{
	IRenderAssetManager::id_map_t::iterator it_id = this->GetIDIterator(obj->GetUUID());
	IRenderAssetManager::name_map_t::iterator it_name = this->GetNameItertator(obj->GetName());

	if (it_id != m_mapID.end() && it_name != m_mapNames.end()) 
	{
		m_mapID.erase(it_id);
		m_mapNames.erase(it_name);
		LOG(TRACE) << "Object removed from assman" << obj->GetName() << obj->GetUUID().toString_c_str();
	}
	else {
		LOG(WARNING) << "No object to remove from assman" << obj->GetName() << obj->GetUUID().toString_c_str();
	}

	if (obj->m_assman == this) obj->m_assman = nullptr;
}

void FWassets::IRenderAssetManager::ChangeName(IRenderAsset * obj, std::string oldname)
{
	// foyltkov
	throw EX(NotImplementedMethodException);
}

void FWassets::IRenderAssetManager::ChangeUUID(IRenderAsset * obj, Guid olduuid)
{
	// foyltkov
	throw EX(NotImplementedMethodException);
}

IRenderAsset * FWassets::IRenderAssetManager::GetObjectByUUID(Guid uuid)
{
	IRenderAssetManager::id_map_t::iterator it = GetIDIterator(uuid);
	return it == m_mapID.end()?nullptr:it->second;
}

IRenderAsset * FWassets::IRenderAssetManager::GetObjectByName(std::string name)
{
	IRenderAssetManager::name_map_t::iterator it = GetNameItertator(name);
	return it == m_mapNames.end()? nullptr : it->second;
}

inline FWassets::IRenderAssetManager::name_map_t::iterator FWassets::IRenderAssetManager::GetNameItertator(std::string name)
{
	return this->m_mapNames.find(name);
}

inline FWassets::IRenderAssetManager::id_map_t::iterator FWassets::IRenderAssetManager::GetIDIterator(Guid uuid)
{
	return this->m_mapID.find(uuid);
}

// ==================================================================================================================================== 

FWassets::IRenderAssetGenerator::IRenderAssetGenerator(IRenderAssetManager * const & assman) :
	m_assman(assman)

{
}

FWassets::IRenderAssetGenerator::~IRenderAssetGenerator()
{
}
