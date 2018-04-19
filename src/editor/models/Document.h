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

	namespace Role {
		class HasSceneGraphRole {
		public:
			virtual Grafkit::SceneGraphRef GetScenegraph() = 0;
		};

		class HasEffectRole {

		};
	}

	class Document : public HasSceneGraphRole
	{
		friend class Editor;
	public:
		Document();
		virtual ~Document();

		void Preload(Grafkit::IResourceManager * const & resman);
		void Initialize(Grafkit::Renderer &render);
		void Shutdown();

		void SetDirty() { m_isDirty = true; }
		bool IsDirty() { return m_isDirty; }

		virtual Grafkit::SceneGraphRef GetScenegraph() { return m_scenegraph; }

	private:
		void InitTestStuff(Grafkit::Renderer & render);

		bool m_isDirty;

		//Ref<Grafkit::Animation::FloatTrack> m_testAnimation;

		Grafkit::ShaderResRef m_vs, m_ps;
		Grafkit::SceneResRef m_scene;
		Grafkit::SceneGraphRef m_scenegraph;

		Grafkit::ShaderResRef m_psShowUv;
		Grafkit::EffectComposerRef m_effect;

		Grafkit::ActorRef m_rootActor;
		Grafkit::ActorRef m_cameraActor;
	};

}
