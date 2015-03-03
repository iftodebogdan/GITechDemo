struct VOut
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

float4x4 worldViewProjMat;

VOut vsmain(float4 position : POSITION, float2 texcoord : TEXCOORD)
{
    VOut output;

    output.position = mul(worldViewProjMat, position);
    output.texcoord = texcoord;
    
    return output;
}

sampler2D Tex;

float4 psmain(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    return tex2D(Tex, texcoord);
    //return float4(1, 1, 1, 1);
}