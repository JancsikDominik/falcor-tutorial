cbuffer VSCBuffer
{
    float4x4 viewProjection;
    float4x4 model;

    // 1 -> X, 2 -> Y, 3 -> XY, any other number -> None
    uint flipTextureOnAxis;
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
    output.normal = mul(model, float4(input.normal, 1)).xyz;

    float2 flipVector = float2(1.0, 1.0);
    if (flipTextureOnAxis == 1)
        flipVector = float2(-1.0, 1.0);
    else if (flipTextureOnAxis == 2)
        flipVector = float2(1.0, -1.0);
    else if (flipTextureOnAxis == 3)
        flipVector = float2(-1.0, -1.0);
    
    output.texCoord = input.texCoord * flipVector;
    return output;
}
