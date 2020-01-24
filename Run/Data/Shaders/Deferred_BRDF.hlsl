// deferred shader - use BRDF 
#include "include/BRDF.hlsl"

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

Texture2D<float4> tPosition : register(t0);
Texture2D<float4> tAlbedo : register(t1);
Texture2D<float4> tNormal : register(t2); 
Texture2D<float4> tMetallic : register(t3);
Texture2D<float4> tRoughness : register(t4); 
Texture2D<float4> tAO : register(t5); 
SamplerState sAlbedo : register(s0);

struct vs_input_t
{
   uint vertex_id        : SV_VertexID;
};

struct v2f_t
{
   float4 position         : SV_Position;
   float2 uv               : UV; 
};

static const float3 FULLSCREEN_TRI[] = {
   float3( -1.0f, -1.0f, 0.0f ),
   float3(  3.0f, -1.0f, 0.0f ), 
   float3( -1.0f,  3.0f, 0.0f )
}; 

static const float2 FULLSCREEN_UV[] = {
   float2(  0.0f,  1.0f ),
   float2(  2.0f,  1.0f ), 
   float2(  0.0f, -1.0f )
}; 

//--------------------------------------------------------------------------------------
// Vertex Shader
v2f_t VertexFunction(vs_input_t input)
{
   v2f_t v2f = (v2f_t)0;

   v2f.position   = float4( FULLSCREEN_TRI[input.vertex_id], 1.0f ); 
   v2f.uv         = FULLSCREEN_UV[input.vertex_id];    

   return v2f;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
float4 FragmentFunction(v2f_t input) : SV_Target0
{
	float3 world_pos = tPosition.Sample(sAlbedo, input.uv).xyz;
	float3 world_normal = tNormal.Sample(sAlbedo, input.uv).xyz;
	float3 surface_albedo = tAlbedo.Sample(sAlbedo, input.uv).xyz;
	float surface_metallic = tMetallic.Sample(sAlbedo, input.uv).x;
	float surface_roughness = tRoughness.Sample(sAlbedo, input.uv).x;

	// implement light - BRDF
	float3 light = Get_BRDF_Lighting(CAMPOS.xyz,
		world_pos,
		world_normal,
		surface_albedo,
		surface_metallic,
		surface_roughness);

	float4 finalColor = float4(light, 1.0f);

	return finalColor;
	//return float4(1.0f,1.0f,0.0f,1.0f);
}