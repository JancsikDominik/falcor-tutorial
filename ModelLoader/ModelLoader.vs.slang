struct VSIn
{
    float3 localPos : SV_POSITION;
};

struct VSOut
{
    float3 worldPos;
    float4 color;
};

cbuffer VSCBuffer
{
    matrix<float, 4, 4> viewProjection;
}

VSOut main(VSIn vsIn)
{
    VSOut out;

    out.worldPos = mul(viewProjection, float4(vsIn.localPos, 1.f)).xyz;
    out.color = float4(0.25, 0.25, 1, 1);

    return out;
}
