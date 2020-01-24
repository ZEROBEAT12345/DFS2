// lit shader 00 - with only ambient and direction 

struct vs_input_t 
{
	float3 position      : POSITION; 
	float3 normal        : NORMAL; 
	float4 color         : COLOR; 
	float2 uv            : TEXCOORD; 
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

cbuffer ambientLight_constants : register(b4)
{
	float4 AMBIENT;
};

cbuffer dirLight_constants : register(b5)
{
	float3 DIR1; float PADDING;
	float4 DIRCOLOR1;
};

cbuffer pointLight_constants : register(b6)
{
	float3 POINT1;
	float4 POINTCOLOR1;
	float3 POINT2;
	float4 POINTCOLOR2;
	float3 POINT3;
	float4 POINTCOLOR3;
	float3 POINT4;
	float4 POINTCOLOR4;
};

Texture2D<float4> tAlbedo : register(t0); 
SamplerState sAlbedo : register(s0);

struct v2f_t 
{
	float4 position : SV_POSITION; 
	float3 normal : NORMAL; 
	float4 color : COLOR; 
	float2 uv : UV;
	float3 worldPos: WORLDPOS;
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
    
   return v2f;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
float4 FragmentFunction( v2f_t input ) : SV_Target0
{
   // First, we sample from our texture
   float4 texColor = tAlbedo.Sample( sAlbedo, input.uv ); 

   // implement light - blinn phong
   //float3 ambient = AMBIENT.xyz;
   //float3 diffuse = DIRCOLOR1.xyz * saturate(dot(input.normal, normalize(DIR1)));
   //float3 viewDir = normalize(CAMPOS.xyz - input.worldPos);
   //float3 halfDir = normalize(DIR1 + viewDir);
   //float3 specular = DIRCOLOR1.xyz * pow(max(0, dot(input.normal,halfDir)), 20.0);

   // output normal as color
   float4 finalColor = float4((input.normal + float3(1.0,1.0,1.0)) / 2, 1.0);
   //float4 finalColor = float4((normalize(CAMPOS.xyz) + float3(1.0, 1.0, 1.0)) / 2, 1.0);

   // output it; 
   return finalColor; 
}