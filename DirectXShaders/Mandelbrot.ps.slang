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
    float4 position : SV_Position;
};

float2 to_mandelbrot_space(float2 screen_pos)
{
    return float2((screen_pos.x / iResolution.x) * 3.5 * (1 / iZoom) - 2.5 + iPoitionOffset.x,
                  (screen_pos.y / iResolution.y) * 2 * (1 / iZoom) - 1 + iPoitionOffset.y);
}

// plotting mandelbrot set using the escape time algorithm
float4 get_pixel_color(float2 c)
{    
    float x = 0.0;
    float y = 0.0;
    int i;
    for (i = 0; (i < iIterations) && ((x * x + y * y) < 4); i++)
    {
        float tmp = (x * x) - (y * y) + c.x;
        y = (2 * x * y) + c.y;
        x = tmp;
    }

    // c is in mandelbrot set
    if (i == iIterations)
    {
        return float4(0, 0, 0, 1);
    }
    else
    {
        // making color palette respect to i

        float r = i * 2;
        // normalizing between 0 and 1
        r = (r % 255) / 255;
        float g = i * 4;
        g = (g % 255) / 255;
        float b = i * 8;
        b = (b % 255) / 255;

        return float4(r, g, b, 1);
    }
}

float4 main(PSInput input) : SV_TARGET
{
    float2 complex_plane_pos = to_mandelbrot_space(input.position.xy);
    return get_pixel_color(complex_plane_pos);
}
