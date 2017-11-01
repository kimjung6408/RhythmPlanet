#include"samplerState.hlsl"

DepthStencilState DisableDepth
{
	DepthEnable=FALSE;
	DepthWriteMask=ZERO;
};

cbuffer inputVariables
{
//¼¯À» ÅØ½ºÃÄ 1,2ÀÇ ÁÂÇ¥
float2 BG_uvOffset;
float2 grid_uvOffset;
}

struct VertexIn
{
	float2 position :POSITION;
};

struct VertexOut
{
	float4 PosH :SV_POSITION;
	float2 BGTexCoord : TEXCOORD0;
	float2 gridTexCoord : TEXCOORD1;
};

Texture2D BGImage;
Texture2D gridImage;

VertexOut VS(VertexIn vin){
	VertexOut vout;

	float2 textureCoord=vin.position.xy/2.0f+float2(0.5,0.5);
	vout.PosH=float4(vin.position, 0, 1);
	vout.gridTexCoord=textureCoord*0.8f;
	vout.BGTexCoord=textureCoord*0.6f;

	return vout;
}

float4 PS(VertexOut pin) :SV_Target
{
	float4 out_color;
	float4 BGColor1= BGImage.Sample(samLinear, pin.BGTexCoord+BG_uvOffset);
	float4 BGColor2= BGImage.Sample(samLinear, pin.BGTexCoord+float2(BG_uvOffset.x+grid_uvOffset.y,0.3f));
	float4 GridColor = gridImage.Sample(samLinear, pin.gridTexCoord+grid_uvOffset);
	GridColor*=0.3f;
	

	//ÅØ½ºÃÄ¸¦ ¼¯´Â´Ù.
	out_color = GridColor+BGColor1+BGColor2;
	return out_color;
}

technique11 BGTech
{
	pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
		SetDepthStencilState(DisableDepth, 0);		
    }
}