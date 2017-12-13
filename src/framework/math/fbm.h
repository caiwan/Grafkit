
#pragma once 

#include <cmath>

#include "../render/dxtypes.h"

/*
QND reverse-engineered Fractional Brownian motion
Avoid using it, and please rely on shaders
*/

// tu make sure it will genrate the same on each run
#define SEED 0xe4d07f97 

namespace Grafkit {
	class PseudoRandom {
	public:
		static inline float Random() {
			static int inited = 0;
			if (!inited) {
				srand(SEED);
				inited = 1;
			}
			return (float)rand() / (float)RAND_MAX;
		}

		static inline float2 Random2D() {
			return float2(Random(), Random());
		}

		static inline float3 Random3D() {
			return float3(Random(), Random(), Random());
		}
	};

	// Fractional brownian motion
	class FBM {

	};
}
