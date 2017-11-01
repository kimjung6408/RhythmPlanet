#pragma once
#include"ShaderProgram.h"
class TriangleShader : public ShaderProgram
{
private:
	//Attribute���� �����ϰ� getAllAttributeLocations���� �ʱ�ȭ
	//skybox�� ���庯ȯ �� view��ȯ�� translation�� �������� �ʴ´�.
	ID3D11Buffer* mVB;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mModelViewProj;

	//inputLayout
	ID3D11InputLayout* mInputLayout;

private:
	//initialize�� ȣ���. c++�ڵ�� ���̴��� ������.
	void getAllAttributeLocations()
	{
		mTech = Shader()->GetTechniqueByName("TriangleTech");
		mModelViewProj = Shader()->GetVariableByName("worldViewProj")->AsMatrix();
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

	//��ȯ ����� ���̴��� �ε��Ѵ�.
	void loadModelViewProjMatrix(XMMATRIX& modelViewProjMatrix)
	{
		mModelViewProj->SetMatrix(reinterpret_cast<float*>(&modelViewProjMatrix));
	}

	//���̴��� ����� input layout�� �ҷ��´�.
	ID3D11InputLayout* InputLayout() { return mInputLayout; }

	//���̴��� Technique�� ��´�.
	ID3DX11EffectTechnique* getTech() { return mTech; }

	ID3D11Buffer* VB() { return mVB; }

	~TriangleShader()
	{
		ReleaseCOM(mVB);
		ReleaseCOM(mInputLayout);
	}
};