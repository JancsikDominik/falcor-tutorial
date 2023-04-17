cbuffer VSCBuffer
{
    float4x4 viewProjection;
    float4x4 model;
}

struct VSOut
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

struct VSIn
{
    float3 objSpacePos : POSOBJ;
    float3 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

VSOut main(in VSIn input)
{
    VSOut output;
    float4x4 mvp = mul(viewProjection, model);
    output.pos = mul(mvp, float4(input.objSpacePos, 1));
    output.color = float4(input.normal, 1);
    return output;
}
