#pragma once

#include "../render/dxtypes.h"

namespace Grafkit
{
	/* Generates halton series*/
	class Halton {

	public:
		Halton(long i, int base) : m_value(0), m_inv_base(0) {
			Number(i, base);
		}
		Halton() : m_value(0), m_inv_base(0) {
		}
		~Halton() {}

		void Number(long i, int base) {
			float f = m_inv_base = 1. / (float)base;
			m_value = 0.;

			// step on [i] element 
			while (i > 0) {
				m_value += f * (double)(i%base);
				i /= base;
				f *= this->m_inv_base;
			}
		}

		void Next() {
			float r = 1. - m_value - 0.0000001;
			if (m_inv_base < r) m_value += m_inv_base;
			else {
				float h = m_inv_base, hh = 0.;
				do {
					hh = h;
					h *= m_inv_base;
				} while (h >= r);

				m_value += hh + h - 1.;
			}
		}

		float Get() { return m_value; }

		float GetNext() { Next(); return m_value; }

	private:
		float m_value, m_inv_base;
	};

	/* Geneates a 2-component vector of halton series
	*/
	class Halton2D {
	private:
		Halton x, y;

	public:
		Halton2D() {
			x.Number(1, 2); y.Number(2, 3);
		}

		inline void Next() { x.Next(), y.Next(); }
		inline float4 Get() { return float4(x.Get(), y.Get(), 0, 1); }
		inline float4 GetNext() { return float4(x.GetNext(), y.GetNext(), 0, 1); }

		static void Distribution(float4* kernels, size_t len) {
			Halton2D h;
			for (int i = 0; i < len; i++) {
				float4 f = h.GetNext();
				kernels[i].x = 2. * f.x - 1;
				kernels[i].y = 2. * f.y - 1;
				kernels[i].z = 0;
				kernels[i].w = 1;
			}
		}
	};


	/* Geneates a 3-component vector of halton series
	*/
	class Halton3D {
	private:
		Halton x, y, z;

	public:
		Halton3D() {
			x.Number(1, 2); y.Number(2, 3); z.Number(5, 9);
		}

		inline void Next() { x.Next(), y.Next(), z.Next(); }
		inline float3 Get() { return float3(x.Get(), y.Get(), z.Get()); }
		inline float3 GetNext() { return float3(x.GetNext(), y.GetNext(), z.GetNext()); }

		static void HemiSphereDistribution(float4* kernels, size_t len) {
			Halton3D h;
			for (int i = 0; i < len; i++) {
				float3 f = h.GetNext();
				kernels[i].x = 2. * f.x - 1;
				kernels[i].y = 2. * f.y - 1;
				kernels[i].z = f.z;
				kernels[i].w = 0;
			}
		}

	};

}