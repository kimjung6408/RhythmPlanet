#pragma once
#include"ShaderProgram.h"
class ParticleShader : public ShaderProgram
{
private:
	//Attribute들을 선언하고 getAllAttributeLocations에서 초기화
	//skybox는 월드변환 및 view변환의 translation을 적용하지 않는다.

	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mModelViewProj;
	ID3DX11EffectShaderResourceVariable* mTexture;
	ID3DX11EffectVariable* mTexOffset1;
	ID3DX11EffectVariable* mTexOffset2;
	ID3DX11EffectVariable* mBlendFactor;
	ID3DX11EffectVariable* mNumOfRows;

	//inputLayout
	ID3D11InputLayout* mInputLayout;

private:
	//initialize시 호출됨. c++코드와 쉐이더를 연결함.
	void getAllAttributeLocations()
	{
		mTech = Shader()->GetTechniqueByName("particleTech");
		mModelViewProj = Shader()->GetVariableByName("modelViewProjMatrix")->AsMatrix();
		mTexOffset1 = Shader()->GetVariableByName("texOffset1");
		mTexOffset2 = Shader()->GetVariableByName("texOffset2");
		mBlendFactor = Shader()->GetVariableByName("blendFactor");
		mNumOfRows = Shader()->GetVariableByName("numberOfRows");
		mTexture = Shader()->GetVariableByName("particleTexture")->AsShaderResource();
	}

	//input layout 설정.
	void setInputLayout()
	{
		D3D11_INPUT_ELEMENT_DESC temp1[1] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		// Create the input layout
		D3DX11_PASS_DESC passDesc;
		mTech->GetPassByIndex(0)->GetDesc(&passDesc);
		HR(Global::Device()->CreateInputLayout(temp1, 1, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayout));
	}

public:
	ParticleShader(LPCWSTR filePath)
		:ShaderProgram(filePath), mTech(0), mModelViewProj(0), mTexture(0)
	{
		getAllAttributeLocations();
		setInputLayout();
	}

	void loadTexture(ID3D11ShaderResourceView* textureSRV)
	{
		mTexture->SetResource(textureSRV);
	}

	//변환 행렬을 쉐이더에 로딩한다.
	void loadModelViewProjMatrix(XMMATRIX& modelViewProjMatrix)
	{
		mModelViewProj->SetMatrix(reinterpret_cast<float*>(&modelViewProjMatrix));
	}

	void loadTextureOffset1(XMFLOAT2 offset)
	{
		mTexOffset1->SetRawValue(&offset, 0, sizeof(XMFLOAT2));
	}

	void loadTextureOffset2(XMFLOAT2 offset)
	{
		mTexOffset2->SetRawValue(&offset, 0, sizeof(XMFLOAT2));
	}

	void loadBlendFactor(float blendFactor)
	{
		mBlendFactor->SetRawValue(&blendFactor, 0, sizeof(float));
	}

	void loadNumberOfRows(int numOfRows)
	{
		//GPU는 floating point가 기본 연산이므로, 연산의 편의를 위해 float로 변환하여 전달함.
		float numValue = (float)numOfRows;
		mNumOfRows->SetRawValue(&numValue, 0, sizeof(float));
	}

	//쉐이더에 저장된 input layout을 불러온다.
	ID3D11InputLayout* InputLayout() { return mInputLayout; }

	//쉐이더의 Technique을 얻는다.
	ID3DX11EffectTechnique* getTech() { return mTech; }

	~ParticleShader()
	{
		ReleaseCOM(mInputLayout);
	}
};