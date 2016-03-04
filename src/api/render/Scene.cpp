#include "Scenegraph.h"
#include "Actor.h"

using FWrender::Actor;
using FWrender::Entity3D;

FWrender::Scene::Scene():
	m_pScenegraph(nullptr)
{
}


FWrender::Scene::~Scene()
{
	delete m_pScenegraph;
}


void FWrender::Scene::Render(FWrender::Renderer &render)
{
	// prerender process goez here

	// + kamerat + fenyket at kell tudni adni valahol meg

	// render scenegraph
	m_pScenegraph->Render(render);
}