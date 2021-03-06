// Calculating dot3 lighting using blinn-phong
#define MAX_LIGHTS (8)

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

lighting_t GetLighting(float3 eye_pos,
	float3 surface_position,
	float3 surface_normal)
{
	lighting_t lighting;
	float3 ambient = AMBIENT.xyz * AMBIENT.w;
	lighting.diffuse = ambient * .5f;
	//lighting.diffuse = float3(0.05f,0.05f,0.05f);

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		light_t light = LIGHTS[i];

		// directional 
		float3 dir_dir = -light.direction;
		float3 point_dir = normalize(light.position - surface_position);
		float3 light_dir = lerp(point_dir, dir_dir, light.is_directional);

		// common things
		// directional light
		float dir_dist = abs(dot((surface_position - light.position), -light.direction));   // for directional
		float point_dist = length(surface_position - light.position);                          // for point
		float distance = lerp(point_dist, dir_dist, light.is_directional);

		// Diffuse Part
		float3 la = light.diffuse_attenuation;
		float attenuation = 1.0f / (la.x + la.y * distance + la.z * distance * distance);
		float dot3 = max(dot(light_dir, surface_normal), 0.0f);

		//float3 diffuse_color = light.color * light.intensity * attenuation * dot3;
		float3 diffuse_color = light.color * light.intensity * dot3;
		lighting.diffuse += diffuse_color;

		// Specular 
		// blinn-phong 
		// dot( H, N );  -> H == half_vector, N == normal
		float3 dir_to_light = light_dir;
		float3 dir_to_eye = normalize(eye_pos - surface_position);
		float3 half_vector = normalize(dir_to_eye + dir_to_light);
		float spec_coefficient = max(dot(half_vector, surface_normal), 0.0f); // DO not saturate - spec can go higher;  

		float3 sa = light.specular_attenuation;
		float spec_attenuation = 1.0f / (sa.x + sa.y * distance + sa.z * distance * distance);

		// finalize coefficient
		spec_coefficient = SPEC_FACTOR * pow(spec_coefficient, SPEC_POWER);
		//float3 specular_color = light.color * light.intensity * spec_attenuation * spec_coefficient;
		float3 specular_color = light.color * light.intensity * spec_coefficient;
		lighting.specular += specular_color;
	}

	lighting.diffuse = saturate(lighting.diffuse); // Clamp between (0, 1)

	return lighting;
}

