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
	class SceneGraph;
	typedef Ref<SceneGraph> SceneRef;

	typedef Resource<SceneGraph> SceneRes;
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
	class Texture1D;
	class Texture2D;
	class TextureCube;

	typedef Ref<Texture1D> Texture1DRef;
	typedef Resource<Texture1D> Texture1DRes;
	typedef Ref<Texture1DRes> Texture1DResRef;

	typedef Ref<Texture2D> Texture2DRef;
	typedef Resource<Texture2D> Texture2DRes;
	typedef Ref<Texture2DRes> Texture2DResRef;

	typedef Ref<Texture2D> TextureRef;
	typedef Ref<Texture2DRes> TextureResRef;

	typedef Ref<TextureCube> TextureCubeRef;
	typedef Resource<TextureCube> TextureCubeRes;
	typedef Ref<TextureCubeRes> TextureCubeResRef;

	class TextureSampler;

	typedef Ref<TextureSampler> TextureSamplerRef;
	typedef Grafkit::Resource<TextureSampler> TextureSamplerRes;
	typedef Ref<TextureSamplerRes> TextureSamplerResRef;

}
