#pragma once
#include"d3dApp.h"
#include"ShaderProgram.h"
#include"GeometryGenerator.h"
#define ENTITY_UNDERSPHERE 0
#define ENTITY_BOX 1
#define ENTITY_SIDEBAR 3
#define ENTITY_PLAYER 4
#define ENTITY_FLOOR 5
#define ENTITY_UNDERBAR 6
#define ENTITY_CAUTIONMARK_FLOORBAR 7
#define ENTITY_CAUTIONMARK_BOMB 8
#define ENTITY_CAUTIONMARK_FALL 9
#define ENTITY_CAUTIONMARK_SIDE_BAR 10
#define ENTITY_CAUTIONMARK_SIDE_MAGIC_CIRCLE 11
#define ENTITY_UPPERSPHERE 12
#define ENTITY_FALLSPHERE 13
class EntityShader : public ShaderProgram
{
	static const LPCWSTR SPHERE_TEX_PATH;//L"Textures/Ingame/ballTexture.jpg";
	static const LPCWSTR UPPERSPHERE_TEX_PATH;
	static const LPCWSTR FALLSPHERE_TEX_PATH;
	static const LPCWSTR BOX_TEX_PATH;
	static const LPCWSTR PLANE_TEX_PATH;
	static const LPCWSTR BAR_TEX_PATH; //튀어나왔다가 들어가는 장애물 텍스쳐.
	static const LPCWSTR MAGICCIRCLE_TEX_PATH;
	static const LPCWSTR BOMB_CAUTION_TEX_PATH;
	static const LPCWSTR SIDEBAR_CAUTION_TEX_PATH;
	static const LPCWSTR FALL_CAUTION_TEX_PATH;
	static const LPCWSTR FLOORBAR_CAUTION_TEX_PATH;
	static const LPCWSTR FLOORBAR_TEX_PATH;

	ID3D11Buffer* pVB;
	ID3D11Buffer* pIB;
	UINT IndexCount;
	ID3D11ShaderResourceView* pSRV;

	ID3DX11EffectTechnique* mTech;
	//라이팅을 위한 월드 행렬 (월드 공간상의 vertex 위치를 구함. 이 위치와 light의 위치를 이용)
	ID3DX11EffectMatrixVariable* mWorld;
	ID3DX11EffectMatrixVariable* mWorldViewProj;
	//라이팅을 위한, 노말의 변환을 위한 world행렬의 inverse transpose
	ID3DX11EffectMatrixVariable* mWorldInvTranspose;
	ID3DX11EffectShaderResourceVariable* mTexture;
	//ID3DX11EffectShaderResourceVariable* mNormalMap;
	ID3DX11EffectVariable* mLightPosition;
	ID3DX11EffectVariable* mAmbientColor;

	//텍스쳐 애니메이션을 위한 offset.
	ID3DX11EffectVariable* mUVOffset;
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

	//ENTITY_SPHERE : 구모양
	//ENTITY_BOX : 박스모양
	//ENTITY_PLAYER : 플레이어
	//ENTITY_BAR : 튀어나왔다가 들어가는 장애물 오브젝트.
	//ENTITY_FLOOR : 바닥 타일 큐브
	//ENTITY_CAUTIONMARK : 경고마크
	EntityShader(LPCWSTR ShaderFilePath, int EntityType)
		:ShaderProgram(ShaderFilePath)
	{
		GeometryGenerator::MeshData meshData;
		GeometryGenerator geoGen=GeometryGenerator();

		switch (EntityType)
		{
		case ENTITY_UNDERSPHERE:
			geoGen.CreateSphere(1.0f, 30, 30, meshData);
			HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), SPHERE_TEX_PATH, 0, 0, &pSRV, 0));
			break;
		case ENTITY_UPPERSPHERE:
			geoGen.CreateSphere(1.0f, 30, 30, meshData);
			HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), UPPERSPHERE_TEX_PATH, 0, 0, &pSRV, 0));
			break;
		case ENTITY_FALLSPHERE:
			geoGen.CreateSphere(1.0f, 30, 30, meshData);
			HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), FALLSPHERE_TEX_PATH, 0, 0, &pSRV, 0));
			break;
		case ENTITY_BOX:
			geoGen.CreateBox(1.0f, 1.0f, 1.0f, meshData);
			HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), BOX_TEX_PATH, 0, 0, &pSRV, 0));
			break;
		case ENTITY_FLOOR:
			geoGen.CreateBox(1.0f, 1.0f, 1.0f, meshData);
			HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), PLANE_TEX_PATH, 0, 0, &pSRV, 0));
			break;
		case ENTITY_SIDEBAR:
			geoGen.CreateBox(1.0f, 1.0f, 1.0f, meshData);
			HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), BAR_TEX_PATH, 0, 0, &pSRV, 0));
			break;
		case ENTITY_CAUTIONMARK_SIDE_MAGIC_CIRCLE:
			geoGen.CreateGrid(3, 3, 4, 4, meshData);
			HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), MAGICCIRCLE_TEX_PATH, 0, 0, &pSRV, 0));
			break;
		case ENTITY_UNDERBAR:
			geoGen.CreateBox(1.0f, 1.0f, 1.0f, meshData);
			HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), BAR_TEX_PATH, 0, 0, &pSRV, 0));
		case ENTITY_CAUTIONMARK_FALL:
			geoGen.CreateGrid(3, 3, 4, 4, meshData);
			HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), FALL_CAUTION_TEX_PATH, 0, 0, &pSRV, 0));
			break;
		case ENTITY_CAUTIONMARK_BOMB:
			geoGen.CreateGrid(4, 4, 4, 4, meshData);
			HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), BOMB_CAUTION_TEX_PATH, 0, 0, &pSRV, 0));
			break;
		case ENTITY_CAUTIONMARK_FLOORBAR:
			geoGen.CreateGrid(1.2f, 1.2f, 4, 4, meshData);
			HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), FLOORBAR_CAUTION_TEX_PATH, 0, 0, &pSRV, 0));
			break;
		case ENTITY_CAUTIONMARK_SIDE_BAR:
			geoGen.CreateGrid(2.5, 2.5, 4, 4, meshData);
			HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), SIDEBAR_CAUTION_TEX_PATH, 0, 0, &pSRV, 0));
			break;
			break;
		}
		//LoadVertexBuffer
		vector<EntityVertex> vertices;
		vector<UINT> indices;
		for (int i = 0; i < meshData.Vertices.size(); i++)
		{
			vertices.push_back(EntityVertex(meshData.Vertices[i].Position, meshData.Vertices[i].Normal, meshData.Vertices[i].TexC));
		}

		for(int i = 0; i < meshData.Indices.size(); i++)
		{
			indices.push_back(meshData.Indices[i]);
		}
		IndexCount = indices.size();

		pVB = nsCreator::createVertexBuffer(vertices);

		////CAUTION MARK가 아닐 때 Index buffer를 생성함.
		//if (EntityType <7)
			pIB = nsCreator::createIndexBuffer(indices);
		//else
		//	pIB = NULL;
		//LoadIndexBuffer
		getAllAttributeLocations();
		setInputLayout();		
	}

	void getAllAttributeLocations()
	{
		mTech = Shader()->GetTechniqueByName("EntityTech");
		mWorldViewProj = Shader()->GetVariableByName("worldViewProjMatrix")->AsMatrix();
		mWorld = Shader()->GetVariableByName("worldMatrix")->AsMatrix();
		mWorldInvTranspose = Shader()->GetVariableByName("worldInvTranspose")->AsMatrix();
		mTexture = Shader()->GetVariableByName("Texture")->AsShaderResource();
		//mNormalMap = Shader()->GetVariableByName("NormalMap")->AsShaderResource();
		mLightPosition = Shader()->GetVariableByName("lightPosition");
		mAmbientColor = Shader()->GetVariableByName("AmbientColor");
		mUVOffset = Shader()->GetVariableByName("UVOffset");

		mTexture->SetResource(pSRV);
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

	ID3D11Buffer* VB()
	{
		return pVB;
	}

	ID3D11Buffer* IB()
	{
		return pIB;
	}

	ID3D11InputLayout* InputLayout() 
	{ return mInputLayout; }

	//texture를 리턴한다.
	ID3D11ShaderResourceView* getSRV()
	{
		return pSRV;
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

	void LoadAmbientColor(XMFLOAT4 Ambient)
	{
		mAmbientColor->SetRawValue(&Ambient, 0, sizeof(XMFLOAT4));
	}

	~EntityShader()
	{
		ReleaseCOM(mInputLayout);
		ReleaseCOM(pVB);
		ReleaseCOM(pIB);
		ReleaseCOM(pSRV);
	}

	UINT getIndexCount()
	{
		return IndexCount;
	}
};
const LPCWSTR EntityShader::SPHERE_TEX_PATH=L"Textures/Ingame/ballTexture.jpg";
const LPCWSTR EntityShader::UPPERSPHERE_TEX_PATH = L"Textures/Ingame/BallRed.jpg";
const LPCWSTR EntityShader::FALLSPHERE_TEX_PATH = L"Textures/Ingame/BallGreen.jpg";

const LPCWSTR EntityShader::BOX_TEX_PATH= L"Textures/Ingame/BoxTexture.jpg";
const LPCWSTR EntityShader::PLANE_TEX_PATH=L"Textures/Ingame/PlaneTexture.jpg";
const LPCWSTR EntityShader::FLOORBAR_TEX_PATH = L"Textures/Ingame/FloorBarTexture.jpg";
const LPCWSTR EntityShader::MAGICCIRCLE_TEX_PATH = L"Textures/Ingame/MagicCircle.png";
const LPCWSTR EntityShader::FALL_CAUTION_TEX_PATH = L"Textures/Ingame/NeonHeart.png";
const LPCWSTR EntityShader::SIDEBAR_CAUTION_TEX_PATH = L"Textures/Ingame/ShinyLight.png";
const LPCWSTR EntityShader::BOMB_CAUTION_TEX_PATH = L"Textures/Ingame/Star.png";
const LPCWSTR EntityShader::FLOORBAR_CAUTION_TEX_PATH = L"Textures/Ingame/FloorCaution.png";
const LPCWSTR EntityShader::FLOORBAR_TEX_PATH = L"Textures/Ingame/FloorBarTexture.jpg";