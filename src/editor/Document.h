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
	class CurveDocument;

	class TreeModel;
	class SceneGraphModel;

	class Document
	{
		friend class Editor;
	public:
		Document();
		virtual ~Document();

		//Ref<Grafkit::Animation::FloatTrack> track() { return m_testAnimation; }

		void Preload(Grafkit::IResourceManager * const & resman);
		void Initialize(Grafkit::Renderer &render);
		void Shutdown();

		void SetDirty() { m_isDirty = true; }
		bool IsDirty() { return m_isDirty; }

		//
		TreeModel * GetOutlineModel();

	private:
		void InitTestStuff(Grafkit::Renderer & render);

		bool m_isDirty;

		//Ref<Grafkit::Animation::FloatTrack> m_testAnimation;

		Grafkit::ShaderResRef m_vs, m_ps;
		Grafkit::SceneResRef m_scenegraph;

		Grafkit::ShaderResRef m_psShowUv;
		Grafkit::EffectComposerRef m_effect;

		Grafkit::ActorRef m_rootActor;
		Grafkit::ActorRef m_cameraActor;

		Idogep::SceneGraphModel *m_outlineViewModel;


	};

}
