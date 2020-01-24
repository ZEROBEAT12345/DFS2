//--------------------------------------------------------------------------------------
cbuffer camera_constants : register(b2)
{
   float4x4 VIEW; 
   float4x4 PROJECTION; 
};

//--------------------------------------------------------------------------------------
cbuffer model_constants : register(b3)
{
   float4x4 MODEL;  // LOCAL_TO_WORLD
}

cbuffer tessellation_constants : register(b7)
{
    float TAMOUNT;
    float3 PADDING;
};

static float gTAMOUNT = 1.0f;

//--------------------------------------------------------------------------------------
Texture2D<float4> tAlbedo : register(t0);
SamplerState sAlbedo : register(s0);   

struct VS_INPUT 
{
   float3 vPosition     : POSITION; 
   float4 vColor        : COLOR; 
   float2 vUV           : TEXCOORD; 
}; 

struct VS_CONTROL_POINT_OUTPUT
{
   float3 vPosition     : POSITION;
   float4 vColor        : COLOR; 
   float2 vUV           : TEXCOORD;
};

struct HS_CONTROL_POINT_OUTPUT
{
   float3 vPosition     : POSITION;
   float4 vColor        : COLOR; 
   float2 vUV           : TEXCOORD;
};

struct HS_CONSTANT_DATA_OUTPUT
{
   float Edges[3]       : SV_TessFactor;
   float Inside         : SV_InsideTessFactor;
};

struct DS_OUTPUT
{
   float4 vPosition     : SV_POSITION;
   float4 vColor        : COLOR;
   float2 vUV           : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
VS_CONTROL_POINT_OUTPUT VertexFunction(VS_INPUT input)
{
   VS_CONTROL_POINT_OUTPUT vso = (VS_CONTROL_POINT_OUTPUT)0;

   vso.vPosition = input.vPosition; 
   vso.vColor = input.vColor; 
   vso.vUV = input.vUV; 
    
   return vso;
}

// Patch constant function - Called once per patch
HS_CONSTANT_DATA_OUTPUT ConstantsHS( InputPatch<VS_CONTROL_POINT_OUTPUT, 3>
p, uint PatchID : SV_PrimitiveID )
{
   HS_CONSTANT_DATA_OUTPUT hco;
   // Assign tessellation factors – in this case use a global
   // tessellation factor for all edges and the inside. These are
   // constant for the whole mesh.
   hco.Edges[0] = gTAMOUNT;
   hco.Edges[1] = gTAMOUNT;
   hco.Edges[2] = gTAMOUNT;
   hco.Inside = gTAMOUNT;
   return hco;
}

// Hull shader - Called once per control point
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")] 
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantsHS")]
HS_CONTROL_POINT_OUTPUT HullFunction( InputPatch<VS_CONTROL_POINT_OUTPUT, 3>
inputPatch, uint uCPID : SV_OutputControlPointID )
{
   HS_CONTROL_POINT_OUTPUT hso;

   hso.vPosition = inputPatch[uCPID].vPosition;
   hso.vColor = inputPatch[uCPID].vColor;
   hso.vUV = inputPatch[uCPID].vUV;

   return hso;
}

// Called once per tessellated vertex
[domain("tri")]
DS_OUTPUT DomainFunction( HS_CONSTANT_DATA_OUTPUT input,
   float3 uvwCoord : SV_DomainLocation,
   const OutputPatch<HS_CONTROL_POINT_OUTPUT, 3> outputPatch )
{
   float3 vPosition;
   DS_OUTPUT dso;

   // Determine the position of the new vertex
   vPosition = uvwCoord.x * outputPatch[0].vPosition + uvwCoord.y * outputPatch[1].vPosition + uvwCoord.z * outputPatch[2].vPosition;

   float4 local_pos = float4( vPosition, 1.0f ); 
   float4 world_pos = mul( MODEL, local_pos );
   float4 view_pos = mul( VIEW, world_pos ); 
   float4 clip_pos = mul( PROJECTION, view_pos );

   dso.vPosition = clip_pos;
   dso.vColor = outputPatch[0].vColor;
   dso.vUV = outputPatch[0].vUV;

   return dso;
} // end of domain shader


float4 FragmentFunction( DS_OUTPUT input ) : SV_Target0
{
   // First, we sample from our texture
   float4 texColor = tAlbedo.Sample( sAlbedo, input.vUV ); 

   // component wise multiply to "tint" the output
   float4 finalColor = texColor * input.vColor; 

   // output it; 
   return finalColor; 
}