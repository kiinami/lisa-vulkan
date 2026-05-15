//
// Created by kinami on 22/04/26
//

import lib.common;

[[vk::push_constant]]
public ConstantBuffer<PushConstants> push_constants;

[shader("vertex")]
float4 main(Input input) : SV_Position
{
    float4 local_pos = float4(input.position, 1.0);
    float4 world_pos = mul(push_constants.object_data.model, local_pos);
    return mul(push_constants.global_data.shadows[0].view_projection, world_pos);
}
