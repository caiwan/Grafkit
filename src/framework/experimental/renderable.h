#pragma once

#include "common.h"
#include "render/renderer.h"

namespace Grafkit
{
    class RenderTarget;
    typedef Ref<RenderTarget> RenderTargetRef;

    class Renderable
    {
    public:
        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;

        virtual void Build(Renderer& render) = 0;
        virtual void Build(Renderer& render, ShaderResRef vs, ShaderResRef ps) = 0; // legacy stuff

        virtual void UpdateScene(Renderer& render, float time) = 0;

        virtual void RenderFrame(Renderer& render, float time/*, RenderTargetRef target = nullptr*/) = 0;
        virtual void Render(Renderer& render/*, RenderTargetRef target = nullptr*/) = 0;
    };

    class RenderTarget : public Referencable
    {
    public:
        RenderTarget() {
        }

        virtual ~RenderTarget() {
        }

        //void Precalc(/* + resman */); // + resman

        virtual void Initialize(Renderer& render, EffectComposerRef& targetFx) = 0;
        virtual void Shutdown(Renderer& render) = 0;

        virtual void BindRenderTarget(Renderer& render) = 0;
        virtual void UnbindRenderTarget(Renderer& render) = 0;
        virtual void BindShader(Renderer& render, ShaderResRef& target) = 0;

    //protected:
    //    virtual Texture2DResRef NewTargetTexture(Renderer& render);
    };

    //class DeferredRenderTarget : public RenderTarget
    //{
    //public:

    //    DeferredRenderTarget() {
    //    }

    //    ~ DeferredRenderTarget() {
    //    }

    //    void Initialize(Renderer& render, EffectComposerRef& targetFx) override;
    //    void Shutdown(Renderer& render) override;

    //    void BindRenderTarget(Renderer& render) override;
    //    void UnbindRenderTarget(Renderer& render) override;
    //    void BindShader(Renderer& render, ShaderResRef& target) override;

    //private:
    //    //Texture2DResRef m_mapColor;
    //    Texture2DResRef m_mapNormal;
    //    Texture2DResRef m_mapPosition;
    //    //Texture2DResRef m_mapHeight;
    //    Texture2DResRef m_mapRoughness;
    //    Texture2DResRef m_mapMetallic;
    //    Texture2DResRef m_mapEmission;

    //    EffectComposerRef m_targetFx;
    //};
}
