#include <types.hlsl>
#include <common.hlsl>

#include <perlin.hlsl>

Texture1D texFFT;

#ifndef PI
#define PI (4 * atan(1))
#endif

// casefully measured zero point 
#define ZERO_POINT (-0.69278611)

Texture2D tex_age; // age, size
Texture2D tex_velocity;
Texture2D tex_position;
Texture2D tex_color;

Texture2D texPalette1;
Texture2D texPaletteDecay;

Texture1D texInitialPos;

cbuffer SceneParams
{
    SceneParamType sceneParams;
};

cbuffer ParticleEngineParams
{
    int dynamicsElemCount;
    float dynamicsMaxAge;
    float dynamicsEmitRate;

    struct DynamicParams_t
    {
        float4 pos;
        int type;
        float strength;
        float _p1, _p2, _p3;
        float4 param1;
        float4 param2;
    } dynamics[24];
}

struct ModelOutput
{
    float4 age;
    float4 velocity;
    float4 position;
    float4 color;
};


SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};


float ADSR(float attack, float decay, float sustain, float sustain_time, float release, float time)
{
	// Attack
    if (attack > 0)
    {
        if (time < attack)
            return lerp(0, 1, smoothstep(0, 1, saturate(time / attack)));
        else if (time < attack + decay + sustain_time)
            return lerp(1, sustain, smoothstep(0, 1, saturate((time - attack) / decay)));
        return lerp(sustain, 0, smoothstep(0, 1, saturate((time - attack - decay - sustain_time) / release)));
        return 0;
    }
    return 0.;
}


ModelOutput ParticleCompute(FXPixelInputType input) : SV_TARGET
{
    ModelOutput output;

    float2 uv = input.tex;
    float age = tex_age.Sample(SampleType, uv).x;
	
    int2 particleMapDimm = int2(0, 0);
    tex_age.GetDimensions(particleMapDimm.x, particleMapDimm.y);
    int2 particleAddr = uv * particleMapDimm;
    int particleID = particleAddr.x + particleMapDimm.x * particleAddr.y;
	
    output.age = float4(0, 0, 0, 0);
    output.velocity = float4(0, 0, 0, 0);
    output.position = float4(0, 0, 0, 0);
    output.color = float4(0, 0, 0, 0);

    float speed = 5;
    float size = .25;
    float4 velocity = tex_velocity.Sample(SampleType, uv);
    float4 pos = tex_position.Sample(SampleType, uv);

    float brownianSpeed = 5;
    float3 brownian = ComputeCurl3(pos.xyz * .001);

    //if (time < 0.001)
    if (sceneParams.frameCount < 1)
    {
        float x = fbm(uv);
        float y = fbm(uv + float2(0, 1));
        float z = fbm(uv + float2(1, 0));

        float h = fbm(uv + float2(1, 1));

        pos.xyz = normalize(float3(x, y, z)) * (256 + 128 * h);
        age = 0;
    }
    else
    {
        float3 attractorPos = float3(0, 10, 0);

        age += sceneParams.deltaTime;

        float force = .1;

        float3 deltaPos = pos.xyz - attractorPos;
        float3 direction = normalize(deltaPos) * force;
        //float3 delataVel = direction - velocity.xyz;

        float accel = 1.;
        velocity.xyz += direction * force;
    }
	
    velocity.xyz = velocity.xyz * speed;
    velocity.w = 0.;

    output.velocity = velocity;

    pos = pos + velocity * sceneParams.deltaTime;

    pos.w = 1.;
    output.position = pos;

    output.age.x = age;
    output.age.y = size;
    
	// ... 
    output.color = float4(uv.x, uv.y, 0, 1);

    return output;
}
