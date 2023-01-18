/**
A generator interface for assets 
*/

#pragma once

#include <map>
#include <string>

#include "reference.h"
#include "node.h"
#include "guid.h"

#include "assets.h"

#include "../render/renderer.h"

#include "exceptions.h"

// egyelore nem szerencsetlenkedunk a guid-dal, de ha megirtam eddig, akkor maradjon
#define _NO_UUID_

namespace FWassets{

	class IRenderAsset;
	class IRenderAssetManager;
	class IRenderAssetBuilder;
	//class IRenderAssetRepository;

	///@todo ezt ki kell pucolni teljesen

	/**
	A base class for collectable render assets for bulk loading
	*/

	class IRenderAsset : virtual public Referencable
	{
		friend class IRenderAssetManager;
		friend class IRenderAssetRepository;
		friend class IRenderAssetGenerator;

	public:
		IRenderAsset();
		virtual ~IRenderAsset();

		std::string GetName() { return this->m_name; }
		void SetName(std::string name);

#ifndef _NO_UUID_
		Guid GetUUID() { return this->m_guid; }
		Guid GenerateUUID();
		void SetUUID(Guid uuid) { this->m_guid = uuid; }
	protected:
		Guid m_guid;
#endif // _NO_UUID_

		virtual const char* GetBucketID() = 0;

	protected:
		/// Sets asset manager 
		std::string m_name;

	};

	///@todo a render asset manager asset repositorykat tarol; 
	///@todo a repositiry-k tartoznak egy-egy generatorhoz; esetleg csatolhatok egy scenehez. - mindezek nevvel lennenek ellatva

	// erre egyelore most nem lesz szukseg; visszadobom az asset managerbe a maradekot
#if 0
#define _HAS_IRenderAssetRepository 
	/**
	IRenderAssetRepository;
		- *Ezen belul bucketek vannak, amik a generatoron belul felhasznalt, es elert asseteket taroljak.												  *
		- *Legyen egy root repository, ahol a mindenki szamara elerhet kozos cuccok vannak; shaderek, texturak, fontok; ezek. - ilyenre kell atalakitani*
	*/
	class IRenderAssetRepository {
	public:
		IRenderAssetRepository() {}
		virtual ~IRenderAssetRepository() {}

	public:
		/**
		Hozzaadja az assetet a repohoz.
		*/
		size_t AddObject(IRenderAsset* obj);
		void RemoveObject(IRenderAsset* obj);

		///@{
		/**
		Ha van asset manager beallitva a render assetek fele, azok minden set name es guid hivaskor meghivjak ezeket.
		Ha lehet, erdmes elobb beallitani az nevet es az uuid-t, majd aztan beallitani az asset managert, es hozza adni az asset repokohz a dolgokat.
		*/
		///@todo ezek ketten nem fognak mukodni egyelore
		/*void ChangeName(IRenderAsset* obj, std::string newname);
		void ChangeUUID(IRenderAsset *obj, Guid newuuid);*/
		///@}

		Ref<IRenderAsset> GetObjectByUUID(std::string bucket, Guid uuid);
		Ref<IRenderAsset> GetObjectByName(std::string bucket, std::string name);

		Ref<IRenderAsset> GetObjPtrByGlobalID(size_t id) { return this->m_assets[id]; }
		Ref<IRenderAsset> GetObjPtr(std::string bucket, size_t id); //{ return this->m_assets[id]; }

	protected:
		std::vector<Ref<IRenderAsset>> m_assets;

		typedef std::map<std::string, size_t> name_map_t;
		typedef std::map<Guid, size_t> id_map_t;

		struct bucket_t {
			std::vector<int> m_ids;
			name_map_t m_mapNames;
			id_map_t m_mapID;
		};

		struct bucket_t* GetBucket(std::string bucket);

		typedef std::map<std::string, bucket_t> name_bucket_map_t;
		name_bucket_map_t m_bucket;
	};

#endif 



	///@todod harelease-ben forditod, akkor eleg egy ":"
//#define ROOT_REPOSITORY ":root"

	/**
	An asset manager that 
	- contains all the existing assets
	- requests them from disk if needed
	*/
	class IRenderAssetManager 
	{
		friend class IRenderAsset;
		friend class IRenderAssetBuilder;

	public:
		IRenderAssetManager();
		virtual ~IRenderAssetManager();

		//virtual FWassets::IResourceFactory* GetResourceFactory() = 0;
		virtual FWrender::Renderer & GetDeviceContext() = 0;

		///Arra az esetre, ha valamit kezdeni akarunk vele
		//virtual IRenderAssetRepository* newRenderAssetRepository() { return new IRenderAssetRepository(); }

		//IRenderAssetRepository* GetRepository(std::string name);

		// --- ezek kellenek 
		size_t AddObject(IRenderAsset* obj);
		void RemoveObject(IRenderAsset* obj);

#ifndef _NO_UUID_
		/*Ref<IRenderAsset>*/ IRenderAsset* GetObjectByUUID(std::string bucket, Guid uuid);
#endif _NO_UUID_
		/*Ref<IRenderAsset>*/ IRenderAsset* GetObjectByName(std::string bucket, std::string name);

	protected:
		/*typedef std::map<std::string, IRenderAssetRepository*> repository_map_t;
		repository_map_t m_repository;*/

		std::vector<Ref<IRenderAsset>> m_assets;
		
		typedef std::map<std::string, size_t> name_map_t;
#ifndef _NO_UUID_
		typedef std::map<Guid, size_t> id_map_t;
#endif //_NO_UUID_

		struct bucket_t {
			std::vector<int> m_ids;
			name_map_t m_mapNames;
#ifndef _NO_UUID_
			id_map_t m_mapID;
#endif //_NO_UUID_
		};

		std::vector<bucket_t> m_buckets;

		typedef std::map<std::string, size_t> buket_map_t;

		size_t GetBucket(std::string bucket); 
		//IRenderAsset *GetAsset(bucket_t* bucket, std::);

	};

	///@todo ez az egesz hobelebanc itten e teljes revizionalasra, es ujratervezesre szorul.

	/**
	Ezt gyujti be a preloader, es gyartja le a megfelelo asseteket
	*/
	class IRenderAssetBuilder
	{
		public:
			IRenderAssetBuilder() {}
			virtual ~IRenderAssetBuilder() {}
			
			virtual void operator () (IRenderAssetManager * const & assman) = 0;
	};
	
}

DEFINE_EXCEPTION(NoAssetFoundByNameException, 1, "No asset found by the given name");
#ifndef _NO_UUID_
DEFINE_EXCEPTION(NoAssetFoundByUUIDException, 2, "No asset found by the given UUID");
#endif //_NO_UUID_
DEFINE_EXCEPTION(NoAssetBucketFoundException, 3, "No asset bucket found");
