#define _USE_MATH_DEFINES
#include <cmath>

#include "fwzSetup.h"

#include "core/system.h"
#include "core/Music.h"

#include "generator/MusicBassLoader.h"
#include "generator/ShaderLoader.h"
#include "generator/SceneLoader.h"

#include "render/renderer.h"
#include "render/Scene.h"
#include "render/shader.h"

#include "render/effect.h"

#include "math/matrix.h"

#include "utils/AssetFile.h"
#include "utils/ResourceManager.h"
#include "utils/ResourcePreloader.h"

#include "utils/InitializeSerializer.h"

#if 1
#define HAVE_MUSIC(x) x
#else
#define HAVE_MUSIC(x)
#endif

using namespace Grafkit;

class Application : public Grafkit::System, protected Grafkit::ResourcePreloader, private Grafkit::ClonableInitializer
{
public:
	Application(fwzSettings &settings) : ClonableInitializer(), Grafkit::System(),  ResourcePreloader(),
		m_file_loader(nullptr)
	{
		int screenWidth, screenHeight;

		screenWidth = settings.scrWidth;
		screenHeight = settings.scrHeight;

		InitializeWindows(screenWidth, screenHeight);

		screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();

		int result = 0;

		result = this->render.Initialize(screenWidth, screenHeight, settings.nVsync, this->m_window.getHWnd(), !settings.nWindowed);
		this->m_file_loader = new FileAssetFactory("./assets/");
	}

	~Application() {
		delete m_file_loader;	///TODO: shutdown elotte
		this->render.Shutdown();
	}

protected:
	Renderer render;

	HAVE_MUSIC(MusicResRef m_music);
	ShaderResRef m_fxFFTVisu;
	ShaderResRef m_vs;
	ShaderResRef m_fs;
	Texture1DRef m_FFTTTex;
	
	SceneResRef m_scene1;
	SceneResRef m_scene2;
	SceneResRef m_scene3;
	
	EffectComposerRef m_postfx;

	float *m_fftData;

	int init() 
	{
		HAVE_MUSIC(m_music = Load<MusicRes>(new MusicBassLoader("padlizsan.ogg")));
		m_fxFFTVisu = Load<ShaderRes>(new PixelShaderLoader("fxFFTvisual", "shaders/postfx.hlsl", "passthrough"));

		m_vs = Load<ShaderRes>(new VertexShaderLoader("vs", "shaders/flat.hlsl", ""));
		m_fs = Load<ShaderRes>(new PixelShaderLoader("fs", "shaders/flat.hlsl", ""));

		m_scene1 = Load<SceneRes>(new SceneLoader("scene1", "scene1.scene"));
		//m_scene2 = Load<SceneRes>(new SceneLoader("scene1", "scene2.scene"));
		//m_scene3 = Load<SceneRes>(new SceneLoader("scene1", "scene3.scene"));

		LoadCache();
		DoPrecalc();

		(*m_scene1)->BuildScene(render, m_vs, m_fs);
		(*m_scene1)->SetActiveCamera("Camera_001");
		
		m_postfx = new EffectComposer();
		m_postfx->AddPass(new EffectPass(m_fxFFTVisu));
		m_postfx->Initialize(render);

		m_FFTTTex = new Texture1D();
		m_FFTTTex->Initialize(render, 256);

		m_fftData = new float[256];

		HAVE_MUSIC((*m_music)->Play());
		
		return 0;
	};

	
	void release() {
		HAVE_MUSIC((*m_music)->Stop());

		delete[] m_fftData;

		SaveCache();
		RemoveAll();
	};

	int mainloop() {
		// update FFT 

		float t = 0.0f;

		HAVE_MUSIC((*m_music)->GetFFT(m_fftData, 256));
		HAVE_MUSIC(t = (*m_music)->GetTime());
		m_FFTTTex->Update(render, m_fftData);

		// --- 
		m_postfx->BindInput(render);
		this->render.BeginScene();
		{
			(*m_scene1)->RenderFrame(render, t);
		}

		// render fx chain 
		(*m_fxFFTVisu)->SetShaderResourceView("fftTex", m_FFTTTex->GetShaderResourceView());

		m_postfx->Render(render);

		this->render.EndScene();

		int res = 0;
		HAVE_MUSIC(res = !(*m_music)->IsPlaying());
		return res;
	};

private:
	IAssetFactory *m_file_loader;
public:
	IAssetFactory *GetAssetFactory() { return m_file_loader; }
	Renderer & GetDeviceContext() { return render; }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	fwzSettings setup;
	setup.hInstance = hInstance;

	setup.scrBPP = 32;
	setup.nVsync = 1;
	setup.nMultisample = 0;

	setup.nAlwaysOnTop = 1;
	setup.scrWidth = 800;
	setup.scrHeight = 600;
	setup.nWindowed = 1;
	if (!OpenSetupDialog(&setup))
		return -1;

	Application app(setup);
	return app.execute();
}
