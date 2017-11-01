#include"Lighting.hlsl"

DepthStencilState DisableDepth
{
	DepthEnable=FALSE;
	DepthWriteMask=ZERO;
};

BlendState AlphaBlending
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

cbuffer cbPerObject
{
	float4x4 worldMatrix;
	float4x4 worldViewProjMatrix;
	float4x4 worldInvTranspose;
	float4 AmbientColor;
	float2 UVOffset;
};

cbuffer cbLightCalc
{
	float3 lightPosition;
};

struct VertexIn
{
	float3 Pos   : POSITION;
	float3 Normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 PosW :POSITION;
	float3 Normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

//ball�� ���� �̹��� ������ shader resource view
Texture2D Texture;

//Normal Map texture
//Texture2D NormalMap;

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	//��� ���� world���� �������� ��ȯ��.
	vout.PosW=mul(float4(vin.Pos,1.0f), worldMatrix).xyz;
	vout.Normal=mul(float4(vin.Normal,0.0f), worldInvTranspose).xyz;

	//Homogeneous space ���� ��ǥ.
	vout.PosH=mul(float4(vin.Pos, 1.0f), worldViewProjMatrix);

	//texture sample ��ǥ.
	vout.texCoord=vin.texCoord;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	pin.Normal=normalize(pin.Normal);

	//float3 toCameraVector=camPosition-pin.PosW;

	////camera�� �� ��ǥ ������ �Ÿ��� ����.
	//float distanceToCam=length(toCameraVector);

	//����ȭ.
	//toCameraVector/=distanceToCam;

	//distance from light
	//float distanceToLight=length(toLightVector);
	float2 TexCoordRepaired=float2(pin.texCoord.x, pin.texCoord.y*1.70f);
	float4 texColor=Texture.Sample(samLinear, TexCoordRepaired);
	return texColor;
///*begin get Normal*/
//	float3 tangentSpaceSample=NormalMap.Sample(samLinear, pin.texCoord).rgb;
//	float3 tangentSpaceNormal=2.0f*tangentSpaceSample-1.0f;

//	//Tangent space ��ǥ�踦 ���Ѵ�.
//	//Gram-Schmidt �˰��� ����.
//	float3 N=pin.Normal;
//	float3 T=normalize(pin.Tangent-dot(pin.Tangent, N)*N);
//	float3 B=cross(N,T);

//	//tangent space���� world space�� ���� ����� ���Ѵ�.
//	float3x3 tangentToWorld=float3x3(T,B,N);

//	//Normal�� tangent space���� world space�� ��ȯ�Ѵ�.
//	//calculate world space normal
//	float3 worldSpaceNormal=mul(tangentSpaceNormal, tangentToWorld);
//	worldSpaceNormal=normalize(worldSpaceNormal);
//	/*end getNormal*/

	//texColor to diffuse Color
}

technique11 EntityTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
		SetDepthStencilState(DisableDepth, 0);	
		SetBlendState(AlphaBlending, float4(0.0f,0.0f,0.0f,0.0f), 0xffffffff);
    }
}
