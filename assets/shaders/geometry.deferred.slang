//
// Created by kinami on 19/4/26.
//

struct Input {
  float3 position;
  float3 normal;
  float2 uv;
}

Sampler2D textures[];

struct PointLight {
  float3 position;
  float intensity;
  float3 color;
  float radius;
}

struct DirLight {
  float3 direction;
  float intensity;
  float3 color;
  float padding;
}

struct GlobalData {
  float4x4 view_projection;
  float4x4 projection;
  float4x4 view;

  PointLight* point_lights;
  uint32_t point_lights_count;

  DirLight* dir_lights;
  uint32_t dir_lights_count;

  float3 camera_position;
  float padding;
};

struct ObjectData {
  float4x4 model;
  float4 color;
  uint32_t texture_index;
};

struct PushConstants {
  GlobalData* global_data;
  ObjectData* object_data;
};

[[vk::push_constant]]
ConstantBuffer<PushConstants> push_constants;

struct Vertex2Fragment {
  float4 position : SV_POSITION;
  float3 normal;
  float3 world_position;
  float2 uv;
  uint32_t texture_index;
};

struct Output {
  float4 albedo : SV_Target0;
  float4 normal : SV_Target1;
  float4 position : SV_Target2;
  float4 material : SV_Target3;
};

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
  output.texture_index = push_constants.object_data->texture_index;

  return output;
}

[shader("fragment")]
Output fragment(Vertex2Fragment input) {
  Output output;

  float3 albedo = textures[input.texture_index].Sample(input.uv).rgb;

  output.albedo = float4(albedo, 1.0);
  output.normal = float4(normalize(input.normal), 0.0);
  output.position = float4(input.world_position, 1.0);
  output.material = float4(0.0, 0.0, 0.0, 1.0);

  return output;
}
