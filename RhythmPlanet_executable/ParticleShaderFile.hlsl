#include"samplerState.hlsl"

DepthStencilState DisableWriteDepth
{
	DepthEnable=TRUE;
	DepthWriteMask=ZERO;
};

BlendState AdditiveBlending
{
	AlphaToCoverageEnable=FALSE;
	BlendEnable[0]=TRUE;
	SrcBlend=SRC_ALPHA;
	DestBlend=ONE;
	BlendOp=ADD;
	SrcBlendAlpha=ZERO;
	DestBlendAlpha=ZERO;
	BlendOpAlpha=ADD;
	RenderTargetWriteMask[0]=0x0F;
};

cbuffer inputVariables
{
//���� �ؽ��� 1,2�� ��ǥ
float2 texOffset1;
float2 texOffset2;
float numberOfRows;

//�ؽ��� 1,2�� ����. 0�� tex1, 1�� tex2�� ������ ����. [0,1]
float blendFactor;

//world*view*proj
matrix modelViewProjMatrix;
}

struct VertexIn
{
	float3 position :POSITION;
};

struct VertexOut
{
	float4 PosH :SV_POSITION;
	float2 texCoords1 :TEXCOORD0;
	float2 texCoords2 :TEXCOORD1;
};

Texture2D particleTexture;

VertexOut VS(VertexIn vin){
	VertexOut vout;
	//�簢�� �̹��� �󿡼��� �ؽ��� ��ǥ�� ���Ѵ�. (textureAtlas ���� ��ǥ)
	float2 textureCoords=vin.position.xy+float2(0.5,0.5);
	textureCoords.y=1.0f-textureCoords.y;
	textureCoords/=numberOfRows;
	vout.texCoords1=textureCoords;
	vout.texCoords2=textureCoords+texOffset2;

	//��ƼŬ�� ��ǥ�� world view proj matrix ����.
	float4 tmp=mul(float4(vin.position,1), modelViewProjMatrix);
	vout.PosH=tmp;

	return vout;
}

float4 PS(VertexOut pin) :SV_Target
{
	float4 out_color;
	float4 color1 = particleTexture.Sample(samLinear, pin.texCoords1);
	float4 color2 = particleTexture.Sample(samLinear, pin.texCoords2);

	//�ؽ��ĸ� ���´�.
	out_color = lerp(color1, color2, blendFactor);
	return out_color;
}

technique11 particleTech
{
	pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
		SetDepthStencilState(DisableWriteDepth, 0);
		SetBlendState(AdditiveBlending, float4(0.0f,0.0f,0.0f,0.0f), 0xffffffff);
		
    }
}