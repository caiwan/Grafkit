#include "demoframework.h"

#include "sceneelem.h"

using namespace GKDemo;
using namespace Grafkit;

GKDemo::DemoApplication::DemoApplication() :
	ClonableInitializer(),
	System(),
	ResourcePreloader(),
	m_loader(nullptr)
{
}

GKDemo::DemoApplication::~DemoApplication()
{
}

int GKDemo::DemoApplication::init()
{
	InitializeLoaderBar(m_render);
	LoadCache();

	float phaseProgress = 0.;
	const float phaseProgressStep = m_scenes.empty() ? 1. : 1. / (float)m_scenes.size();

	// 
	OnBeforePreload();

	phaseProgress = 0.;
	m_activeLoadPhase = Phase_BeforePreload;
	for (auto it = m_scenes.begin(); it != m_scenes.end(); it++) {
		SceneElem *elem = dynamic_cast<SceneElem*>(it->Get());

		elem->OnBeforePreload(m_render, this, this);

		UpdateLoaderBar(phaseProgress);
		phaseProgress += phaseProgressStep;
	}

	//
	DoPrecalc();

	//
	OnAfterPreload();

	m_activeLoadPhase = Phase_AfterPreload;
	for (auto it = m_scenes.begin(); it != m_scenes.end(); it++) {
		SceneElem *elem = dynamic_cast<SceneElem*>(it->Get());

		elem->OnAfterPreload(m_render, this);

		UpdateLoaderBar(phaseProgress);
		phaseProgress += phaseProgressStep;

	}

	UpdateLoaderBar(1.);

	return 0;
}

void GKDemo::DemoApplication::release()
{
	SaveCache();
	RemoveAll();

	for (auto it = m_scenes.begin(); it != m_scenes.end(); it++) {
		SceneElem *elem = dynamic_cast<SceneElem*>(it->Get());
		elem->Shutdown();
	}

	Shutdown();
}


// ------------------------------------------------------

void GKDemo::DemoApplication::UpdateLoaderBar(float p)
{
	const float pf = 1. / (float)Phase_COUNT;;
	const float pk = pf * (p  * pf + (float)m_activeLoadPhase);
	DrawLoaderBar(m_render, pk);
}
