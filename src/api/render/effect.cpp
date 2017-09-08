#include "stdafx.h"

#include "effect.h"

using namespace Grafkit;

Grafkit::EffectComposer::EffectComposer() :
	m_pTexRead(nullptr), m_pTexWrite(nullptr), m_pTexBack(nullptr), m_singlepass(false)
{
}

Grafkit::EffectComposer::~EffectComposer()
{
	this->Shutdown();
}

/// @tod ezeket valami elegansabb helyre el kellene rakni
namespace {
	const char shader_source[] = "\r\n"

		// GLOBALS //
		"Texture2D effectInput;\r\n"

		"SamplerState SampleType{\n"
		"Filter = MIN_MAG_MIP_LINEAR;\n"
		"AddressU = Wrap;\n"
		"AddressV = Wrap;\n"
		"};\n"

		"cbuffer EffectParams{\n"
		"float4 fxScreen;"
		"float4 fxViewport;"
		"}\n"

		// TYPEDEFS //
		"struct FXPixelInputType"
		"{"
		"float4 position : SV_POSITION;"
		"float2 tex : TEXCOORD;"
		"};"

		"struct FXVertexInputType"
		"{"
		"float4 position : POSITION;"
		"float2 tex : TEXCOORD0;"
		"};"

		// VertexShader
		"FXPixelInputType FullscreenQuad(FXVertexInputType input)"
		"{"
		"FXPixelInputType output;"
		"input.position.w = 1.0f;"
		"output.position = input.position;"
		"output.tex = input.tex;"
		"return output;"
		"}"

		// PixelShader
		"float4 CopyScreen(FXPixelInputType input) : SV_TARGET"
		"{"
		"float4 textureColor = float4(0,0,0,1);"
		"textureColor = effectInput.Sample(SampleType, input.position / fxScreen.xy);"
		"return textureColor;"
		"}"

		"";
}


#include "../builtin_data/cube.h"

// ---------------------------------------------------------------------------------------------------
void Grafkit::EffectComposer::Initialize(Renderer & render, bool singlepass)
{
	m_singlepass = singlepass;
	LOGGER(Log::Logger().Trace("Initializing FX Chain"));

	if (!m_singlepass) {
		m_texOut[0] = new Texture2D(); m_texOut[0]->Initialize(render); m_pTexFront = m_texOut[0];
		m_texOut[1] = new Texture2D(); m_texOut[0]->Initialize(render); m_pTexBack = m_texOut[1];
		m_texOut[2] = new Texture2D(); m_texOut[1]->Initialize(render); m_pTexRead = m_texOut[2];
		m_texOut[3] = new Texture2D(); m_texOut[2]->Initialize(render); m_pTexWrite = m_texOut[3];
	}
	// --- 
	m_shaderFullscreenQuad = new VertexShader();
	m_shaderFullscreenQuad->LoadFromMemory(render, "FullscreenQuad", shader_source, sizeof(shader_source), "FullscreenQuad");

	m_shaderCopyScreen = new PixelShader();
	m_shaderCopyScreen->LoadFromMemory(render, "CopyScreen", shader_source, sizeof(shader_source), "CopyScreen");

	m_screen_params.s = float4(0, 0, 0, 0);
	m_screen_params.v = float4(0, 0, 0, 0);
	render.GetScreenSizef(m_screen_params.s.x, m_screen_params.s.y);
	render.GetViewportSizef(m_screen_params.v.x, m_screen_params.v.y);
	m_screen_params.v.z = render.GetAspectRatio();

	if (!m_singlepass) {
		m_shaderCopyScreen->SetParam(render, "EffectParams", &m_screen_params);
	}

	// --- 
	m_fullscreenquad = new Mesh();
	m_fullscreenquad->AddPointer("POSITION", sizeof(GrafkitData::quad[0]) * 4 * 4, GrafkitData::quad);
	m_fullscreenquad->AddPointer("TEXCOORD", sizeof(GrafkitData::quad_texcoord[0]) * 4 * 4, GrafkitData::quad_texcoord);
	m_fullscreenquad->SetIndices(4, 6, GrafkitData::quadIndices);
	m_fullscreenquad->Build(render, m_shaderFullscreenQuad);

	// -- 
	m_textureSampler = new TextureSampler();
	m_textureSampler->Initialize(render);

	for (auto it = m_effectChain.begin(); it != m_effectChain.end(); ++it) {
		(*it)->Initialize(render);
	}

	LOGGER(Log::Logger().Trace("FX Chain OK"));
}

void Grafkit::EffectComposer::Shutdown()
{
	// nothing to do 
	// a refcounter elviekben megsemmisit mindent
}

// ---------------------------------------------------------------------------------------------------
void Grafkit::EffectComposer::BindInput(Renderer & render)
{
	if (m_singlepass)
		return;

	size_t count = 1;
	render.SetRenderTargetView(m_pTexFront->GetRenderTargetView(), 0);

	for (auto it = m_input_map.begin(); it != m_input_map.end(); it++) {
		if (it->second.Valid()) {
			it->second->SetRenderTargetView(render, it->first);
			count++;
		}
	}

	render.ApplyRenderTargetView(count);
}

void Grafkit::EffectComposer::UnbindInput(Renderer & render)
{
	if (m_singlepass)
		return;

	size_t count = 1;
	render.SetRenderTargetView(nullptr, 0);

	for (auto it = m_input_map.begin(); it != m_input_map.end(); it++) {
		if (it->second.Valid()) {
			render.SetRenderTargetView(nullptr, it->first);
			count++;
		}
	}

	render.ApplyRenderTargetView(1);
}

// ---------------------------------------------------------------------------------------------------
void Grafkit::EffectComposer::Render(Renderer & render, int autoflush)
{
	if (m_singlepass)
		return RenderChain(render);

	UnbindInput(render);
	RenderChain(render);
	if (autoflush)
		this->Flush(render);
}

// ---------------------------------------------------------------------------------------------------
void Grafkit::EffectComposer::SwapBuffers()
{
	if (m_singlepass)
		return;

	Texture2D* tmp = m_pTexWrite;
	m_pTexWrite = m_pTexRead;
	m_pTexRead = tmp;
}

void Grafkit::EffectComposer::FlushBuffers()
{
	if (m_singlepass)
		return;

	Texture2D* tmp = m_pTexBack;
	m_pTexBack = m_pTexWrite;
	m_pTexWrite = tmp;

	tmp = m_pTexFront;
	m_pTexBack = m_pTexFront;
	m_pTexFront = tmp;
}

void Grafkit::EffectComposer::RenderChain(Renderer & render)
{
	///@todo a kozbenso effektek inputjait is bindelje ossze
	m_shaderFullscreenQuad->Bind(render);

	size_t fxcount = m_singlepass ? 1 : m_effectChain.size();

	for (size_t i = 0; i < fxcount; i++)
	{
		EffectPass *fx = m_effectChain[i].Get();
		if (!m_singlepass) {
			this->m_pTexWrite->SetRenderTargetView(render);
			render.ApplyRenderTargetView(1);
			render.BeginScene();
		}
		{
			if (fx && fx->GetShader().Valid()) {
				if (!m_singlepass) {
					fx->GetShader()->SetShaderResourceView(render, "backBuffer", m_pTexBack->GetShaderResourceView());
					fx->GetShader()->SetShaderResourceView(render, "frontBuffer", m_pTexFront->GetShaderResourceView());
					if (fxcount == 0)
						fx->GetShader()->SetShaderResourceView(render, "effectInput", m_pTexFront->GetShaderResourceView());
					else
						fx->GetShader()->SetShaderResourceView(render, "effectInput", m_pTexRead->GetShaderResourceView());
				}
				fx->GetShader()->SetSamplerSatate(render, "SampleType", m_textureSampler->GetSamplerState());
				fx->GetShader()->SetParam(render, "EffectParams", &m_screen_params);
				fx->BindFx(render);
			}

			m_fullscreenquad->RenderMesh(render);

			if (fx && fx->GetShader().Valid()) {
				fx->UnbindFx(render);
				//fx->UnbindOutputs(render);
			}
		}

		this->SwapBuffers();
	}

	m_shaderFullscreenQuad->Unbind(render);
}

void Grafkit::EffectComposer::Flush(Renderer & render)
{
	// present the result 
	if (m_singlepass)
		return;

	render.SetRenderTargetView();
	render.BeginScene();

	m_shaderFullscreenQuad->Bind(render);

	m_shaderCopyScreen->SetShaderResourceView(render, "effectInput", m_pTexRead->GetShaderResourceView());

	m_shaderCopyScreen->Bind(render);

	m_fullscreenquad->RenderMesh(render);

	m_shaderCopyScreen->Unbind(render);

	m_shaderFullscreenQuad->Unbind(render);

	this->FlushBuffers();
}

// ===================================================================================================

Grafkit::EffectPass::EffectPass(ShaderResRef shader) : m_shader(shader)
{
}

Grafkit::EffectPass::~EffectPass()
{
	this->Shutdown();
}

// ---------------------------------------------------------------------------------------------------

void Grafkit::EffectPass::Initialize(Renderer & render)
{
	// ... 
	LOGGER(Log::Logger().Trace("FX Init pass %s", m_shader->GetName().c_str()));
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

size_t Grafkit::EffectPass::UnbindOutputs(Renderer & render)
{
	size_t count = 0;
	for (auto it = m_output_map.begin(); it != m_output_map.end(); it++) {
		if (it->second.Valid()) {
			render.SetRenderTargetView(nullptr, it->first);
			count++;
		}
	}
	return count;
}

void Grafkit::EffectPass::BindFx(Renderer & render)
{
	for (auto it = m_input_map.begin(); it != m_input_map.end(); it++) {
		m_shader->Get()->SetShaderResourceView(render, it->first, it->second->GetShaderResourceView());
	}
	m_shader->Get()->Bind(render);
}

void Grafkit::EffectPass::UnbindFx(Renderer & render)
{
	for (auto it = m_input_map.begin(); it != m_input_map.end(); it++) {
		m_shader->Get()->SetShaderResourceView(render, it->first, nullptr);
	}
	m_shader->Get()->Unbind(render);
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
