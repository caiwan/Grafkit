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

	class ParticleDynamics : public Referencable {
		friend class ParticleEngine;
	public:
		struct DynamicParams_t {
			float4 pos;
			int type;
			float strength;
			float _p1, _p2, _p3;
			float4 param1;
			float4 param2;
		};

		enum ParticleDynamicsType_e {
			PD_INVALID = 0,
			PD_emitter = 1,
			PD_attractor = 2,
			PD_deflector = 3,
			PD_reflector = 4
		};

	public:
		virtual void Calculate();
		virtual ParticleDynamicsType_e GetType() = 0;

	private:
		DynamicParams_t dynamicParams;
	};

	typedef Ref<ParticleDynamics> ParticleDynamicsRef;

	class ParticleEngine
	{
	public:
		ParticleEngine();
		~ParticleEngine();

		// the first one SHOULD BE an enitter
		// up to 24 elements
		void AddDynamicElem(ParticleDynamicsRef &elem);

		void Initialize(Renderer & render, ShaderResRef engine, size_t particleRes);
		void Render(Renderer& render);

		ComputeRef GetCompute() const { return m_particleCompute; }

	protected:
		std::vector<Ref<ParticleDynamics>> m_dynamicElements;

		struct engineParams_t {
			int elemCount;
			float max_age;
			float emit_rate;
			ParticleDynamics::DynamicParams_t params[PARTICLE_NG_MAX_ELEM];
		};

		engineParams_t m_shaderParams;

	private:
		ComputeRef m_particleCompute;
		ShaderResRef m_fsParticleEngine;
	};


	// ========================================================================

	class ParticleEmitter : public ParticleDynamics {
	public:
		virtual ParticleDynamicsType_e GetType() { return ParticleDynamics::PD_emitter; }


	};

	class ParticleAtractor : public ParticleDynamics {
	public:
		virtual ParticleDynamicsType_e GetType() { return ParticleDynamics::PD_attractor; }

	};

	class ParticleDeflector : public ParticleDynamics {
	public:
		virtual ParticleDynamicsType_e GetType() { return ParticleDynamics::PD_deflector; }

	};

	class ParticleReflector : public ParticleDynamics {
	public:
		virtual ParticleDynamicsType_e GetType() { return ParticleDynamics::PD_reflector; }

	};

}
