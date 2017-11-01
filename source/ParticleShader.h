#pragma once
#include"ShaderProgram.h"
class ParticleShader : public ShaderProgram
{
private:
	//Attribute���� �����ϰ� getAllAttributeLocations���� �ʱ�ȭ
	//skybox�� ���庯ȯ �� view��ȯ�� translation�� �������� �ʴ´�.

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
	//initialize�� ȣ���. c++�ڵ�� ���̴��� ������.
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

	//input layout ����.
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

	//��ȯ ����� ���̴��� �ε��Ѵ�.
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
		//GPU�� floating point�� �⺻ �����̹Ƿ�, ������ ���Ǹ� ���� float�� ��ȯ�Ͽ� ������.
		float numValue = (float)numOfRows;
		mNumOfRows->SetRawValue(&numValue, 0, sizeof(float));
	}

	//���̴��� ����� input layout�� �ҷ��´�.
	ID3D11InputLayout* InputLayout() { return mInputLayout; }

	//���̴��� Technique�� ��´�.
	ID3DX11EffectTechnique* getTech() { return mTech; }

	~ParticleShader()
	{
		ReleaseCOM(mInputLayout);
	}
};