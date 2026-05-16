//
// Created by kinami on 22/04/26
//

import lib.common;

[[vk::push_constant]]
public ConstantBuffer<PushConstants> push_constants;

static const uint MAX_SHADOW_LAYERS = 28;

struct Vertex2Geometry
{
    float4 position : TEXCOORD0;
};

struct Output
{
    float4 position : SV_Position;
    uint layer : SV_RenderTargetArrayIndex;
};

[shader("vertex")]
Vertex2Geometry vertex(Input input)
{
    Vertex2Geometry output;
    float4 local_pos = float4(input.position, 1.0);
    output.position = mul(push_constants.object_data->model, local_pos);
    return output;
}

[shader("geometry")]
[maxvertexcount(84)]
void geometry(
    triangle Vertex2Geometry v[3],
    inout TriangleStream<Output> stream
) {
    uint count = push_constants.global_data->shadows_count;
    if (count > MAX_SHADOW_LAYERS) count = MAX_SHADOW_LAYERS;

    for (uint s = 0; s < count; ++s)
    {
        Shadow sh = push_constants.global_data->shadows[s];

        Output o;
        o.layer = sh.index;

        o.position = mul(sh.view_projection, v[0].position); stream.Append(o);
        o.position = mul(sh.view_projection, v[1].position); stream.Append(o);
        o.position = mul(sh.view_projection, v[2].position); stream.Append(o);

        stream.RestartStrip();
    }
}
