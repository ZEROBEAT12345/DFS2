// lit shader 00 - with only ambient and direction 
#include "include/dot3_lighting.hlsl"

struct vs_input_t
{
	float3 position      : POSITION;
	float3 normal        : NORMAL;
	float4 color         : COLOR;
	float2 uv            : TEXCOORD;
	float4 tangent       : TANGENT;
};

cbuffer camera_constants : register(b2)
{
	float4x4 VIEW;
	float4x4 PROJECTION;
	float4 CAMPOS;
};

cbuffer model_constants : register(b3)
{
	float4x4 MODEL;
};

Texture2D<float4> tAlbedo : register(t0); 
Texture2D<float4> tNormal : register(t1); 
Texture2D<float4> tEmissive : register(t3); 
SamplerState sAlbedo : register(s0);

struct v2f_t 
{
	float4 position : SV_POSITION; 
	float3 normal : NORMAL; 
	float4 color : COLOR; 
	float2 uv : UV;
	float3 worldPos: WORLDPOS;
	float4 tangent: TANGENT;
}; 

//--------------------------------------------------------------------------------------
float RangeMap( float v, float inMin, float inMax, float outMin, float outMax ) 
{ 
	return (v - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
v2f_t VertexFunction(vs_input_t input)
{
   v2f_t v2f = (v2f_t)0;

   // calculate pos
   float4 local_pos = float4( input.position, 1.0f ); 
   float4 world_pos = mul( MODEL, local_pos ); 
   float4 view_pos = mul( VIEW, world_pos ); 
   float4 clip_pos = mul( PROJECTION, view_pos ); 

   //clip_pos.w = 1.f;
   v2f.position = clip_pos;
   float4 worldNormal = mul(MODEL, float4(input.normal, 0));
   v2f.normal = worldNormal.xyz;
   v2f.color = input.color; 
   v2f.uv = input.uv;
   v2f.worldPos = world_pos.xyz;
   float4 worldTangent = mul(MODEL, input.tangent);
   v2f.tangent = worldTangent;
    
   return v2f;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
float4 FragmentFunction(v2f_t input) : SV_Target0
{
	float4 normal_color = tNormal.Sample(sAlbedo, input.uv);

	float3 vertex_normal =  normalize(input.normal);

	//float4 texColor = tAlbedo.Sample(sAlbedo, input.uv);

	// implement light - blinn phong
	lighting_t light = GetLighting(CAMPOS.xyz,
		input.worldPos,
		vertex_normal);

	float4 finalColor = float4(light.diffuse, 1.0f) * input.color;// + float4(light.specular, 0.0);

	// output it; 
	return finalColor;
}