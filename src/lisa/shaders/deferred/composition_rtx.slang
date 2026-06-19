//
// Created by kinami on 6/18/26.
//

import lib.common;
import lib.constants;

[[vk::binding(0, 0)]] Sampler2D textures[];

struct CompositionRtxPushConstants {
  GlobalData* global_data;
  uint32_t albedo_idx;
  uint32_t normal_idx;
  uint32_t position_idx;
  uint32_t material_idx;
  uint32_t ssao_idx;
  uint32_t shadow_idx;
};

[[vk::push_constant]]
ConstantBuffer<CompositionRtxPushConstants> push_constants;

struct Vertex2Fragment {
  float4 position : SV_POSITION;
  float2 uv;
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
Vertex2Fragment vertex(uint32_t vertex_id: SV_VertexID) {
  Vertex2Fragment output;
  output.uv = float2((vertex_id << 1) & 2, vertex_id & 2);
  output.position = float4(output.uv * 2.0f - 1.0f, 0.0f, 1.0f);
  return output;
}

[shader("fragment")]
float4 fragment(Vertex2Fragment input) : SV_Target {
  float3 albedo = textures[push_constants.albedo_idx].Sample(input.uv).rgb;
  float3 N = normalize(textures[push_constants.normal_idx].Sample(input.uv).xyz);
  float3 world_position = textures[push_constants.position_idx].Sample(input.uv).xyz;
  float4 material = textures[push_constants.material_idx].Sample(input.uv);
  float roughness = material.x;
  float metallic = material.y;

  float ssao = 1.0;
  if (push_constants.ssao_idx != 0xFFFFFFFF) {
    ssao = textures[push_constants.ssao_idx].Sample(input.uv).r;
  }

  float shadow = textures[push_constants.shadow_idx].Sample(input.uv).r;

  float3 V = normalize(push_constants.global_data->camera_position - world_position);

  uint32_t dir_count = push_constants.global_data->dir_lights_count;
  uint32_t pt_count  = push_constants.global_data->point_lights_count;
  uint32_t amb_count = push_constants.global_data->ambient_lights_count;

  float3 shading = float3(0.0);

  for (uint32_t i = 0; i < dir_count; i++) {
    DirLight light = push_constants.global_data->dir_lights[i];
    float3 L = normalize(-light.direction);
    float NdotL = saturate(dot(N, L));
    float s = (light.shadow_index != 0xFFFFFFFF) ? shadow : 1.0;
    shading += brdf(albedo, roughness, metallic, N, L, V) * light.color * light.intensity * NdotL * s;
  }

  for (uint32_t i = 0; i < pt_count; i++) {
    PointLight light = push_constants.global_data->point_lights[i];
    float3 lightVec = light.position - world_position;
    float distance = length(lightVec);
    float3 L = normalize(lightVec);
    float falloff = pow(saturate(1.0 - pow(distance / light.radius, 4)), 2) / (distance * distance + 1.0);
    float NdotL = saturate(dot(N, L));
    float s = (light.shadow_base_index != 0xFFFFFFFF) ? shadow : 1.0;
    shading += brdf(albedo, roughness, metallic, N, L, V) * light.color * light.intensity * falloff * NdotL * s;
  }

  for (uint32_t i = 0; i < amb_count; i++) {
    AmbientLight light = push_constants.global_data->ambient_lights[i];
    float3 diffuse_color = albedo * (1.0 - metallic);
    shading += light.color * light.intensity * diffuse_color * ssao;
  }

  return float4(shading, 1.0);
}
