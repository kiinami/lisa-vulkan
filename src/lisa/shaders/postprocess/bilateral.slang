//
// Created by kinami on 6/18/26.
//

import lib.common;

[[vk::binding(0, 0)]] Sampler2D textures[];

struct BilateralPushConstants {
  GlobalData* global_data;
  uint32_t shadow_idx;
  uint32_t position_idx;
  uint32_t normal_idx;
  float sigma_spatial;
  float sigma_depth;
  float sigma_normal;
};

[[vk::push_constant]]
ConstantBuffer<BilateralPushConstants> push;

struct Vertex2Fragment {
  float4 position : SV_POSITION;
  float2 uv;
};

static const int BILATERAL_RADIUS = 5;

[shader("vertex")]
Vertex2Fragment vertex(uint32_t vertex_id: SV_VertexID) {
  Vertex2Fragment output;
  output.uv = float2((vertex_id << 1) & 2, vertex_id & 2);
  output.position = float4(output.uv * 2.0f - 1.0f, 0.0f, 1.0f);
  return output;
}

[shader("fragment")]
float4 fragment(Vertex2Fragment input) : SV_Target {
  float3 pos_p = textures[push.position_idx].Sample(input.uv).xyz;
  float3 n_p   = normalize(textures[push.normal_idx].Sample(input.uv).xyz);
  float2 ts    = push.global_data->texel_size;

  // hoist counts — llvmpipe crashes when BDA fields read in multiple loop
  // conditions simultaneously with R8Unorm render target
  uint32_t pt_count  = push.global_data->point_lights_count;
  uint32_t dir_count = push.global_data->dir_lights_count;

  float cone_sum   = 0.0;
  float weight_sum = 0.0;

  for (uint32_t pi = 0; pi < pt_count; pi++) {
    PointLight light = push.global_data->point_lights[pi];
    float dist   = length(light.position - pos_p);
    float cone_r = light.source_radius / max(dist, 0.001);
    cone_sum    += light.intensity * cone_r;
    weight_sum  += light.intensity;
  }

  for (uint32_t di = 0; di < dir_count; di++) {
    DirLight light = push.global_data->dir_lights[di];
    float cone_r = sin(light.angular_radius);
    cone_sum    += light.intensity * cone_r;
    weight_sum  += light.intensity;
  }

  float effective_cone_r = weight_sum > 0.0 ? cone_sum / weight_sum : 0.0;
  float sigma_s = max(push.sigma_spatial * effective_cone_r, 0.01);

  float sum_w = 0.0;
  float sum_v = 0.0;

  for (int dy = -BILATERAL_RADIUS; dy <= BILATERAL_RADIUS; dy++) {
    for (int dx = -BILATERAL_RADIUS; dx <= BILATERAL_RADIUS; dx++) {
      float2 uv_q  = input.uv + float2(float(dx), float(dy)) * ts;
      float  v_q   = textures[push.shadow_idx].Sample(uv_q).r;
      float3 pos_q = textures[push.position_idx].Sample(uv_q).xyz;
      float3 n_q   = normalize(textures[push.normal_idx].Sample(uv_q).xyz);

      float d2  = float(dx * dx + dy * dy);
      float w_s = exp(-d2 / (2.0 * sigma_s * sigma_s));

      float dp  = length(pos_q - pos_p);
      float w_d = exp(-(dp * dp) / (2.0 * push.sigma_depth * push.sigma_depth));

      float w_n = pow(max(0.0, dot(n_p, n_q)), push.sigma_normal);

      float w = w_s * w_d * w_n;
      sum_w += w;
      sum_v += w * v_q;
    }
  }

  float vis = sum_w > 0.0 ? sum_v / sum_w : 0.0;
  return float4(vis, 0.0, 0.0, 1.0);
}
