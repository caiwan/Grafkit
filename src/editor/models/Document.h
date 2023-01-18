#pragma once 

#include "common.h"

#include "render/renderer.h"
#include "render/effect.h"

namespace Grafkit {
	class IResourceManager;
}

namespace Idogep {

	class Editor;
	class Document;
	
	class TreeModel;
	class SceneModel;

	class Music;

	namespace Roles {
		class HasSceneGraphRole : virtual public Referencable {
		public:
			virtual Grafkit::SceneGraphRef GetScenegraph() = 0;
		};

		class HasEffectRole : virtual public Referencable {
			// ... 
		};

		class HasPlaybackRole : virtual public Referencable {
		public:
			virtual Music * GetMusic() { return m_musicWrapper; }
		protected:
			Idogep::Music * m_musicWrapper;
		};
	}

	class Document : virtual public Referencable,
		public Roles::HasSceneGraphRole,
		public Roles::HasEffectRole,
		public Roles::HasPlaybackRole
	{
		friend class Editor;
	public:
		Document();
		virtual ~Document();

		void Preload(Grafkit::IResourceManager * const & resman);
		void Initialize(Grafkit::Renderer &render);
		//void Shutdown();

		void SetDirty() { m_isDirty = true; }
		bool IsDirty() const { return m_isDirty; }

		virtual Grafkit::SceneGraphRef GetScenegraph() { return m_scenegraph; }

	private:
		void InitTestStuff(Grafkit::Renderer & render);

		bool m_isDirty;

		Grafkit::ShaderResRef m_vs, m_ps;
		Grafkit::SceneResRef m_scene;
		Grafkit::SceneGraphRef m_scenegraph;

		Grafkit::ShaderResRef m_psShowUv;
		Grafkit::EffectComposerRef m_effect;

		Grafkit::ActorRef m_rootActor;
		Grafkit::ActorRef m_cameraActor;


	};

}
