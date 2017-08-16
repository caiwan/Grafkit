#include <types.hlsl>


cbuffer MatrixBuffer {
    WorldMatrices matrices; 
}

float4 mainPixel(FXPixelInputType input) : SV_TARGET
{
    return float4(input.tex.x,input.tex.y,0,1);
}