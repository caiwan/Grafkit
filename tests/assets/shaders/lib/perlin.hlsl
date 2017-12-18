
#define FBM_OCTAVE_MAX 9

float2 random2(float2 c)
{
    float j = 4906.0 * sin(dot(c, float2(169.7, 5.8)));
    float2 r;
    r.x = frac(512.0 * j);
    j *= .125;
    r.y = frac(512.0 * j);
    return r - 0.5;
}

float taylorInvSqrt(float r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

float4 taylorInvSqrt(float4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

// 1 / 289
#define NOISE_SIMPLEX_1_DIV_289 0.00346020761245674740484429065744f

float mod289(float x)
{
    return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
}

float2 mod289(float2 x)
{
    return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
}

float3 mod289(float3 x)
{
    return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
}

float4 mod289(float4 x)
{
    return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
}


// ( x*34.0 + 1.0 )*x = 
// x*x*34.0 + x
float permute(float x)
{
    return mod289(
		x * x * 34.0 + x
	);
}

float3 permute(float3 x)
{
    return mod289(
		x * x * 34.0 + x
	);
}

float4 permute(float4 x)
{
    return mod289(
		x * x * 34.0 + x
	);
}

float hash(float f)
{
    return frac(56546.235423 * sin(f * 235345.345345));
}

const float F2 = 0.3660254; // skew coordinate     F = (sqrt(n+1)-1) / n
const float G2 = -0.2113249; // unskew coordinate  G = (1-1/(sqrt(n+1))) / n

const float F3 = 0.3333333; // F = (sqrt(n+1)-1) / n
const float G3 = -0.1666666; // G = (1-1/(sqrt(n+1))) / n

// perlin noise for a paricular frequency 
float simplex2d(float2 p)
{
    float2 s = floor(p + (p.x + p.y) * F2), x = p - s - (s.x + s.y) * G2;
    float e = step(0.0, x.x - x.y);
    float2 i1 = float2(e, 1.0 - e), x1 = x - i1 - G2, x2 = x - 1.0 - 2.0 * G2;
    float3 w, d;
    w.x = dot(x, x);
    w.y = dot(x1, x1);
    w.z = dot(x2, x2);
    w = max(0.5 - w, 0.0);
    d.x = dot(random2(s + 0.0), x);
    d.y = dot(random2(s + i1), x1);
    d.z = dot(random2(s + 1.0), x2);
    w *= w;
    w *= w;
    d *= w;
    return dot(d, float3(70.0, 70.0, 70.0));
}

float simplex3d(float3 v)
{
    const float2 C = float2(
		F3, // 1/6
		G3 // 1/3
	);
    const float4 D = float4(0.0, 0.5, 1.0, 2.0);
	
// First corner
    float3 i = floor(v + dot(v, C.yyy));
    float3 x0 = v - i + dot(i, C.xxx);
	
// Other corners
    float3 g = step(x0.yzx, x0.xyz);
    float3 l = 1 - g;
    float3 i1 = min(g.xyz, l.zxy);
    float3 i2 = max(g.xyz, l.zxy);
	
    float3 x1 = x0 - i1 + C.xxx;
    float3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
    float3 x3 = x0 - D.yyy; // -1.0+3.0*C.x = -0.5 = -D.y
	
// Permutations
    i = mod289(i);
    float4 p = permute(
		permute(
			permute(
					i.z + float4(0.0, i1.z, i2.z, 1.0)
			) + i.y + float4(0.0, i1.y, i2.y, 1.0)
		) + i.x + float4(0.0, i1.x, i2.x, 1.0)
	);
	
// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    float n_ = 0.142857142857; // 1/7
    float3 ns = n_ * D.wyz - D.xzx;
	
    float4 j = p - 49.0 * floor(p * ns.z * ns.z); // mod(p,7*7)
	
    float4 x_ = floor(j * ns.z);
    float4 y_ = floor(j - 7.0 * x_); // mod(j,N)
	
    float4 x = x_ * ns.x + ns.yyyy;
    float4 y = y_ * ns.x + ns.yyyy;
    float4 h = 1.0 - abs(x) - abs(y);
	
    float4 b0 = float4(x.xy, y.xy);
    float4 b1 = float4(x.zw, y.zw);
	
	//float4 s0 = float4(lessThan(b0,0.0))*2.0 - 1.0;
	//float4 s1 = float4(lessThan(b1,0.0))*2.0 - 1.0;
    float4 s0 = floor(b0) * 2.0 + 1.0;
    float4 s1 = floor(b1) * 2.0 + 1.0;
    float4 sh = -step(h, 0.0);
	
    float4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
    float4 a1 = b1.xzyw + s1.xzyw * sh.zzww;
	
    float3 p0 = float3(a0.xy, h.x);
    float3 p1 = float3(a0.zw, h.y);
    float3 p2 = float3(a1.xy, h.z);
    float3 p3 = float3(a1.zw, h.w);
	
//Normalise gradients
    float4 norm = taylorInvSqrt(float4(
		dot(p0, p0),
		dot(p1, p1),
		dot(p2, p2),
		dot(p3, p3)
	));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;
	
// Mix final noise value
    float4 m = max(
		0.6 - float4(
			dot(x0, x0),
			dot(x1, x1),
			dot(x2, x2),
			dot(x3, x3)
		),
		0.0
	);
    m = m * m;
    return 42.0 * dot(
		m * m,
		float4(
			dot(p0, x0),
			dot(p1, x1),
			dot(p2, x2),
			dot(p3, x3)
		)
	);
}

// perlin noise
float fbm(float2 n)
{
    float total = 0.0, amplitude = 1.0;
    for (int i = 0; i < FBM_OCTAVE_MAX; i++)
    {
        total += simplex2d(n) * amplitude;
        n += n;
        amplitude *= 0.5;
    }
    return total;
}

// creates some funky perlin noise offseted by the given coefficients
float fbm2(float2 uv, float t, float u0, float v0, float w0)
{
    float offset = fbm(uv + t * u0);
    float2 value = float2(fbm(uv + offset + t * v0), fbm(uv + offset - t * w0));
    return sin(value.x) * value.y;
}

float fbm3(float3 n)
{
    float total = 0.0, amplitude = 1.0;
    for (int i = 0; i < FBM_OCTAVE_MAX; i++)
    {
        total += simplex3d(n) * amplitude;
        n += n;
        amplitude *= 0.5;
    }
    return total;
}

float fbm32(float3 uvw, float t, float u0, float v0, float w0)
{
    float offset = fbm3(uvw + t * u0);
    float2 value = float2(fbm3(uvw + offset + t * v0), fbm3(uvw + offset - t * w0));
    return sin(value.x) * value.y;
}


float2 ComputeCurl(float2 p)
{
    float eps = 1.0;
    float n1, n2, a, b;

    n1 = fbm(float2(p.x, p.y + eps));
    n2 = fbm(float2(p.x, p.y - eps));
    a = (n1 - n2) / (2 * eps);

    n1 = fbm(float2(p.x + eps, p.y));
    n2 = fbm(float2(p.x - eps, p.y));
    b = (n1 - n2) / (2 * eps);

    float2 curl = float2(a, -b);
    return curl;
}


float3 ComputeCurl3(float3 p)
{
    float eps = 1.0 / float(FBM_OCTAVE_MAX);
    float n1, n2, a, b;
    float x = p.x, y = p.y, z = p.z;
    float3 curl;

    n1 = fbm3(float3(p.x, p.y + eps, p.z));
    n2 = fbm3(float3(p.x, p.y - eps, p.z));
    a = (n1 - n2) / (2 * eps);

    n1 = fbm3(float3(p.x, p.y, p.z + eps));
    n2 = fbm3(float3(p.x, p.y, p.z - eps));
    b = (n1 - n2) / (2 * eps);

    curl.x = a - b;

    n1 = fbm3(float3(p.x, p.y, p.z + eps));
    n2 = fbm3(float3(p.x, p.y, p.z - eps));
    a = (n1 - n2) / (2 * eps);

    n1 = fbm3(float3(p.x + eps, p.y, p.z));
    n2 = fbm3(float3(p.x + eps, p.y, p.z));
    b = (n1 - n2) / (2 * eps);
    
	curl.y = a - b;

    n1 = fbm3(float3(p.x + eps, p.y, p.z));
    n2 = fbm3(float3(p.x - eps, p.y, p.z));
    a = (n1 - n2) / (2 * eps);

    n1 = fbm3(float3(p.x, p.y + eps, p.z));
    n2 = fbm3(float3(p.x, p.y - eps, p.z));
    b = (n1 - n2) / (2 * eps);

    curl.z = a - b;

    return curl;
}