#include "renderable.h"

#include "render/texture.h"
#include "render/effect.h"

using namespace Grafkit;

// this is not the way I want to do this

//Texture2DResRef RenderTarget::NewTargetTexture(Renderer& render)
//{
//    Texture2DRef texture = new Texture2D();
//    texture->InitializeFloatRGBA(render);
//    Texture2DResRef textureRes = new Texture2DRes(texture);
//    return textureRes;
//}
//
//void DeferredRenderTarget::Initialize(Renderer& render, EffectComposerRef& targetFx)
//{
//    m_targetFx = targetFx;
//    // these cruds needs to be resized when it comes to resize the rendering surface itself
//    //Texture2DResRef m_mapColor = NewTargetTexture(render);
//    Texture2DResRef m_mapNormal = NewTargetTexture(render);
//    Texture2DResRef m_mapPosition = NewTargetTexture(render);
//    //Texture2DResRef m_mapHeight = NewTargetTexture(render);
//    Texture2DResRef m_mapRoughness = NewTargetTexture(render);
//    Texture2DResRef m_mapMetallic = NewTargetTexture(render);
//    Texture2DResRef m_mapEmission = NewTargetTexture(render);
//
//    //m_targetFx->SetInput(0, m_mapColor);
//    m_targetFx->SetInput(1, m_mapNormal);
//    m_targetFx->SetInput(2, m_mapPosition);
//    //m_targetFx->SetInput(3, m_mapHeight);
//    m_targetFx->SetInput(3, m_mapRoughness);
//    m_targetFx->SetInput(4, m_mapEmission);
//    m_targetFx->SetInput(5, m_mapMetallic);
//
//}
//
//void DeferredRenderTarget::Shutdown(Renderer& render) {
//}
//
//void DeferredRenderTarget::BindRenderTarget(Renderer& render) {
//    m_targetFx->BindInput(render);
//}
//
//void DeferredRenderTarget::UnbindRenderTarget(Renderer& render) {
//    m_targetFx->UnbindInput(render);
//
//}
//
//void DeferredRenderTarget::BindShader(Renderer& render, ShaderResRef& target) {
//    (*target)->SetShaderResourceView(render, "normalMap", (m_mapNormal)->GetShaderResourceView());
//    (*target)->SetShaderResourceView(render, "viewMap", (m_mapPosition)->GetShaderResourceView());
//    (*target)->SetShaderResourceView(render, "normalMap", (m_mapRoughness)->GetShaderResourceView());
//    (*target)->SetShaderResourceView(render, "viewMap", (m_mapPosition)->GetShaderResourceView());
//}
