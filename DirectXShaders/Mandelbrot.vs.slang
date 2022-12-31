struct VSOutput
{
    float4 position: SV_Position;
};

static const float4 positions[6] =
{
    float4(1, 1, 0, 1),
	float4(-1, 1, 0, 1),
    float4(1, -1, 0, 1),

	float4(-1, -1, 0, 1),
	float4(1, -1, 0, 1),
	float4(-1, 1, 0, 1)
};

VSOutput main(uint vertexID : SV_VertexID)
{   
    VSOutput output;
    output.position = positions[vertexID];

    return output;
}
