// deferred shader 00 - output to multiple RTs

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

struct v2p_t 
{
	float4 position : SV_POSITION; 
	float3 normal : NORMAL; 
	float4 color : COLOR; 
	float2 uv : UV;
	float3 worldPos: WORLDPOS;
	float4 tangent: TANGENT;
}; 

struct p2f_t
{
	float4 position: SV_Target0;
	float4 albedo: SV_Target1;
	float4 normal: SV_Target2;
	float4 metallic: SV_Target3;
	float4 roughness: SV_Target4;
	//float4 : SV_Target5;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
v2p_t VertexFunction(vs_input_t input)
{
   v2p_t v2p = (v2p_t)0;

   // calculate pos
   float4 local_pos = float4( input.position, 1.0f ); 
   float4 world_pos = mul( MODEL, local_pos ); 
   float4 view_pos = mul( VIEW, world_pos ); 
   float4 clip_pos = mul( PROJECTION, view_pos ); 

   v2p.position = clip_pos;
   float4 worldNormal = mul(MODEL, float4(input.normal, 0));
   v2p.normal = worldNormal.xyz;
   v2p.color = input.color; 
   v2p.uv = input.uv;
   v2p.worldPos = world_pos.xyz;
   float4 worldTangent = mul(MODEL, input.tangent);
   v2p.tangent = worldTangent;
    
   return v2p;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
p2f_t FragmentFunction(v2p_t input)
{
	p2f_t p2f = (p2f_t)0;

	// Calculate worldNormal
	float4 normal_color = tNormal.Sample(sAlbedo, input.uv);
	float3 surface_normal = normal_color.xyz * float3(2, 2, 2) - float3(1, 1, 1);

	float3 vertex_tangent = normalize(input.tangent.xyz);
	float3 vertex_bitan =	normalize(cross(input.tangent.xyz, input.normal));
	float3 vertex_normal =  normalize(input.normal);

	float3x3 surface_to_world = float3x3(vertex_tangent, vertex_bitan, vertex_normal);

	float3 world_normal = mul(surface_normal, surface_to_world);

	// Calculate diffuse tex color
	float4 diffuse_Color = tAlbedo.Sample(sAlbedo, input.uv) * input.color;

	// Calculate specular tex color
	float4 matallic_color = tMetallic.Sample(sAlbedo, input.uv);
	float4 roughness_color = tRoughness.Sample(sAlbedo, input.uv);

    p2f.position = float4(input.worldPos, 1.f);
	p2f.albedo = diffuse_Color;
	p2f.normal = float4(world_normal, 1.f);
	p2f.metallic = matallic_color;
	p2f.roughness = roughness_color;

	return p2f;
}