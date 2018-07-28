#include "stdafx.h"
#include "compute.h"

#include "shader.h"

#include "utils/exceptions.h"

#include "builtin_data/cube.h"
#include "builtin_data/defaultShader.h"

#include "texture.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

Compute::Compute()
{
	inputChannels = &channels[0];
	outputChannels = &channels[1];
}

void Compute::Initialize(Renderer & render, ShaderResRef shader, size_t samples)
{
	sampleCount = samples;
	computePixelShader = shader;
	shaderFullscreenQuad = new PixelShader();
	shaderFullscreenQuad = new VertexShader();
	shaderFullscreenQuad->LoadFromMemory(
		render,
		GrafkitData::effectFullscreenQuadEntry,
		GrafkitData::effectShader, strlen(GrafkitData::effectShader),
		"FullscreenQuad"
	);

	fullscreenQuad = GrafkitData::CreateQuad();
	fullscreenQuad->Build(render, shaderFullscreenQuad);

	for (size_t i = 0; i < channels[0].size(); i++) {
		channels[0][i]->InitializeFloatRGBA(render, sampleCount, sampleCount);
		channels[1][i]->InitializeFloatRGBA(render, sampleCount, sampleCount);
	}

	
	// --- snip 
	// This should be the responsibility of the texture module 

	// depth stencil view 
	HRESULT result = 0;

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = sampleCount;
	depthBufferDesc.Height = sampleCount;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = render.GetDevice()->CreateTexture2D(&depthBufferDesc, nullptr, &depthStencilBuffer);
	if (FAILED(result))
		THROW_EX_HRESULT(InitializeComputeShaderExcepton, result);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = render.GetDevice()->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	if (FAILED(result))
		THROW_EX_HRESULT(InitializeComputeShaderExcepton, result);
	
	// --- snap

	LOGGER(Log::Logger().Trace("Compute mockup shader OK"));

}

void Compute::Shutdown()
{
	for (size_t i = 0; i < channels[0].size(); i++) {
		channels[0][i]->Shutdown();
		channels[1][i]->Shutdown();
	}

	shaderFullscreenQuad->Shutdown();
}

void Compute::AddChannel(std::string name, Texture2DRef inputCondition)
{
	inputNames.push_back(name);
	channels[0].push_back(inputCondition.Valid() ? inputCondition : new Texture2D());
	
	Texture2DRef tex = new Texture2D();
	channels[1].push_back(tex);

	Texture2DResRef res = new Texture2DRes(tex);
	res->SetName(name);
	materialResources.push_back(res);
}

void Compute::Render(Renderer &render)
{
	ShaderRef shader = computePixelShader->Get();
	shader->Bind(render);

	// bind input && output
	for (size_t i = 0; i < inputChannels->size(); i++) {
		Texture2DRef input = inputChannels->at(i);
		shader->SetShaderResourceView(render, inputNames[i], input->GetShaderResourceView());
	}

	render.SetDepthStencilView(depthStencilView);

	size_t targetCount = outputChannels->size();
	for (size_t i = 0; i < targetCount; i++) {
		Texture2DRef output = outputChannels->at(i);
		render.SetRenderTargetView(static_cast<ID3D11RenderTargetView*>(*output), i);
	}

	render.ApplyRenderTargetView(targetCount);

	// save viewport metrics to be able to reset it
	int w = 0, h = 0, x = 0, y = 0;
	render.GetViewportSize(w, h, x, y);

	// do the stuff 
	render.SetViewport(sampleCount, sampleCount);
	render.BeginScene();
	shaderFullscreenQuad->Bind(render);
	fullscreenQuad->RenderMesh(render);

	// wrap up
	shaderFullscreenQuad->Unbind(render);
	shader->Unbind(render);

	render.SetDepthStencilView();
	render.SetRenderTargetView();
	render.ApplyRenderTargetView(1);

	render.SetViewport(w, h, x, y);
	
	SwapBuffers();
}

void Compute::BindOutputs(Renderer & render, ShaderRef & shader) const
{
	for (size_t i = 0; i < outputChannels->size(); i++) {
		shader->SetShaderResourceView(render, inputNames[i], outputChannels->at(i)->GetShaderResourceView());
	}
}

void Compute::BindMaterial(Renderer & render, MaterialRef & material) const
{
	for (size_t i = 0; i < outputChannels->size(); i++) {
		material->SetTexture(materialResources[i], inputNames[i]);
	}
}

void Compute::SwapBuffers()
{
	auto tmp = outputChannels;
	outputChannels = inputChannels;
	inputChannels = tmp;

	// swp
	for (size_t i = 0; i < materialResources.size(); i++) {
		materialResources[i]->AssingnRef(outputChannels->at(i));
	}
}
