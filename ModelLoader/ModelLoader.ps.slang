struct PSIn
{
    float3 worldPos : POSW;
    float4 color;
};

float4 main(PSIn in) : SV_TARGET
{
    return in.color;
}
