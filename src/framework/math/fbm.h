
#pragma once 

#include "../render/dxtypes.h"


// to make sure it will genrate the same on each run
#define SEED 0xe4d07f97 

namespace Grafkit {
    class PseudoRandom {
    public:
        static float Random() {
            static int inited = 0;
            if (!inited) {
                srand(SEED);
                inited = 1;
            }
            return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        }

        static float2 Random2D() {
            return float2(Random(), Random());
        }

        static float3 Random3D() {
            return float3(Random(), Random(), Random());
        }

        static float4 Random4D() {
            return float4(Random(), Random(), Random(), Random());
        }
    };

    /*
    QND reverse-engineered Fractional Brownian motion
    Avoid using it, and please rely on shaders
    */
    // Fractional brownian motion
    class FBM {

    };
}
