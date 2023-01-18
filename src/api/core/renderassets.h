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

		//IRenderAsset(IRenderAssetManager * assman = nullptr);
		IRenderAsset();
		virtual ~IRenderAsset();

		std::string GetName() { return this->m_name; }
		void SetName(std::string name);

		Guid GetUUID() { return this->m_guid; }
		Guid GenerateGUID();

	protected:
		/// Sets asset manager 
		void SetAssmanPtr(IRenderAssetManager * assman = nullptr);

		std::string m_name;
		Guid m_guid;

		IRenderAssetManager* m_assman;
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

	protected:
		void AddObject(IRenderAsset* obj);
		void RemoveObject(IRenderAsset* obj);
		void ChangeName(IRenderAsset* obj, std::string oldname);
		void ChangeUUID(IRenderAsset *obj, Guid olduuid);

		IRenderAsset* GetObjectByUUID(Guid uuid);
		IRenderAsset* GetObjectByName(std::string name);

	private:
		typedef std::map<std::string, IRenderAsset*> name_map_t;
		typedef std::map<Guid, IRenderAsset*> id_map_t;

		name_map_t::iterator GetNameItertator(std::string name);
		id_map_t::iterator GetIDIterator(Guid uuid);

		name_map_t m_mapNames;
		id_map_t m_mapID;
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
