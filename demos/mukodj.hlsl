//cbuffer Frame{
//	float time;
//	float2 resolution; 
// };

cbuffer Camera {
	float3 ceye;
	float3 ccenter;
	float cfov;
};

#define MAX_DEPTH 60.
#define MAX_ITERATION 64
#define MIN_PRECISSION 0.0001

float udBox(float3 p, float3 b ){return length(max(abs(p)-b,0.0));}

float map(float3 pos){
	return udBox(pos, float3(1,1,1));
}

float intersect(float3 ro, float3 rd)
{
    float h = MIN_PRECISSION*2.0;
    float t = 0.0;
		
    for( int i=0; i<MAX_ITERATION; i++ )
    {
        if( abs(h)<MIN_PRECISSION||t>MAX_DEPTH ) break;
        t += h;
	    h = map( ro+rd*t);
    }

    return t;
}


float4 main( float4 position : SV_POSITION, float2 TexCoord : TEXCOORD ) : SV_TARGET
{
	float2 pos = -1 + 2.* TexCoord.xy;
		 
	// camera	
	float3 eye = ceye + float3( 3.0, 1.5 , 3.5);
	float3 center = ccenter + float3( 0.0, 0.0, 0.0 );
	
	// camera tx
	float3 cw = normalize(center-eye);
	float3 cp = float3( 0.0, 1.0, 0.0 );
	float3 cu = normalize( cross(cw,cp) );
	float3 cv = normalize( cross(cu,cw) );
	float3 rayDir = normalize( pos.x*cu + pos.y*cv + 2.5*cw );

	float d = intersect(eye, rayDir);

	d /= d+1;
	float3 col = float3(d,d,d);

	return float4(col, 1.);
}
