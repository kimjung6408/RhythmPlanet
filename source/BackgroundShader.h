#pragma once
#include"d3dApp.h"
#include"globalDeviceContext.h"
#include"ShaderProgram.h"

class BackgroundShader : public ShaderProgram
{
private:
	ID3D11InputLayout* mInputLayout;
	ID3DX11EffectTechnique* mTech;
	ID3D11Buffer* mVB; //사각 평면 vertex 정보를 저장할 버퍼.
	ID3DX11EffectShaderResourceVariable* mBGTexture;
	ID3DX11EffectShaderResourceVariable* mGridTexture;

	ID3DX11EffectVariable* m_uvOffset_BG;
	ID3DX11EffectVariable* m_uvOffset_Grid;


private:
	//initialize시 호출됨. c++코드와 쉐이더를 연결함.
	void getAllAttributeLocations()
	{
		mTech = Shader()->GetTechniqueByName("BGTech");
		m_uvOffset_BG = Shader()->GetVariableByName("BG_uvOffset");
		m_uvOffset_Grid = Shader()->GetVariableByName("grid_uvOffset");
		mBGTexture = Shader()->GetVariableByName("BGImage")->AsShaderResource();
		mGridTexture = Shader()->GetVariableByName("gridImage")->AsShaderResource();
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
	BackgroundShader(LPCWSTR ShaderFilePath)
		:ShaderProgram(ShaderFilePath), mVB(0), mInputLayout(0), mTech(0), mBGTexture(0), mGridTexture(0),m_uvOffset_BG(0),
		m_uvOffset_Grid(0)
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
		vertices.push_back(v1);
		vertices.push_back(v2);
		vertices.push_back(v3);
		vertices.push_back(v4);
		vertices.push_back(v5);
		vertices.push_back(v6);

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

	void LoadBG(ID3D11ShaderResourceView* SRV_BG)
	{
		mBGTexture->SetResource(SRV_BG);
	}

	void LoadGridImage(ID3D11ShaderResourceView* SRV_Grid)
	{
		mGridTexture->SetResource(SRV_Grid);
	}

	void Load_uvOffsetBG(XMFLOAT2 BG_uvOffset)
	{
		m_uvOffset_BG->SetRawValue(&BG_uvOffset, 0, sizeof(XMFLOAT2));
	}

	void Load_uvOffsetGrid(XMFLOAT2 Grid_uvOffset)
	{
		m_uvOffset_Grid->SetRawValue(&Grid_uvOffset, 0, sizeof(XMFLOAT2));
	}

	ID3D11Buffer* VB()
	{
		return mVB;
	}
};