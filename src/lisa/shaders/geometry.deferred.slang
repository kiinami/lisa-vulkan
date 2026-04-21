//
// Created by kinami on 19/4/26.
//

struct Input {
  float3 position;
  float3 normal;
  float2 uv;
  float3 tangent;
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
  float roughness;
  float metallic;
  uint32_t diffuse_texture_index;
  uint32_t roughness_texture_index;
  uint32_t metallic_texture_index;
  uint32_t normal_texture_index;
  float2 padding;
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
  float3 tangent;
  uint32_t diffuse_texture_index;
  uint32_t roughness_texture_index;
  uint32_t metallic_texture_index;
  uint32_t normal_texture_index;
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
  output.tangent = mul((float3x3) model, input.tangent);
  output.diffuse_texture_index = push_constants.object_data->diffuse_texture_index;
  output.roughness_texture_index = push_constants.object_data->roughness_texture_index;
  output.metallic_texture_index = push_constants.object_data->metallic_texture_index;
  output.normal_texture_index = push_constants.object_data->normal_texture_index;

  return output;
}

[shader("fragment")]
Output fragment(Vertex2Fragment input) {
Output output;
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

  output.albedo = float4(albedo, 1.0);
  output.normal = float4(N, 0.0);
  output.position = float4(input.world_position, 1.0);
  output.material = float4(final_roughness, final_metallic, 0.0, 1.0);

  return output;
}
