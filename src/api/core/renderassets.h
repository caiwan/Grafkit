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

namespace FWassets{

	class IRenderAsset;
	class IRenderAssetManager;
	class IRenderAssetBuilder;

	/**
	A base class for collectable render assets for bulk loading
	*/

	class IRenderAsset : virtual public Referencable
	{
		friend class IRenderAssetManager;
		friend class IRenderAssetGenerator;

	public:
		IRenderAsset();
		virtual ~IRenderAsset();

		std::string GetName() { return this->m_name; }
		void SetName(std::string name);

		Guid GetUUID() { return this->m_guid; }
		Guid GenerateUUID();
		void SetUUID(Guid uuid) { this->m_guid = uuid; }

		virtual const char* GetBucketID() = 0;

	protected:
		/// Sets asset manager 
		std::string m_name;
		Guid m_guid;
	};

	///@todo a render asset manager asset repositorykat tarol; 
	///@todo a repositiry-k tartoznak egy-egy generatorhoz; esetleg csatolhatok egy scenehez. - mindezek nevvel lennenek ellatva

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

		Ref<IRenderAsset> GetObjPtr(size_t id) { return this->m_assets[id]; }

	protected:
		std::vector<Ref<IRenderAsset>> m_assets;

		typedef std::map<std::string, size_t> name_map_t;
		typedef std::map<Guid, size_t> id_map_t;

		struct bucket_t {
			name_map_t m_mapNames;
			id_map_t m_mapID;
		};

		struct bucket_t* GetBucket(std::string bucket);

		typedef std::map<std::string, bucket_t> name_bucket_map_t;
		name_bucket_map_t m_bucket;
	};

	
	///@todod harelease-ben forditod, akkor eleg egy ":"
#define ROOT_REPOSITORY ":root"

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
		virtual IRenderAssetRepository* newRenderAssetRepository() { return new IRenderAssetRepository(); }

		IRenderAssetRepository* GetRepository(std::string name);

	protected:
		typedef std::map<std::string, IRenderAssetRepository*> repository_map_t;
		repository_map_t m_repository;


		///@todo valahova ide be kellene rakni egy mapet, ami megfelelteti az uuid-t a megfelelo rempository bucktjanak az objektumokat
	};

	///@todo ez az egesz hobelebanc itten e teljes revizionalasra, es ujratervezesre szorul.

	/**
	ez az az interface, ami a bejovo input ertekre - nev, vagy uuid, legeneralja a megfelelo asset buildert
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
DEFINE_EXCEPTION(NoAssetFoundByUUIDException, 2, "No asset found by the given UUID");
DEFINE_EXCEPTION(NoAssetBucketFoundException, 3, "No asset bucket found");
