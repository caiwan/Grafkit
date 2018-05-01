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

	class Document : virtual public Referencable
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

	    Grafkit::SceneGraphRef GetScenegraph() const { return m_scenegraph; }
		Grafkit::SceneResRef GetScene() const { return m_scene; }

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
