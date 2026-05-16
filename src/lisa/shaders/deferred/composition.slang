//
// Created by kinami on 19/4/26.
//

import lib.common;
import lib.constants;


[[vk::binding(0)]] Sampler2D textures[];
[[vk::binding(0)]] Sampler2DArray array_textures[];

struct CompositionPushConstants {
  GlobalData* global_data;
  uint32_t albedo_idx;
  uint32_t normal_idx;
  uint32_t position_idx;
  uint32_t material_idx;
  uint32_t ssao_idx;
  uint32_t shadow_idx;
};

[[vk::push_constant]]
ConstantBuffer<CompositionPushConstants> push_constants;

struct Vertex2Fragment {
  float4 position : SV_POSITION;
  float2 uv;
};

float3 brdf( float3 albedo, float roughness, float metallic, float3 n, float3 l, float3 v ) {
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

float linearize_depth(float depth, float near, float far) {
  return near * far / (far - depth * (far - near));
}

float eval_visibility_shadow_map(float3 world_position, uint32_t shadow_layer) {
  if (shadow_layer == 0xFFFFFFFF) return 1.0;

  Shadow sh = push_constants.global_data->shadows[shadow_layer];
  float4 shadow_pos = mul(sh.view_projection, float4(world_position, 1.0));
  shadow_pos.xyz /= shadow_pos.w;
  shadow_pos.xyz = shadow_pos.xyz * 0.5 + 0.5;

  if (shadow_pos.x < 0.0 || shadow_pos.x > 1.0 ||
      shadow_pos.y < 0.0 || shadow_pos.y > 1.0 ||
      shadow_pos.z < 0.0 || shadow_pos.z > 1.0) {
    return 1.0;
  }

  float depth = array_textures[push_constants.shadow_idx].SampleLevel(float3(shadow_pos.xy, shadow_layer), 0).r;
  return float(shadow_pos.z - SHADOW_BIAS <= depth);
}

float eval_visibility_shadow_cubemap(float3 world_position, uint32_t shadow_base_layer, float3 light_position, float near, float far) {
  if (shadow_base_layer == 0xFFFFFFFF) return 1.0;

  float3 light_to_frag = world_position - light_position;
  
  float3 abs_dist = abs(light_to_frag);
  float current_depth_planar = max(abs_dist.x, max(abs_dist.y, abs_dist.z));

  float3 dir = normalize(light_to_frag);
  float3 abs_dir = abs(dir);

  uint32_t face;
  float2 uv;
  if (abs_dir.x >= abs_dir.y && abs_dir.x >= abs_dir.z) {
    if (dir.x > 0.0) { face = 0; uv = float2(-dir.z, -dir.y) / abs_dir.x; }
    else             { face = 1; uv = float2( dir.z, -dir.y) / abs_dir.x; }
  } else if (abs_dir.y >= abs_dir.x && abs_dir.y >= abs_dir.z) {
    if (dir.y > 0.0) { face = 2; uv = float2( dir.x,  dir.z) / abs_dir.y; }
    else             { face = 3; uv = float2( dir.x, -dir.z) / abs_dir.y; }
  } else {
    if (dir.z > 0.0) { face = 4; uv = float2( dir.x, -dir.y) / abs_dir.z; }
    else             { face = 5; uv = float2(-dir.x, -dir.y) / abs_dir.z; }
  }
  
  uv = uv * 0.5 + 0.5;
  
  uv.y = 1.0 - uv.y;

  float closest_depth_ndc = array_textures[push_constants.shadow_idx].SampleLevel(float3(uv, shadow_base_layer + face), 0).r;
  float closest_depth = linearize_depth(closest_depth_ndc, near, far);
  
  return float(current_depth_planar - SHADOW_BIAS <= closest_depth);
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

  float3 V = normalize(push_constants.global_data->camera_position - world_position);

  float3 shading = float3(0.0);

  for (uint32_t i = 0; i < push_constants.global_data->dir_lights_count; i++) {
    DirLight light = push_constants.global_data->dir_lights[i];
    float3 L = normalize(-light.direction);
    float NdotL = saturate(dot(N, L));
    float shadow = eval_visibility_shadow_map(world_position, light.shadow_index);
    shading += brdf(albedo, roughness, metallic, N, L, V) * light.color * light.intensity * NdotL * shadow;
  }

  for (uint32_t i = 0; i < push_constants.global_data->point_lights_count; i++) {
    PointLight light = push_constants.global_data->point_lights[i];
    float3 lightVec = light.position - world_position;
    float distance = length(lightVec);
    float3 L = normalize(lightVec);
    float falloff = pow(saturate(1.0 - pow(distance / light.radius, 4)), 2) / (distance * distance + 1.0);
    float NdotL = saturate(dot(N, L));
    float shadow = eval_visibility_shadow_cubemap(world_position, light.shadow_base_index, light.position, light.near, light.far);
    shading += brdf(albedo, roughness, metallic, N, L, V) * light.color * light.intensity * falloff * NdotL * shadow;
  }

  for (uint32_t i = 0; i < push_constants.global_data->ambient_lights_count; i++) {
    AmbientLight light = push_constants.global_data->ambient_lights[i];
    float3 diffuse_color = albedo * (1.0 - metallic);
    shading += light.color * light.intensity * diffuse_color * ssao;
  }

  PointLight light = push_constants.global_data->point_lights[0];

float3 light_to_frag = world_position - light.position;
float current_depth = length(light_to_frag);
float3 dir = normalize(light_to_frag);
float3 abs_dir = abs(dir);

uint32_t face;
float2 uv;
if (abs_dir.x >= abs_dir.y && abs_dir.x >= abs_dir.z) {
    if (dir.x > 0.0) { face = 0; uv = float2(-dir.z, -dir.y) / abs_dir.x; }
    else             { face = 1; uv = float2( dir.z, -dir.y) / abs_dir.x; }
} else if (abs_dir.y >= abs_dir.x && abs_dir.y >= abs_dir.z) {
    if (dir.y > 0.0) { face = 2; uv = float2( dir.x,  dir.z) / abs_dir.y; }
    else             { face = 3; uv = float2( dir.x, -dir.z) / abs_dir.y; }
} else {
    if (dir.z > 0.0) { face = 4; uv = float2( dir.x, -dir.y) / abs_dir.z; }
    else             { face = 5; uv = float2(-dir.x, -dir.y) / abs_dir.z; }
}
uv = uv * 0.5 + 0.5;

float closest_depth_ndc = array_textures[push_constants.shadow_idx].SampleLevel(float3(uv, light.shadow_base_index + face), 0).r;
float closest_depth = linearize_depth(closest_depth_ndc, light.near, light.far);

// return float4(closest_depth_ndc.rrr, 1.0);          // A: raw NDC depth from shadow map
// return float4(current_depth / light.far, 0, 0, 1.0); // B: current depth normalized
// return float4(closest_depth / light.far, 0, 0, 1.0); // C: linearized depth normalized  
// return float4(float(current_depth - SHADOW_BIAS <= closest_depth).rrr, 1.0); // D: final mask

  return float4(shading, 1.0);
}
