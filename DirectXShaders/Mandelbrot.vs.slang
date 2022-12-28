struct VSOutput
{
    float4 color: COLOR;
    float4 position: SV_Position;
};

VSOutput main(uint vertexID : SV_VertexID)
{
    float4 positions[6] =
    {
	    float4( 1,  1, 0, 1),
	    float4(-1,  1, 0, 1),
        float4( 1, -1, 0, 1),

	    float4(-1, -1, 0, 1),
	    float4( 1, -1, 0, 1),
	    float4(-1,  1, 0, 1)
    };

    float4 colors[6] =
    {
	    float4(0, 1, 0, 1),
        float4(1, 0, 0, 1),
	    float4(0, 0, 1, 1),

	    float4(0, 1, 0, 1),
	    float4(0, 0, 1, 1),
	    float4(1, 1, 1, 1)
    };
    
    VSOutput output;

    output.position = positions[vertexID];
    output.color = colors[vertexID];

    return output;
}
