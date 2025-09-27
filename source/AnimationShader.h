#pragma once
#include"d3dApp.h"
#include"globalDeviceContext.h"
#include"ShaderProgram.h"

class AnimationShader : public ShaderProgram
{
private:
	ID3D11InputLayout* mInputLayout;
	ID3DX11EffectTechnique* mTech;
	ID3D11Buffer* mVB; //사각 평면 vertex 정보를 저장할 버퍼.
	ID3DX11EffectShaderResourceVariable* mTexture;
	ID3DX11EffectVariable* mFadeFactor;
	ID3DX11EffectVariable* mAlpha;
	ID3DX11EffectVariable* mRow;
	ID3DX11EffectVariable* mColumn;
	ID3DX11EffectVariable* mUVSize;
	ID3DX11EffectMatrixVariable* mWorld;

	float numColumn;
	float numRow;

private:
	//initialize시 호출됨. c++코드와 쉐이더를 연결함.
	void getAllAttributeLocations()
	{
		mTech = Shader()->GetTechniqueByName("AnimTech");
		mTexture = Shader()->GetVariableByName("Image")->AsShaderResource();
		mWorld = Shader()->GetVariableByName("matWorld")->AsMatrix();
		mFadeFactor = Shader()->GetVariableByName("fadeFactor");
		mAlpha = Shader()->GetVariableByName("Alpha");
		mUVSize = Shader()->GetVariableByName("UVSize");
		mRow= Shader()->GetVariableByName("Row");
		mColumn = Shader()->GetVariableByName("Column");
	}

	//input layout 설정.
	void setInputLayout()
	{
		D3D11_INPUT_ELEMENT_DESC temp1[1] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		// Create the input layout
		D3DX11_PASS_DESC passDesc;
		mTech->GetPassByIndex(0)->GetDesc(&passDesc);
		HR(Global::Device()->CreateInputLayout(temp1, 1, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayout));
	}
public:
	AnimationShader(LPCWSTR ShaderFilePath)
		:ShaderProgram(ShaderFilePath), mVB(0), mInputLayout(0), mTech(0), mTexture(0), mWorld(0)
	{

		//Background가 그려질
		XMFLOAT2 v3(-1.0f, 1.0f);
		XMFLOAT2 v2(-1.0f, -1.0f);
		XMFLOAT2 v1(1.0f, 1.0f);
		XMFLOAT2 v6(1.0f, 1.0f);
		XMFLOAT2 v5(-1.0f, -1.0f);
		XMFLOAT2 v4(1.0f, -1.0f);

		vector<XMFLOAT2> vertices;
		//vertices.push_back(v4);
		vertices.push_back(v3);
		vertices.push_back(v2);
		vertices.push_back(v1);
		vertices.push_back(v6);
		vertices.push_back(v5);
		vertices.push_back(v4);

		mVB = nsCreator::createVertexBuffer(vertices);

		getAllAttributeLocations();
		setInputLayout();
	}

	ID3DX11EffectTechnique* getTech()
	{
		return mTech;
	}

	ID3D11InputLayout* InputLayout()
	{
		return mInputLayout;
	}

	void LoadTexture(ID3D11ShaderResourceView* SRV)
	{
		if (SRV != NULL)
			mTexture->SetResource(SRV);
	}

	ID3D11Buffer* VB()
	{
		return mVB;
	}

	void LoadWorldMatrix(XMMATRIX& world)
	{
		mWorld->SetMatrix(reinterpret_cast<float*>(&world));
	}

	void LoadFadeFactor(float FadeFactor)
	{
		mFadeFactor->SetRawValue(&FadeFactor, 0, sizeof(float));
	}

	void LoadNumRowColumn(int numRow, int numColumn)
	{
		float uScale = 1.0f / (float)numColumn;
		float vScale = 1.0f / (float)numRow;
		XMFLOAT2 uvScale = XMFLOAT2(uScale, vScale);
		mUVSize->SetRawValue(&uvScale, 0, sizeof(XMFLOAT2));
	}

	void LoadCurrentIndex(float row, float column)
	{
		mRow->SetRawValue(&row, 0, sizeof(float));
		mColumn->SetRawValue(&column, 0, sizeof(float));
	}

	void LoadAlpha(float Alpha)
	{
		mAlpha->SetRawValue(&Alpha, 0, sizeof(float));
	}

	~AnimationShader()
	{
		ReleaseCOM(mInputLayout);
		ReleaseCOM(mVB);
	}
};
