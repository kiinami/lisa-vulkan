//
// Created by kinami on 22/04/26
//

import lib.common;

[[vk::push_constant]]
public ConstantBuffer<PushConstants> push_constants;

struct Output {
  float4 position : SV_Position;
  uint   layer    : SV_RenderTargetArrayIndex;
};

[shader("vertex")]
Output main(Input input, uint instanceID : SV_InstanceID)
{
  Shadow sh = push_constants.global_data->shadows[instanceID];

  Output o;
  o.layer = sh.index;
  o.position = mul(
    sh.view_projection,
    mul(
      push_constants.object_data->model,
      float4(input.position, 1.0)
    )
  );
  return o;
}
