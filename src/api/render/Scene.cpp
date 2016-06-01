#include "../stdafx.h"

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
}

void Grafkit::Scene::Render(Grafkit::Renderer & render)
{
	m_currentWorldMatrix.Identity();
	
	// + kamerat + fenyket at kell tudni adni valahol meg

	struct {
		matrix worldMatrix;
		matrix viewMatrix;
		matrix projectionMatrix;
	} viewMatrices;

	CameraRef &camera = GetCamera();
	camera->Calculate(render);

	viewMatrices.worldMatrix = XMMatrixTranspose(m_currentWorldMatrix.Get());
	viewMatrices.viewMatrix = XMMatrixTranspose(camera->GetViewMatrix().Get());
	viewMatrices.projectionMatrix = XMMatrixTranspose(camera->GetProjectionMatrix().Get());

	//((Shader)(*m_vertexShader))["MatrixBuffer"] = &viewMatrices;
	m_vertexShader->GetParam("MatrixBuffer").SetP(&viewMatrices);

	//ez itt elviekben jo kell, hogy legyen
	m_vertexShader->Render(render);
	m_fragmentShader->Render(render);

	// render scenegraph
	RenderNode(render, m_pScenegraph);
}

void Grafkit::Scene::SetCameraNode(ActorRef & camera)
{
	m_pCameraNode = camera;
}

void Grafkit::Scene::AddLightNode(ActorRef & light)
{
	m_pvLightNodes.push_back(light);
}


void Grafkit::Scene::PreRender(Grafkit::Renderer & render)
{
	// fogja a camera nodeot
	// kiszamolja a framere vonatkozoan a koordinatat
	// beseteli a cameraRef-be
	// orul

	// ugyanezt a fenyekre
	for (size_t i = 0; i < m_pvLightNodes.size(); i++) {
		// ... 
	}

	// minden nodeot prerendererel, ha kell;
}

void Grafkit::Scene::RenderNode(Grafkit::Renderer & render, Actor * actor, int maxdepth)
{
	if (maxdepth < 0) return;
	if (!actor) return;

	push();

	m_currentWorldMatrix.Multiply(actor->Matrix());
	m_currentWorldMatrix.Multiply(actor->Transform());
	matrix viewMatrix = XMMatrixTranspose(m_currentWorldMatrix.Get());
	//((Shader)(*m_vertexShader))["MatrixBuffer"]["worldMatrix"] = viewMatrix;
	m_vertexShader->GetParam("MatrixBuffer").Get("worldMatrix").SetP(&viewMatrix);

	actor->Render(render, this);
	
	//push();
	
	for (size_t i = 0; i < actor->m_pChildren.size(); i++) {
		RenderNode(render, actor->m_pChildren[i].Get(), maxdepth - 1);
	}
	
	pop();
}

void Grafkit::Scene::push()
{
	this->m_worldMatrixStack.push(m_currentWorldMatrix);
}

void Grafkit::Scene::pop()
{
	m_currentWorldMatrix = this->m_worldMatrixStack.top();
	this->m_worldMatrixStack.pop();
}
