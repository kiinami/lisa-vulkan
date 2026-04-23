//
// Created by kinami on 14/4/26.
//

struct Input {
  float3 position;
  float3 normal;
  float2 uv;
};

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
  float roughness;
  float metallic;
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
  uint32_t texture_index;
};

[shader("vertex")]
Vertex2Fragment vertex(Input input) {
  Vertex2Fragment output;

  float4x4 model = push_constants.object_data->model;
  float4x4 view = push_constants.global_data->view;
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
float4 fragmentMain(Vertex2Fragment input) {
  float3 N = normalize(input.normal);
  float3 V = normalize(push_constants.global_data.camera_position - input.world_position);

  float3 shading = float3(0.0);
  float4 base_color = push_constants.object_data->color;
  float3 albedo = base_color.rgb;
  if (input.texture_index != 0xFFFFFFFF) {
    albedo *= textures[input.texture_index].Sample(input.uv).rgb;
  }

  // Point lights
  for (uint32_t i = 0; i < push_constants.global_data.point_lights_count; i++) {
    PointLight light = push_constants.global_data->point_lights[i];

    float3 L = light.position - input.world_position;
    float distance = length(L);
    L = normalize(L);

    float attenuation = 1.0 / (1.0 + (distance * distance));
    float window = clamp(1.0 - pow(distance / light.radius, 4.0), 0.0, 1.0);
    attenuation *= window * window;

    if (attenuation > 0.0) {
      float3 R = reflect(-L, N);

      float diffuse = max(dot(N, L), 0.0);
      float specular = pow(max(dot(R, V), 0.0), 16.0) * 0.75;
      float3 radiance = light.color * light.intensity * attenuation;

      shading += (diffuse * albedo * specular) * radiance;
    }
  }

  // Directional lights
  for (uint32_t i = 0; i < push_constants.global_data.point_lights_count; i++) {
    DirLight light = push_constants.global_data->dir_lights[i];

    float3 L = normalize(-light.direction);
    float3 R = reflect(-L, N);

    float diffuse = max(dot(N, L), 0.0);
    float specular = pow(max(dot(R, V), 0.0), 16.0) * 0.75;
    float3 radiance = light.color * light.intensity;

    shading += (diffuse * albedo * specular) * radiance;
  }

  float3 ambient = albedo * 0.02;

  return float4(shading + ambient, 1.0);
}