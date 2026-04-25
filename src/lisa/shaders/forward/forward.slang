//
// Created by kinami on 14/4/26.
//

import lib.common;
import lib.constants;

[[vk::push_constant]]
ConstantBuffer<PushConstants> push_constants;

Sampler2D textures[];

struct Vertex2Fragment {
  float4 position : SV_POSITION;
  float3 normal;
  float3 world_position;
  float2 uv;
  float3 tangent;
  uint32_t diffuse_texture_index;
  uint32_t roughness_texture_index;
  uint32_t metallic_texture_index;
  uint32_t normal_texture_index;
};

float3 brdf(float3 albedo, float roughness, float metallic, float3 n, float3 l, float3 v) {
  float3 diffuse = (1.0 - metallic) * albedo / PI;
  float3 h = normalize(l + v);
  float NdotH = saturate(dot(n, h));
  float NdotL = max(dot(n, l), 0.0001);
  float NdotV = max(dot(n, v), 0.0001);
  float VdotH = saturate(dot(v, h));
  float alpha_sq = pow(roughness, 4);
  float D = alpha_sq / (PI * pow(NdotH * NdotH * (alpha_sq - 1.0) + 1.0, 2));
  float3 F0 = lerp(float3(0.04), albedo, metallic);
  float3 F = F0 + (1.0 - F0) * pow(2.0, (-5.55473 * VdotH - 6.98316) * VdotH);
  float k = pow(roughness + 1.0, 2) / 8.0;
  float G1L = NdotL / (NdotL * (1.0 - k) + k);
  float G1V = NdotV / (NdotV * (1.0 - k) + k);
  float G = G1L * G1V;
  float3 specular = (D * F * G) / (4.0 * NdotL * NdotV);
  return diffuse + specular;
}

[shader("vertex")]
Vertex2Fragment vertex(Input input) {
  Vertex2Fragment output;

  float4x4 model = push_constants.object_data->model;
  float4x4 view_projection = push_constants.global_data->view_projection;
  float4 world_pos = mul(model, float4(input.position, 1.0));

  output.position = mul(view_projection, world_pos);
  output.world_position = world_pos.xyz;
  output.normal = mul((float3x3) model, input.normal);
  output.uv = input.uv;
  output.tangent = mul((float3x3) model, input.tangent);
  output.diffuse_texture_index = push_constants.object_data->diffuse_texture_index;
  output.roughness_texture_index = push_constants.object_data->roughness_texture_index;
  output.metallic_texture_index = push_constants.object_data->metallic_texture_index;
  output.normal_texture_index = push_constants.object_data->normal_texture_index;

  return output;
}

[shader("fragment")]
float4 fragment(Vertex2Fragment input) : SV_Target {
  float4 base_color = push_constants.object_data->color;

  float3 albedo = base_color.rgb;
  if (input.diffuse_texture_index != 0xFFFFFFFF) {
    albedo *= textures[input.diffuse_texture_index].Sample(input.uv).rgb;
  }

  float final_roughness = push_constants.object_data->roughness;
  if (input.roughness_texture_index != 0xFFFFFFFF) {
    final_roughness *= textures[input.roughness_texture_index].Sample(input.uv).r;
  }

  float final_metallic = push_constants.object_data->metallic;
  if (input.metallic_texture_index != 0xFFFFFFFF) {
    final_metallic *= textures[input.metallic_texture_index].Sample(input.uv).r;
  }

  float3 N = normalize(input.normal);

  if (input.normal_texture_index != 0xFFFFFFFF) {
    float3 T = normalize(input.tangent);
    T = normalize(T - dot(T, N) * N);
    if (length(T) < 0.001) {
      float3 up = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
      T = normalize(cross(N, up));
    }
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    float3 normalMap = textures[input.normal_texture_index].Sample(input.uv).rgb;
    normalMap = normalMap * 2.0 - 1.0;
    N = normalize(mul(normalMap, TBN));
  }

  float3 V = normalize(push_constants.global_data->camera_position - input.world_position);

  float3 shading = float3(0.0);

  for (uint32_t i = 0; i < push_constants.global_data->dir_lights_count; i++) {
    DirLight light = push_constants.global_data->dir_lights[i];
    float3 L = normalize(-light.direction);
    float NdotL = saturate(dot(N, L));
    shading += brdf(albedo, final_roughness, final_metallic, N, L, V) * light.color * light.intensity * NdotL;
  }

  for (uint32_t i = 0; i < push_constants.global_data->point_lights_count; i++) {
    PointLight light = push_constants.global_data->point_lights[i];
    float3 lightVec = light.position - input.world_position;
    float distance = length(lightVec);
    float3 L = normalize(lightVec);
    float falloff = pow(saturate(1.0 - pow(distance / light.radius, 4)), 2) / (distance * distance + 1.0);
    float NdotL = saturate(dot(N, L));
    shading += brdf(albedo, final_roughness, final_metallic, N, L, V) * light.color * light.intensity * falloff * NdotL;
  }

  for (uint32_t i = 0; i < push_constants.global_data->ambient_lights_count; i++) {
    AmbientLight light = push_constants.global_data->ambient_lights[i];
    float3 diffuse_color = albedo * (1.0 - final_metallic);
    shading += light.color * light.intensity * diffuse_color;
  }

  return float4(shading, 1.0);
}
