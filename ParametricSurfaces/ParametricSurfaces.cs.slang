RWTexture2D<float4> result;

cbuffer CSCBuffer
{
    float res;
}

float dropOff(float x);
float noise(uint3 id);

[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    result[id.xy] = (1.0 + noise(id)) * 0.5;
}

float dropOff(float x)
{
    float v = 1 - abs(x);
    return 6 * pow(v, 5) - 15 * pow(v, 4) + 10 * pow(v, 3);
}

float noise(uint3 id)
{
    float2 g = normalize(float2(0.2, 0.1));
    float2 v = float2(id.xy) / (res * 0.5) - float2(1.0, 1.0);
    return dot(g, v) * dropOff(v.x) * dropOff(v.y);
}
