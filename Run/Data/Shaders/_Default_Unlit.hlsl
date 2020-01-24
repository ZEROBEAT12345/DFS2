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

//--------------------------------------------------------------------------------------
// Programmable Shader Stages
//--------------------------------------------------------------------------------------

struct v2f_t 
{
   float4 position : SV_POSITION; 
   float4 color : COLOR; 
   float2 uv : UV; 
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
    
   return v2f;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
float4 FragmentFunction( v2f_t input ) : SV_Target0
{
   // First, we sample from our texture
   float4 texColor = tAlbedo.Sample( sAlbedo, input.uv ); 

   // component wise multiply to "tint" the output
   float4 finalColor = texColor * input.color; 

   // output it; 
   return finalColor; 
}