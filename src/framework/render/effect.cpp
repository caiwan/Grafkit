#include "stdafx.h"

#include "effect.h"
#include "texture.h"
#include "mesh.h"

#include "shaderparameter.h"

using namespace Grafkit;

EffectComposer::EffectComposer() :
    m_pTexRead(nullptr)
    , m_pTexWrite(nullptr)
    , m_pTexFront(nullptr)
    , m_pTexBack(nullptr)
    , m_singlepass(false) {
}

#include "builtin_data/cube.h"
#include "builtin_data/defaultShader.h"

using namespace GrafkitData;

// ---------------------------------------------------------------------------------------------------
void EffectComposer::Initialize(Renderer& render, bool singlepass)
{
    m_singlepass = singlepass;
    LOGGER(Log::Logger().Trace("Initializing FX Chain"));

    if (!m_singlepass)
    {
        for (int i = 0; i < 4; i++)
        {
            m_texOut[i] = Texture2DRes(Texture2DRef(new Texture2D()));
            m_texOut[i]->Initialize(render);
        }

        m_pTexFront = *m_texOut[0];
        m_pTexBack = *m_texOut[1];
        m_pTexRead = *m_texOut[2];
        m_pTexWrite = *m_texOut[3];
    }

    // -- 
    m_textureSampler = TextureSamplerRes(TextureSamplerRef(new TextureSampler()));
    m_textureSampler->Initialize(render);

    // --- 
    m_shaderFullscreenQuad = ShaderRes(ShaderRef(new VertexShader()));
    m_shaderFullscreenQuad->LoadFromMemory(
        render,
        effectFullscreenQuadEntry,
        effectShader, strlen(effectShader),
        "FullscreenQuad"
    );

    m_shaderCopyScreen = ShaderRes(ShaderRef(new PixelShader()));
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

    if (!m_singlepass)
    {
        m_shaderCopyScreen->SetParam(render, "EffectParams", &m_screen_params);
        m_shaderCopyScreen->SetSamplerSatate(render, "SamplerType", m_textureSampler->GetSamplerState());
    }

    // --- 
    m_fullscreenquad = CreateQuad();
    m_fullscreenquad->Build(render, m_shaderFullscreenQuad);

    for (auto it = m_effectChain.begin(); it != m_effectChain.end(); ++it) { (*it)->Initialize(render); }

    LOGGER(Log::Logger().Trace("FX Chain OK"));
}

// ---------------------------------------------------------------------------------------------------
void EffectComposer::BindInput(Renderer& render)
{
    if (m_singlepass)
        return;

    size_t count = 1;
    render.SetRenderTargetView(m_pTexFront->GetRenderTargetView(), 0);

    std::for_each(m_inputMap.begin(), m_inputMap.end(), [&count, &render](auto &elem)
    {
// szar
        //if (*elem->second->Valid()) {
        //    render.SetRenderTargetView(static_cast<ID3D11RenderTargetView *>((**(elem->second))), elem->first);
        //    count++;
        //}
    });

    render.ApplyRenderTargetView(count);
}

void EffectComposer::UnbindInput(Renderer& render)
{
    //if (m_singlepass)
    //	return;

    size_t count = 1;
    render.SetRenderTargetView(nullptr, 0);

    std::for_each(m_inputMap.begin(), m_inputMap.end(), [&count, &render](auto &elem)
    {
        // szar
        //if (elem->second->Valid()) {
        //    render.SetRenderTargetView(nullptr, elem->first);
        //    count++;
        //}
    });

    render.ApplyRenderTargetView(count);
}

// ---------------------------------------------------------------------------------------------------
void EffectComposer::Render(Renderer& render, int autoflush)
{
    if (m_singlepass) { RenderChain(render); }
    else
    {
        UnbindInput(render);

        if (m_inputMap.find(0) != m_inputMap.end()) { m_pTexFront = *m_inputMap[0]; }

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

    const auto &tmp = m_pTexWrite;
    m_pTexWrite = m_pTexRead;
    m_pTexRead = tmp;
}

void EffectComposer::FlushBuffers()
{
    if (m_singlepass)
        return;

    SwapBuffers();

    const auto &tmp = m_pTexFront;
    m_pTexFront = m_pTexBack;
    m_pTexBack = tmp;
}

void EffectComposer::RenderChain(Renderer& render)
{
    m_shaderFullscreenQuad->Bind(render);

    size_t fxcount = m_singlepass ? 1 : m_effectChain.size();

    bool firstNode = true;
    for (size_t fxid = 0; fxid < fxcount; fxid++)
    {
        Ref<EffectPass> const & fx = m_effectChain[fxid];
        if (!m_singlepass)
        {
            render.SetRenderTargetView(m_pTexWrite->GetRenderTargetView());
            render.ApplyRenderTargetView(1);
            render.BeginScene();
        }

        if (fx && fx->GetShader())
        {
            ShaderRef fxShader = *fx->GetShader();
            if (!m_singlepass)
            {
                fxShader->SetShaderResourceView(render, "backBuffer", m_pTexBack->GetShaderResourceView());
                fxShader->SetShaderResourceView(render, "frontBuffer", m_pTexFront->GetShaderResourceView());

                if (firstNode)
                    fxShader->SetShaderResourceView(render, "effectInput", m_pTexFront->GetShaderResourceView());
                else
                    fxShader->SetShaderResourceView(render, "effectInput", m_pTexRead->GetShaderResourceView());

                firstNode = false;
            }
            fxShader->SetSamplerSatate(render, "SampleType", m_textureSampler->GetSamplerState());
            fxShader->SetParam(render, "EffectParams", &m_screen_params);
            fx->BindFx(render);
        }

        m_fullscreenquad->RenderMesh(render);

        if (fx && fx->GetShader())
        {
            fx->UnbindFx(render);
            fx->UnbindOutputs(render);
        }

        this->SwapBuffers();
    }

    m_shaderFullscreenQuad->Unbind(render);
}

void EffectComposer::Flush(Renderer& render)
{
    // present the result 
    if (m_singlepass)
        return;

    // ezt ki kell majd baszni innen 
    if (!m_chainOutput) { render.SetRenderTargetView(); }
    else
    {
        render.SetRenderTargetView(m_chainOutput->GetRenderTargetView());
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
    if (m_chainOutput)
    {
        render.SetRenderTargetView();
        render.ApplyRenderTargetView();
    }

    this->FlushBuffers();
}

// ===================================================================================================


EffectRender::EffectRender() : m_pTexRead(nullptr)
, m_pTexWrite(nullptr) {
}

void EffectRender::Initialize(Renderer& render)
{
    LOGGER(Log::Logger().Trace("Initializing FX Chain"));

    for (int i = 0; i < 2; i++)
    {
        m_texOut[i] = Texture2DRes(Texture2DRef(new Texture2D()));
        m_texOut[i]->Initialize(render);
    }
    m_pTexRead = *m_texOut[0];
    m_pTexWrite = *m_texOut[1];

    // -- 
    m_textureSampler = TextureSamplerRes(TextureSamplerRef(new TextureSampler()));
    m_textureSampler->Initialize(render);

    // --- 
    m_shaderFullscreenQuad = ShaderRes(ShaderRef(new VertexShader()));
    m_shaderFullscreenQuad->LoadFromMemory(
        render,
        effectFullscreenQuadEntry,
        effectShader, strlen(effectShader),
        "FullscreenQuad"
    );

    m_shaderCopyScreen = ShaderRes(ShaderRef(new PixelShader()));
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

//TODO: No outpout is used here
void EffectRender::Render(Renderer& render, const Texture2DRef &output)
{
    m_shaderFullscreenQuad->Bind(render);

    size_t fxcount = m_effects.size();

    bool firstNode = true;
    for (size_t fxid = 0; fxid < fxcount; fxid++)
    {
        Ref<EffectPass> const & fx = m_effects[fxid];

        render.SetRenderTargetView(m_pTexWrite->GetRenderTargetView());
        render.ApplyRenderTargetView(1);
        render.BeginScene();

        if (fx && fx->GetShader())
            fx->BindFx(render);

        m_fullscreenquad->RenderMesh(render);

        if (fx && fx->GetShader())
        {
            fx->UnbindFx(render);
            fx->UnbindOutputs(render);
        }
    }

    m_shaderFullscreenQuad->Unbind(render);
}


// ===================================================================================================

EffectPass::EffectPass(ShaderResRef shader) : m_shader(shader) {
}

// ---------------------------------------------------------------------------------------------------

void EffectPass::Initialize(Renderer& render)
{
    ShaderParameter* shaderParameter = new ShaderParameter();
    shaderParameter->Initialize(render, m_shader);

    m_shaderParameter = ShaderParameterRef(shaderParameter);
    LOGGER(Log::Logger().Trace("FX Init pass %s", m_shader->GetName().c_str()));
}

// ---------------------------------------------------------------------------------------------------

size_t EffectPass::BindOutputs(Renderer& render)
{
    size_t count = 0;
    for (auto it = m_output_map.begin(); it != m_output_map.end(); ++it)
    {
        if (it->second)
        {
            render.SetRenderTargetView(it->second->GetRenderTargetView(), it->first);
            count++;
        }
    }
    return count;
}

size_t EffectPass::UnbindOutputs(Renderer& render)
{
    size_t count = 0;
    for (auto it = m_output_map.begin(); it != m_output_map.end(); ++it)
    {
        if (it->second)
        {
            render.SetRenderTargetView(nullptr, it->first);
            count++;
        }
    }
    return count;
}

void EffectPass::BindFx(Renderer& render)
{
    for (auto it = m_inputMap.begin(); it != m_inputMap.end(); ++it) { m_shader->SetShaderResourceView(render, it->first, (*it->second)->GetShaderResourceView()); }

    m_shaderParameter->BindParameters(render);
    m_shader->Bind(render);
}

void EffectPass::UnbindFx(Renderer& render)
{
    for (auto it = m_inputMap.begin(); it != m_inputMap.end(); ++it) { m_shader->SetShaderResourceView(render, it->first, nullptr); }
    m_shader->Unbind(render);
}

Texture2DResRef EffectPass::GetOutput(size_t bind)
{
    auto it = m_output_map.find(bind);
    return it == m_output_map.end() ? Texture2DResRef() : it->second;
}

Texture2DResRef EffectPass::GetInput(std::string name)
{
    auto it = m_inputMap.find(name);
    return it == m_inputMap.end() ? Texture2DResRef() : it->second;
}
