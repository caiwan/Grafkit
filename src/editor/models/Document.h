#pragma once 

#include "common.h"

#include "render/renderer.h"
#include "render/effect.h"
#include "utils/ResourcePreloader.h"

namespace Grafkit {
	class IResourceManager;
}

namespace Idogep {

	class Editor;
	class Document;

	class TreeModel;
	class SceneModel;

	class Music;

	class Document : virtual public Referencable, public Grafkit::ResourcePreloader
	{
		friend class Editor;
	public:
		Document();
		virtual ~Document();

		void Preload(IResourceManager * const & resman);
		void Initialize(Grafkit::Renderer &render);
		//void Shutdown();

		void SetDirty() { m_isDirty = true; }
		bool IsDirty() const { return m_isDirty; }

		Grafkit::SceneGraphRef GetScenegraph() const;
		Grafkit::SceneResRef GetScene() const;
		Grafkit::MusicResRef GetMusic() const;

	private:
		void InitTestStuff(Grafkit::Renderer & render);

		bool m_isDirty;



		Grafkit::MusicResRef m_music;
	};

}
