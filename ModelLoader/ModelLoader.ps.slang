struct PSIn
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

float4 main(PSIn input) : SV_TARGET
{
    return input.color;
}
