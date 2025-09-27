#pragma once
#include"d3dApp.h"
#include"ShaderProgram.h"
#include"GeometryGenerator.h"
#include"Model.h"
class ModelShader : public ShaderProgram
{

	ID3DX11EffectTechnique* mTech;
	//라이팅을 위한 월드 행렬 (월드 공간상의 vertex 위치를 구함. 이 위치와 light의 위치를 이용)
	ID3DX11EffectMatrixVariable* mWorld;
	ID3DX11EffectMatrixVariable* mWorldViewProj;
	//라이팅을 위한, 노말의 변환을 위한 world행렬의 inverse transpose
	ID3DX11EffectMatrixVariable* mWorldInvTranspose;
	ID3DX11EffectShaderResourceVariable* mTexture_Diffuse[3];
	ID3DX11EffectShaderResourceVariable* mTexture_Specular[2];
	//ID3DX11EffectShaderResourceVariable* mNormalMap;
	ID3DX11EffectVariable* mLightPosition;
	ID3DX11EffectVariable* mAmbientColor;

	//텍스쳐 애니메이션을 위한 offset.
	ID3DX11EffectVariable* mUVOffset;
	ID3DX11EffectVariable* mMeshNumber;
	//inputLayout
	ID3D11InputLayout* mInputLayout;

public:
	struct EntityVertex
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT2 UV;

		EntityVertex(XMFLOAT3 pos, XMFLOAT3 norm, XMFLOAT2 uv)
		{
			Position = pos;
			Normal = norm;
			UV = uv;
		}
	};

	//경로에 맞게 데이터를 불러와서 쉐이더 오브젝트 생성.
	ModelShader(LPCWSTR ShaderFilePath)
		:ShaderProgram(ShaderFilePath), mAmbientColor(0)
	{
		//LoadTextures

		getAllAttributeLocations();
		setInputLayout();
	}

	void getAllAttributeLocations()
	{
		mTech = Shader()->GetTechniqueByName("ModelTech");
		mWorldViewProj = Shader()->GetVariableByName("worldViewProjMatrix")->AsMatrix();
		mWorld = Shader()->GetVariableByName("worldMatrix")->AsMatrix();
		mAmbientColor = Shader()->GetVariableByName("AmbientColor");
		mWorldInvTranspose = Shader()->GetVariableByName("worldInvTranspose")->AsMatrix();
		mTexture_Diffuse[0] = Shader()->GetVariableByName("texture_diffuse1")->AsShaderResource();
		mTexture_Diffuse[1] = Shader()->GetVariableByName("texture_diffuse2")->AsShaderResource();
		mTexture_Diffuse[2]= Shader()->GetVariableByName("texture_diffuse3")->AsShaderResource();
		mTexture_Specular[0] = Shader()->GetVariableByName("texture_specular1")->AsShaderResource();
		mTexture_Specular[1] = Shader()->GetVariableByName("texture_specular2")->AsShaderResource();
		//mNormalMap = Shader()->GetVariableByName("NormalMap")->AsShaderResource();
		mLightPosition = Shader()->GetVariableByName("lightPosition");
		mUVOffset = Shader()->GetVariableByName("UVOffset");
		mMeshNumber = Shader()->GetVariableByName("MeshNumber");
		//mTexture->SetResource(pSRV);
	}

	void setInputLayout()
	{
		D3D11_INPUT_ELEMENT_DESC t;

		D3D11_INPUT_ELEMENT_DESC temp1[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		// Create the input layout
		D3DX11_PASS_DESC passDesc;
		mTech->GetPassByIndex(0)->GetDesc(&passDesc);
		HR(Global::Device()->CreateInputLayout(temp1, 3, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayout));
	}

	ID3DX11EffectTechnique* getTech()
	{
		return mTech;
	}

	ID3D11InputLayout* InputLayout()
	{
		return mInputLayout;
	}


	//변환 행렬을 쉐이더에 로딩한다.
	void LoadWorldViewProjMatrix(XMMATRIX& modelViewProjMatrix)
	{
		mWorldViewProj->SetMatrix(reinterpret_cast<float*>(&modelViewProjMatrix));
	}

	void LoadWorldMatrix(XMMATRIX& worldMatrix)
	{
		mWorld->SetMatrix(reinterpret_cast<float*>(&worldMatrix));
	}

	void LoadWorldInvTranspose(XMMATRIX& worldMatrix)
	{
		XMMATRIX targetMatrix = worldMatrix;

		targetMatrix = XMMatrixInverse(&XMMatrixDeterminant(worldMatrix), worldMatrix);
		targetMatrix = XMMatrixTranspose(targetMatrix);
		mWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&targetMatrix));
	}

	void LoadLightPosition(XMFLOAT3& lightPosition)
	{
		mLightPosition->SetRawValue(&lightPosition, 0, sizeof(XMFLOAT3));
	}

	void LoadUVOffset(XMFLOAT2 UVOffset)
	{
		mUVOffset->SetRawValue(&UVOffset, 0, sizeof(XMFLOAT2));
	}

	void LoadAmbientColor(XMFLOAT4& Ambient)
	{
		mAmbientColor->SetRawValue(&Ambient, 0, sizeof(XMFLOAT4));
	}

	void LoadTexture(ID3D11ShaderResourceView* SRV, string type, int index)
	{
		if (type == "texture_diffuse")
		{
			mTexture_Diffuse[index - 1]->SetResource(SRV);
		}
		else if (type == "texture_specular")
		{
			mTexture_Specular[index - 1]->SetResource(SRV);
		}
	}

	~ModelShader()
	{
		ReleaseCOM(mInputLayout);
	}

	void LoadMeshNumber(int MeshNumber)
	{
		mMeshNumber->SetRawValue(&MeshNumber, 0, sizeof(int));
	}
};
