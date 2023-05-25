cbuffer VSCBuffer
{
    float4x4 viewProjection;
    float4x4 model;
    float4x4 modelIT;
}

struct VSOut
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct VSIn
{
    float3 objSpacePos : POSOBJ;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

VSOut main(in VSIn input)
{
    VSOut output;
    float4x4 mvp = mul(viewProjection, model);
    output.pos = mul(mvp, float4(input.objSpacePos, 1));
    output.normal = mul(modelIT, float4(input.normal, 1)).xyz;
    output.texCoord = input.texCoord;
    return output;
}
