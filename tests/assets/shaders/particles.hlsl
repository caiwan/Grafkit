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
    float age = tex_age.Sample(SampleType, uv).x;
	
    int2 particleMapDimm = int2(0, 0);
    tex_age.GetDimensions(particleMapDimm.x, particleMapDimm.y);
    int2 particleAddr = uv * particleMapDimm;
    int particleID = particleAddr.x + particleMapDimm.x * particleAddr.y;
	
    output.age = float4(0, 0, 0, 0);
    output.acceleration = float4(0, 0, 0, 0);
    output.velocity = float4(0, 0, 0, 0);
    output.position = float4(0, 0, 0, 1);
    output.color = float4(0, 0, 0, 0);

    float speed = 5.;
    float size = .25;
    float4 acceleration = tex_acceleration.Sample(SampleType, uv);
    float4 velocity = tex_velocity.Sample(SampleType, uv);
    float4 pos = tex_position.Sample(SampleType, uv);

    float brownianSpeed = 3;
    float4 brownian = ComputeCurl3((pos.xyz + float3(0,sceneParams.globalTime,0)) * .001);

    float4 sumAccel = float4(0, 0, 0, 0);

    if (sceneParams.frameCount < 2)
    {
        float x = fbm(uv);
        float y = fbm(uv + float2(0, 1));
        float z = fbm(uv + float2(1, 0));

        float h = fbm(uv + float2(1, 1));

        pos.xyz = normalize(float3(x, y, z)) * (256 + 128 * h);
        age = 0;

        velocity.xyz = float3(0, 0, 0);
        acceleration.xyz = float3(0, 0, 0);
    }
    else
    {
        float4 attractorPos = float4(0, 400, 0, 1);

        age += sceneParams.deltaTime;

        float4 deltaPos = attractorPos - pos;
        
        float force = .1;

        float4 direction = normalize(deltaPos);
        float4 deltaSpeed = direction * force * length(deltaPos);
        float4 deltaVel = deltaSpeed - velocity;

        float accel = 1.25;
        sumAccel += accel * deltaVel;
    }

    float accelDecay = .002 * sceneParams.deltaTime;

    output.acceleration += accelDecay * acceleration;
    output.acceleration += sumAccel;
    output.acceleration.w = 0.;

    output.velocity += (velocity + output.acceleration * speed* sceneParams.deltaTime);
    output.velocity += brownian * brownianSpeed;
    velocity.w = 0.;

    output.position += pos;
    output.position += output.velocity * sceneParams.deltaTime;
    output.position.w = 1.;

    output.age.x = age;
    output.age.y = size;
    
	// ... 
    output.color = float4(uv.x, uv.y, 0, 1);

    return output;
}
