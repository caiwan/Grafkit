#include "stdafx.h"
#include "loaderbar.h"

using namespace Grafkit;

namespace {
	static const char pPSahderSrc[] = ""
		"#define LIM .001\n"
		"#define THICK .08\n"
		"#define BORDER .01\n"
		"\n"
		"struct PixelInputType\n"
		"	{\n"
		"		float4 position : SV_POSITION;\n"
		"		float2 tex : TEXCOORD;\n"
		"	};\n"
		"\n"
		"	cbuffer LoaderBar{\n"
		"		float4 barParams;"
		"	}\n"
		"\n"
		"	static const float4 barColor = float4(1., 1., 1., 1.);\n"
		"	static const float4 borderColor = float4(1., 1., 1., 1.);\n"
		"\n"
		"	float udBox(float2 p, float2 b) { return length(max(abs(p) - b, 0.0)); }\n"
		"\n"
		//"	float2 sincos(float x) { return float2(sin(x), cos(x)); }\n"
		//"	float2 rotate2d(float2 uv, float phi) { float2 t = sincos(phi); return float2(uv.x*t.y - uv.y*t.x, uv.x*t.x + uv.y*t.y); }\n"
		"\n"
		"	float4 getLoader(float2 uv, float transition) {\n"
		"		if (udBox(uv, float2(1., THICK + BORDER))<LIM && udBox(uv, float2(1. - BORDER * 2, THICK - BORDER))>LIM) return borderColor;\n"
		"		if (udBox(uv + float2(1. - transition, 0.), float2(transition - BORDER * 4., THICK - BORDER*3.))<LIM) return barColor;\n"
		"		return float4(0.,0.,0.,0.);\n"
		"	}\n"
		"\n"
		"	float4 main(PixelInputType input) : SV_TARGET\n"
		"	{\n"
		"		float2 uv = 2. * (input.tex.xy - .5);\n"
		"		uv.y = uv.y / barParams.x;"
		"\n"
		"	float4 color = getLoader(1.6 * uv , barParams.z);\n"
		"	color.a = 1.;"
		"	return color;\n"
		"	}\n"
		;
}

GrafkitData::LoaderBar::LoaderBar()
{
}

GrafkitData::LoaderBar::~LoaderBar()
{
	ShutdownLoaderBar();
}

void GrafkitData::LoaderBar::InitializeLoaderBar(Grafkit::Renderer & renderer)
{
	m_ps = CreateShader(renderer);
	m_loaderbar = new EffectComposer();
	m_loaderbar->AddPass(new EffectPass(m_ps));
	m_loaderbar->Initialize(renderer);
}

void GrafkitData::LoaderBar::ShutdownLoaderBar()
{
	if (m_ps.Valid())
		if (m_ps->Valid()) {
			m_ps->Get()->Shutdown();
			m_ps->AssingnRef(nullptr);
		}
	if (m_loaderbar.Valid()) {
		m_loaderbar->Shutdown();
		m_loaderbar.AssingnRef(nullptr);
	}
}

void GrafkitData::LoaderBar::OnElemLoad(size_t actual, size_t count)
{
	UpdateLoaderBar((float)actual / (float)count);
}

void GrafkitData::LoaderBar::DrawLoaderBar(Grafkit::Renderer & render, float p)
{
	float4 par;
	par.x = render.GetAspectRatio();
	par.z = p;

	m_ps->Get()->SetParamT<float4>(render, "LoaderBar", par);

	//
	m_loaderbar->BindInput(render);
	render.BeginScene(0, 0, 0, 1);

	//
	m_loaderbar->Render(render);
	render.EndScene();
}

Grafkit::ShaderResRef GrafkitData::LoaderBar::CreateShader(Grafkit::Renderer & renderer)
{
	ShaderRef ps = new PixelShader();
	ps->LoadFromMemory(renderer, "main", pPSahderSrc, sizeof(pPSahderSrc), "loaderbar");
	return new ShaderRes(ps);
}

