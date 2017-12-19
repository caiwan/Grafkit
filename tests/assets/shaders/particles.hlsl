#include <particleEngine.hlsl>
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
Texture2D tex_acceleration;
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

    DynamicElem_t dynamics[24];
}

struct ModelOutput
{
    float4 age;
    float4 acceleration;
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
	
    int2 particleMapDimm = int2(0, 0);
    tex_age.GetDimensions(particleMapDimm.x, particleMapDimm.y);
    int2 particleAddr = uv * particleMapDimm;
    int particleID = particleAddr.x + particleMapDimm.x * particleAddr.y;
	
    float speed = 5.;

    output.age = float4(0, 0, 0, 0);
    output.acceleration = float4(0, 0, 0, 0);
    output.velocity = float4(0, 0, 0, 0);
    output.position = float4(0, 0, 0, 1);
    output.color = float4(0, 0, 0, 0);

    Particle particle;

    particle.age = tex_age.Sample(SampleType, uv).x;
    particle.size = .25;
    particle.acceleration = tex_acceleration.Sample(SampleType, uv);
    particle.velocity = tex_velocity.Sample(SampleType, uv);
    particle.position = tex_position.Sample(SampleType, uv);

    float brownianSpeed = 3;
    float4 brownian = ComputeCurl3((particle.position.xyz + float3(0, sceneParams.globalTime, 0)) * .001);

    float4 sumAccel = float4(0, 0, 0, 1);

    if (sceneParams.frameCount < 2)
    {
        float x = fbm(uv);
        float y = fbm(uv + float2(0, 1));
        float z = fbm(uv + float2(1, 0));

        float h = fbm(uv + float2(1, 1));

        particle.position.xyz = normalize(float3(x, y, z)) * (256 + 128 * h);
        particle.age = 0;

        particle.velocity = float4(0, 0, 0, 0);
        particle.acceleration = float4(0, 0, 0, 0);
    }
    else
    {
        DynamicParams_t params;

        params.position = float4(0, 400, 0, 1);
        params.param0 = float4(.1, 0, 0, 0);
        params.param1 = float4(0, 0, 0, 0);
        params.param2 = float4(0, 0, 0, 0);

        sumAccel += 5 * 1.25 * attractor(params, particle);

        params.position = float4(0, -400, 0, 1);

        sumAccel += 5 * 1.25 * attractor(params, particle);

    }

    float accelDecay = .002 * sceneParams.deltaTime;

    output.acceleration += accelDecay * particle.acceleration;
    output.acceleration += sumAccel;
    output.acceleration.w = 0.;

    output.velocity += (particle.velocity + output.acceleration * sceneParams.deltaTime);
    output.velocity += brownian * brownianSpeed;
    output.velocity.w = 0.;

    output.position += particle.position;
    output.position += output.velocity * speed * sceneParams.deltaTime;
    output.position.w = 1.;

    output.age.x = particle.age + sceneParams.deltaTime;
    output.age.y = particle.size;
    
	// ... 
    output.color = float4(uv.x, uv.y, 0, 1);

    return output;
}
