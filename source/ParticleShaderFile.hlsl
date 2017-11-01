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
//섞을 텍스쳐 1,2의 좌표
float2 texOffset1;
float2 texOffset2;
float numberOfRows;

//텍스쳐 1,2의 비율. 0은 tex1, 1은 tex2의 비율이 높음. [0,1]
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
	//사각형 이미지 상에서의 텍스쳐 좌표를 구한다. (textureAtlas 상의 좌표)
	float2 textureCoords=vin.position.xy+float2(0.5,0.5);
	textureCoords.y=1.0f-textureCoords.y;
	textureCoords/=numberOfRows;
	vout.texCoords1=textureCoords;
	vout.texCoords2=textureCoords+texOffset2;

	//파티클의 좌표에 world view proj matrix 적용.
	float4 tmp=mul(float4(vin.position,1), modelViewProjMatrix);
	vout.PosH=tmp;

	return vout;
}

float4 PS(VertexOut pin) :SV_Target
{
	float4 out_color;
	float4 color1 = particleTexture.Sample(samLinear, pin.texCoords1);
	float4 color2 = particleTexture.Sample(samLinear, pin.texCoords2);

	//텍스쳐를 섞는다.
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