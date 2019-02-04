#include "stdafx.h"
#include "compute.h"

#include "texture.h"
#include "shader.h"
#include "mesh.h"
#include "material.h"

#include "core/exceptions.h"

#include "builtin_data/cube.h"
#include "builtin_data/defaultShader.h"

#include "texture.h"

DEFINE_EXCEPTION(InitializeComputeShaderExcepton, 1, "Can not initialize compute");

using namespace Grafkit;
using namespace FWdebugExceptions;

Compute::Compute(): m_sampleCount(0)
    , m_depthStencilBuffer(nullptr)
    , m_depthStencilState(nullptr)
    , m_depthStencilView(nullptr)
{
    m_inputChannels = &m_channels[0];
    m_outputChannels = &m_channels[1];
}

Compute::~Compute() = default;

void Compute::Initialize(Renderer& render, const ShaderResRef& shader, size_t samples)
{
    m_sampleCount = samples;
    m_computePixelShader = shader;
    m_shaderFullscreenQuad = ShaderRes(ShaderRef(new PixelShader()));
    m_shaderFullscreenQuad = ShaderRes(ShaderRef(new VertexShader()));
    m_shaderFullscreenQuad->LoadFromMemory(
        render,
        GrafkitData::effectFullscreenQuadEntry,
        GrafkitData::effectShader, strlen(GrafkitData::effectShader),
        "FullscreenQuad"
    );

    m_fullscreenQuad = GrafkitData::CreateQuad();
    m_fullscreenQuad->Build(render, m_shaderFullscreenQuad);

    std::for_each(m_channels.begin(), m_channels.end(), [&](auto& channels) { std::for_each(channels.begin(), channels.end(), [&](auto& channel) { channel->InitializeFloatRGBA(render, m_sampleCount, m_sampleCount); }); });

    // --- snip 
    // This should be the responsibility of the texture module 

    // depth stencil view 
    HRESULT result = 0;

    D3D11_TEXTURE2D_DESC depthBufferDesc;
    // Initialize the description of the depth buffer.
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    // Set up the description of the depth buffer.
    depthBufferDesc.Width = m_sampleCount;
    depthBufferDesc.Height = m_sampleCount;
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
    result = render.GetDevice()->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
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
    result = render.GetDevice()->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, &m_depthStencilView);
    if (FAILED(result)) {
        THROW_EX_HRESULT(InitializeComputeShaderExcepton, result);
    }
    // --- snap

    LOGGER(Log::Logger().Trace("Compute mockup shader OK"));
}

void Compute::Shutdown()
{
    std::for_each(m_channels.begin(), m_channels.end(), [&](auto& channels) { std::for_each(channels.begin(), channels.end(), [&](auto& channel) { channel->Shutdown(); }); });
}

void Compute::AddChannel(std::string name, const Texture2DRef & inputCondition)
{
    m_inputNames.push_back(name);
    m_channels[0].push_back(inputCondition ? inputCondition : Texture2DRef(new Texture2D()));

    Texture2DRef tex = Texture2DRef(new Texture2D());
    m_channels[1].push_back(tex);

    Texture2DResRef res = Texture2DResRef(tex);
    res->SetName(name);
    m_materialResources.push_back(res);
}

void Compute::Render(Renderer& render)
{
    ShaderRef shader = *m_computePixelShader;
    shader->Bind(render);

    // bind input && output
    for (size_t i = 0; i < m_inputChannels->size(); i++)
    {
        Texture2DRef input = m_inputChannels->at(i);
        shader->SetShaderResourceView(render, m_inputNames[i], input->GetShaderResourceView());
    }

    render.SetDepthStencilView(m_depthStencilView);

    size_t targetCount = m_outputChannels->size();
    for (size_t i = 0; i < targetCount; i++)
    {
        Texture2DRef output = m_outputChannels->at(i);
        render.SetRenderTargetView(output->GetRenderTargetView(), i);
    }

    render.ApplyRenderTargetView(targetCount);

    // save viewport metrics to be able to reset it
    int w = 0, h = 0, x = 0, y = 0;
    render.GetViewportSize(w, h, x, y);

    // do the stuff 
    render.SetViewport(m_sampleCount, m_sampleCount);
    render.BeginScene();
    m_shaderFullscreenQuad->Bind(render);
    m_fullscreenQuad->RenderMesh(render);

    // wrap up
    m_shaderFullscreenQuad->Unbind(render);
    shader->Unbind(render);

    render.SetDepthStencilView();
    render.SetRenderTargetView();
    render.ApplyRenderTargetView(1);

    render.SetViewport(w, h, x, y);

    SwapBuffers();
}

void Compute::BindOutputs(Renderer& render, const ShaderRef& shader) const { for (size_t i = 0; i < m_outputChannels->size(); i++) { shader->SetShaderResourceView(render, m_inputNames[i], m_outputChannels->at(i)->GetShaderResourceView()); } }

void Compute::BindMaterial(Renderer& render, const MaterialRef& material) const { for (size_t i = 0; i < m_outputChannels->size(); i++) { material->SetTexture(m_materialResources[i], m_inputNames[i]); } }

void Compute::SwapBuffers()
{
    auto tmp = m_outputChannels;
    m_outputChannels = m_inputChannels;
    m_inputChannels = tmp;

    // swp
    for (size_t i = 0; i < m_materialResources.size(); i++) { m_materialResources[i].AssignRef(m_outputChannels->at(i)); }
}
