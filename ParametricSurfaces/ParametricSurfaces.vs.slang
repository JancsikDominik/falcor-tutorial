struct ModelSettings
{
    float3 ambient;
    float3 diffuse;
    float3 specular;

    float4x4 transform;

    Texture2D tex;
};

cbuffer VSCBuffer
{
    float4x4 viewProjection;
    ModelSettings settings[32];
}

struct VSOut
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    uint modelIndex : SV_RenderTargetArrayIndex;
};

struct VSIn
{
    float3 objSpacePos : POSOBJ;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    uint modelIndex : MODELINDEX;
};

VSOut main(in VSIn input)
{
    VSOut output;
    ModelSettings obj = settings[input.modelIndex];
    float4x4 mvp = mul(viewProjection, obj.transform);
    output.pos = mul(mvp, float4(input.objSpacePos, 1));
    output.normal = mul(obj.transform, float4(input.normal, 1)).xyz;
    output.texCoord = input.texCoord;
    output.modelIndex = input.modelIndex;
    return output;
}
