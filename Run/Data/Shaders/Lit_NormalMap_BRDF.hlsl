// lit shader 00 - with only ambient and direction 
#include "include/BRDF.hlsl"

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
Texture2D<float4> tMetallic : register(t2);
Texture2D<float4> tRoughness : register(t3); 
Texture2D<float4> tAO : register(t4); 
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

	// transform from color space to a hemisphere (z can't be negative, so we give double precision to z)
	// shorthand - map (0, 0, 0) to (1, 1, 1) -> (-1, -1, 0) to (1, 1, 1)
	float3 surface_normal = normal_color.xyz * float3(2, 2, 2) - float3(1, 1, 1);

	float3 vertex_tangent = normalize(input.tangent.xyz);
	float3 vertex_bitan =	normalize(cross(input.tangent.xyz, input.normal));
	float3 vertex_normal =  normalize(input.normal);

	// commonly referred to the TBN matrix
	float3x3 surface_to_world = float3x3(vertex_tangent, vertex_bitan, vertex_normal);

	// if you just go with my matrix format...
	float3 world_normal = mul(surface_normal, surface_to_world);

	// Calculate diffuse tex color
	float4 diffuse_Color = pow(abs(tAlbedo.Sample(sAlbedo, input.uv) * input.color), 2.2);

	// Calculate specular tex color
	float4 metallic_color = tMetallic.Sample(sAlbedo, input.uv);
	float4 roughness_color = tRoughness.Sample(sAlbedo, input.uv);

	float3 light = Get_BRDF_Lighting(
		//float3(0.f, 0.f, 0.f),
		CAMPOS.xyz,
		input.worldPos,
		world_normal,
		diffuse_Color.xyz,
		metallic_color.x,
		roughness_color.x);

	float4 finalColor = float4(light, 1.0f);
	//float4 finalColor = float4(world_normal, 1.0f);

	return finalColor;
	//return float4(world_normal, 1.0f);
}