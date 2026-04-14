/* Copyright (c) 2025-2026, Sascha Willems
 *
 * SPDX-License-Identifier: MIT
 *
 */

struct VSInput {
    float3 Pos;
    float3 Normal;
    float2 UV;
};

Sampler2D textures[];

struct GlobalViewData {
    float4x4 view_projection;
    float4x4 projection;
    float4x4 view;
};

struct ObjectData {
    float4x4 model;
    float4 color;
    uint32_t texture_index;
};

struct PushConstants {
    GlobalViewData* globalData;
    ObjectData* objectData;
};

[[vk::push_constant]]
ConstantBuffer<PushConstants> pushConstants;

struct VSOutput {
    float4 Pos : SV_POSITION;
    float3 Normal;
    float2 UV;
    float3 LightVec;
    float3 ViewVec;
    uint32_t texture_index;
};

[shader("vertex")]
VSOutput vertexMain(VSInput input) {
    VSOutput output;
    float4x4 modelMat = pushConstants.objectData->model;
    float4x4 viewMat = pushConstants.globalData->view;
    float4x4 projMat = pushConstants.globalData->projection;

    output.Normal = mul((float3x3)mul(viewMat, modelMat), input.Normal);
    output.UV = input.UV;
    output.Pos = mul(projMat, mul(viewMat, mul(modelMat, float4(input.Pos.xyz, 1.0))));
    output.texture_index = pushConstants.objectData->texture_index;

    float4 fragPos = mul(mul(viewMat, modelMat), float4(input.Pos.xyz, 1.0));
    // Hardcoded light pos for now or put in globalData
    float4 lightPos = float4(0.0, 5.0, 0.0, 1.0);
    output.LightVec = lightPos.xyz - fragPos.xyz;
    output.ViewVec = -fragPos.xyz;
    return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) {
    float3 N = normalize(input.Normal);
    float3 L = normalize(input.LightVec);
    float3 V = normalize(input.ViewVec);
    float3 R = reflect(-L, N);
    float3 diffuse = max(dot(N, L), 0.0025);
    float3 specular = pow(max(dot(R, V), 0.0), 16.0) * 0.75;

    float3 color = textures[input.texture_index].Sample(input.UV).rgb;
    return float4(diffuse * color.rgb + specular, 1.0);
}