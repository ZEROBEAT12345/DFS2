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

float3 ConvoluteRadianceMap(float3 wNormal)
{
   float3 irradiance = float3(0.f, 0.f, 0.f);

   float3 up = float3(0.f, 1.f, 0.f);
   float3 right = cross(up, wNormal);
   up = cross(wNormal, right);

   float sampleDelta = 0.025;
   float nrSamples = 0.0; 
   for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
   {
      for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
      {
         // spherical to cartesian (in tangent space)
         float3 tangentSample = float3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
         // tangent space to world
         float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * wNormal; 

         float2 uv = SampleSphericalMap(sampleVec);

         irradiance += tAlbedo.Sample( sAlbedo, uv ).xyz * cos(theta) * sin(theta);
         nrSamples++;
      }
   }

   irradiance = PI * irradiance * (1.0 / nrSamples);
   return irradiance;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
float4 FragmentFunction( v2f_t input ) : SV_Target0
{
   float3 radiance = ConvoluteRadianceMap(input.wPosition);

   return float4(radiance, 1.f);
}