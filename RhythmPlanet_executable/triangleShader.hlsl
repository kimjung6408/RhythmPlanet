#include"samplerState.hlsl"

struct VertexIn
{
	float2 Pos   : POSITION;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

Texture2D imgTexture;

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH=float4(vin.Pos,0.0f, 1.0f);
	vout.texCoord=vin.Pos+float2(0.5,0.5);
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
   return imgTexture.Sample(samLinear, pin.texCoord);
}

technique11 triTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
