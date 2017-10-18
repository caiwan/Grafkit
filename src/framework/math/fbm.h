
#pragma once 

#include <cmath>

#include "common.h"

/*
QND reverse-engineered Fractional Brownian motion
Avoid using it, and please rely on shaders
*/

namespace Grafkit {

	float fractf(float f) {
		return f - (int)f;
	}

	float dot(float2 v, float2 w) {
		return v.x * w.x + v.y * w.y;
	}

float hash2(float2 co){
static float[64*64] lut;
int fisttime = 0;
}

	float hash(float2 co) {
		float a = 12.9898, b = 78.233, c = 43758.5453;
		float dt = dot(co, float2(a, b));
		float sn = fmod(dt, 3.14);
		return fractf(sin(sn) * c);
	}

	float2 _m2(float2 p) {
		float2 r;
		r.x = p.x * 1.6 + p.y * -1.2;
		r.x = p.x * 1.2 + p.y * 1.6;
		return r;
	}

	float fbm(float2 p)
	{
		float f = 0.5000*hash(p); p = _m2(p);
		f += 0.2500*hash(p); p = _m2(p);
		f += 0.1666*hash(p); p = _m2(p);
		f += 0.0834*hash(p);
		return f;
	}
}