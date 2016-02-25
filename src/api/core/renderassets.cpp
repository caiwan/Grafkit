#include "renderassets.h"

#include "easyloggingpp.h"

#include "exceptions.h"

//using namespace FWrender;
using namespace FWassets;
using namespace FWdebugExceptions;

FWassets::IRenderAsset::IRenderAsset() 
	//: m_assman(nullptr)
{
	GuidGenerator gen;
	this->m_guid = gen.newGuid();
}

FWassets::IRenderAsset::~IRenderAsset()
{
	///@todo a destruktora ennek nem fasza, a virtualis fuggvenyeket a leszarmazott deletekor viszi magaval. Valami koser megoldas kellene a torlesre, ami megkeruli ezt 
	//if (m_assman != nullptr) {
	//	// m_assman->RemoveObject(this); 
	//}
}

void FWassets::IRenderAsset::SetName(std::string name)
{
	/*if (m_assman != nullptr) {
		m_assman->ChangeName(this, name);
	}*/
	m_name = name;
}

Guid FWassets::IRenderAsset::GenerateUUID()
{
	Guid uuid = m_guid;
	
	GuidGenerator gen;
	uuid = gen.newGuid();

	//if (m_assman != nullptr) {
	//	this->m_assman->ChangeUUID(this, uuid);
	//}

	m_guid = uuid;

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

size_t FWassets::IRenderAssetManager::AddObject(IRenderAsset * obj)
{
	//if (obj->m_assman == nullptr) obj->m_assman = this;
	size_t bid = obj->GetBucketID();

	this->m_assets.push_back(Ref<IRenderAsset>(obj));
	size_t id = this->m_assets.size() - 1;
	
	///@todo ha van ilyen objektum, akkor visitson.
	this->m_mapID[bid][obj->GetUUID()] = id;
	this->m_mapNames[bid][obj->GetName()] = id;

	LOG(TRACE) << "Object added to assman" << obj->GetName() << obj->GetUUID().toString().c_str() << id << bid;

	return id;
}

void FWassets::IRenderAssetManager::RemoveObject(IRenderAsset * obj)
{
	///@todo a destruktora az objektumoknek nem fasza, a virtualis fuggvenyeket a leszarmazott deletekor viszi magaval. Valami koser megoldas kellene a torlesre, ami megkeruli ezt 
	size_t bid = obj->GetBucketID();
	IRenderAssetManager::id_map_t::iterator it_id = this->m_mapID[bid].find(obj->GetUUID());
	IRenderAssetManager::name_map_t::iterator it_name = this->m_mapNames[bid].find(obj->GetName());

	size_t id = it_id->second;

	if (it_id != m_mapID[bid].end() && it_name != m_mapNames[bid].end()) 
	{
		m_mapID[bid].erase(it_id);
		m_mapNames[bid].erase(it_name);

		m_assets[id]->Release();

		LOG(TRACE) << "Object removed from assman" << obj->GetName() << obj->GetUUID().toString().c_str() << bid;
	}
	else {
		LOG(WARNING) << "No object to remove from assman" << obj->GetName() << obj->GetUUID().toString().c_str() << bid;
	}

	//if (obj->m_assman == this) obj->m_assman = nullptr;
}

void FWassets::IRenderAssetManager::ChangeName(IRenderAsset * obj, std::string newname)
{
	size_t bid = obj->GetBucketID(); 
	IRenderAssetManager::name_map_t::iterator it_name = this->m_mapNames[bid].find(obj->GetName());

	size_t id = it_name->second;

	this->m_mapNames[id].erase(it_name);
	this->m_mapNames[id][newname] = id;
}

void FWassets::IRenderAssetManager::ChangeUUID(IRenderAsset * obj, Guid newid)
{
	size_t bid = obj->GetBucketID();
	IRenderAssetManager::id_map_t::iterator it_id = this->m_mapID[bid].find(obj->GetUUID());

	size_t id = it_id->second;

	this->m_mapID[id].erase(it_id);
	this->m_mapID[id][newid] = id;
}

IRenderAsset * FWassets::IRenderAssetManager::GetObjectByUUID(enum IRenderAsset::RA_type_e type, Guid uuid)
{
	IRenderAssetManager::id_map_t::iterator it = this->m_mapID[type].find(uuid);
	return it == m_mapID[type].end() ? nullptr : m_assets[it->second];
	
}

IRenderAsset * FWassets::IRenderAssetManager::GetObjectByName(enum IRenderAsset::RA_type_e type, std::string name)
{
	IRenderAssetManager::name_map_t::iterator it = this->m_mapNames[type].find(name);
	return it == m_mapNames[type].end()? nullptr : m_assets[it->second];
}

// ==================================================================================================================================== 
//
//FWassets::IRenderAssetGenerator::IRenderAssetGenerator(IRenderAssetManager * const & assman) :
//	m_assman(assman)
//
//{
//}
//
//FWassets::IRenderAssetGenerator::~IRenderAssetGenerator()
//{
//}
