#define _USE_MATH_DEFINES
#include <cmath>

#include "core/system.h"
#include "core/Music.h"

#include "generator/MusicBassLoader.h"
#include "generator/ShaderLoader.h"

#include "render/renderer.h"
#include "render/shader.h"

#include "render/effect.h"

#include "math/matrix.h"

#include "utils/AssetFile.h"
#include "utils/ResourceManager.h"
#include "utils/ResourcePreloader.h"

#include "utils/InitializeSerializer.h"

using namespace Grafkit;

class Application : public Grafkit::System, protected Grafkit::ResourcePreloader, private Grafkit::ClonableInitializer
{
public:
	Application() : ClonableInitializer(), Grafkit::System(),  ResourcePreloader(),
		m_file_loader(nullptr)
	{
		int screenWidth, screenHeight;

		screenWidth = 800;
		screenHeight = 600;

		InitializeWindows(screenWidth, screenHeight);

		screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
		const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;

		int result = 0;

		result = this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);
		this->m_file_loader = new FileAssetFactory("./../assets/");
	}

	~Application() {
		delete m_file_loader;	///TODO: shutdown elotte
		this->render.Shutdown();
	}

protected:
	Renderer render;
	MusicResRef m_music;
	ShaderResRef m_fxFFTVisu;
	Texture1DRef m_FFTTTex;
	EffectComposerRef m_postfx;

	float *m_fftData;

	int init() 
	{
		m_music = Load<MusicRes>(new MusicBassLoader("alpha_c_-_euh.ogg"));
		m_fxFFTVisu = Load<ShaderRes>(new PixelShaderLoader("fxFFTvisual", "shaders/FFTVisual.hlsl", "fftVisual"));

		LoadCache();
		DoPrecalc();

		m_postfx = new EffectComposer();
		m_postfx->AddPass(new EffectPass(m_fxFFTVisu));
		m_postfx->Initialize(render);

		m_FFTTTex = new Texture1D();
		m_FFTTTex->Initialize(render, 256);

		m_fftData = new float[256];

		(*m_music)->Play();

		return 0;
	};

	
	void release() {
		(*m_music)->Stop();

		delete[] m_fftData;

		SaveCache();
		RemoveAll();
	};

	int mainloop() {
		// update FFT 
		(*m_music)->GetFFT(m_fftData, 256);
		m_FFTTTex->Update(render, m_fftData);

		// --- 
		m_postfx->BindInput(render);
		this->render.BeginScene();
		{
			// do nothing, the blank screen we have
		}

		// render fx chain 
		(*m_fxFFTVisu)->SetShaderResourceView(render, "fftTex", m_FFTTTex->GetShaderResourceView());

		m_postfx->Render(render);

		this->render.EndScene();

		return !(*m_music)->IsPlaying();
	};

private:
	IAssetFactory *m_file_loader;
public:
	IAssetFactory *GetAssetFactory() { return m_file_loader; }
	Renderer & GetDeviceContext() { return render; }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application app;
	return app.execute();
}
