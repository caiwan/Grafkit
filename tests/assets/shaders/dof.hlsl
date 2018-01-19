/* 

depth of field with hexagonal kernel - by musk

That Pipeline:

            --> p1 --> p2 --
          /                  \
--> p0 --+----> p3 --> p4 ----+--> p7 -->
          \                  /
            --> p5 --> p6 --


1) Compute the circle of confusion:

	color & depth -> p0_comp_blur_amount.hlsl

2) Separable blur 3 times:

	p0 -> p1 -> p2
	p0 -> p3 -> p4
	p0 -> p5 -> p6

3) Combine them to form a hexagonal kernel

	p2 & p4 & p6 -> p7_combine.hlsl

    
Angle values for ea. blur pass ((x*pi) / 3)

   | 1      | 2      | 3 
---+--------+--------+--------  
 0 | 1 (p1) | 3 (p3) | 5 (p5) 
 1 | 3 (p2) | 5 (p4) | 1 (p6)
   
*/

SamplerState sm:register(s1);

Texture2D depth:register(t1); // depth input buffer
Texture2D input:register(t0); // color input buffer

Texture2D input1:register(t0); // result of p2_blur11
Texture2D input2:register(t1); // result of p4_blur21
Texture2D input3:register(t2); // result of p6_blur31

#define pi (3.14159265359)
#define angle(x) (pi/3*(x))
#define direction(x) (float2(cos(angle(x)),sin(angle(x))*(1.6/0.9)))
#define sampleCount 10

#define distanceUnit (16.0)

cbuffer CircleOfConfusionParams : register(b1)
{
	//https://en.wikipedia.org/wiki/Circle_of_confusion
	float Aperture;
	float Focus; // multiplied with distanceUnit (see above)
	float Limit; // blur limit, screen space
}

cbuffer BlurParams : register(b1)
{
	float Aperture;
	float Focus;
    float angle;
}

float getDepth(float2 uv) { //return depth of pixel
	return depth.Sample(sm, uv).z;
}

// p0

float4 calculateBlurAmount(float4 PositionSS : SV_Position, float2 t : TEXCOORD) : SV_TARGET0
{
	float4 c = input.Sample(sm,t);
	float limit = Limit;
	float aperture = Aperture*Aperture;
	limit = limit*limit;

	float f = Focus*distanceUnit;
	float Object = getDepth(t);
	float Result = aperture*abs(Object - f) / Object;

	c.w = min(Result,limit); //pack color and coc into a pixel

	return c; //send it away to the next pass
}

// p1 .. p6

float4 blur(float4 PositionSS : SV_Position, float2 t : TEXCOORD) : SV_TARGET0
{
	float4 c = input.Sample(sm,t);
	float4 acc = 0;

	for (float i = 0.5; i<sampleCount; i++) {
		float fi = float(i) / sampleCount;
		float2 offset = direction*fi;
		float4 s = input.Sample(sm,t + offset*c.w);
		acc += s;
	}
	acc /= sampleCount;
	return acc;
}

// p7 

float4 combine(float4 PositionSS : SV_Position, float2 t : TEXCOORD) : SV_TARGET0
{
	float4 acc = 0;
	acc += input1.Sample(sm,t);
	acc += input2.Sample(sm,t);
	acc += input3.Sample(sm,t);
	acc *= 0.33;
	return acc;
}
