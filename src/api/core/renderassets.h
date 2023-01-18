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

		/// @todo ezt ki kell venni az osztalybol, es a nevterbe tenni eggyel feljebb
		enum RA_type_e {
			RA_TYPE_Texture,
			RA_TYPE_Font,
			RA_TYPE_Material,
			RA_TYPE_Light,
			RA_TYPE_Shader,  //RA_TYPE_Shader_VS, RA_TYPE_Shader_PS, RA_TYPE_Shader_GM, RA_TYPE_Shader_CP,
			RA_TYPE_Entity3D, /// @todo per-modelenkkent kell meg egy bucket; vagy meg kellene kovetelni a teljses nev egyediseget
			//RA_TYPE_Aux0,
			//RA_TYPE_Aux1,
			RA_TYPE_COUNT
		};

		virtual enum RA_type_e GetBucketID() = 0;

	protected:
		/// Sets asset manager 
		std::string m_name;
		Guid m_guid;
	};

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
		void ChangeName(IRenderAsset* obj, std::string newname);
		void ChangeUUID(IRenderAsset *obj, Guid newuuid);
		///@}

		Ref<IRenderAsset> GetObjectByUUID(enum IRenderAsset::RA_type_e type, Guid uuid);
		Ref<IRenderAsset> GetObjectByName(enum IRenderAsset::RA_type_e type, std::string name);

	protected:
		std::vector<Ref<IRenderAsset>> m_assets;

		typedef std::map<std::string, size_t> name_map_t;
		typedef std::map<Guid, size_t> id_map_t;

		name_map_t m_mapNames[IRenderAsset::RA_TYPE_COUNT];
		id_map_t m_mapID[IRenderAsset::RA_TYPE_COUNT];
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
