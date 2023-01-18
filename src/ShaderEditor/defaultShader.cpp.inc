#ifdef INCLUDE_DEFAULT_FSHADER
char * defaultShaderFilename = "shader.dx11.hlsl";

#if 0
char defaultShader[65536] = 
	"{%textures:begin%}" // leave off \n here
	"Texture2D {%textures:name%};\n"
	"{%textures:end%}" // leave off \n here
	"Texture1D texFFT; // towards 0.0 is bass / lower freq, towards 1.0 is higher / treble freq\n"
	"Texture1D texFFTSmoothed; // this one has longer falloff and less harsh transients\n"
	"SamplerState smp;\n"
	"\n"
	"cbuffer constants\n"
	"{\n"
	"  float fGlobalTime; // in seconds\n"
	"  float2 v2Resolution; // viewport resolution (in pixels)\n"
	"{%midi:begin%}"
	"  float {%midi:name%};\n"
	"{%midi:end%}"
	"}\n"
	"\n"
	"float4 plas( float2 v, float time )\n"
	"{\n"
	"  float c = 0.5 + sin( v.x * 10.0 ) + cos( sin( time + v.y ) * 20.0 );\n"
	"  return float4( sin(c * 0.2 + cos(time)), c * 0.15, cos( c * 0.1 + time / .4 ) * .25, 1.0 );\n"
	"}\n"
	"float4 main( float4 position : SV_POSITION, float2 TexCoord : TEXCOORD ) : SV_TARGET\n"
	"{\n"
	"  float2 uv = TexCoord;\n"
	"  uv -= 0.5;\n"
	"  uv /= float2(v2Resolution.y / v2Resolution.x, 1);"
	"\n"
	"  float2 m;\n"
	"  m.x = atan(uv.x / uv.y) / 3.14;\n"
	"  m.y = 1 / length(uv) * .2;\n"
	"  float d = m.y;\n"
	"\n"
	"  float f = texFFT.Sample( smp, d ).r * 100;\n"
	"  m.x += sin( fGlobalTime ) * 0.1;\n"
	"  m.y += fGlobalTime * 0.25;\n"
	"\n"
	"  float4 t = plas( m * 3.14, fGlobalTime ) / d;\n"
	"  t = saturate( t );\n"
	"  return f + t;\n"
	"}";
#else
char defaultShader[65536] = 
	
	"float4 main( float4 position : SV_POSITION, float2 TexCoord : TEXCOORD ) : SV_TARGET\n"
	"{\n"
	"  return position;\n"
	"}\n";
#endif

#endif // INCLUDE_DEFAULT_FSHADER

#ifdef INCLUDE_DEFAULT_VSHADER
char defaultVertexShader[65536] = 
	"struct VS_INPUT_PP { float4 Pos : POSITION; float2 TexCoord : TEXCOORD; };\n"
	"struct VS_OUTPUT_PP { float4 Pos : SV_POSITION; float2 TexCoord : TEXCOORD; };\n"
	"\n"
	"VS_OUTPUT_PP main( VS_INPUT_PP In )\n"
	"{\n"
	"  VS_OUTPUT_PP Out;\n"
	"  Out.Pos = In.Pos; Out.Pos.w=1.0;\n"
	"  Out.TexCoord = In.TexCoord;\n"
	"  return Out;\n"
	"}\n";

#endif //INCLUDE_DEFAULT_VSHADER
