#pragma once 

#include"utils/reference.h"
#include"utils/resource.h"

// predefine classes to reduce compile time 

namespace Grafkit {

	class Shader;
	
	class Vertexshader;
	class PixelShader;
	class GemoetryShader;
	class ComputeShader;

	typedef Ref<Shader> ShaderRef;
	typedef Resource<Shader> ShaderRes;
	typedef Ref<ShaderRes> ShaderResRef;

	// ---
	class Scene;
	typedef Ref<Scene> SceneRef;
	
	typedef Resource<Scene> SceneRes;
	typedef Ref<SceneRes> SceneResRef;

	class Animation;
	typedef Ref<Animation> AnimationRef;

	// --- 

	class Mesh;
	typedef Ref<Mesh> MeshRef;

	class Material;
	typedef Ref<Material> MaterialRef;

	class Model;
	typedef Ref<Model> ModelRef;

	class Actor;
	typedef Ref<Actor> ActorRef;

	class Entity3D;
	typedef Ref<Entity3D> Entity3DRef;
	
	class Camera;
	typedef Ref<Camera> CameraRef;

	class Light;
	typedef Ref<Light> LightRef;

	// ---
	class Bitmap;
	class Texture;

	typedef Ref<Texture> TextureRef;
	typedef Resource<Texture> TextureRes;
	typedef Ref<TextureRes> TextureResRef;

	class TextureSampler;

	typedef Ref<TextureSampler> TextureSamplerRef;
	typedef Grafkit::Resource<TextureSampler> TextureSamplerResource;
	typedef Ref<TextureSamplerResource> TextureSamplerResourceRef;

}
