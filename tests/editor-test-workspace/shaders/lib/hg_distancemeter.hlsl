//
// scale-invariante distance meter
//
// be less confused by your distances!
//
// (as seen in the screenshots on http://mercury.sexy/hg_sdf/)
//
// tries to paint a plane in useful colors. outputs HDR colors, so can easily
// be integrated into your existing rendering without looking bad. 
//
//

// constants and stuff

#ifndef PI
#define PI acos(-1.)
#endif

#define INFINITY pow(2.,8.)


// tone mapping stolen from zavie's shader
float3 Uncharted2ToneMapping(float3 color)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    float exposure = 2.;
    color *= exposure;
    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
    color /= white;
    return color;
}

//
// interesting part starts here
//
// the meter uses the "fusion" gradient, which goes from dark magenta (0) to white (1)
// (often seen in heatmaps in papers etc)
//

float3 fusion(float x)
{
    float t = clamp(x, 0.0, 1.0);
    return clamp(float3(sqrt(t), t * t * t, max(sin(PI * 1.75 * t), pow(t, 12.0))), 0.0, 1.0);
}

// HDR version
float3 fusionHDR(float x)
{
    float t = clamp(x, 0.0, 1.0);
    return fusion(sqrt(t)) * (0.5 + 2. * t);
}


//
// distance meter function. needs a bit more than just the distance
// to estimate the zoom level that it paints at.
//
// if you have real opengl, you can additionally use derivatives (dFdx, dFdy)
// to detect discontinuities, i had to strip that for webgl
//
// visualizing the magnitude of the gradient is also useful
//

float3 distanceMeter(float dist, float rayLength, float3 rayDir, float camHeight)
{
    float idealGridDistance = 20.0 / rayLength * pow(abs(rayDir.y), 0.8);
    float nearestBase = floor(log(idealGridDistance) / log(10.));
    float relativeDist = abs(dist / camHeight);
    
    float largerDistance = pow(10.0, nearestBase + 1.);
    float smallerDistance = pow(10.0, nearestBase);

   
    float3 col = fusionHDR(log(1. + relativeDist));
    col = max(float3(0., 0., 0.), col);
    if (sign(dist) < 0.)
    {
        col = col.grb * 3.;
    }

    float l0 = (pow(0.5 + 0.5 * cos(dist * PI * 2. * smallerDistance), 10.0));
    float l1 = (pow(0.5 + 0.5 * cos(dist * PI * 2. * largerDistance), 10.0));
    
    float x = frac(log(idealGridDistance) / log(10.));
    l0 = lerp(l0, 0., smoothstep(0.5, 1.0, x));
    l1 = lerp(0., l1, smoothstep(0.0, 0.5, x));

    col.rgb *= 0.1 + 0.9 * (1. - l0) * (1. - l1);
    return col;
}


//
// interesting part ends here
//

#if 0

float vmax(float3 v) {
	return max(max(v.x, v.y), v.z);
}

float fBox(float3 p, float3 b) {
	float3 d = abs(p) - b;
	return length(max(d, float3(0.))) + vmax(min(d, float3(0.)));
}

void pR(inout float2 p, float a) {
	p = cos(a)*p + sin(a)*float2(p.y, -p.x);
}


// the actual SDF:
float f (float3 p) {
    p.z += 1.0;
    float sphere = length(p) - 1.;   
    float box = fBox(p - float3(1.0,0.0,1.0), float3(1.0,1.0,1.0));
    
    return max(-sphere,box);
}

void mainImage( out float4 fragColor, in float2 fragCoord ) {
    float2 screenCoord = (-iResolution.xy + 2.0*fragCoord.xy)/iResolution.y;
    float2 m = iMouse.xy/iResolution.xy;

    float camDolly = iMouse.z>0.?m.y:(0.5+0.5*sin(iTime*0.5));
    float camRotation = iMouse.z>0.?m.x*-5.:(0.2*iTime);
    float3 cam_pos = float3(0.0,1.0,1.4) * pow(10., camDolly*4. - 2.0);
    float3 cam_dir = normalize(float3(0.0,-0.8,-1.0));
    float3 cam_up = normalize(float3(0.0,1.0,-0.8));
    
    pR(cam_up.xz,camRotation);
    pR(cam_dir.xz,camRotation);
    pR(cam_pos.xz,camRotation);
    
    float cam_fov = 45.0;
    float3 u = cross(cam_dir, cam_up);
	float3 v = cam_up;
	float3 w = -cam_dir;

	float3 o = cam_pos;
	
	float3 d = float3(screenCoord, -1.0/tan(cam_fov*PI/360.0));
    d = normalize(((mat3(u, v, w)) * d));
    
    float t = INFINITY;
    if (d.y < 0.) {
		t = (o.y)/-d.y;
	}
   
    float3 p = d*t + o;
    
    float dist = f(p);
    
    float3 col = distanceMeter(dist, t, d, cam_pos.y);
    
    col = Uncharted2ToneMapping(col);
    
    fragColor = float4(pow(col, float3(1./2.2)), 1.0 );
}

#endif