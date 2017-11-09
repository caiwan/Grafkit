#include "defaultShader.h"

const char * const GrafkitData::effectShader = "\r\n"

// GLOBALS //
"Texture2D effectInput;\r\n"

"SamplerState SampleType{\n"
"Filter = MIN_MAG_MIP_LINEAR;\n"
"AddressU = Wrap;\n"
"AddressV = Wrap;\n"
"};\n"

"cbuffer EffectParams{\n"
"float4 fxScreen;"
"float4 fxViewport;"
"}\n"

// TYPEDEFS //
"struct FXPixelInputType"
"{"
"float4 position : SV_POSITION;"
"float2 tex : TEXCOORD;"
"};"

"struct FXVertexInputType"
"{"
"float4 position : POSITION;"
"float2 tex : TEXCOORD0;"
"};"

// VertexShader
"FXPixelInputType FullscreenQuad(FXVertexInputType input)"
"{"
"FXPixelInputType output;"
"input.position.w = 1.0f;"
"output.position = input.position;"
"output.tex = input.tex;"
"return output;"
"}"

// PixelShader
"float4 CopyScreen(FXPixelInputType input) : SV_TARGET"
"{"
"float4 textureColor = float4(0,0,0,1);"
"textureColor = effectInput.Sample(SampleType, input.position / fxScreen.xy);"
"return textureColor;"
"}"

"";

const char * const GrafkitData::effectFullscreenQuadEntry = "FullscreenQuad";
const char * const GrafkitData::effectCopyScreenEntry = "CopyScreen";
