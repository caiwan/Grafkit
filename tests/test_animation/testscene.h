#pragma once 

#include "render/mesh.h"
#include "render/model.h"
#include "render/material.h"

#include "render/SceneGraph.h"

#include "builtin_data/cube.h"

#include "animation/scene.h"

using namespace Grafkit;

class TestScene {

public:
	static void createTestScene(SceneResRef &scene) {

		// -- model 
		ModelRef model = new Model(new Mesh());
		model->SetMaterial(new Material());
		//model->GetMaterial()->AddTexture(texture, Material::TT_diffuse);
		model->GetMaterial()->SetName("GridMaterial");

		// -- camera
		CameraRef camera = new Camera();
		camera->SetName("Camera");

		ActorRef cameraActor = new Actor(camera);
		cameraActor->SetName("CameraActor");

		cameraActor->Matrix().Identity();
		//cameraActor->Matrix().LookAtLH(float3(0, 0, 10));
		cameraActor->Matrix().Translate(float3(0, 0, 10));
		//m_cameraActor = cameraActor;

		model->SetName("cube");
		model->GetMesh()->AddPointer("POSITION", GrafkitData::cubeVertexSize, GrafkitData::cubeVertices);
		model->GetMesh()->AddPointer("TEXCOORD", GrafkitData::cubeVertexSize, GrafkitData::cubeTextureUVs);
		model->GetMesh()->AddPointer("NORMAL", GrafkitData::cubeVertexSize, GrafkitData::cubeNormals);
		model->GetMesh()->SetIndices(GrafkitData::cubeVertexCount, GrafkitData::cubeIndicesCount, GrafkitData::cubeIndices);
		//model->GetMesh()->Build(render, m_vs); // a scene buildel magatol 

		// -- actors

		ActorRef cubeActor = new Actor(model);
		cubeActor->SetName("CubeActor");

		ActorRef rootActor = new Actor();
		rootActor->SetName("RootActor");

		rootActor->AddChild(cameraActor);
		rootActor->AddChild(cubeActor);

		SceneGraphRef scenegraph = new SceneGraph();
		scenegraph->SetRootNode(rootActor);

		scene->Get()->SetSceneGraph(scenegraph);

	}

};
