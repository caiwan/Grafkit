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

namespace FWassets{

	class IRenderAsset;
	class IRenderAssetManager;
	class IRenderAssetGenerator;

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
		Guid GenerateGUID();

		enum RA_type_e {
			RA_TYPE_Texture,
			RA_TYPE_Font,
			RA_TYPE_Material,
			RA_TYPE_Shader,
			RA_TYPE_Entity3D,
			RA_TYPE_Aux0,
			RA_TYPE_Aux1,
			RA_TYPE_Aux2,
			RA_TYPE_Aux3,
			RA_TYPE_Aux4,
			RA_TYPE_COUNT
		};

		virtual enum RA_type_e GetBucketID() = 0;

	protected:
		/// Sets asset manager 
		std::string m_name;
		Guid m_guid;

		// IRenderAssetManager* m_assman;
	};

	/**
	An asset manager that 
	- contains all the existing assets
	- requests them from disk if needed
	*/
	class IRenderAssetManager 
	{
		friend class IRenderAsset;
		friend class IRenderAssetGenerator;

	public:
		IRenderAssetManager();
		virtual ~IRenderAssetManager();

		virtual FWassets::IResourceFactory* GetResourceFactory() = 0;
		virtual FWrender::Renderer & GetDeviceContext() = 0;

	public:
		/**
		Hozzaadja az assetet a repohoz.
		*/
		void AddObject(IRenderAsset* obj);
		void RemoveObject(IRenderAsset* obj);

		///@{
		/**
		Ha van asset manager beállitva a render assetek fele, azok minden set name es guid hivaskor meghivjak ezeket.
		Ha lehet, érdmes elõbb beállítani az nevet es az uuid-t, majd aztan beallitani az asset managert, es hozza adni az asset repokohz a dolgokat.
		*/
		void ChangeName(IRenderAsset* obj, std::string newname);
		void ChangeUUID(IRenderAsset *obj, Guid newuuid);
		///@}
		IRenderAsset* GetObjectByUUID(enum IRenderAsset::RA_type_e type, Guid uuid);
		IRenderAsset* GetObjectByName(enum IRenderAsset::RA_type_e type, std::string name);

	private:
		typedef std::map<std::string, IRenderAsset*> name_map_t;
		typedef std::map<Guid, IRenderAsset*> id_map_t;

		name_map_t m_mapNames[IRenderAsset::RA_TYPE_COUNT];
		id_map_t m_mapID[IRenderAsset::RA_TYPE_COUNT];
	};

	/**
	An asset generator interface that is used during the loader sequence; 
	*/
	class IRenderAssetGenerator
	{
		public:
			IRenderAssetGenerator(IRenderAssetManager * const & assman);
			virtual ~IRenderAssetGenerator();
			
			virtual void operator () () = 0;
		
		protected:
				/// segg ember
				IRenderAssetManager * const & m_assman;
	};
	
	/**
	A container for render assets
	*/
	class RenderAssetGeneratorContainer : virtual public Referencable, public Iterable
	{
		public:
			RenderAssetGeneratorContainer();
			virtual ~RenderAssetGeneratorContainer();
			
			RenderAssetGeneratorContainer& operator<< (IRenderAssetGenerator* ptr);

			virtual Iterator* getIterator();
			
		private:
			Node * m_first;
			Node * m_last;
	};
}
