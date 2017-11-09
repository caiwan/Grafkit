#include <types.hlsl>
#include <common.hlsl>

Texture2D age;
Texture2D velocity;
Texture2D speed;
Texture2D position;

struct ModelOutput {
    float4 age;
    float4 velocity;
    float4 speed;
    float4 position;
};

ModelOutput LiquidCompute(FXPixelInputType input) : SV_TARGET
{
    ModelOutput output;
    
    output.age = float4(1,1,1,1);
    output.position = float4(1,0,0,1);
    output.speed = float4(0,1,0,1);
    output.velocity = float4(0,0,1,1);
    
    return output;
}