//
// Created by kinami on 6/18/26.
//

import lib.common;
import lib.constants;

[[vk::binding(0, 0)]] Sampler2D textures[];
[[vk::binding(0, 1)]] RaytracingAccelerationStructure tlas;

struct ShadowRtxPushConstants {
  GlobalData* global_data;
  uint32_t position_idx;
  uint32_t normal_idx;
  uint32_t has_tlas;
  uint32_t num_shadow_samples;
};

[[vk::push_constant]]
ConstantBuffer<ShadowRtxPushConstants> push_constants;

struct Vertex2Fragment {
  float4 position : SV_POSITION;
  float2 uv;
};

uint pcg(uint v) {
  uint state = v * 747796405u + 2891336453u;
  uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
  return (word >> 22u) ^ word;
}

float2 rand2(uint px, uint py, uint sample_i) {
  uint seed = pcg(px ^ pcg(py ^ pcg(sample_i)));
  uint s2   = pcg(seed);
  return float2(float(seed), float(s2)) / 4294967295.0;
}

void make_onb(float3 n, out float3 t, out float3 b) {
  t = abs(n.x) < 0.99 ? float3(1, 0, 0) : float3(0, 1, 0);
  b = normalize(cross(n, t));
  t = cross(b, n);
}

float eval_visibility_rtx(float3 world_pos, float3 N, float3 L, float t_max,
                           float area_radius, uint2 pixel, uint light_idx) {
  if (push_constants.has_tlas == 0) return 1.0;

  int n = area_radius > 0.0 ? int(push_constants.num_shadow_samples) : 1;
  float3 tan, bit;
  make_onb(L, tan, bit);

  float vis = 0.0;
  for (int i = 0; i < n; i++) {
    float3 dir = L;
    if (area_radius > 0.0) {
      float2 xi    = rand2(pixel.x, pixel.y, light_idx * push_constants.num_shadow_samples + uint(i));
      float  r     = area_radius * sqrt(xi.x);
      float  theta = 6.28318 * xi.y;
      dir = normalize(L + (cos(theta) * tan + sin(theta) * bit) * r);
    }
    RayDesc ray;
    ray.Origin    = world_pos + N * 0.001;
    ray.Direction = dir;
    ray.TMin      = 0.0;
    ray.TMax      = t_max;
    RayQuery<RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH |
             RAY_FLAG_SKIP_CLOSEST_HIT_SHADER> rq;
    rq.TraceRayInline(tlas, RAY_FLAG_NONE, 0xFF, ray);
    rq.Proceed();
    vis += rq.CommittedStatus() == COMMITTED_TRIANGLE_HIT ? 0.0 : 1.0;
  }
  return vis / float(n);
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
  float3 world_pos = textures[push_constants.position_idx].Sample(input.uv).xyz;
  float3 N = normalize(textures[push_constants.normal_idx].Sample(input.uv).xyz);
  uint2 pixel = uint2(input.position.xy);

  if (length(world_pos) < 0.0001)
    return float4(1.0, 0.0, 0.0, 1.0);

  // hoist counts to locals — llvmpipe crashes when BDA fields are read in
  // multiple for-loop conditions simultaneously with single-channel RT output
  uint32_t dir_count = push_constants.global_data->dir_lights_count;
  uint32_t pt_count  = push_constants.global_data->point_lights_count;

  float total_vis    = 0.0;
  float total_weight = 0.0;

  for (uint32_t di = 0; di < dir_count; di++) {
    DirLight light = push_constants.global_data->dir_lights[di];
    if (light.shadow_index == 0xFFFFFFFF) continue;
    float3 L    = normalize(-light.direction);
    float NdotL = saturate(dot(N, L));
    if (NdotL > 0.0) {
      float vis    = eval_visibility_rtx(world_pos, N, L, 10000.0,
                                         sin(light.angular_radius), pixel, di);
      float weight = NdotL * light.intensity;
      total_vis    += vis * weight;
      total_weight += weight;
    }
  }

  for (uint32_t pi = 0; pi < pt_count; pi++) {
    PointLight light = push_constants.global_data->point_lights[pi];
    if (light.shadow_base_index == 0xFFFFFFFF) continue;
    float3 lightVec = light.position - world_pos;
    float  dist     = length(lightVec);
    float3 L        = normalize(lightVec);
    float  NdotL    = saturate(dot(N, L));
    if (NdotL > 0.0) {
      float falloff = pow(saturate(1.0 - pow(dist / light.radius, 4.0)), 2.0)
                    / (dist * dist + 1.0);
      float disk_r  = light.source_radius / max(dist, 0.0001);
      float vis     = eval_visibility_rtx(world_pos, N, L, dist, disk_r, pixel,
                                          dir_count + pi);
      float weight  = NdotL * light.intensity * falloff;
      total_vis    += vis * weight;
      total_weight += weight;
    }
  }

  float vis = total_weight > 0.0 ? total_vis / total_weight : 1.0;
  return float4(vis, 0.0, 0.0, 1.0);
}
