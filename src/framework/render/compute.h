#pragma once 

#include <vector>
#include "common.h"

/** Compute FW w/ support of pixel shaders instead of compute shaders
*/

namespace Grafkit {

    class Mesh;
    typedef Ref<Mesh> MeshRef;

    class Compute;
    typedef Ref<Compute> ComputeRef;
    //typedef std::shared_ptr<Compute> ComputeRef;

	__declspec(align(16)) class Compute : virtual public Referencable, public AlignedNew<Compute>
	{
		/*
			Initial Condition -> Tick -> Result
					+---------- < ---------+
		*/
	public:
		Compute();
        ~Compute();

		// sampleCount has to be power of 2
		void Initialize(Renderer & render, const ShaderResRef & computePixelShader, size_t sampleCount);
		void Shutdown();

		void AddChannel(std::string name, const Texture2DRef & inputCondition = nullptr);
		void Render(Renderer &render);

		// QnD way to bind results to shaders
		void BindOutputs(Renderer & render, const ShaderRef& shader) const ;
		void BindMaterial(Renderer & render, const MaterialRef &material) const ;

	protected:
		void SwapBuffers();

	private:
		size_t m_sampleCount;

		ShaderResRef m_computePixelShader;

		ShaderRef m_shaderFullscreenQuad;
		MeshRef m_fullscreenQuad;

		std::vector<std::string> m_inputNames;
		std::array<std::vector<Texture2DRef>,2> m_channels;
        
	    // These channels will be swapped back and forth during th erender process
		std::vector<Texture2DRef> *m_inputChannels;
		std::vector<Texture2DRef> *m_outputChannels;

		std::vector<Texture2DResRef> m_materialResources;

		// --- snip 
		// This should be the responsibility of the texture module 
		ID3D11Texture2D* m_depthStencilBuffer;
		ID3D11DepthStencilState* m_depthStencilState;
		ID3D11DepthStencilView * m_depthStencilView;
		// --- snap 
	};


}
