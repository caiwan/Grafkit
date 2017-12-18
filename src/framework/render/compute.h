#pragma once 

#include <vector>

#include "shader.h"
#include "mesh.h"
#include "material.h"
#include "texture.h"
#include "../utils/reference.h"


/** Compute FW w/ support of pixel shaders instead of compute shaders
*/

namespace Grafkit {

	__declspec(align(16)) class Compute : virtual public Referencable, public AlignedNew<Compute>
	{
		/*
			Initial Condition -> Tick -> Result
					+---------- < ---------+
		*/
	public:
		Compute();
		~Compute(){}

		// sampleCount has to be power of 2
		void Initialize(Renderer & render, ShaderResRef computePixelShader, size_t sampleCount);
		void Shutdown();

		void AddChannel(std::string name, Texture2DRef inputCondition = nullptr);
		void Render(Renderer &render);

		// QnD way to bind results to shaders
		void BindOutputs(Renderer & render, ShaderRef& shader) const ;
		void BindMaterial(Renderer & render, MaterialRef &material) const ;

	protected:
		void SwapBuffers();

	private:
		size_t sampleCount;

		ShaderResRef computePixelShader;

		ShaderRef shaderFullscreenQuad;
		MeshRef fullscreenQuad;

		std::vector<std::string> inputNames;
		std::vector<Texture2DRef> channels[2];
		std::vector<Texture2DRef> *inputChannels;
		std::vector<Texture2DRef> *outputChannels;

		std::vector<Texture2DResRef> materialResources;

		// --- snip 
		// This should be the responsibility of the texture module 
		ID3D11Texture2D* depthStencilBuffer;
		ID3D11DepthStencilState* depthStencilState;
		ID3D11DepthStencilView * depthStencilView;
		// --- snap 
	};

	typedef Ref<Compute> ComputeRef;

}

DEFINE_EXCEPTION(InitializeComputeShaderExcepton, 1, "Can not initialize compute");

