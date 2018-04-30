#pragma once 

#include "common.h"

#include "render/renderer.h"
#include "render/effect.h"

#include "animation/scene.h"

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
			virtual Grafkit::SceneGraphRef GetScenegraph() const = 0;
			virtual Grafkit::SceneResRef GetScene() const = 0;
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
		void Shutdown();

		void SetDirty() { m_isDirty = true; }
		bool IsDirty() const { return m_isDirty; }

	    Grafkit::SceneGraphRef GetScenegraph() const override { return m_scenegraph; }
		Grafkit::SceneResRef GetScene() const override { return m_scene; }

	private:
		void InitTestStuff(Grafkit::Renderer & render);

	private:
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
