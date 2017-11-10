#include <types.hlsl>
#include <common.hlsl>

Texture2D tex_age;
Texture2D tex_velocity;
Texture2D tex_speed;
Texture2D tex_position;

struct ModelOutput
{
    float4 age;
    float4 velocity;
    float4 speed;
    float4 position;
};

SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float2 random2(float2 c)
{
    float j = 4906.0 * sin(dot(c, float2(169.7, 5.8)));
    float2 r;
    r.x = frac(512.0 * j);
    j *= .125;
    r.y = frac(512.0 * j);
    return r - 0.5;
}


float hash(float f)
{
    return frac(56546.235423 * sin(f * 235345.345345));
}

const float F2 = 0.3660254;
const float G2 = -0.2113249;

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

// perlin noise
float fbm(float2 n)
{
    float total = 0.0, amplitude = 1.0;
    for (int i = 0; i < 9; i++)
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

ModelOutput LiquidCompute(FXPixelInputType input) : SV_TARGET
{
    ModelOutput output;

    float2 uv = input.tex;
    float age = tex_age.Sample(SampleType, uv).r;
	
    float iTime = age;
    float t = iTime * .2; // let globally control the time	
    output.velocity.x = fbm2(uv, t, .1, .7, .6);
    output.velocity.y = fbm2(uv, t, .2, .5, .4);
    output.velocity.z = fbm2(uv, t, .4, .3, .2);
    output.velocity.w = 1.;

    float4 speed = tex_speed.Sample(SampleType, uv);
    output.speed = speed + output.velocity;
    output.speed.w = 1.;

    float4 position = tex_position.Sample(SampleType, uv);
    output.position = position + output.speed;
    output.position.w = 1;
    
    age += 1 * .25 * pow(length(output.speed), 2) * 0.0625 * pow(length(output.velocity),4);

    if (age >= 1000.)
    {		
        age = 0;
        output.position = float4(0, 0, 0, 1);
        output.speed = float4(0, 0, 0, 1);
        output.velocity = float4(0, 0, 0, 1);

    }
    output.age = float4(age, age, age, 1);

    return output;
    }