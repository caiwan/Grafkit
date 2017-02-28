
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float4 normal :	NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;
	float2 tex : TEXCOORD;
};

[maxvertexcount(12)]
void mainGeometry( triangle VertexInputType input[3], inout TriangleStream<VertexInputType> TriStream )
{
	VertexInputType output;
    
	for (int j = 0; j < 2; j++) {
		float k = -1. + 2. * j;
		for (int i = 0; i < 3; i++)
		{
			output = input[i];
			output.position = input[i].position * k;

			TriStream.Append(output);
		}
		TriStream.RestartStrip();
	}
}
