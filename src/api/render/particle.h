#include "renderer.h"
#include "mesh.h"

#define DEFAULT_PARTICLE_COUNT 4096

namespace FWrender
{
	/** Particle generator
	*/
	class Particle 
	{	
		public:
			Particle(size_t n = DEFAULT_PARTICLE_COUNT);
			~Particle();

			// input layout ? 

		protected:
			struct particle {
				float3 vertex;
				UINT index;
				float2 uv;
				
				// a jovoben valami gpu accelerated loszar kell majd ehelyett
				float3 position;
				float2 size;
				float4 color;
				float angle;
			};

			struct particle m_pParticles;
			size_t count;
	};
}