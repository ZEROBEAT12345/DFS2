#include "include/GGX_ImportanceSampling.hlsl"

//--------------------------------------------------------------------------------------
// Stream Input
//--------------------------------------------------------------------------------------
struct vs_input_t 
{
   float3 position      : POSITION; 
   float4 color         : COLOR; 
   float2 uv            : TEXCOORD; 
}; 


//--------------------------------------------------------------------------------------
// Uniform Input
//--------------------------------------------------------------------------------------
cbuffer camera_constants : register(b2)
{
   float4x4 VIEW; 
   float4x4 PROJECTION;
   float4 CAMPOS;
};

cbuffer model_constants : register(b3)
{
   float4x4 MODEL;  // LOCAL_TO_WORLD
}

cbuffer mip_constants : register(b5)
{
   uint CURRENT_MIP_LEVEL;
	uint TOTAL_MIP_LEVELS;
   float2 MIP_ZERO_DIMENSION;
}

//--------------------------------------------------------------------------------------
// Texures & Samplers
//--------------------------------------------------------------------------------------

Texture2D<float4> tAlbedo : register(t0);
SamplerState sAlbedo : register(s0);   
#define PI 3.141592653589793238462

//--------------------------------------------------------------------------------------
// Programmable Shader Stages
//--------------------------------------------------------------------------------------

struct v2f_t 
{
   float4 position : SV_POSITION; 
   float4 color : COLOR; 
   float2 uv : UV;
   float3 wPosition : WORLDPOS; 
}; 

//--------------------------------------------------------------------------------------
// Vertex Shader
v2f_t VertexFunction(vs_input_t input)
{
   v2f_t v2f = (v2f_t)0;

   float4 local_pos = float4( input.position, 1.0f ); 
   float4 world_pos = mul( MODEL, local_pos );
   float4 view_pos = mul( VIEW, world_pos ); 
   float4 clip_pos = mul( PROJECTION, view_pos ); 

   v2f.position = clip_pos; 
   v2f.color = input.color; 
   v2f.uv = input.uv;
   v2f.wPosition = world_pos.xyz;
    
   return v2f;
}


static float2 invAtan = float2(1.f / (2 * PI), 1.f / PI);
float2 SampleSphericalMap(float3 v)
{
    float2 uv = float2(atan2(v.z, v.x), atan2(v.y, sqrt(v.x * v.x + v.z * v.z))); // Get 
    //float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += float2(0.5f, 0.5f);
    return uv;
}

float3 PreFilterEnvMap(float3 wNormal, float roughness)
{
   float3 R = wNormal;
   float3 V = R;

   uint SAMPLE_COUNT = 1024;
   float totalWeight = 0.0;   
   float3 prefilteredColor = float3(0.f, 0.f, 0.f);     
   for(uint i = 0u; i < SAMPLE_COUNT; ++i)
   {
      float2 Xi = Hammersley(i, SAMPLE_COUNT);
      float3 H  = ImportanceSampleGGX(Xi, wNormal, roughness);
      float3 L  = normalize(2.0 * dot(V, H) * H - V);

      float NdotL = saturate(dot(wNormal, L));
      if(NdotL > 0.0)
      {
         float2 uv = SampleSphericalMap(L);

         prefilteredColor += tAlbedo.Sample( sAlbedo, uv ).xyz * NdotL;
         totalWeight      += NdotL;
      }
   }
   prefilteredColor = prefilteredColor / totalWeight;

   return prefilteredColor;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
float4 FragmentFunction( v2f_t input ) : SV_Target0
{
   float linear_roughness = (float) CURRENT_MIP_LEVEL / (float) (TOTAL_MIP_LEVELS - 1);
   linear_roughness *= linear_roughness;

   // first mip level is the exact same as the input cubemap
   // this allows us to do mirror-like specular reflections
   if(CURRENT_MIP_LEVEL == 0){
      float2 uv = SampleSphericalMap(input.wPosition);

      float3 envColor = tAlbedo.Sample( sAlbedo, uv ).xyz;
      return float4(envColor, 1.0f);
   }

   float3 prefilteredColor = PreFilterEnvMap(input.wPosition, linear_roughness);

   return float4(prefilteredColor, 1.f);
}