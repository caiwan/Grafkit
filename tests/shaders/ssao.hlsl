// Ssao shader
// elso iteracio; THREE.js peldabol lenyulva
// ezt a multipass chain elso elemehez kell betenni

#define DL 2.399963229728653  // PI * ( 3.0 - sqrt( 5.0 ) )
#define EULER 2.718281828459045

Texture2D positionMap;
Texture2D normalMap;
Texture2D effectInput;
SamplerState SampleType;

cbuffer camera {
	float cameraNear;
	float cameraFar;
}

cbuffer SSAOparams{
	bool onlyAO;      // use only ambient occlusion pass?

	float2 size;        // texture width, height
	float aoClamp;    // depth clamp - reduces haloing at screen edges

	float lumInfluence;  // how much luminance affects occlusion
}


// user variables

const int samples = 8;     // ao sample count
const float radius = 5.0;  // ao radius

const bool useNoise = false;      // use noise instead of pattern for sample dithering
const float noiseAmount = 0.0003; // dithering amount

const float diffArea = 0.4;   // self-shadowing reduction
const float gDisplace = 0.4;  // gauss bell center

// RGBA depth

//float unpackDepth(const in vec4 rgba_depth) {
//
//	const vec4 bit_shift = vec4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
//	float depth = dot(rgba_depth, bit_shift);
//	return depth;
//
//}

float unpackDepth(float2 coord) {
	return positionMap.Sample(SampleType, coord);
}

// generating noise / pattern texture for dithering

float2 rand(const float2 coord) 
{

	float2 noise;

	if (useNoise) {

		float nx = dot(coord, float2(12.9898, 78.233));
		float ny = dot(coord, float2(12.9898, 78.233) * 2.0);

		noise = clamp(frac(43758.5453 * sin(float2(nx, ny))), 0.0, 1.0);

	}
	else {

		float ff = frac(1.0 - coord.x * (size.x / 2.0));
		float gg = frac(coord.y * (size.y / 2.0));

		noise = float2(0.25, 0.75) * float2(ff,ff)+float2(0.75, 0.25) * gg;

	}

	return (noise * 2.0 - 1.0) * noiseAmount;

}

float readDepth(const in float2 coord) 
{
	float cameraFarPlusNear = camera.cameraFar + camera.cameraNear;
	float cameraFarMinusNear = camera.cameraFar - camera.cameraNear;
	float cameraCoef = 2.0 * camera.cameraNear;

	//  return ( 2.0 * cameraNear ) / ( cameraFar + cameraNear - unpackDepth( texture2D( tDepth, coord ) ) * ( cameraFar - cameraNear ) );
	return cameraCoef / (cameraFarPlusNear - unpackDepth(coord) * cameraFarMinusNear);


}

float compareDepths(const in float depth1, const in float depth2, inout int far) 
{
	float garea = 2.0;                         // gauss bell width
	float diff = (depth1 - depth2) * 100.0;  // depth difference (0-100)

	// reduce left bell width to avoid self-shadowing
	if (diff < gDisplace) {

		garea = diffArea;

	}
	else {

		far = 1;

	}

	float dd = diff - gDisplace;
	float gauss = pow(EULER, -2.0 * dd * dd / (garea * garea));
	return gauss;

}

float calcAO(float depth, float dw, float dh) 
{
	float dd = radius - depth * radius;
	float2 vv = float2(dw, dh);

	float2 coord1 = vUv + dd * vv;
	float2 coord2 = vUv - dd * vv;

	float temp1 = 0.0;
	float temp2 = 0.0;

	int far = 0;
	temp1 = compareDepths(depth, readDepth(coord1), far);

	// DEPTH EXTRAPOLATION

	if (far > 0) {

		temp2 = compareDepths(readDepth(coord2), depth, far);
		temp1 += (1.0 - temp1) * temp2;

	}

	return temp1;

}

// TYPEDEFS //

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD;
};

// MAIN

float4 main(PixelInputType input) : SV_TARGET
{
	float2 noise = rand(input.tex);
	float depth = readDepth(input.tex);

	float tt = clamp(depth, SSAOparams.aoClamp, 1.0);

	float w = (1.0 / SSAOparams.size.x) / tt + (noise.x * (1.0 - noise.x));
	float h = (1.0 / SSAOparams.size.y) / tt + (noise.y * (1.0 - noise.y));

	float ao = 0.0;

	float dz = 1.0 / float(samples);
	float z = 1.0 - dz / 2.0;
	float l = 0.0;

	for (int i = 0; i <= samples; i++) {

		float r = sqrt(1.0 - z);

		float pw = cos(l) * r;
		float ph = sin(l) * r;
		ao += calcAO(depth, pw * w, ph * h);
		z = z - dz;
		l = l + DL;

	}

	ao /= float(samples);
	ao = 1.0 - ao;

	float3 color = texture2D(tDiffuse, vUv).rgb;

	float3 lumcoeff = float3(0.299, 0.587, 0.114);
	float lum = dot(color.rgb, lumcoeff);
	float3 luminance = float3(lum);

	float3 final = float3(color * mix(float3(ao), float3(1.0), luminance * SSAOparams.lumInfluence));  // mix( color * ao, white, luminance )

	if (SSAOparams.onlyAO) {
		final = float3(mix(float3(ao), float3(1.0), luminance * SSAOparams.lumInfluence));  // ambient occlusion only
	}

	return flaot4(final, 1.0);

}