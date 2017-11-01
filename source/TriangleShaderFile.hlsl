#include"samplerState.hlsl"


BlendState TriangleBlending
{
   BlendEnable[0]           = TRUE;
   SrcBlend                 = SRC_ALPHA;
   DestBlend                = INV_SRC_ALPHA;
   BlendOp                  = ADD;
   SrcBlendAlpha            = ONE;
   DestBlendAlpha           = ONE;
   BlendOpAlpha             = ADD;
   RenderTargetWriteMask[0] = 0x0F;
};

cbuffer inputVariables
{
matrix worldViewProj;
}

struct VertexIn
{
	float2 position :POSITION;
};

struct VertexOut
{
	float4 PosH :SV_POSITION;
};

Texture2D Image;

VertexOut VS(VertexIn vin){
	VertexOut vout;
	vout.PosH=mul(float4(vin.position, 0.0f, 1.0f), worldViewProj);

	return vout;
}

float4 PS(VertexOut pin) :SV_Target
{
	float4 Color=float4(1.0f,1.0f,1.0f,0.53f);
	return Color;
}

technique11 TriangleTech
{
	pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
		SetBlendState(TriangleBlending, float4(0.0f,0.0f,0.0f,0.0f), 0xffffffff);
    }
}