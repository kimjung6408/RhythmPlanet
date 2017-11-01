#pragma once
#include"d3dApp.h"
#include"SkyboxShader.h"
#include"Camera.h"
#include"RenderState.h"

#define SIZE 500

struct skyVertex
{
	XMFLOAT3 Pos;
};

class Skybox
{
private:

	//스카이박스 cubemap 데이터를 불러와 저장할 shader resource view
	ID3D11ShaderResourceView* cubemapResourceView;

	//스카이박스의 정점들을 저장할 vertex buffer의 포인터
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	//index 수
	UINT indexCount;
public:
	//스카이박스를 생성하고, 해당 경로에 있는 cubemap texture를 가져온다.
	Skybox(LPCWSTR filePath)
	{

		//Vertex 데이터를 저장하고 Vertex Buffer, Index Buffer를 생성함.

		vector<XMFLOAT3> Vertices;
		vector<UINT> indices;
		GeometryGenerator geoGen = GeometryGenerator();
		GeometryGenerator::MeshData mesh;
		geoGen.CreateBox(5000.0f, 5000.0f, 5000.0f, mesh);

		for (int i = 0; i < mesh.Vertices.size(); i++)
			Vertices.push_back(mesh.Vertices[i].Position);
		for (int i = 0; i < mesh.Indices.size(); i++)
			indices.push_back(mesh.Indices[i]);
		
		indexCount = indices.size();
		mVertexBuffer = nsCreator::createVertexBuffer(Vertices);
		mIndexBuffer = nsCreator::createIndexBuffer(indices);


		//cubemap 이미지를 불러와서 저장한다.
		HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), filePath, 0, 0, &cubemapResourceView, 0));
	}

	//스카이박스를 쉐이더, 카메라에 따라 렌더링한다.
	void Render(SkyboxShader* shader , Camera& cam)
	{
		Global::Context()->IASetInputLayout(shader->InputLayout());
		Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//backface culling을 방지하여 스카이박스가 온전하게 출력되게 한다.
		Global::Context()->RSSetState(RenderState::NoCullRS);

		//VertexBuffer와 Index Buffer를 설정한다.
		UINT stride = sizeof(skyVertex);
		UINT offset = 0;
		Global::Context()->IASetVertexBuffers(0, 1, &mVertexBuffer,&stride, &offset);
		Global::Context()->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());

		//쉐이더에 데이터를 로딩한다.
		shader->loadWorldViewProjMatrix(cam.View(), cam.Proj());
		shader->loadCubeMap(cubemapResourceView);

		//그린다.
		Global::Context()->DrawIndexed(indexCount, 0, 0);

		//렌더 상태를 원상복귀 시킨다.
		Global::Context()->RSSetState(0);
	}

};