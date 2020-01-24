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

cbuffer projector_constants : register(b5)
{
	float4x4 PJVIEW;
	float4x4 PJPROJECTION;
	float4 PJPOS;
};

Texture2D<float4> tAlbedo : register(t0); 
Texture2D<float4> tProject : register(t4); 
SamplerState sAlbedo : register(s0);

struct v2f_t 
{
	float4 position : SV_POSITION; 
	float3 normal : NORMAL; 
	float4 color : COLOR; 
	float2 uv : UV;
	float3 worldPos: WORLDPOS;
	float4 projectPos: PROJECTORPOS;
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

   float4 pj_local_pos = float4(input.position, 1.0f);
   float4 pj_world_pos = mul( MODEL, pj_local_pos);
   float4 pj_view_pos = mul(PJVIEW, pj_world_pos);
   float4 pj_clip_pos = mul(PJPROJECTION, pj_view_pos);

   //clip_pos.w = 1.f;
   v2f.position = clip_pos;
   float4 worldNormal = mul(MODEL, float4(input.normal, 0));
   v2f.normal = worldNormal.xyz;
   v2f.color = input.color; 
   v2f.uv = input.uv;
   v2f.worldPos = world_pos.xyz;
   v2f.projectPos = pj_clip_pos;
    
   return v2f;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
float4 FragmentFunction(v2f_t input) : SV_Target0
{
	// First, we sample from our texture
	float4 texColor = tAlbedo.Sample(sAlbedo, input.uv);

	// implement light - blinn phong
	lighting_t light = GetLighting(CAMPOS.xyz,
		input.worldPos,
		input.normal);

	// Sample from projected texture


	float4 finalColor = texColor * float4(light.diffuse, 1.0) * input.color + float4(light.specular, 0.0);
	//return finalColor;
	
	//float4 finalColor = texColor;
	//float4 finalColor = float4((normalize(CAMPOS.xyz) + float3(1.0, 1.0, 1.0)) / 2, 1.0);

	float2 proTexUV = float2(input.projectPos.x / input.projectPos.w, input.projectPos.y / input.projectPos.w);
	proTexUV = (proTexUV + float2(1.0, 1.0)) / 2;
	float3 dir_to_pro = PJPOS.xyz - input.worldPos;
	if (proTexUV.x < 1.0 && proTexUV.x > 0.0 && proTexUV.y < 1.0 && proTexUV.y > 0.0 && dot(input.normal, dir_to_pro) >= 0)
	{
		float4 pjTexColor = tProject.Sample(sAlbedo, proTexUV);
		finalColor = lerp( finalColor, pjTexColor, pjTexColor.a );
	}

	// output it; 
	return finalColor;
}