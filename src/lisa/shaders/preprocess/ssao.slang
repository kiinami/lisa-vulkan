//
// Created by kinami on 23/04/26
//

import lib.common;
import lib.constants;

Sampler2D textures[];

struct SsaoPushConstants {
  GlobalData* global_data;
  float4* samples;
  uint32_t depth_idx;
  uint32_t normal_idx;
  uint32_t position_idx;
  uint32_t noise_idx;
};

[[vk::push_constant]]
ConstantBuffer<SsaoPushConstants> push;

struct Vertex2Fragment {
  float4 position : SV_POSITION;
  float2 uv;
};

[shader("vertex")]
Vertex2Fragment vertex(uint32_t vertex_id: SV_VertexID) {
  Vertex2Fragment output;

  output.uv = float2((vertex_id << 1) & 2, vertex_id & 2);
  output.position = float4(output.uv * 2.0f - 1.0f, 0.0f, 1.0f);

  return output;
}

[shader("fragment")]
float4 fragment(Vertex2Fragment input) : SV_Target {
  uint screen_width, screen_height;
  textures[push.position_idx].GetDimensions(screen_width, screen_height);

  float2 noise_scale = float2(float(screen_width) / 4.0, float(screen_height) / 4.0);

  float3 position = textures[push.position_idx].Sample(input.uv).xyz;
  float3 normal = normalize(textures[push.normal_idx].Sample(input.uv).xyz);
  float3 noise = textures[push.noise_idx].Sample(input.uv * noise_scale).xyz;

  float3 tangent = normalize(noise - normal * dot(noise, normal));
  float3 bitangent = cross(normal, tangent);
  float3x3 TBN = float3x3(tangent, bitangent, normal);

  float occlusion = 0.0;
  float radius = 0.5;
  float bias = 0.025;

  for(int i = 0; i < 64; i++) {
    float3 sample_pos = mul(TBN, push.samples[i].xyz);
    sample_pos = position + sample_pos * radius;
    
    float4 offset = float4(sample_pos, 1.0);
    offset = mul(push.global_data->view_projection, offset);
    offset.xyz /= offset.w;
    offset.xyz = offset.xyz * 0.5 + 0.5;
    
    float sample_depth = textures[push.position_idx].Sample(offset.xy).z;
    float range_check = smoothstep(0.0, 1.0, radius / abs(position.z - sample_depth));
    occlusion += (sample_depth >= sample_pos.z + bias ? 1.0 : 0.0) * range_check;
  }

  occlusion = 1.0 - (occlusion / 64.0);
  return float4(occlusion, occlusion, occlusion, 1.0);
}
