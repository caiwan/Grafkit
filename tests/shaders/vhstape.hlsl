uniform float time;
uniform vec2 resolution;

uniform sampler2D tex0;

uniform float expLevel;
uniform float bloomLevel;
uniform float expBias;

vec4 hdrExpKorrekcioFelold(vec4 v){
	//v = clamp(v, 0, 1.);
	//v *= 255./256.;
	return v/(1.-v);
}

float hash(vec2 co){
    co /= 100.;
	highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

float rand(vec2 cc)
{
	vec2 c0 = floor(cc);
    //vec2 c0 = cc;

	float r00 = hash(c0);
	float r01 = hash(c0 + vec2(1., 0.));
	float r10 = hash(c0 + vec2(0., 1.));
	float r11 = hash(c0 + vec2(1., 1.));

	vec2 cv = fract(cc);
	cv = cv*cv*(3.0-2.0*cv); // fuv*fuv*(3.0-2.0*fuv)
	float r0 = mix(r00, r01, cv.x);
	float r1 = mix(r10, r11, cv.x);

	float r = mix(r0, r1, cv.y);

	return r;
}

float onOff(float a, float b, float c)
{
	return step(c, sin(time + a*cos(time*b)));
}

float ramp(float y, float start, float end)
{
	float inside = step(start,y) - step(end,y);
	float fact = (y-start)/(end-start)*inside;
	return (1.-fact) * inside;
	
}

float stripes(vec2 uv)
{
	
	float noi = rand(uv*vec2(0.5,1.) + vec2(1.,3.));
	return ramp(mod(uv.y*4. + time/2.+sin(time + sin(time*0.63)),1.),0.5,0.6)*noi;
}

vec3 vhsnoise1(vec2 uv)
{
	highp float magnitude = 0.009;

	vec2 offsetRedUV = uv;
	offsetRedUV.x = uv.x + rand(vec2(time*0.03,uv.y*0.42)) * 0.001;
	offsetRedUV.x += sin(rand(vec2(time*0.2, uv.y)))*magnitude;
	
	vec2 offsetGreenUV = uv;
	offsetGreenUV.x = uv.x + rand(vec2(time*0.004,uv.y*0.002)) * 0.004;
	offsetGreenUV.x += sin(time*9.0)*magnitude;
	
	vec2 offsetBlueUV = uv;
	offsetBlueUV.x = uv.y;
	offsetBlueUV.x += rand(vec2(cos(time*0.01),sin(uv.y)));
	
	// Load Texture
	float r = texture2D(iChannel0, offsetRedUV).r;
	float g = texture2D(iChannel0, offsetGreenUV).g;
	float b = texture2D(iChannel0, uv).b;
	
	return vec3(r,g,b);
	
}

vec3 getVideo(vec2 uv)
{
	vec2 look = uv;
	float window = 1./(1.+20.*(look.y-mod(time/4.,1.))*(look.y-mod(time/4.,1.)));
	look.x = look.x + sin(look.y*10. + time)/50.*onOff(4.,4.,.3)*(1.+cos(time*80.))*window;
	float vShift = 0.4*onOff(2.,3.,.9)*(sin(time)*sin(time*20.) + (0.5 + 0.1*sin(time*200.)*cos(time)));
	look.y = mod(look.y + vShift, 1.);
    vec3 video = vhsnoise1(look).rgb;
	return video;
}

vec2 screenDistort(vec2 uv, float amount)
{
	uv -= vec2(.5,.5);
	uv = uv*amount*(1./amount+2.*uv.x*uv.x*uv.y*uv.y);
	uv += vec2(.5,.5);
	return uv;
}



void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
	uv = screenDistort(uv, .95);
	vec3 video = getVideo(uv);
	float vigAmt = 3.+.3*sin(time + 5.*cos(time*5.));
	float vignette = (1.-vigAmt*(uv.y-.5)*(uv.y-.5))*(1.-vigAmt*(uv.x-.5)*(uv.x-.5));
	
	video += stripes(uv);
	video += rand((2.*time+uv)*iResolution.xy)/2.;
	video *= vignette;
	video *= (12.+mod(uv.y*30.+time,1.))/13.;
	
	fragColor = vec4(video,1.0);
}


