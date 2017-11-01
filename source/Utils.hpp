#pragma once
#include"d3dApp.h"
#include"globalDeviceContext.h"
//#include<fbxsdk.h>
#include<vector>
#include<string>
using namespace std;

enum SceneStatus { MAIN, SELECT_MUSIC, OPTION, INGAME, RESULT, ENDGAME};

struct BoxCollider
{
	XMFLOAT3 MinPoint;
	XMFLOAT3 MaxPoint;
};

//ns : namespace
//shaderUtils : 하는 일
//쉐이더와 관련된 일을 수행한다.
namespace nsShaderUtils
{

//shader파일을 컴파일하고 shader 포인터가 가리키게 한다.
void Compile(LPCWSTR filename, ID3DX11Effect** shader)
	{
		DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3D10_SHADER_DEBUG;
		shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

		ID3D10Blob* compiledShader = 0;
		ID3D10Blob* compilationMsgs = 0;
		HRESULT hr = D3DX11CompileFromFile(filename, 0, 0, 0, "fx_5_0", shaderFlags,
			0, 0, &compiledShader, &compilationMsgs, 0);

		// compilationMsgs can store errors or warnings.
		if (compilationMsgs != 0)
		{
			MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationMsgs);
		}

		// Even if there are no compilationMsgs, check to make sure there were no other errors.
		if (FAILED(hr))
		{
			DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
		}

		HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
			0, Global::Device(), shader));

		// Done with compiled shader.
		ReleaseCOM(compiledShader);
	}

//vertex layout 생성에 사용할 스타일들.
enum VertexStyle {
	STRUCTURE_Position,
	STRUCTURE_PositionTexcoordNormal,
	STRUCTURE_PositionTexcoordNormalTangent,
	STRUCTURE_PositionTexcoord,
	STRUCTURE_PositionTexcoord0_1_2 //multi-texturing
};
//vertex input layout을 생성하고 P0에 전달한 뒤, 포인터를 리턴한다.
ID3D11InputLayout* loadInputLayout(VertexStyle style, ID3DX11EffectTechnique* tech)
	{
		//input layout 저장 포인터
		ID3D11InputLayout* outputInputLayout;

		//element 개수
		UINT numElements=0;

		if (style == STRUCTURE_Position)
		{
			D3D11_INPUT_ELEMENT_DESC temp1[1] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			// Create the input layout
			D3DX11_PASS_DESC passDesc;
			tech->GetPassByName("P0")->GetDesc(&passDesc);
			HR(Global::Device()->CreateInputLayout(temp1, 1, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &outputInputLayout));

			return outputInputLayout;
		}
		else if (style == STRUCTURE_PositionTexcoord)
		{
			D3D11_INPUT_ELEMENT_DESC temp2[2] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			// Create the input layout
			D3DX11_PASS_DESC passDesc;
			tech->GetPassByIndex(0)->GetDesc(&passDesc);
			HR(Global::Device()->CreateInputLayout(temp2, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &outputInputLayout));

			return outputInputLayout;
		}
		if (style == STRUCTURE_PositionTexcoordNormal)
		{
			D3D11_INPUT_ELEMENT_DESC temp3[3] = { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
													,{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
													,{ "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
			// Create the input layout
			D3DX11_PASS_DESC passDesc;
			tech->GetPassByIndex(0)->GetDesc(&passDesc);
			HR(Global::Device()->CreateInputLayout(temp3, 3, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &outputInputLayout));

			return outputInputLayout;
		}
		if (style == STRUCTURE_PositionTexcoordNormalTangent)
		{
			D3D11_INPUT_ELEMENT_DESC temp4[4] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				 { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				 { "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				 { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
			// Create the input layout
			D3DX11_PASS_DESC passDesc;
			tech->GetPassByIndex(0)->GetDesc(&passDesc);
			HR(Global::Device()->CreateInputLayout(temp4, 4, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &outputInputLayout));
			return outputInputLayout;
		}
		else
			return NULL;
	}


}

namespace nsCreator
{

	//vertex 데이터를 저장하고, immutable vertex buffer를 생성한다.
	template<typename VertexData>
	ID3D11Buffer* createVertexBuffer(vector<VertexData>& vertices)
	{
		ID3D11Buffer* retVertexBuffer;

		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(VertexData) * vertices.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &vertices[0];
		HR(Global::Device()->CreateBuffer(&vbd, &vinitData, &retVertexBuffer));

		return retVertexBuffer;
	}

	template<typename VertexData>
	ID3D11Buffer* createMutableVertexBuffer(vector<VertexData>& vertices)
	{
		ID3D11Buffer* retVertexBuffer;

		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_DYNAMIC;
		vbd.ByteWidth = sizeof(VertexData) * vertices.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &vertices[0];
		HR(Global::Device()->CreateBuffer(&vbd, &vinitData, &retVertexBuffer));

		return retVertexBuffer;
	}

	//index 데이터를 저장하고, immutable index buffer를 생성한다.
	ID3D11Buffer* createIndexBuffer(vector<UINT>& indices)
	{
		//리턴할 index buffer의 주소
		ID3D11Buffer* retIndexBuffer;

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(UINT) * indices.size();
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = &indices[0];
		HR(Global::Device()->CreateBuffer(&ibd, &iinitData, &retIndexBuffer));

		return retIndexBuffer;
	}


}

namespace nsMath
{
	//안됨
	XMFLOAT3 normalize(XMFLOAT3& value)
	{
		float length = sqrtf(value.x*value.x + value.y*value.y + value.z*value.z);
		XMFLOAT3 ret;
		ret=XMFLOAT3(value.x / length, value.y / length, value.z / length);
		return ret;
	}
	
	//안됨
	XMFLOAT2 normalize(XMFLOAT2& value)
	{
		float length = sqrtf(value.x*value.x + value.y*value.y);
		return XMFLOAT2(value.x / length, value.y / length);
	}

	float length(XMFLOAT3 value)
	{
		return sqrtf(value.x*value.x + value.y*value.y + value.z*value.z);
	}

	float length(XMFLOAT2 value)
	{
		return sqrtf(value.x*value.x + value.y*value.y);
	}

	float dot(XMFLOAT3& V1, XMFLOAT3& V2)
	{
		return V1.x*V2.x + V1.y*V2.y + V1.z*V2.z;
	}

	float dot(XMFLOAT2& V1, XMFLOAT2& V2)
	{
		return V1.x*V2.x + V1.y*V2.y;
	}

	//안됨
	XMFLOAT3 cross(XMFLOAT3& V1, XMFLOAT3& V2)
	{
		XMFLOAT3 ret= XMFLOAT3(V1.y*V2.z - V2.z*V1.y, -(V1.x*V2.z - V2.x*V1.z), V1.x*V2.y - V2.x*V1.y);
		return ret;
	}

	//안됨
	XMFLOAT2 Scaling(XMFLOAT2& V1, float scaleFactor)
	{
		return XMFLOAT2(V1.x*scaleFactor, V1.y*scaleFactor);
	}

	//안됨
	XMFLOAT3 Scaling(XMFLOAT3& V1, float scaleFactor)
	{
		return XMFLOAT3(V1.x*scaleFactor, V1.y*scaleFactor, V1.z*scaleFactor);
	}

	//안됨
	XMFLOAT3 Add(XMFLOAT3& V1, XMFLOAT3& V2)
	{
		return XMFLOAT3(V1.x + V2.x, V1.y + V2.y, V1.z + V2.z);
	}

	//안됨
	XMFLOAT2 Add(XMFLOAT2& V1, XMFLOAT2& V2)
	{
		return XMFLOAT2(V1.x + V2.x, V1.y + V2.y);
	}

	//return V1-V2 안됨
	XMFLOAT3 Subtract(XMFLOAT3& V1, XMFLOAT3& V2)
	{
		return XMFLOAT3(V1.x - V2.x, V1.y - V2.y, V1.z - V2.z);
	}

	//return V1-V2 안됨
	XMFLOAT2 Subtract(XMFLOAT2& V1, XMFLOAT2& V2)
	{
		return XMFLOAT2(V1.x - V2.x, V1.y - V2.y);
	}

	//성분단위 곱셈을 하여 리턴한다. 안됨
	XMFLOAT3 ComponentBasedMultiple(XMFLOAT3& V1, XMFLOAT3& V2)
	{
		return XMFLOAT3(V1.x*V2.x,V1.y*V2.y, V1.z*V2.z);
	}
}

namespace nsGlobalUtils
{
	void clearScreen(ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView)
	{
		Global::Context()->ClearRenderTargetView(renderTargetView, reinterpret_cast<const float*>(&Colors::Black));
		Global::Context()->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

}

namespace nsCollision
{
	bool CheckCubeCubeCollision(XMFLOAT3 v1Min, XMFLOAT3 v1Max, XMFLOAT3 v2Min, XMFLOAT3 v2Max)
	{
		if (v1Min.x <= v2Max.x && v1Max.x >= v2Min.x &&
			v1Min.y <= v2Max.y && v2Max.y >= v2Min.y &&
			v1Min.z <= v2Max.z && v1Max.z >= v2Min.z)
			return true;

		return false;
	}

	bool CheckSphereSphereCollision(XMFLOAT3 sphere1Center, float sphere1Radius, XMFLOAT3 sphere2Center, float sphere2Radius)
	{
		XMFLOAT3 LengthVector = XMFLOAT3(sphere1Center.x - sphere2Center.x, sphere1Center.y - sphere2Center.y, sphere1Center.z - sphere2Center.z);
		float distanceSquared = LengthVector.x*LengthVector.x + LengthVector.y*LengthVector.y + LengthVector.z*LengthVector.z;
		float RadiusSumSquared = (sphere1Radius + sphere2Radius)*(sphere1Radius + sphere2Radius);
		
		if (distanceSquared <= RadiusSumSquared)
			return true;

		return false;
	}

	bool CheckCubeSphereCollision(BoxCollider box, XMFLOAT3 SphereCenter, float SphereRadius)
	{
		XMFLOAT3 rectCenter;
		float Width = abs(box.MaxPoint.x - box.MinPoint.x);
		float Height = abs(box.MaxPoint.y - box.MinPoint.y);
		float Depth = abs(box.MaxPoint.z - box.MaxPoint.z);

		rectCenter.x = (box.MinPoint.x + box.MaxPoint.x)/2.0f;
		rectCenter.y = (box.MinPoint.y + box.MaxPoint.y) / 2.0f;
		rectCenter.z = (box.MinPoint.z + box.MaxPoint.z) / 2.0f;

		float sphereXDistance = abs(SphereCenter.x- rectCenter.x);
		float sphereYDistance = abs(SphereCenter.y - rectCenter.y);
		float sphereZDistance = abs(SphereCenter.z - rectCenter.z);

		if (sphereXDistance >= (Width + SphereRadius)) { return false; }
		if (sphereYDistance >= (Height + SphereRadius)) { return false; }
		if (sphereZDistance >= (Depth + SphereRadius)) { return false; }

		if (sphereXDistance < (Width)) { return true; }
		if (sphereYDistance < (Height)) { return true; }
		if (sphereZDistance < (Depth)) { return true; }

		float cornerDistance_sq = ((sphereXDistance - Width) * (sphereXDistance - Width)) +
			((sphereYDistance - Height) * (sphereYDistance - Height) +
				((sphereYDistance - Depth) * (sphereYDistance - Depth)));

		return (cornerDistance_sq < (SphereRadius * SphereRadius));
	}
}

namespace nsFBX
{
	//struct Triangle
	//{

	//};

	//void ProcessMesh(FbxNode* inNode)
	//{
	//	FbxMesh* currMesh = inNode->GetMesh();

	//	vector<Triangle> mTriangles;
	//	int mTriangleCount = currMesh->GetPolygonCount();
	//	int vertexCounter = 0;
	//	mTriangles.reserve(mTriangleCount);

	//	for (unsigned int i = 0; i < mTriangleCount; ++i)
	//	{
	//		XMFLOAT3 normal[3];
	//		XMFLOAT3 tangent[3];
	//		XMFLOAT3 binormal[3];
	//		XMFLOAT2 UV[3][2];
	//		Triangle currTriangle;
	//		mTriangles.push_back(currTriangle);

	//		for (unsigned int j = 0; j < 3; ++j)
	//		{
	//			int ctrlPointIndex = currMesh->GetPolygonVertex(i, j);
	//			CtrlPoint* currCtrlPoint = mControlPoints[ctrlPointIndex];


	//			ReadNormal(currMesh, ctrlPointIndex, vertexCounter, normal[j]);
	//			// We only have diffuse texture
	//			for (int k = 0; k < 1; ++k)
	//			{
	//				ReadUV(currMesh, ctrlPointIndex, currMesh->GetTextureUVIndex(i, j), k, UV[j][k]);
	//			}


	//			PNTIWVertex temp;
	//			temp.mPosition = currCtrlPoint->mPosition;
	//			temp.mNormal = normal[j];
	//			temp.mUV = UV[j][0];
	//			// Copy the blending info from each control point
	//			for (unsigned int i = 0; i < currCtrlPoint->mBlendingInfo.size(); ++i)
	//			{
	//				VertexBlendingInfo currBlendingInfo;
	//				currBlendingInfo.mBlendingIndex = currCtrlPoint->mBlendingInfo[i].mBlendingIndex;
	//				currBlendingInfo.mBlendingWeight = currCtrlPoint->mBlendingInfo[i].mBlendingWeight;
	//				temp.mVertexBlendingInfos.push_back(currBlendingInfo);
	//			}
	//			// Sort the blending info so that later we can remove
	//			// duplicated vertices
	//			temp.SortBlendingInfoByWeight();

	//			mVertices.push_back(temp);
	//			mTriangles.back().mIndices.push_back(vertexCounter);
	//			++vertexCounter;
	//		}
	//	}

	//	// Now mControlPoints has served its purpose
	//	// We can free its memory
	//	for (auto itr = mControlPoints.begin(); itr != mControlPoints.end(); ++itr)
	//	{
	//		delete itr->second;
	//	}
	//	mControlPoints.clear();
	//}

	//void ProcessControlPoints(FbxNode* inNode)
	//{
	//	FbxMesh* currMesh = inNode->GetMesh();
	//	unsigned int ctrlPointCount = currMesh->GetControlPointsCount();
	//	for (unsigned int i = 0; i < ctrlPointCount; ++i)
	//	{
	//		CtrlPoint* currCtrlPoint = new CtrlPoint();
	//		XMFLOAT3 currPosition;
	//		currPosition.x = static_cast<float>(currMesh->GetControlPointAt(i).mData[0]);
	//		currPosition.y = static_cast<float>(currMesh->GetControlPointAt(i).mData[1]);
	//		currPosition.z = static_cast<float>(currMesh->GetControlPointAt(i).mData[2]);
	//		currCtrlPoint->mPosition = currPosition;
	//		mControlPoints[i] = currCtrlPoint;
	//	}
	//}

	//void ReadNormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outNormal)
	//{
	//	if (inMesh->GetElementNormalCount() < 1)
	//	{
	//		throw std::exception("Invalid Normal Number");
	//	}

	//	FbxGeometryElementNormal* vertexNormal = inMesh->GetElementNormal(0);
	//	switch (vertexNormal->GetMappingMode())
	//	{
	//	case FbxGeometryElement::eByControlPoint:
	//		switch (vertexNormal->GetReferenceMode())
	//		{
	//		case FbxGeometryElement::eDirect:
	//		{
	//			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
	//			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
	//			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
	//		}
	//		break;

	//		case FbxGeometryElement::eIndexToDirect:
	//		{
	//			int index = vertexNormal->GetIndexArray().GetAt(inCtrlPointIndex);
	//			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
	//			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
	//			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
	//		}
	//		break;

	//		default:
	//			throw std::exception("Invalid Reference");
	//		}
	//		break;

	//	case FbxGeometryElement::eByPolygonVertex:
	//		switch (vertexNormal->GetReferenceMode())
	//		{
	//		case FbxGeometryElement::eDirect:
	//		{
	//			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[0]);
	//			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[1]);
	//			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[2]);
	//		}
	//		break;

	//		case FbxGeometryElement::eIndexToDirect:
	//		{
	//			int index = vertexNormal->GetIndexArray().GetAt(inVertexCounter);
	//			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
	//			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
	//			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
	//		}
	//		break;

	//		default:
	//			throw std::exception("Invalid Reference");
	//		}
	//		break;
	//	}
	//}


}