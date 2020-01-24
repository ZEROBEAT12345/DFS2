// Calculating BRDF lighting - using Cook-Torrance model
#define MAX_LIGHTS (8)
#define PI 3.141592653589793238462

// Random Sampling
//#include "include/Random.h"

// Light structure
struct light_t
{
	float3 color;
	float intensity;

	float3 position;
	float is_directional;

	float3 direction;
	float pad10;

	float3 diffuse_attenuation;
	float pad20;

	float3 specular_attenuation;
	float pad30;
};

cbuffer light_constants : register(b4)
{
	float4 AMBIENT;

	float SPEC_FACTOR;
	float SPEC_POWER;
	float2 pad00;

	light_t LIGHTS[MAX_LIGHTS];
};

struct lighting_t
{
	float3 diffuse;
	float3 specular;
};

// material parameter
// albedo/normal/metallic/roughness/AO

// IBL radiance map


// BRDF function

// Fresnel - using Schlick Approximation
float3 Fresnel_Schlick_approx(float3 f0, float l_dot_h)
{
	return f0 + (float3(1.0f, 1.0f, 1.0f) - f0) * pow((1.0f - l_dot_h), 5.0f);
}

// Geometry Attenuation Function - remapping for IBL

// TBD

// Geometry Attenuation Function - direct
float GAF_Direct_Schlick_GGX(float n_dot_v, float k)
{
	float nom = n_dot_v;
	float denom = n_dot_v * (1.0f - k) + k;

	return nom / denom;
}

float GAF_Direct_Smith(float3 normal, float3 v, float3 light, float roughness)
{
	float n_dot_v = saturate(dot(normal, v));
	float n_dot_l = saturate(dot(light, normal));
	// Different for IBL and direct
	float k = ( roughness + 1.0f ) * ( roughness + 1.0f ) / 8.0f;

	float ggx_v = GAF_Direct_Schlick_GGX(n_dot_v, k);
	float ggx_l = GAF_Direct_Schlick_GGX(n_dot_l, k);

	return ggx_v * ggx_l;
}

// Normal Distribution Function - use 
float NDF_GGX(float3 normal, float3 half_v, float roughness)
{
	float n_dot_h = saturate(dot(normal, half_v));
	// use square in roughness looks more natural
	float r2 = roughness * roughness * roughness * roughness;
	float x = (r2 - 1.f) * n_dot_h * n_dot_h + 1.0f;
	return r2 / (x * x) / PI;
}

float BRDF_specular()
{
	
}

// BRDF Diffuse - EA version
float BRDF_diffuse_energy_EA( float3 normal, float3 light, float3 v, float3 half_v, float roughness)
{
	float l_dot_h = dot(light, half_v);
	float n_dot_v = dot(normal, v);
	float n_dot_l = dot(light, normal);
}

float3 Get_BRDF_Lighting(float3 eye_pos,
	float3 surface_position,
	float3 surface_normal,
	float3 surface_albedo,
	float surface_metallic,
	float surface_roughness)
{
	float3 N = normalize(surface_normal);
	float3 V = normalize(eye_pos - surface_position);

	// Set the F0 Fresnel used
	float lor = 0.04f;
	float3 F0 = float3(lor, lor, lor);
	F0 = lerp(F0, surface_albedo, surface_metallic);
	//F0 = lerp(F0, surface_albedo, 0.0f);

	// reflectance equation
	float3 Lo = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		light_t light = LIGHTS[i];

		// calculate radiance per light
		float3 dir_dir = -light.direction;
		float3 point_dir = normalize(light.position - surface_position);
		float3 L = normalize(lerp(point_dir, dir_dir, light.is_directional));
		float3 H = normalize( V + L );

		float dir_dist = abs(dot((surface_position - light.position), -light.direction));   // for directional
		float point_dist = length(surface_position - light.position);                          // for point
		float distance = lerp(point_dist, dir_dist, light.is_directional);

		float attenuation = 1.0f / distance * distance;
		float3 radiance = light.color * attenuation;

		// BRDF
		float NDF = NDF_GGX(N, H, surface_roughness);
		float G = GAF_Direct_Smith(N, V, L, surface_roughness);
		float3 F = Fresnel_Schlick_approx(F0, saturate(dot(V, H)));

		float3 kS =  F;
        float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
        kD = kD * (1.0f - surface_metallic);     

        float3 nominator = NDF * G * F;
        float  denominator = 4.0f * saturate(dot(N, V)) * saturate(dot(N, L)) + 0.001f; 
        float3 specular = nominator / denominator;

		// finalize radiance
		float n_dot_l = saturate(dot(N, L));
		//Lo += (kD * surface_albedo / PI + specular) * radiance * n_dot_l;
		Lo += (kD * surface_albedo + specular) * radiance * n_dot_l;
		//Lo = F;
	}

	//float3 ambient = AMBIENT.xyz * AMBIENT.w;
	float3 color = Lo + float3(0.05, 0.05, 0.05) * surface_albedo;
	//float3 color = Lo;

	// Gamma Correction
	//color = color / (color + float3(1.0f, 1.0f, 1.0f));
	color = pow(abs(color), float3(1.0f/2.2f, 1.0f/2.2f, 1.0f/2.2f)); 

	return color;
	//return float3(0.4f,0.4f,0.4f);
}

