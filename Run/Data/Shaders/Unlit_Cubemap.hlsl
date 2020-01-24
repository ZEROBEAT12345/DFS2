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

//--------------------------------------------------------------------------------------
// Fragment Shader
float4 FragmentFunction( v2f_t input ) : SV_Target0
{
   float2 uv = SampleSphericalMap(input.wPosition);

   // First, we sample from our texture
   float4 texColor = tAlbedo.Sample( sAlbedo, uv ); 

   // component wise multiply to "tint" the output
   float4 finalColor = texColor; 
   // float3 dir = input.wPosition; 
   // //dir = normalize(dir); 
   // dir = (dir + float3(1,1,1)) * .5f;
   // float4 finalColor = float4(dir, 1.f); 

   // output it; 
   return finalColor;
}