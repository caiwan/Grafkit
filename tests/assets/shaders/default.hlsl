#include "commons/projection.hlsl"
#include "commons/lightmaterial.hlsl"

PixelInputType mainVertex(VertexInputType input)
{
	PixelInputType output = projectScene(input);
	return output;
}
