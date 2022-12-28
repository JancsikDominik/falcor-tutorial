cbuffer MandelbrotPSCB
{
    float2  iResolution;
    int     iIterations;
    float2  iPoitionOffset;
    float   iZoom;
};

// VertexShader output
struct PSInput
{
    float4 color : COLOR;
    float4 position : SV_Position;
};

// function that multiplies 2 complex numbers
float2 cpx_mul(float2 c1, float2 c2)
{
    float a = c1.x;
    float b = c1.y;
    float c = c2.x;
    float d = c2.y;
    return float2(a * c - b * d, b * c + a * d);
}

float2 to_mandelbrot_space(float2 screen_pos)
{
    return float2((screen_pos.x / iResolution.x) * 3.5 * (1 / iZoom) - 2.5 + iPoitionOffset.x,
                  (screen_pos.y / iResolution.y) * 2 * (1 / iZoom) - 1 + iPoitionOffset.y);
}

// TODO: return fractal color instead, so it can have a nicer gradient
bool is_mandelbrot(float2 c)
{    
    float2 n = c;
    for (int i = 0; i < iIterations; ++i)
    {
        n = cpx_mul(n, n) + c;
    }

    return sqrt(n.x * n.x + n.y * n.y) < 0xFFFFFF;
}

float4 main(PSInput input) : SV_TARGET
{
    float2 asd = to_mandelbrot_space(input.position.xy);
    if (is_mandelbrot(asd))
    {
        return input.color;
    }
    else
    {
        return float4(0, 0, 0, 1);
    }
}
