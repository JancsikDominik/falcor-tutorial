struct PSIn
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};


cbuffer PSCBuffer
{
    //// light properties: ambient, diffuse, specular
    //float3 lightAmbient;
    //float3 lightDiffuse;
    //float3 lightSpecular;

    //float3 lightDir;

    //// material properties: ambient, diffuse, specular
    //float3 materialAmbient;
    //float3 materialDiffuse;
    //float3 materialSpecular;

    float3 cameraPosition;

    //// textures
    //Texture2D objTexture;
    //SamplerState texSampler;

    //bool isTextureLoaded;
};

float4 main(PSIn input) : SV_TARGET
{
    return float4(0, 1, 0, 1);
}
