#include "renderer.h"


namespace FWrender
{
	/** Particle generator
	*/
	class Particle 
	{
		protected:
			struct particle{
				float3 vertex;
				UINT index;
				float3 position;
				float2 size;
				float4 color;
				float2 uv;
				float angle;
			};

			/*typedef union {
				struct {
					bool has_vertex : 1;
					bool has_index : 1;
					bool has_position : 1;
					bool has_size : 1;
					bool has_color : 1;
					bool has_uv : 1;
					bool has_angle : 1;
				};
				UINT mask;
			} cbuffer_flags;*/
			
		public:
			Particle();
			~Particle();

			// input layout ? 
	};
}