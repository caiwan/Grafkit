#include "stdafx.h"

#include "effect.h"
#include "mesh.h"

#include "shaderparameter.h"

using namespace Grafkit;

EffectComposer::EffectComposer() :
	m_pTexRead(nullptr), m_pTexWrite(nullptr), m_pTexBack(nullptr), m_singlepass(false)
{
}

EffectComposer::~EffectComposer()
{
	this->Shutdown();
}

#include "../builtin_data/cube.h"
#include "../builtin_data/defaultShader.h"

using namespace GrafkitData;

// ---------------------------------------------------------------------------------------------------
void EffectComposer::Initialize(Renderer & render, bool singlepass)
{
	m_singlepass = singlepass;
	LOGGER(Log::Logger().Trace("Initializing FX Chain"));

	if (!m_singlepass) {
		for (int i = 0; i < 4; i++) {
			m_texOut[i] = new Texture2D();
			m_texOut[i]->Initialize(render);
		}

		m_pTexFront = m_texOut[0];
		m_pTexBack = m_texOut[1];
		m_pTexRead = m_texOut[2];
		m_pTexWrite = m_texOut[3];

	}

	// -- 
	m_textureSampler = new TextureSampler();
	m_textureSampler->Initialize(render);

	// --- 
	m_shaderFullscreenQuad = new VertexShader();
	m_shaderFullscreenQuad->LoadFromMemory(
		render,
		effectFullscreenQuadEntry,
		effectShader, strlen(effectShader),
		"FullscreenQuad"
	);

	m_shaderCopyScreen = new PixelShader();
	m_shaderCopyScreen->LoadFromMemory(
		render,
		effectCopyScreenEntry,
		effectShader, strlen(effectShader),
		"CopyScreen"
	);

	m_screen_params.s = float4(0, 0, 0, 0);
	m_screen_params.v = float4(0, 0, 0, 0);
	render.GetScreenSizef(m_screen_params.s.x, m_screen_params.s.y);
	render.GetViewportSizef(m_screen_params.v.x, m_screen_params.v.y);
	m_screen_params.v.z = render.GetAspectRatio();

	if (!m_singlepass) {
		m_shaderCopyScreen->SetParam(render, "EffectParams", &m_screen_params);
		m_shaderCopyScreen->SetSamplerSatate(render, "SamplerType", m_textureSampler->GetSamplerState());
	}

	// --- 
	m_fullscreenquad = CreateQuad();
	m_fullscreenquad->Build(render, m_shaderFullscreenQuad);

	for (auto it = m_effectChain.begin(); it != m_effectChain.end(); ++it) {
		(*it)->Initialize(render);
	}

	LOGGER(Log::Logger().Trace("FX Chain OK"));
}

void EffectComposer::Shutdown()
{
	// nothing to do 
	// a refcounter elviekben megsemmisit mindent
}

// ---------------------------------------------------------------------------------------------------
void EffectComposer::BindInput(Renderer & render)
{
	if (m_singlepass)
		return;

	size_t count = 1;
	render.SetRenderTargetView(m_pTexFront->GetRenderTargetView(), 0);

	for (auto it = m_inputMap.begin(); it != m_inputMap.end(); it++) {
		if (it->second.Valid()) {
			render.SetRenderTargetView(*(it->second), it->first);
			count++;
		}
	}

	render.ApplyRenderTargetView(count);
}

void EffectComposer::UnbindInput(Renderer & render)
{
	//if (m_singlepass)
	//	return;

	size_t count = 1;
	render.SetRenderTargetView(nullptr, 0);

	for (auto it = m_inputMap.begin(); it != m_inputMap.end(); it++) {
		if (it->second.Valid()) {
			render.SetRenderTargetView(nullptr, it->first);
			count++;
		}
	}

	render.ApplyRenderTargetView(count);
}

// ---------------------------------------------------------------------------------------------------
void EffectComposer::Render(Renderer & render, int autoflush)
{
	if (m_singlepass)
	{
		RenderChain(render);
	}
	else {
		UnbindInput(render);

		if (m_inputMap.find(0) != m_inputMap.end()) {
			m_pTexFront = m_inputMap[0];
		}

		RenderChain(render);
	}
	if (autoflush)
		this->Flush(render);
}

// ---------------------------------------------------------------------------------------------------
void EffectComposer::SwapBuffers()
{
	if (m_singlepass)
		return;

	Texture2D* tmp = m_pTexWrite;
	m_pTexWrite = m_pTexRead;
	m_pTexRead = tmp;
}

void EffectComposer::FlushBuffers()
{
	if (m_singlepass)
		return;

	SwapBuffers();

	auto tmp = m_pTexFront;
	m_pTexFront = m_pTexBack;
	m_pTexBack = tmp;

}

void EffectComposer::RenderChain(Renderer & render)
{
	m_shaderFullscreenQuad->Bind(render);

	size_t fxcount = m_singlepass ? 1 : m_effectChain.size();

	bool firstNode = true;
	for (size_t fxid = 0; fxid < fxcount; fxid++)
	{
		EffectPass *fx = m_effectChain[fxid].Get();
		if (!m_singlepass) {
			render.SetRenderTargetView(*m_pTexWrite);
			render.ApplyRenderTargetView(1);
			render.BeginScene();
		}

		if (fx && fx->GetShader().Valid()) {
			if (!m_singlepass) {
				fx->GetShader()->SetShaderResourceView(render, "backBuffer", m_pTexBack->GetShaderResourceView());
				fx->GetShader()->SetShaderResourceView(render, "frontBuffer", m_pTexFront->GetShaderResourceView());

				if (firstNode)
					fx->GetShader()->SetShaderResourceView(render, "effectInput", m_pTexFront->GetShaderResourceView());
				else
					fx->GetShader()->SetShaderResourceView(render, "effectInput", m_pTexRead->GetShaderResourceView());

				firstNode = false;
			}
			fx->GetShader()->SetSamplerSatate(render, "SampleType", m_textureSampler->GetSamplerState());
			fx->GetShader()->SetParam(render, "EffectParams", &m_screen_params);
			fx->BindFx(render);
		}

		m_fullscreenquad->RenderMesh(render);

		if (fx && fx->GetShader().Valid()) {
			fx->UnbindFx(render);
			fx->UnbindOutputs(render);
		}

		this->SwapBuffers();
	}

	m_shaderFullscreenQuad->Unbind(render);
}

void EffectComposer::Flush(Renderer & render)
{
	// present the result 
	if (m_singlepass)
		return;

	// ezt ki kell majd baszni innen 
	if (m_chainOutput.Invalid()) {
		render.SetRenderTargetView();
	}
	else {
		render.SetRenderTargetView(*m_chainOutput);
		//m_chainOutput->SetRenderTargetView(0);
	}

	render.ApplyRenderTargetView();
	render.BeginScene();

	m_shaderFullscreenQuad->Bind(render);

	size_t fxcount = m_singlepass ? 1 : m_effectChain.size();

	if (fxcount != 0)
		m_shaderCopyScreen->SetShaderResourceView(render, "effectInput", m_pTexRead->GetShaderResourceView());
	else
		m_shaderCopyScreen->SetShaderResourceView(render, "effectInput", m_pTexFront->GetShaderResourceView());

	m_shaderCopyScreen->Bind(render);
	m_fullscreenquad->RenderMesh(render);
	m_shaderCopyScreen->Unbind(render);
	m_shaderFullscreenQuad->Unbind(render);

	// ezt ki kell majd baszni innen 
	if (m_chainOutput.Valid()) {
		render.SetRenderTargetView();
		render.ApplyRenderTargetView();
	}

	this->FlushBuffers();
}

// ===================================================================================================


EffectRender::EffectRender()
{
}

EffectRender::~EffectRender()
{
}

void EffectRender::Initialize(Renderer & render)
{
	LOGGER(Log::Logger().Trace("Initializing FX Chain"));

	for (int i = 0; i < 2; i++) {
		m_texOut[i] = new Texture2D();
		m_texOut[i]->Initialize(render);
	}
	m_pTexRead = m_texOut[0];
	m_pTexWrite = m_texOut[1];

	// -- 
	m_textureSampler = new TextureSampler();
	m_textureSampler->Initialize(render);

	// --- 
	m_shaderFullscreenQuad = new VertexShader();
	m_shaderFullscreenQuad->LoadFromMemory(
		render,
		effectFullscreenQuadEntry,
		effectShader, strlen(effectShader),
		"FullscreenQuad"
	);

	m_shaderCopyScreen = new PixelShader();
	m_shaderCopyScreen->LoadFromMemory(
		render,
		effectCopyScreenEntry,
		effectShader, strlen(effectShader),
		"CopyScreen"
	);

	m_viewportParams.screen = float4(0, 0, 0, 0);
	m_viewportParams.view = float4(0, 0, 0, 0);
	render.GetScreenSizef(m_viewportParams.screen.x, m_viewportParams.screen.y);
	render.GetViewportSizef(m_viewportParams.view.x, m_viewportParams.view.y);
	m_viewportParams.view.z = render.GetAspectRatio();


	m_shaderCopyScreen->SetParam(render, "EffectParams", &m_viewportParams);
	m_shaderCopyScreen->SetSamplerSatate(render, "SamplerType", m_textureSampler->GetSamplerState());

	// --- 
	m_fullscreenquad = CreateQuad();
	m_fullscreenquad->Build(render, m_shaderFullscreenQuad);

	for (auto it = m_effects.begin(); it != m_effects.end(); ++it)
	{
		(*it)->Initialize(render);
		// itt kell meg valamit susmutyolni a parameterekkel
	}

	LOGGER(Log::Logger().Trace("FX Chain OK"));
}

void EffectRender::Shutdown()
{
	// ... 
}

void EffectRender::Render(Renderer & render, TextureResRef output)
{
	m_shaderFullscreenQuad->Bind(render);

	size_t fxcount = m_effects.size();

	bool firstNode = true;
	for (size_t fxid = 0; fxid < fxcount; fxid++)
	{
		EffectPass *fx = m_effects[fxid].Get();

		render.SetRenderTargetView(*m_pTexWrite);
		render.ApplyRenderTargetView(1);
		render.BeginScene();

		if (fx && fx->GetShader().Valid()) 
			fx->BindFx(render);

		m_fullscreenquad->RenderMesh(render);

		if (fx && fx->GetShader().Valid()) {
			fx->UnbindFx(render);
			fx->UnbindOutputs(render);
		}
	}

	m_shaderFullscreenQuad->Unbind(render);
}


// ===================================================================================================

EffectPass::EffectPass(ShaderResRef shader) : m_shader(shader)
{
}

EffectPass::~EffectPass()
{
	this->Shutdown();
}

// ---------------------------------------------------------------------------------------------------

void EffectPass::Initialize(Renderer & render)
{
	ShaderParameter *shaderParameter = new ShaderParameter();
	shaderParameter->Initialize(render, m_shader);

	m_shaderParameter = shaderParameter;

	LOGGER(Log::Logger().Trace("FX Init pass %s", m_shader->GetName().c_str()));
}

void EffectPass::Shutdown()
{
	m_shaderParameter = nullptr;
}

// ---------------------------------------------------------------------------------------------------

size_t EffectPass::BindOutputs(Renderer &render)
{
	size_t count = 0;
	for (auto it = m_output_map.begin(); it != m_output_map.end(); it++) {
		if (it->second.Valid()) {
			render.SetRenderTargetView(*(it->second), it->first);
			count++;
		}
	}
	return count;
}

size_t EffectPass::UnbindOutputs(Renderer & render)
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

void EffectPass::BindFx(Renderer & render)
{
	for (auto it = m_inputMap.begin(); it != m_inputMap.end(); it++) {
		m_shader->Get()->SetShaderResourceView(render, it->first, it->second->GetShaderResourceView());
	}

	m_shaderParameter->BindParameters(render);
	m_shader->Get()->Bind(render);
}

void EffectPass::UnbindFx(Renderer & render)
{
	for (auto it = m_inputMap.begin(); it != m_inputMap.end(); it++) {
		m_shader->Get()->SetShaderResourceView(render, it->first, nullptr);
	}
	m_shader->Get()->Unbind(render);
}

TextureRef EffectPass::GetOutput(size_t bind)
{
	auto it = m_output_map.find(bind);
	return it == m_output_map.end() ? TextureRef() : it->second;
}

TextureRef EffectPass::GetInput(std::string name)
{
	auto it = m_inputMap.find(name);
	return it == m_inputMap.end() ? TextureRef() : it->second;
}
