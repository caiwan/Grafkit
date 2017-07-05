#include "spline.h"
#include "../debug/exceptions.h"

using namespace FWmath;
using namespace FWdebug;

#ifdef SPLINE_BROKEN

Spline::Spline(){
	this->lastSeg = 0;
}

Spline::~Spline(){
}

void Spline::calculate(){
		// check buffer size and realign if needed
	int i, j, l, m, n = this->vals.size();

	int nm1 = n-1;
	vec4float t;

	if (n < 2) 
		throw new ExceptionEX(1001, "no possible interpolation");

	if (this->b.size() < n || this->c.size() < n || this->d.size() < n){
		this->b.insert(n - this->b.size());
		this->c.insert(n - this->c.size());
		this->d.insert(n - this->d.size());
	}
	
	// ---

	// kontrol pontok kulcsai novekvo sorrendben vannak-e

	for (i = 1; i < n; ++i) 
		if (keys[i] <= this->keys[i-1]) 
			throw new ExceptionEX(1001, "keys are not in ascending order");

	if (n >= 3){
		/* ---- At least quadratic ---- */
		/* ---- Set up the symmetric tri-diagonal system; b = diagonal, d = offdiagonal, c = right-hand-side  */

		d[0] = keys[1] - keys[0];
		c[1] = (vals[1]- vals[0]) / d[0];

		for (i = 1; i < nm1; ++i){
			d[i]   = keys[i+1] - keys[i];

			b[i]   = (d[i-1] + d[i]) * 2.;

			c[i+1] = (vals[i+1] - vals[i]) / d[i];
			c[i]   = c[i+1] - c[i];
		
		}

		/* --- Default End conditions 
			Third derivatives at x[0] and x[n-1] obtained from divided differences 
		*/
		b[0] = -d[0],  b[nm1] = -d[n-2];
		c[0] =   0.0,  c[nm1] = 0.0;

		if (n != 3){
			c[0]   = c[2]   / (keys[3]   - keys[1]  ) - c[1]   /  (keys[2]   - keys[0]  );
			c[nm1] = c[n-2] / (keys[nm1] - keys[n-3]) - c[n-3] /  (keys[n-2] - keys[n-4]);

			/// @todo check

			c[0]   =  c[0]   * d[0]   * d[0]   / (keys[3]   - keys[0]);
			c[nm1] = -c[nm1] * d[n-2] * d[n-2] / (keys[nm1] - keys[n-4]);
		}

		/// todo
		/* Alternative end conditions -- known slopes */
		
		/*
			end1, end2  : = 1 to specify the slopes at the end points; = 0 to obtain the default conditions
			slope1, slope2  : the slopes at the end points x[0] and x[n-1] respectively
		*/

		/*
			if (end1 == 1)
			{
				b[0] = 2.0 * (x[1] - x[0]);
				c[0] = (y[1] - y[0]) / (x[1] - x[0]) - slope1;
			}

			if (end2 == 1)
			{
				b[nm1] = 2.0 * (x[nm1] - x[n-2]);
				c[nm1] = slope2 - (y[nm1] - y[n-2]) / (x[nm1] - x[n-2]);
			}
		*/

			/* Forward elimination */
			for (i = 1; i < n; ++i){
				t    = d[i-1] / b[i-1];
				b[i] = b[i  ] - t * d[i-1];
				c[i] = c[i  ] - t * c[i-1];
			}

			/* Back substitution */
			c[nm1] = c[nm1] / b[nm1];
			for (j = 0; j < nm1; ++j){
				i    = n - j - 2;
				c[i] = (c[i] - c[i+1] * d[i]) / b[i];
			}

			/* c[i] is now the sigma[i] of the text */

			/* Compute the polynomial coefficients */
			b[nm1] = (vals[nm1] - vals[n-2]) / d[n-2] + d[n-2] * (c[n-2] + c[nm1] * 2.0);
			
			for (i = 0; i < nm1; ++i){
				b[i] = (vals[i+1]- vals[i ]) / d[i  ] - d[i  ] * (c[i+1] + c[i  ] * 2.0);
				d[i] = (c[i+1] - c[i]) / d[i];
				c[i] = c[i] * 3.0;

				/// @todo check
			}

			c[nm1] = c[nm1] * 3.0;
			d[nm1] = d[n-2];

	} else {

		/* linear segment only  */
		b[0] = (vals[1]- vals[0]) / (keys[1] - keys[0]);
		c[0] = 0.0;
		d[0] = 0.0;
		b[1] = b[0];
		c[1] = 0.0;
		d[1] = 0.0;
	}
}

vec4float Spline::eval(float u){
	int    i, j, k;
	vec4float w;

	int n = this->vals.size();

	/// @todo: -> findNearestIndex function

	// --- snip ---
	i = this->lastSeg;
	if (i >= n-1) i = 0;
	if (i < 0)  i = 0;

	if ((keys[i] > u) || (keys[i+1] < u))
	{ 
		/* ---- perform a binary search ---- */
		i = 0, j = n;
		do{
			k = (i + j) / 2;				/* split the domain to search */
			if (u < keys[k])  j = k;		/* move the upper bound */
			if (u >= keys[k]) i = k;		/* move the lower bound */
		}while (j > i+1); /* there are no more segments to search */ 
	}
	this->lastSeg = i;
	// --- snap ---

	// --- eval
		
	w = vec4float(u) - vals[i];
	w = vec4float(keys[i]) + w * (b[i] + w * (c[i] + w * d[i]));

	return (w);

}

vec4float Spline::derivate(float u){
	int    i, j, k;
	vec4float w;

	int n = this->vals.size();

	/// @todo: -> findNearestIndex function

	// --- snip ---
	i = this->lastSeg;
	if (i >= n-1) i = 0;
	if (i < 0)  i = 0;

	if ((keys[i] > u) || (keys[i+1] < u))
	{ 
		/* ---- perform a binary search ---- */
		i = 0, j = n;
		do{
			k = (i + j) / 2;				/* split the domain to search */
			if (u < keys[k])  j = k;		/* move the upper bound */
			if (u >= keys[k]) i = k;		/* move the lower bound */
		}while (j > i+1); /* there are no more segments to search */ 
	}
	this->lastSeg = i;
	// --- snap ---


	// --- eval
	w = vec4float(u) - this->vals[i];
	w = b[i] + w * (c[i] * 2.0 + w * 3.0 * d[i]);

	return (w);
}

vec4float Spline::integrate(float u){
	///@todo
	int    i, j, k;
	vec4float sum, dx;

	int n = this->vals.size();

	/// @todo: -> findNearestIndex function
	// --- snip ---
	i = this->lastSeg;
	if (i >= n-1) i = 0;
	if (i < 0)  i = 0;

	if ((keys[i] > u) || (keys[i+1] < u))
	{ 
		/* ---- perform a binary search ---- */
		i = 0, j = n;
		do{
			k = (i + j) / 2;				/* split the domain to search */
			if (u < keys[k])  j = k;		/* move the upper bound */
			if (u >= keys[k]) i = k;		/* move the lower bound */
		}while (j > i+1); /* there are no more segments to search */ 
	}
	this->lastSeg = i;
	// --- snap ---



	/* ---- Evaluate the integral for segments x < u ---- */
	for (j = 0; j < i; ++j){
		dx = vals[j+1] - vals[j];
		sum += dx *
			(vec4float(keys[j]) + dx *
			(b[j] * 0.5 + dx *
			(c[j] / 3.0 + dx * 0.25 * d[j])));
	}

	dx = vec4float(u) - vals[j];
	sum += dx *
		(vec4float(keys[j]) + dx *
		(b[j] * 0.5 + dx *
		(c[j] / 3.0 + dx * 0.25 * d[j])));

	return (sum);
}

#else /*SPLINE_BROKEN*/


Spline::Spline(){
	this->lastSeg = 0;
	this->lastDerSeg = 0;
	this->lastIntSeg = 0;
}

Spline::Spline(vec4float *data, int len){
	this->lastSeg = 0;
	this->lastDerSeg = 0;
	this->lastIntSeg = 0;

	for(int i=0; i<len; i++){
		this->insertKnotf(vec4float(data[i].xyz(), 0.), data[i].w);
	}
}

Spline::~Spline(){
	// kontenerek maguktol semmisulnek meg
}
			
void Spline::calculate(){
	int n = this->kx.size();
	int res = 0;
	
	checkAllocation();

	/* x */ if (!this->calculate(n, 0, 0, 0., 0., kx.get(), vx.get(), cx.b.get(), cx.c.get(), cx.d.get(), &res) && res) CHECKERROR();
	/* y */ if (!this->calculate(n, 0, 0, 0., 0., kx.get(), vy.get(), cy.b.get(), cy.c.get(), cy.d.get(), &res) && res) CHECKERROR();
	/* z */ if (!this->calculate(n, 0, 0, 0., 0., kx.get(), vz.get(), cz.b.get(), cz.c.get(), cz.d.get(), &res) && res) CHECKERROR();
	/* w */ if (!this->calculate(n, 0, 0, 0., 0., kx.get(), vw.get(), cw.b.get(), cw.c.get(), cw.d.get(), &res) && res) CHECKERROR();

#undef CHECKERROR

	return;
}

vec4float Spline::evalf(float t){
	vec4double r = eval(t);
	return vec4float(r.x, r.y, r.z, r.w);
}

vec4float Spline::derivatef(float t){
	vec4double r = derivate(t);
	return vec4float(r.x, r.y, r.z, r.w);
}

vec4float Spline::integratef(float t){
	vec4double r = integrate(t);
	return vec4float(r.x, r.y, r.z, r.w);
}

// --- 

vec4double Spline::eval(double t){
	vec4double result;
	int n = this->kx.size();
	
	result.x = this->seval(n, t, kx.get(), vx.get(), cx.b.get(), cx.c.get(), cx.d.get(), &this->lastSeg);
	result.y = this->seval(n, t, kx.get(), vy.get(), cy.b.get(), cy.c.get(), cy.d.get(), &this->lastSeg);
	result.z = this->seval(n, t, kx.get(), vz.get(), cz.b.get(), cz.c.get(), cz.d.get(), &this->lastSeg);
	result.w = this->seval(n, t, kx.get(), vw.get(), cw.b.get(), cw.c.get(), cw.d.get(), &this->lastSeg);

	return result;
}

vec4double Spline::derivate(double t){
	vec4double result;
	int n = this->kx.size();
	
	result.x = this->deriv(n, t, kx.get(), cx.b.get(), cx.c.get(), cx.d.get(), &this->lastSeg);
	result.y = this->deriv(n, t, kx.get(), cy.b.get(), cy.c.get(), cy.d.get(), &this->lastSeg);
	result.z = this->deriv(n, t, kx.get(), cz.b.get(), cz.c.get(), cz.d.get(), &this->lastSeg);
	result.w = this->deriv(n, t, kx.get(), cw.b.get(), cw.c.get(), cw.d.get(), &this->lastSeg);

	return result;
}

vec4double Spline::integrate(double t){
	vec4double result;
	int n = this->kx.size();
	
	result.x = this->sinteg(n, t, kx.get(), vx.get(), cx.b.get(), cx.c.get(), cx.d.get(), &this->lastSeg);
	result.y = this->sinteg(n, t, kx.get(), vy.get(), cy.b.get(), cy.c.get(), cy.d.get(), &this->lastSeg);
	result.z = this->sinteg(n, t, kx.get(), vz.get(), cz.b.get(), cz.c.get(), cz.d.get(), &this->lastSeg);
	result.w = this->sinteg(n, t, kx.get(), vw.get(), cw.b.get(), cw.c.get(), cw.d.get(), &this->lastSeg);

	return result;
}	


void Spline::checkAllocation(){
	int n = this->kx.size();

	#define ROKAYUK(K)\
	if (K.b.size() < n || K.c.size() < n || K.d.size() < n){ \
		K.b.insert(n - K.b.size()); \
		K.c.insert(n - K.c.size()); \
		K.d.insert(n - K.d.size()); \
	} 

	ROKAYUK(cx);
	ROKAYUK(cy);
	ROKAYUK(cz);
	ROKAYUK(cw);

	#undef ROKAYUK

}

int Spline::calculate (int n, int end1, int end2, double slope1, double slope2, double x[], double y[], double b[], double c[], double d[], int *iflag){
		int    nm1, ib, i;
		double t;
		int    ascend;

		nm1    = n - 1;
		*iflag = 0;

		if (n < 2)
			{  /* no possible interpolation */
				*iflag = 1;
				return 0;
			}
				
		ascend = 1;
		for (i = 1; i < n; ++i) 
				if (x[i] <= x[i-1]) ascend = 0;
		if (!ascend)
			{
				*iflag = 2;
				return 0;
			}
				
		if (n >= 3)
			{    /* ---- At least quadratic ---- */

			/* ---- Set up the symmetric tri-diagonal system
							b = diagonal
							d = offdiagonal
							c = right-hand-side  */
			d[0] = x[1] - x[0];
			c[1] = (y[1] - y[0]) / d[0];
			for (i = 1; i < nm1; ++i)
					{
					d[i]   = x[i+1] - x[i];
					b[i]   = 2.0 * (d[i-1] + d[i]);
					c[i+1] = (y[i+1] - y[i]) / d[i];
					c[i]   = c[i+1] - c[i];
					}

			/* ---- Default End conditions
							Third derivatives at x[0] and x[n-1] obtained
							from divided differences  */
			b[0]   = -d[0];
			b[nm1] = -d[n-2];
			c[0]   = 0.0;
			c[nm1] = 0.0;
			if (n != 3)
					{
					c[0]   = c[2] / (x[3] - x[1]) - c[1] / (x[2] - x[0]);
					c[nm1] = c[n-2] / (x[nm1] - x[n-3]) - c[n-3] / (x[n-2] - x[n-4]);
					
					c[0]   = c[0] * d[0] * d[0] / (x[3] - x[0]);
					c[nm1] = -c[nm1] * d[n-2] * d[n-2] / (x[nm1] - x[n-4]);
					}

			/* Alternative end conditions -- known slopes */
			if (end1 == 1)
					{
					b[0] = 2.0 * (x[1] - x[0]);
					c[0] = (y[1] - y[0]) / (x[1] - x[0]) - slope1;
					}
			if (end2 == 1)
					{
					b[nm1] = 2.0 * (x[nm1] - x[n-2]);
					c[nm1] = slope2 - (y[nm1] - y[n-2]) / (x[nm1] - x[n-2]);
					}

			/* Forward elimination */
			for (i = 1; i < n; ++i)
					{
					t    = d[i-1] / b[i-1];
					b[i] = b[i] - t * d[i-1];
					c[i] = c[i] - t * c[i-1];
					}

			/* Back substitution */
			c[nm1] = c[nm1] / b[nm1];
			for (ib = 0; ib < nm1; ++ib)
					{
					i    = n - ib - 2;
					c[i] = (c[i] - d[i] * c[i+1]) / b[i];
					}

			/* c[i] is now the sigma[i] of the text */

			/* Compute the polynomial coefficients */
			b[nm1] = (y[nm1] - y[n-2]) / d[n-2] + d[n-2] * (c[n-2] + 2.0 * c[nm1]);
			for (i = 0; i < nm1; ++i)
					{
					b[i] = (y[i+1] - y[i]) / d[i] - d[i] * (c[i+1] + 2.0 * c[i]);
					d[i] = (c[i+1] - c[i]) / d[i];
					c[i] = 3.0 * c[i];
					}
			c[nm1] = 3.0 * c[nm1];
			d[nm1] = d[n-2];

			}  /* at least quadratic */

		else  /* if n >= 3 */
			{  /* linear segment only  */
			b[0] = (y[1] - y[0]) / (x[1] - x[0]);
			c[0] = 0.0;
			d[0] = 0.0;
			b[1] = b[0];
			c[1] = 0.0;
			d[1] = 0.0;
			}

		return 1;
}

double Spline::seval  (int n, double u, double x[], double y[], double b[], double c[], double d[], int *last){
		int    i, j, k;
		double w;

		// -- snip
		i = *last;
		if (i >= n-1) i = 0;
		if (i < 0)  i = 0;

		if ((x[i] > u) || (x[i+1] < u))
			{  /* ---- perform a binary search ---- */
			i = 0;
			j = n;
			do
				{
				k = (i + j) / 2;         /* split the domain to search */
				if (u < x[k])  j = k;    /* move the upper bound */
				if (u >= x[k]) i = k;    /* move the lower bound */
				}                        /* there are no more segments to search */
			while (j > i+1);
			}
		*last = i;
		// -- snap

		/* ---- Evaluate the spline ---- */
		w = u - x[i];
		w = y[i] + w * (b[i] + w * (c[i] + w * d[i]));
		return (w);
}

double Spline::deriv  (int n, double u, double x[], double b[], double c[], double d[], int *last){
	int    i, j, k;
	double w;

	// --- snip
	i = *last;
	if (i >= n-1) i = 0;
	if (i < 0) i = 0;

	if ((x[i] > u) || (x[i+1] < u))
	{  /* ---- perform a binary search ---- */
		i = 0;
		j = n;
		do
			{
			k = (i + j) / 2;          /* split the domain to search */
			if (u < x[k])  j = k;     /* move the upper bound */
			if (u >= x[k]) i = k;     /* move the lower bound */
			}                         /* there are no more segments to search */
		while (j > i+1);
	}
	*last = i;
	// --- snap

	/* ---- Evaluate the derivative ---- */
	w = u - x[i];
	w = b[i] + w * (2.0 * c[i] + w * 3.0 * d[i]);
	return (w);
}

double Spline::sinteg (int n, double u, double x[], double y[], double b[], double c[], double d[], int *last){
	int    i, j, k;
	double sum, dx;

	// --- snip
	i = *last;
	if (i >= n-1) i = 0;
	if (i < 0)  i = 0;

	if ((x[i] > u) || (x[i+1] < u))
	{  /* ---- perform a binary search ---- */
		i = 0;
		j = n;
		do
			{
			k = (i + j) / 2;         /* split the domain to search */
			if (u < x[k])  j = k;    /* move the upper bound */
			if (u >= x[k]) i = k;    /* move the lower bound */
			}                        /* there are no more segments to search */
		while (j > i+1);
	}
	*last = i;
	// --- snap

	sum = 0.0;
	/* ---- Evaluate the integral for segments x < u ---- */
	for (j = 0; j < i; ++j){
		dx = x[j+1] - x[j];
		sum += dx *
			(y[j] + dx *
			(0.5 * b[j] + dx *
			(c[j] / 3.0 + dx * 0.25 * d[j])));
	}

	/* ---- Evaluate the integral fot this segment ---- */
	dx = u - x[i];
	sum += dx *
		(y[i] + dx *
		(0.5 * b[i] + dx *
		(c[i] / 3.0 + dx * 0.25 * d[i])));

	return (sum);
}


#endif /*SPLINE_BROKEN*/