#include"samplerState.hlsl"

cbuffer cbPerObject
{
	float4x4 worldViewProj;
};

struct VertexIn
{
	float3 Pos   : POSITION;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	float4 outPos;
	outPos = mul(float4(vin.Pos,1.0), worldViewProj);
    vout.PosH=outPos;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
   return float4(1,0,0,1);
}

technique11 boxTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
