#pragma once
#include"ShaderProgram.h"
#include"Utils.hpp"


class SkyboxShader : public ShaderProgram
{
private:
	//Attribute���� �����ϰ� getAllAttributeLocations���� �ʱ�ȭ
	//skybox�� ���庯ȯ �� view��ȯ�� translation�� �������� �ʴ´�.

	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mWorldViewProj;
	ID3DX11EffectShaderResourceVariable* mCubeMap;

	//inputLayout
	ID3D11InputLayout* mInputLayout;

private:
	//initialize�� ȣ���. c++�ڵ�� ���̴��� ������.
	void getAllAttributeLocations()
	{
		mTech = Shader()->GetTechniqueByName("skyTech");
		mWorldViewProj = Shader()->GetVariableByName("worldViewProj")->AsMatrix();
		mCubeMap = Shader()->GetVariableByName("texCubeMap")->AsShaderResource();
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
	SkyboxShader(LPCWSTR filePath)
		:ShaderProgram(filePath), mTech(0), mWorldViewProj(0), mCubeMap(0)
	{
		getAllAttributeLocations();
		setInputLayout();
	}

	//��ī�̹ڽ��� ���� ť����� ���̴��� �ε��Ѵ�.
	void loadCubeMap(ID3D11ShaderResourceView* pCubeMapResourceView)
	{
		mCubeMap->SetResource(pCubeMapResourceView);
	}

	//view ��ȯ ����� ���̴��� �ε��Ѵ�.
	void loadWorldViewProjMatrix(XMMATRIX& inViewMatrix, XMMATRIX& projMatrix) {
		//skybox�� �̵��ϴ� ���� �����ϱ� ���ؼ� x,y,z �̵����е��� 0���� ��ȯ�Ѵ�.
		//����, ȸ����ȯ�� �����Ѵ�.
		XMMATRIX viewMatrix = inViewMatrix;

		viewMatrix._41 = 0;
		viewMatrix._42 = 0;
		viewMatrix._43 = 0;

		XMMATRIX worldViewProjMatrix = viewMatrix*projMatrix;

		mWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProjMatrix));
	}

	//���̴��� ����� input layout�� �ҷ��´�.
	ID3D11InputLayout* InputLayout() { return mInputLayout; }
	
	//���̴��� Technique�� ��´�.
	ID3DX11EffectTechnique* getTech() { return mTech; }
};