#include "Scene.h"
#include "Actor.h"
#include "camera.h"

using Grafkit::Actor;
using Grafkit::Entity3D;

Grafkit::Scene::Scene():
	m_pScenegraph(nullptr)
{
}


Grafkit::Scene::~Scene()
{
	delete m_pScenegraph;
}


void Grafkit::Scene::Render(Grafkit::Renderer &render)
{
	PreRender(render);

	// + kamerat + fenyket at kell tudni adni valahol meg

	// render scenegraph
	RenderNode(render, m_pScenegraph);
}

void Grafkit::Scene::PreRender(Grafkit::Renderer & render)
{
	// fogja a camera nodeot
	// kiszamolja a framere vonatkozoan a koordinatat
	// beseteli a cameraRef-be
	// orul

	// ugyanezt a fenyekre

	// minden nodeot prerendererel, ha kell;
}

void Grafkit::Scene::RenderNode(Grafkit::Renderer & render, Actor * actor, int maxdepth)
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

void Grafkit::Scene::push()
{
	this->m_viewMatrixStack.push(m_cureentViewMatrix);
}

void Grafkit::Scene::pop()
{
	m_cureentViewMatrix = this->m_viewMatrixStack.top();
	this->m_viewMatrixStack.pop();
}
