#pragma once

namespace Grakfit {

	/* Quick and dirty
	 Piecewise Cubic Hermite Interpolation
	*/
	class Hermite {

	public:
		Hermite(double * x, double *y, unsigned int n) {
			m_n = n;
			m_x = new double[n];
			m_y = new double[n];

			int k = n;
			while (k--) {
				m_x[k] = x[k];
				m_y[k] = y[k];
			}

		}

		~Hermite() {
			delete[] m_x;
			delete[] m_y;
		}

		double calculate(double t) {
			int i = findkey(t);

			double t0 = getkey(i + 0);
			double t1 = getkey(i + 1);

			double tt = (t - t0) / (t1 - t0);

			double a = getval(i - 1);
			double b = getval(i + 0);
			double c = getval(i + 1);
			double d = getval(i + 2);

			return cubicHermite(a, b, c, d, tt);

		}

	private:

		double getval(int key) {
			if (key < 0)
				return m_y[0];
			if (key >= m_n)
				return m_y[m_n - 1];
			return m_y[key];
		}

		double getkey(int key) {
			if (key < 0)
				return m_x[0];
			if (key >= m_n)
				return m_x[m_n - 1];
			return m_x[key];
		}

		int findkey(double t) {
			int k = m_n;
			while (k--) {
				if (m_x[k] <= t)
					return k;
			}
			return 0;
		}


		double cubicHermite(double A, double B, double C, double D, double t)
		{
			double a = -A / 2.0f + (3.0f*B) / 2.0f - (3.0f*C) / 2.0f + D / 2.0f;
			double b = A - (5.0f*B) / 2.0f + 2.0f*C - D / 2.0f;
			double c = -A / 2.0f + C / 2.0f;
			double d = B;

			return a*t*t*t + b*t*t + c*t + d;
		}

		double *m_x, *m_y;
		unsigned int m_n;

	};

}
