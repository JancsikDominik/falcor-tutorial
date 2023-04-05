struct PSIn
{
    float4 worldPos : SV_Position;
};

float4 main(PSIn in) : SV_TARGET
{
    return in.worldPos;
}
