#pragma once
#include"ShaderProgram.h"
class TriangleShader : public ShaderProgram
{
private:
	//Attribute들을 선언하고 getAllAttributeLocations에서 초기화
	//skybox는 월드변환 및 view변환의 translation을 적용하지 않는다.
	ID3D11Buffer* mVB;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mModelViewProj;

	//inputLayout
	ID3D11InputLayout* mInputLayout;

private:
	//initialize시 호출됨. c++코드와 쉐이더를 연결함.
	void getAllAttributeLocations()
	{
		mTech = Shader()->GetTechniqueByName("TriangleTech");
		mModelViewProj = Shader()->GetVariableByName("worldViewProj")->AsMatrix();
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
	TriangleShader(LPCWSTR filePath)
		:ShaderProgram(filePath), mTech(0), mModelViewProj(0)
	{
		XMFLOAT3 v3(-0.5f, 0.5f, 0);
		XMFLOAT3 v2(-0.5f, -0.5f, 0);
		XMFLOAT3 v1(0.5f, 0.5f, 0);

		vector<XMFLOAT3> vertices;
		//vertices.push_back(v4);
		vertices.push_back(v3);
		vertices.push_back(v2);
		vertices.push_back(v1);

		mVB = nsCreator::createVertexBuffer(vertices);


		getAllAttributeLocations();
		setInputLayout();
	}

	//변환 행렬을 쉐이더에 로딩한다.
	void loadModelViewProjMatrix(XMMATRIX& modelViewProjMatrix)
	{
		mModelViewProj->SetMatrix(reinterpret_cast<float*>(&modelViewProjMatrix));
	}

	//쉐이더에 저장된 input layout을 불러온다.
	ID3D11InputLayout* InputLayout() { return mInputLayout; }

	//쉐이더의 Technique을 얻는다.
	ID3DX11EffectTechnique* getTech() { return mTech; }

	ID3D11Buffer* VB() { return mVB; }

	~TriangleShader()
	{
		ReleaseCOM(mVB);
		ReleaseCOM(mInputLayout);
	}
};