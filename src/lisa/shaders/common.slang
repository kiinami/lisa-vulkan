//
// Created by kinami on 22/04/26
//

module common;

public struct Input {
  public float3 position;
  public float3 normal;
  public float2 uv;
  public float3 tangent;
}

public struct PointLight {
  public float3 position;
  public float intensity;
  public float3 color;
  public float radius;
}

public struct DirLight {
  public float3 direction;
  public float intensity;
  public float3 color;
  float padding;
}

public struct GlobalData {
  public float4x4 view_projection;
  public float4x4 projection;
  public float4x4 view;

  public PointLight* point_lights;
  public uint32_t point_lights_count;

  public DirLight* dir_lights;
  public uint32_t dir_lights_count;

  public float3 camera_position;
  float padding;
};

public struct ObjectData {
  public float4x4 model;
  public float4 color;
  public float roughness;
  public float metallic;
  public uint32_t diffuse_texture_index;
  public uint32_t roughness_texture_index;
  public uint32_t metallic_texture_index;
  public uint32_t normal_texture_index;
  float2 padding;
};

public struct PushConstants {
  public GlobalData* global_data;
  public ObjectData* object_data;
};
