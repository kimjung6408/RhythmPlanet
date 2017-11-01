#include"samplerState.hlsl"

DepthStencilState DisableDepth
{
	DepthEnable=FALSE;
	DepthWriteMask=ZERO;
};

BlendState AnimBlending
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
matrix matWorld;
float fadeFactor;
float Alpha;
float2 UVSize;
float Row;
float Column;
}

struct VertexIn
{
	float2 position :POSITION;
};

struct VertexOut
{
	float4 PosH :SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

Texture2D Image;

VertexOut VS(VertexIn vin){
	VertexOut vout;

	vout.TexCoord=float2(vin.position.x, -vin.position.y)/2.0f+float2(0.5,0.5);
	vout.PosH=mul(float4(vin.position, 0.0f, 1.0f), matWorld);
	vout.PosH.z=1.0f;

	return vout;
}

float4 PS(VertexOut pin) :SV_Target
{
	float u = Column / UVSize.u;
	float v = Row / UVSize.v;
	float2 ScaledTexCoord=float2(pin.TexCoord.u*UVSize.u, pin.TexCoord.v*UVSize.v)+float2(u,v);
	float4 Color= Image.Sample(samLinear, ScaledTexCoord);
	Color.a*=Alpha;
	Color*=(0.5f+abs(0.5f-fadeFactor));
	return Color;
}

technique11 AnimTech
{
	pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
		SetDepthStencilState(DisableDepth, 0);	
		SetBlendState(AnimBlending, float4(0.0f,0.0f,0.0f,0.0f), 0xffffffff);
    }
}