#pragma once
#include"ShaderProgram.h"
#include"Utils.hpp"


class SkyboxShader : public ShaderProgram
{
private:
	//Attribute들을 선언하고 getAllAttributeLocations에서 초기화
	//skybox는 월드변환 및 view변환의 translation을 적용하지 않는다.

	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mWorldViewProj;
	ID3DX11EffectShaderResourceVariable* mCubeMap;

	//inputLayout
	ID3D11InputLayout* mInputLayout;

private:
	//initialize시 호출됨. c++코드와 쉐이더를 연결함.
	void getAllAttributeLocations()
	{
		mTech = Shader()->GetTechniqueByName("skyTech");
		mWorldViewProj = Shader()->GetVariableByName("worldViewProj")->AsMatrix();
		mCubeMap = Shader()->GetVariableByName("texCubeMap")->AsShaderResource();
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
	SkyboxShader(LPCWSTR filePath)
		:ShaderProgram(filePath), mTech(0), mWorldViewProj(0), mCubeMap(0)
	{
		getAllAttributeLocations();
		setInputLayout();
	}

	~SkyboxShader()
	{
		ReleaseCOM(mInputLayout);
	}
};
	void loadCubeMap(ID3D11ShaderResourceView* pCubeMapResourceView)
	{
		mCubeMap->SetResource(pCubeMapResourceView);
	}

	//view 변환 행렬을 쉐이더에 로딩한다.
	void loadWorldViewProjMatrix(XMMATRIX& inViewMatrix, XMMATRIX& projMatrix) {
		//skybox가 이동하는 것을 방지하기 위해서 x,y,z 이동성분들을 0으로 변환한다.
		//따라서, 회전변환만 적용한다.
		XMMATRIX viewMatrix = inViewMatrix;

		viewMatrix._41 = 0;
		viewMatrix._42 = 0;
		viewMatrix._43 = 0;

		XMMATRIX worldViewProjMatrix = viewMatrix*projMatrix;

		mWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProjMatrix));
	}

	//쉐이더에 저장된 input layout을 불러온다.
	ID3D11InputLayout* InputLayout() { return mInputLayout; }
	
	//쉐이더의 Technique을 얻는다.
	ID3DX11EffectTechnique* getTech() { return mTech; }
};