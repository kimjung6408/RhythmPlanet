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

	//��ī�̹ڽ� cubemap �����͸� �ҷ��� ������ shader resource view
	ID3D11ShaderResourceView* cubemapResourceView;

	//��ī�̹ڽ��� �������� ������ vertex buffer�� ������
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	//index ��
	UINT indexCount;
public:
	//��ī�̹ڽ��� �����ϰ�, �ش� ��ο� �ִ� cubemap texture�� �����´�.
	Skybox(LPCWSTR filePath)
	{

		//Vertex �����͸� �����ϰ� Vertex Buffer, Index Buffer�� ������.

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


		//cubemap �̹����� �ҷ��ͼ� �����Ѵ�.
		HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), filePath, 0, 0, &cubemapResourceView, 0));
	}

	//��ī�̹ڽ��� ���̴�, ī�޶� ���� �������Ѵ�.
	void Render(SkyboxShader* shader , Camera& cam)
	{
		Global::Context()->IASetInputLayout(shader->InputLayout());
		Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//backface culling�� �����Ͽ� ��ī�̹ڽ��� �����ϰ� ��µǰ� �Ѵ�.
		Global::Context()->RSSetState(RenderState::NoCullRS);

		//VertexBuffer�� Index Buffer�� �����Ѵ�.
		UINT stride = sizeof(skyVertex);
		UINT offset = 0;
		Global::Context()->IASetVertexBuffers(0, 1, &mVertexBuffer,&stride, &offset);
		Global::Context()->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());

		//���̴��� �����͸� �ε��Ѵ�.
		shader->loadWorldViewProjMatrix(cam.View(), cam.Proj());
		shader->loadCubeMap(cubemapResourceView);

		//�׸���.
		Global::Context()->DrawIndexed(indexCount, 0, 0);

		//���� ���¸� ���󺹱� ��Ų��.
		Global::Context()->RSSetState(0);
	}

};