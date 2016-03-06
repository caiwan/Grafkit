#include "Scene.h"
#include "Actor.h"
#include "camera.h"

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
	PreRender(render);

	// + kamerat + fenyket at kell tudni adni valahol meg

	// render scenegraph
	RenderNode(render, m_pScenegraph);
}

void FWrender::Scene::PreRender(FWrender::Renderer & render)
{
	// fogja a camera nodeot
	// kiszamolja a framere vonatkozoan a koordinatat
	// beseteli a cameraRef-be
	// orul

	// ugyanezt a fenyekre

	// minden nodeot prerendererel, ha kell;
}

void FWrender::Scene::RenderNode(FWrender::Renderer & render, Actor * actor, int maxdepth)
{
	if (maxdepth < 0) return;
	if (!actor) return;

	m_cureentViewMatrix.Multiply(actor->Matrix());

	// setup camera goez here

	actor->Render(render);	///todo CAMERA + FENYEK paramter atadasa esetleg ??
	push();
	
	for (size_t i = 0; i < actor->m_pChildren.size(); i++) {
		RenderNode(render, actor->m_pChildren[i], maxdepth - 1);
	}
	
	pop();
}

void FWrender::Scene::push()
{
	this->m_viewMatrixStack.push(m_cureentViewMatrix);
}

void FWrender::Scene::pop()
{
	m_cureentViewMatrix = this->m_viewMatrixStack.top();
	this->m_viewMatrixStack.pop();
}
