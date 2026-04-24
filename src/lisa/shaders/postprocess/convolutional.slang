//
// Created by kinami on 23/04/26
//

import lib.common;
import lib.constants;

Sampler2D textures[];

struct ConvolutionalPushConstants {
  GlobalData* global_data;
  float* kernel;
  uint32_t kernel_size;
  uint32_t input_idx;
};

[[vk::push_constant]]
ConstantBuffer<ConvolutionalPushConstants> push;

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
  float4 output = float4(0.0);

  int half = int(push.kernel_size) / 2;

  for (int row = -half; row <= half; row++) {
    for (int col = -half; col <= half; col++) {
      float k = push.kernel[(row + half) * push.kernel_size + (col + half)];
      float2 offset = float2(col, row) * push.global_data->texel_size;
      output += textures[push.input_idx].Sample(input.uv + offset) * k;
    }
  }

  return output;
}
