#include "../stdafx.h"

#include "Scene.h"
#include "Actor.h"
#include "camera.h"

using Grafkit::Actor;
using Grafkit::Entity3D;

using namespace FWdebugExceptions;

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
	
	// ezt a semat ki kell baszni innen 
	struct {
		matrix worldMatrix;
		matrix viewMatrix;
		matrix projectionMatrix;
	} viewMatrices;

	// CameraRef &camera = GetActiveCamera();
	// camera->Calculate(render);

	viewMatrices.worldMatrix = XMMatrixTranspose(m_currentWorldMatrix.Get());
	
	viewMatrices.viewMatrix = XMMatrixTranspose(m_cameraViewMatrix.Get());
	viewMatrices.projectionMatrix = XMMatrixTranspose(m_cameraProjectionMatrix.Get());

	m_vertexShader->GetParam("MatrixBuffer").SetP(&viewMatrices);

	//ez itt elviekben jo kell, hogy legyen
	m_vertexShader->Render(render);
	m_fragmentShader->Render(render);

	// render scenegraph
	RenderNode(render, m_pScenegraph);
}

void Grafkit::Scene::AddCameraNode(ActorRef camera)
{
	m_cameraNodes.push_back(camera);
	SetActiveCamera();
}

void Grafkit::Scene::AddLightNode(ActorRef light)
{
	m_lightNodes.push_back(light);
}


void Grafkit::Scene::PreRender(Grafkit::Renderer & render)
{
	// --- kamera
	ActorRef &cameraActor = GetActiveCamera();
	if (cameraActor.Valid() && (!cameraActor->GetEntities().empty() && cameraActor->GetEntities()[0].Valid())) {
		// itt csak az elso entitast vesszuk figyelembe
		Camera * camera = dynamic_cast<Camera *>(cameraActor->GetEntities()[0].Get());
		if (camera) {
			camera->Calculate(render);
			m_cameraProjectionMatrix = camera->GetProjectionMatrix();

			// m_cameraViewMatrix = camera->GetViewMatrix();
			m_cameraViewMatrix = CalcNodeTransformTree(cameraActor, camera->GetViewMatrix());
		}
		else {
			throw EX(NullPointerException);
		}
	}
	else {
		throw EX_DETAILS(NullPointerException, "Camera actor nem jo, vagy Nem seteltel be a nodeba kamerat");
	}
	
	// --- fenyek
	// ugyanezt a fenyekre
	for (size_t i = 0; i < m_lightNodes.size(); i++) {
		// ... 
	}

	return;

	// minden nodeot prerendererel, ha kell;
	// ... 
}

void Grafkit::Scene::RenderNode(Grafkit::Renderer & render, Actor * actor, int maxdepth)
{
	if (maxdepth < 0) return;
	if (!actor) return;

	push();

	m_currentWorldMatrix.Multiply(actor->Matrix());
	m_currentWorldMatrix.Multiply(actor->Transform());
	matrix worldMatrix = XMMatrixTranspose(m_currentWorldMatrix.Get());
	m_vertexShader->GetParam("MatrixBuffer").Get("worldMatrix").SetP(&worldMatrix);

	actor->Render(render, this);
	
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

Grafkit::Matrix Grafkit::Scene::CalcNodeTransformTree(ActorRef & actor, Grafkit::Matrix & matrix)
{
	ActorRef & node = actor;

	Matrix result;// = matrix;

	do {
		Matrix mat;
#if 0
		MAtrix matTM = actor->Transform();
		matTM.Multiply(actor->Matrix());
#else
		Matrix matTM = actor->Matrix();
		matTM.Multiply(actor->Transform());
#endif
		matTM.Invert();
		mat.Identity();
#if 0
		mat = matTM;
		mat.Multiply(result);

		result = mat;
#else 
		mat.Multiply(result);
		mat.Multiply(matTM);
		
#endif

		result = mat;
		

		node = node->GetParent();
	} while (node.Valid());

	result.Multiply(matrix);

	return result;
}

