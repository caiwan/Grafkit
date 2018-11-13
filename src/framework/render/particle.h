#pragma once

#include <vector>

#include "compute.h"

#include "dxtypes.h"
#include "renderer.h"

#define PARTICLE_NG_MAX_ELEM 24

namespace Grafkit
{
	/** QndParticleEngine
	*/

	class ParticleEngine;
    typedef Ref<ParticleEngine> ParticleEngineRef;
    //typedef std::shared_ptr<ParticleEngine> ParticleEngineRef;

	class ParticleDynamics;
    typedef Ref<ParticleDynamics> ParticleDynamicsRef;
    //typedef std::shared_ptr<ParticleDynamics> ParticleDynamicsRef;

	// ========================================================================

	class ParticleDynamics : public Referencable {
		friend class ParticleEngine;
	public:
		enum ParticleDynamicsType_e {
			PD_INVALID = 0,
			PD_emitter = 1,
			PD_attractor = 2,
			PD_deflector = 3,
			PD_reflector = 4
		};
		struct DynamicArguments_t {
			float4 position;
			float4 param0;
			float4 param1;
			float4 param2;
		};
		struct DynamicElem_t
		{
			union
			{
				struct {
					int type;
					float weight;
				};
				char _[16]; // alignment
			};
			DynamicArguments_t args;
		};

	public:
	    explicit ParticleDynamics(float4 position = float4(0, 0, 0, 1), float weight = 1.f);

		DynamicElem_t GetParams() const { return m_params; }
		void SetParams(DynamicElem_t params) { m_params = params; }

		DynamicArguments_t GetArgs() const { return m_params.args; }
		void SetArgs(DynamicArguments_t args) { m_params.args = args; }

		void SetEnabled(bool isEnabled) {
			m_enabled = isEnabled;
		}

	protected:
		virtual void Calculate();
		virtual ParticleDynamicsType_e GetType() = 0;

        DynamicElem_t m_params;
		bool m_enabled;
	};

	// ========================================================================

	class ParticleEngine : public Referencable
	{
	public:
		ParticleEngine();
		~ParticleEngine();

		// the first one SHOULD BE an enitter
		// up to 24 elements
		void AddDynamics(ParticleDynamicsRef elem);

		void Initialize(Renderer & render, const ShaderResRef &engine, size_t particleRes);
		void Render(Renderer& render);

		ComputeRef GetCompute() const { return m_particleCompute; }

	protected:
		std::vector<Ref<ParticleDynamics>> m_dynamicElements;

	public:

		struct ParticleGlobalParams_t {
			float maxAge;
			float emitRate;

			float brownianSpeed;
			float brownianScale;
			float brownianSeed;

			float speedScale;
		};

		struct ParticleShaderParams_t {
			union
			{
				struct {
					ParticleGlobalParams_t globals;
					int elemCount;
				};
				char _0[32]; // aligned to 2x16 byte 
			};
			ParticleDynamics::DynamicElem_t args[PARTICLE_NG_MAX_ELEM];
		};

		ParticleGlobalParams_t GetGlobals() const { return m_shaderParams.globals; }
		void SetGlobals(ParticleGlobalParams_t globals) { m_shaderParams.globals = globals; }

		// ... 

	protected:
		ParticleShaderParams_t m_shaderParams;

	private:
		ComputeRef m_particleCompute;
		ShaderResRef m_fsParticleEngine;
	};

	// ========================================================================

	class ParticleEmitter : public ParticleDynamics {
	public:
	    ParticleDynamicsType_e GetType() override { return PD_emitter; }
	};

	// ------------------------------------------------------------------------


	class ParticleAttractor : public ParticleDynamics {
	public:
	    explicit ParticleAttractor(float4 pos = float4(0, 0, 0, 1), float weight = 1., float force = 1.f);
	    ParticleDynamicsType_e GetType() override { return PD_attractor; }
	};

	// ------------------------------------------------------------------------

	class ParticleDeflector : public ParticleDynamics {
	public:
	    ParticleDynamicsType_e GetType() override { return PD_deflector; }
	};

	// ------------------------------------------------------------------------
	class ParticleReflector : public ParticleDynamics {
	public:
	    ParticleDynamicsType_e GetType() override { return PD_reflector; }
	};

}
