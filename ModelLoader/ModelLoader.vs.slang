struct VSIn
{
    float4 objSpacePos : SV_Position;
};

struct VSOut
{
    float4 worldPos : SV_Position;
};

cbuffer VSCBuffer
{
    matrix<float, 4, 4> viewProjection;
}

VSOut main(VSIn vsIn)
{
    VSOut out;

    out.worldPos = mul(viewProjection, vsIn.objSpacePos);

    return out;
}
