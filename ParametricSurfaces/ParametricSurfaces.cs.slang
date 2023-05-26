RWTexture2D<float4> result;

cbuffer CSCBuffer
{
    float res;
    float seed;
    uint freq;
}

[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    float4 v = float4(id.xyxy) / res;
    float h = 0.0;
    for (int i = 0; i < freq; i++)
    {
        h += noise(v * pow(2.0, i)) * pow(2.0, -(i + 2));
    }
    result[id.xy] = h;
}


float fade(float x)
{
    float v = 1 - abs(x);
    return ((6 * v - 15) * v + 10) * pow(v, 3);
}

// https://en.wikipedia.org/wiki/Perlin_noise
float2 random_gradient(int ix, int iy) {
    // No precomputed gradients mean this works for any number of grid coordinates
    const uint w = 8 * 32;
    const uint s = w / 2; // rotation width
    uint a = ix, b = iy;
    a *= 3284157443; b ^= a << s | a >> w-s;
    b *= 1911520717; a ^= b << s | b >> w-s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
    float2 v;
    v.x = cos(random); v.y = sin(random);
    return v;
}

float dot_gradient(float2 g, float2 v)
{
    return dot(g, v) * fade(v.x) * fade(v.y);
}

float noise(float4 v)
{
    v += float4(seed, 0.0, seed, 0.0);
    uint4 gridCoord = floor(v) + uint4(0, 0, 1, 1);
    v = frac(v) - float4(0.0, 0.0, 1.0, 1.0);
    
    return dot_gradient(random_gradient(gridCoord.x, gridCoord.y), v.xy) + // bottom left
           dot_gradient(random_gradient(gridCoord.z, gridCoord.y), v.zy) + // bottom right
           dot_gradient(random_gradient(gridCoord.x, gridCoord.w), v.xw) + // top left
           dot_gradient(random_gradient(gridCoord.z, gridCoord.w), v.zw);  // top right
}
