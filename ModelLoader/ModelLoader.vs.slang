cbuffer VSCBuffer
{
    matrix<float, 4, 4> viewProjection;
}

struct VSOut
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

VSOut main(float3 objSpacePos : POSW)
{
    VSOut output;
    output.pos = mul(viewProjection, float4(objSpacePos, 1));
    output.color = float4(objSpacePos, 1);
    return output;
}
