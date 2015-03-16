struct VSOut
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

sampler2D Tex;
float4x4 worldViewProjMat;

VSOut vsmain(float4 position : POSITION, float2 texcoord : TEXCOORD)
{
    VSOut output;

    output.position = mul(worldViewProjMat, position);
    output.texcoord = texcoord;
    
    return output;
}


float4 psmain(VSOut input) : SV_TARGET
{
    return tex2D(Tex, input.texcoord);
    //return float4(1, 1, 1, 1);
}