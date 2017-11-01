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
	float3 texPosition : POSITION;
};

TextureCube texCubeMap;

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.texPosition=vin.Pos;
	vout.PosH = mul(float4(vin.Pos, 1.0f), worldViewProj);
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
   return texCubeMap.Sample(samLinear, pin.texPosition);
}

technique11 skyTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
