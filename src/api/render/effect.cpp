#include "../stdafx.h"

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
	size_t count = 1;
	render.SetRenderTargetView(m_pTexRead->GetRenderTargetView(), 0);

	for (auto it = m_input_map.begin(); it != m_input_map.end(); it++) {
		if (it->second.Valid()) {
			it->second->SetRenderTargetView(render, it->first);
			count++;
		}
	}

	render.ApplyRenderTargetView(count);
}

// ---------------------------------------------------------------------------------------------------
void Grafkit::EffectComposer::Render(Renderer & render, int autoflush)
{
	RenderChain(render);
	if (autoflush)
		this->Flush(render);
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

void Grafkit::EffectComposer::RenderChain(Renderer & render)
{
	///@todo a kozbenso effektek inputjait is bindelje ossze

	m_shaderFullscreenQuad->Render(render);

	for (size_t i = 0; i < m_effectChain.size(); i++)
	{
		EffectPass *fx = m_effectChain[i].Get();
		this->m_pTexWrite->SetRenderTargetView(render);
		render.ApplyRenderTargetView(1);
		render.BeginScene();
		{
			if (fx && fx->GetShader().Valid()) {
				fx->GetShader()->GetBRes("backBuffer").Set(m_pTexBack->GetTextureResource());
				fx->GetShader()->GetBRes("effectInput").Set(m_pTexRead->GetTextureResource());
				fx->GetShader()->GetBRes("SampleType").Set(m_textureSampler->GetSamplerState());
				fx->Render(render);
			}

			m_fullscreenquad->RenderMesh(render);
		}

		this->SwapBuffers();
	}
}

void Grafkit::EffectComposer::Flush(Renderer & render)
{
	// present the result 
	render.SetRenderTargetView();
	//render.ApplyRenderTargetView(1);
	render.BeginScene();

	m_shaderFullscreenQuad->Render(render);

	m_shaderCopyScreen->GetBRes("effectInput").Set(m_pTexRead->GetTextureResource());
	m_shaderCopyScreen->Render(render);

	m_fullscreenquad->RenderMesh(render);

	this->FlushBuffers();
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

size_t Grafkit::EffectPass::BindOutputs(Renderer &render)
{
	size_t count = 0;
	for (auto it = m_output_map.begin(); it != m_output_map.end(); it++) {
		if (it->second.Valid()) {
			it->second->SetRenderTargetView(render, it->first);
			count++;
		}
	}
	return count;
}

void Grafkit::EffectPass::Render(Renderer & render)
{
	for (auto it = m_input_map.begin(); it != m_input_map.end(); it++) {
		m_shader->GetBRes(it->first).Set(it->second->GetTextureResource());
	}
	m_shader->Render(render);
}

TextureRef Grafkit::EffectPass::GetOutput(size_t bind)
{
	auto it = m_output_map.find(bind);
	return it == m_output_map.end() ? TextureRef() : it->second;
}

TextureRef Grafkit::EffectPass::GetInput(std::string name)
{
	auto it = m_input_map.find(name);
	return it == m_input_map.end() ? TextureRef() : it->second;
}
