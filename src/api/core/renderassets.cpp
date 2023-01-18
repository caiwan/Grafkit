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
}

void FWassets::IRenderAsset::SetName(std::string name)
{
	m_name = name;
}

Guid FWassets::IRenderAsset::GenerateUUID()
{
	Guid uuid = m_guid;
	
	GuidGenerator gen;
	uuid = gen.newGuid();

	m_guid = uuid;

	return this->m_guid;
}

// ==================================================================================================================================== 
size_t FWassets::IRenderAssetRepository::AddObject(IRenderAsset * obj)
{
	m_bucket[obj->GetBucketID()]; // ez automatikusan letrehoz egy ures vodrot, ha nincs iylen

	bucket_t *bucket_entry = GetBucket(obj->GetBucketID());

	this->m_assets.push_back(Ref<IRenderAsset>(obj));
	size_t id = this->m_assets.size() - 1;

	///@todo ha van ilyen objektum, akkor visitson. Duplan visitson;
	bucket_entry->m_ids.push_back(id);
	bucket_entry->m_mapID[obj->GetUUID()] = id;
	bucket_entry->m_mapNames[obj->GetName()] = id;

	LOG(TRACE) << "Object added to assman" << obj->GetName() << obj->GetUUID().toString().c_str() << id << obj->GetBucketID();

	return id;
}

void FWassets::IRenderAssetRepository::RemoveObject(IRenderAsset * obj)
{
	///@todo a destruktora az objektumoknek nem fasza, a virtualis fuggvenyeket a leszarmazott deletekor viszi magaval. Valami koser megoldas kellene a torlesre, ami megkeruli ezt 

	bucket_t *bucket_entry = GetBucket(obj->GetBucketID());

	///@todo a nevek csak masodlagosak; valahogy szurni kell rajuk 

	IRenderAssetRepository::id_map_t::iterator it_id = bucket_entry->m_mapID.find(obj->GetUUID());
	IRenderAssetRepository::name_map_t::iterator it_name = bucket_entry->m_mapNames.find(obj->GetName());

	size_t id = it_id->second;

	if (it_id != bucket_entry->m_mapID.end() && it_name != bucket_entry->m_mapNames.end())
	{
		///@todo proper remove from this - bucket_entry->m_ids[it_id->second] = -1;
		bucket_entry->m_mapID.erase(it_id);
		bucket_entry->m_mapNames.erase(it_name);

		m_assets[id]->Release();

		///@todo logot ki kell makrozni
		LOG(TRACE) << "Object removed from assman" << obj->GetName() << obj->GetUUID().toString().c_str() << obj->GetBucketID();
	}
	else {
		LOG(WARNING) << "No object to remove from assman" << obj->GetName() << obj->GetUUID().toString().c_str() << obj->GetBucketID();
	}
}

///@todo ezek ketten nem fognak mukodni egyelore
//void FWassets::IRenderAssetRepository::ChangeName(IRenderAsset * obj, std::string newname)
//{
//	size_t bid = obj->GetBucketID();
//	IRenderAssetManager::name_map_t::iterator it_name = this->m_mapNames[bid].find(obj->GetName());
//
//	size_t id = it_name->second;
//
//	this->m_mapNames[id].erase(it_name);
//	this->m_mapNames[id][newname] = id;
//}
//
//void FWassets::IRenderAssetRepository::ChangeUUID(IRenderAsset * obj, Guid newid)
//{
//	size_t bid = obj->GetBucketID();
//	IRenderAssetManager::id_map_t::iterator it_id = this->m_mapID[bid].find(obj->GetUUID());
//
//	size_t id = it_id->second;
//
//	this->m_mapID[id].erase(it_id);
//	this->m_mapID[id][newid] = id;
//}

Ref<IRenderAsset> FWassets::IRenderAssetRepository::GetObjectByUUID(std::string bucket, Guid uuid)
{
	bucket_t *bucket_entry = GetBucket(bucket);
	if (bucket_entry) {
		IRenderAssetRepository::id_map_t::iterator it = bucket_entry->m_mapID.find(uuid);
		///@todo ezt kimakrozni, ha olyan, releasebe ne forduljon be
		if (it == bucket_entry->m_mapID.end())
		{
			LOG(WARNING) << "Could not find " << uuid.toString().c_str();
			// mivel mindenkit betoltunk a lemezrol, ezert nem art, ha az assetmanager dob azonal egy hibat, ahelyett, hogy nullobjektumunk lenne
			throw EX_DETAILS(NoAssetFoundByNameException, uuid.toString().c_str());
		}
		return it == bucket_entry->m_mapID.end() ? nullptr : m_assets[it->second];
	}
	else {
		///@todo ezt kimakrozni, ha olyan
		LOG(WARNING) << "Could not find bucket" << bucket.c_str();
		throw EX_DETAILS(NoAssetBucketFoundException, bucket.c_str());

		return nullptr;
	}
}

Ref<IRenderAsset> FWassets::IRenderAssetRepository::GetObjectByName(std::string bucket, std::string name)
{	
	bucket_t *bucket_entry = GetBucket(bucket);
	if (bucket_entry) {
		///@todo ezt kimakrozni, ha olyan, releasebe ne forduljon be
		IRenderAssetRepository::name_map_t::iterator it = bucket_entry->m_mapNames.find(name);
		if (it == bucket_entry->m_mapNames.end()) {
			LOG(WARNING) << "Could not find " << name;
			throw EX_DETAILS(NoAssetFoundByNameException, name.c_str());
		}
		return it == bucket_entry->m_mapNames.end() ? nullptr : m_assets[it->second];
	}
	else {
		///@todo ezt kimakrozni, ha olyan; releasebe ne forduljon be
		LOG(WARNING) << "Could not find bucket" << bucket.c_str();
		throw EX_DETAILS(NoAssetBucketFoundException, bucket.c_str());
		
		return nullptr;
	}
}

Ref<IRenderAsset> FWassets::IRenderAssetRepository::GetObjPtr(std::string bucket, size_t id)
{
	bucket_t* pbucket = GetBucket(bucket);
	if (!pbucket) {
		return Ref<IRenderAsset>();
	}
	
	int id_ = pbucket->m_ids[id];
	return id >= 0 ? Ref<IRenderAsset>() : m_assets[id_];
}

FWassets::IRenderAssetRepository::bucket_t * FWassets::IRenderAssetRepository::GetBucket(std::string bucket)
{
	FWassets::IRenderAssetRepository::name_bucket_map_t::iterator it =  m_bucket.find(bucket);
	if (it == m_bucket.end())
		return nullptr;
	return &it->second;
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

IRenderAssetRepository * FWassets::IRenderAssetManager::GetRepository(std::string name)
{
	if (!m_repository[name])
		m_repository[name] = newRenderAssetRepository();

	return m_repository[name];
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
