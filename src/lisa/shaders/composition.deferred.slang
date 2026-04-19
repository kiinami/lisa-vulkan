//
// Created by kinami on 19/4/26.
//

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

struct CompositionPushConstants {
  GlobalData* global_data;
  uint32_t albedo_idx;
  uint32_t normal_idx;
  uint32_t position_idx;
  uint32_t material_idx;
};

[[vk::push_constant]]
ConstantBuffer<CompositionPushConstants> push_constants;

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
  float3 albedo = textures[push_constants.albedo_idx].Sample(input.uv).rgb;
  float3 N = normalize(textures[push_constants.normal_idx].Sample(input.uv).xyz);
  float3 world_position = textures[push_constants.position_idx].Sample(input.uv).xyz;
  float4 material = textures[push_constants.material_idx].Sample(input.uv);

  float3 V = normalize(push_constants.global_data->camera_position - world_position);

  float3 shading = float3(0.0);

  for (uint32_t i = 0; i < push_constants.global_data->point_lights_count; i++) {
    PointLight light = push_constants.global_data->point_lights[i];
    float3 L = light.position - world_position;
    float distance = length(L);
    L = normalize(L);

    float attenuation = 1.0 / (1.0 + (distance * distance));
    float window = clamp(1.0 - pow(distance / light.radius, 4.0), 0.0, 1.0);
    attenuation *= window * window;

    if (attenuation > 0.0) {
      float3 R = reflect(-L, N);
      float diffuse = max(dot(N, L), 0.0);
      float specular = pow(max(dot(R, V), 0.0), 16.0) * 0.75;
      shading += (diffuse * albedo * specular) * light.color * light.intensity * attenuation;
    }
  }

  for (uint32_t i = 0; i < push_constants.global_data->dir_lights_count; i++) {
    DirLight light = push_constants.global_data->dir_lights[i];
    float3 L = normalize(-light.direction);
    float3 R = reflect(-L, N);
    float diffuse = max(dot(N, L), 0.0);
    float specular = pow(max(dot(R, V), 0.0), 16.0) * 0.75;
    shading += (diffuse * albedo * specular) * light.color * light.intensity;
  }

  float3 ambient = albedo * 0.02;

  return float4(shading + ambient, 1.0);
}