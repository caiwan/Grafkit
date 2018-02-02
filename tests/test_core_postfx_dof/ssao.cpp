#include "ssao.h"

#include "generator/ShaderLoader.h"
#include "generator/TextureLoader.h"

#include "math/matrix.h"
#include "math/halton.h"

using namespace Grafkit;
using namespace GKDemo;

SsaoEffect::SsaoEffect()
{
}

SsaoEffect::~SsaoEffect()
{
}

void SsaoEffect::Shutdown()
{
	delete[] kernels;
	kernels = nullptr;
}

void SsaoEffect::OnBeforePreload(Grafkit::Renderer & render, GKDemo::DemoApplication * const & context, Grafkit::IResourceManager * const & resman)
{
	fsAo = resman->Load<ShaderRes>(new PixelShaderLoader("fxSSAOShader", "shaders/ssao.hlsl", "SSAO"));
	fsBlur = resman->Load<ShaderRes>(new PixelShaderLoader("fxSSAOSmooth", "shaders/ssao.hlsl", "SSAOSmooth"));

	texNoiseMap = resman->Load<Texture2DRes>(new TextureNoiseMap("Noise256", 256));
}

void SsaoEffect::OnAfterPreload(Grafkit::Renderer & render, GKDemo::DemoApplication * const & context)
{
	kernels = new float4[128];
	Halton3D::HemiSphereDistribution(kernels, 128);


}
