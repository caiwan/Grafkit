#include "effect.h"

using namespace Grafkit;

Grafkit::EffectComposer::EffectComposer():
	m_pTexRead(nullptr), m_pTexWrite(nullptr), m_pTexBack(nullptr)
{
}

Grafkit::EffectComposer::~EffectComposer()
{
	this->Shutdown();
}

/// @tod ezeket valami elegansabb helyre el kellene rakni
namespace {
	const char shader_source [] = "\r\n"

	// GLOBALS //
	"Texture2D effectInput;\r\n"
	"SamplerState SampleType;"

	// TYPEDEFS //
	"struct PixelInputType"
	"{"
		"float4 position : SV_POSITION;"
		"float2 tex : TEXCOORD;"
	"};"

	"struct VertexInputType"
	"{"
		"float4 position : POSITION;"
		"float2 tex : TEXCOORD;"
	"};"

	// VertexShader
	"PixelInputType FullscreenQuad(VertexInputType input)"
	"{"
		"PixelInputType output;"
		"input.position.w = 1.0f;"
		"output.position = input.position;"
		"output.tex = input.tex;"
		"return output;"
	"}"

	// PixelShader
	"float4 CopyScreen(PixelInputType input) : SV_TARGET"
	"{"
		"float4 textureColor = float4(0,0,0,1);"
		"textureColor = effectInput.Sample(SampleType, input.tex);"
		"return textureColor;"
	"}"

	"";
}


#include "../builtin_data/cube.h"

// ---------------------------------------------------------------------------------------------------
void Grafkit::EffectComposer::Initialize(Renderer & render)
{
	m_texOut[0] = new Texture(); m_texOut[0]->Initialize(render); m_pTexBack = m_texOut[0];
	m_texOut[1] = new Texture(); m_texOut[1]->Initialize(render); m_pTexRead = m_texOut[1];
	m_texOut[2] = new Texture(); m_texOut[2]->Initialize(render); m_pTexWrite = m_texOut[2];

	// --- 
	m_shaderFullscreenQuad = new Shader();
	m_shaderFullscreenQuad->LoadFromMemory(render, "FullscreenQuad", shader_source, sizeof(shader_source), ST_Vertex);

	m_shaderCopyScreen = new Shader();
	m_shaderCopyScreen->LoadFromMemory(render, "CopyScreen", shader_source, sizeof(shader_source), ST_Pixel);

	// --- 
	SimpleMeshGenerator meshGen(render, m_shaderFullscreenQuad);

	meshGen["POSITION"] = GrafkitData::quad;
	meshGen["TEXCOORD"] = GrafkitData::quad_texcoord;
	m_fullscreenquad = meshGen(4, 6, GrafkitData::quadIndices);

	// -- 
	m_textureSampler = new TextureSampler();
	m_textureSampler->Initialize(render);
}

void Grafkit::EffectComposer::Shutdown()
{
	// nothing to do 
	// a refcounter elviekben megsemmisit mindent
}

// ---------------------------------------------------------------------------------------------------
void Grafkit::EffectComposer::BindInput(Renderer & render)
{
	render.SetRenderTargetView(m_pTexRead->GetRenderTargetView());
}

// ---------------------------------------------------------------------------------------------------
void Grafkit::EffectComposer::Render(Renderer & render)
{
	// a render passban a write bufferbe irutnk elozoleg, igy meg kell cserelni a chain render elott
	this->SwapBuffers();

	for (fx_chain_it_t it = this->m_effectChain.begin(); it != this->m_effectChain.end(); it++) {
		this->m_pTexWrite->SetRenderTargetView(render);
		render.BeginScene(); 
		{
			if ((*it)->GetShader().Valid()) {
				(*it)->GetShader()->GetBRes("backBuffer") = m_pTexBack->GetTextureResource();
				(*it)->GetShader()->GetBRes("effectInput") = m_pTexRead->GetTextureResource();
				(*it)->GetShader()->GetBRes("SampleType") = m_textureSampler->GetSamplerState();
				(*it)->Render(render);
			}

			m_fullscreenquad->RenderMesh(render);
		}

		render.EndScene();
		this->SwapBuffers();
	}

	// present the result 
	render.SetRenderTargetView();

	m_shaderFullscreenQuad->Render(render);
	m_shaderCopyScreen->Render(render);
	m_shaderCopyScreen->GetBRes("effectInput") = m_pTexWrite->GetTextureResource();

	m_fullscreenquad->RenderMesh(render);

	this->FlushBuffers();
}

// ---------------------------------------------------------------------------------------------------
void Grafkit::EffectComposer::SwapBuffers()
{
	Texture* tmp = m_pTexWrite;
	m_pTexWrite = m_pTexRead;
	m_pTexRead = tmp;
}

void Grafkit::EffectComposer::FlushBuffers()
{
	Texture* tmp = m_pTexBack;
	m_pTexBack = m_pTexWrite;
	m_pTexWrite = tmp;
}

// ===================================================================================================

Grafkit::EffectPass::EffectPass()
{
	// nothing to do
}

Grafkit::EffectPass::~EffectPass()
{
	this->Shutdown();
}

// ---------------------------------------------------------------------------------------------------

void Grafkit::EffectPass::Initialize(Renderer &render, ShaderRef shader)
{
	m_shader = shader;
}

void Grafkit::EffectPass::Shutdown()
{
	// nothing to do
}

// ---------------------------------------------------------------------------------------------------

void Grafkit::EffectPass::Render(Renderer & render)
{
	m_shader->Render(render);
}
