#include <types.hlsl>

// PixelShader
//------------------------------------------------------------------------------------
PixelOutType mainPixel(PixelInputType input)
{
	PixelOutType output;
	output.normal = input.normal;
	output.view = input.view;
	//output.diff = input.color0;
	//output.diff = float4(1,0,0,0);
	output.diff = .5 + input.normal / 2;
	return output;
}
