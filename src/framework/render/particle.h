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

	class ParticleDynamics;
	typedef Ref<ParticleDynamics> ParticleDynamicsRef;

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
		ParticleDynamics(float4 position = float4(0, 0, 0, 1), float weight = 1.f);

		ParticleDynamics::DynamicElem_t GetParams() { return m_params; }
		void SetParams(ParticleDynamics::DynamicElem_t params) { m_params = params; }

		virtual void Calculate();
		virtual ParticleDynamicsType_e GetType() = 0;

	protected:
		DynamicElem_t m_params;
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

		void Initialize(Renderer & render, ShaderResRef engine, size_t particleRes);
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

		ParticleEngine::ParticleGlobalParams_t GetGlobals() { return m_shaderParams.globals; }
		void SetGlobals(ParticleEngine::ParticleGlobalParams_t globals) { m_shaderParams.globals = globals; }

	protected:
		ParticleShaderParams_t m_shaderParams;

	private:
		ComputeRef m_particleCompute;
		ShaderResRef m_fsParticleEngine;
	};

	// ========================================================================

	class ParticleEmitter : public ParticleDynamics {
	public:
		virtual ParticleDynamicsType_e GetType() { return ParticleDynamics::PD_emitter; }
	};

	// ------------------------------------------------------------------------


	class ParticleAttractor : public ParticleDynamics {
	public:
		ParticleAttractor(float4 pos = float4(0, 0, 0, 1), float weight = 1., float force = 1.f);

		virtual ParticleDynamicsType_e GetType() { return ParticleDynamics::PD_attractor; }

	public:
		//struct AttractorArgs_t
		//{
		//	union {
		//		float force;
		//		struct DynamicArguments_t _params;
		//	};
		//};

		//AttractorArgs_t& Arguments() { return *reinterpret_cast<AttractorArgs_t*>(&m_params.args); }
		//AttractorArgs_t GetArgs() {return }
	};

	// ------------------------------------------------------------------------

	class ParticleDeflector : public ParticleDynamics {
	public:
		virtual ParticleDynamicsType_e GetType() { return ParticleDynamics::PD_deflector; }

	};

	// ------------------------------------------------------------------------
	class ParticleReflector : public ParticleDynamics {
	public:
		virtual ParticleDynamicsType_e GetType() { return ParticleDynamics::PD_reflector; }

	};

}
