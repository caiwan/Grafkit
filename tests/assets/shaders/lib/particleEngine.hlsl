struct DynamicParams_t
{
    float4 position;
    float4 param0;
    float4 param1;
    float4 param2;
};

struct DynamicElem_t
{
    int type;
    float weight;
    DynamicParams_t params;
};

struct Particle
{
    float age;
    float size;
    float4 position;
    float4 velocity;
    float4 acceleration;
};

// calculates an attractor 
// pos - position of attractor
// param0.x - amunt of force that pushes particles toward
float4 attractor(DynamicParams_t params, Particle particle)
{
    float force = params.param0.x;

    float4 deltaPos = params.position - particle.position;

    float4 direction = normalize(deltaPos);
    float4 deltaSpeed = direction * force * length(deltaPos);
    float4 deltaVel = deltaSpeed - particle.velocity;

    return deltaVel;
}
