#pragma once
#include"d3dApp.h"
#include"Utils.hpp"
#include"globalDeviceContext.h"
#include"GeometryGenerator.h"
#include"Camera.h"
class Sphere
{
private:
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;
	ID3DX11EffectMatrixVariable* shaderWorldViewProj;

	ID3DX11Effect* shader;
	ID3D11InputLayout* inputLayout;
	UINT indexCount;

public:
	Sphere(LPCWSTR filePath)
		:mVB(0), mIB(0), shader(0), inputLayout(0)
	{

		nsShaderUtils::Compile(filePath, &shader);
		shaderWorldViewProj = shader->GetVariableByName("worldViewProj")->AsMatrix();

		D3D11_INPUT_ELEMENT_DESC temp1[1] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		// Create the input layout
		D3DX11_PASS_DESC passDesc;
		shader->GetTechniqueByName("boxTech")->GetPassByIndex(0)->GetDesc(&passDesc);
		HR(Global::Device()->CreateInputLayout(temp1, 1, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &inputLayout));

		GeometryGenerator geoGen = GeometryGenerator();
		GeometryGenerator::MeshData mesh;
		geoGen.CreateSphere(5.0f, 40, 40, mesh);

		vector<XMFLOAT3> vertices;
		for (int i = 0; i < mesh.Vertices.size(); i++)
			vertices.push_back(mesh.Vertices[i].Position);
		vector<UINT> indices;
		for (int i = 0; i < mesh.Indices.size(); i++)
			indices.push_back(mesh.Indices[i]);

		indexCount = mesh.Indices.size();

		mVB = nsCreator::createVertexBuffer(vertices);
		mIB = nsCreator::createIndexBuffer(indices);
	}

	void Render(Camera& cam ,XMMATRIX& World)
	{
		Global::Context()->IASetInputLayout(inputLayout);
		Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(XMFLOAT3);
		UINT offset = 0;
		Global::Context()->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		Global::Context()->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
		

		shader->GetTechniqueByName("boxTech")->GetPassByIndex(0)->Apply(0, Global::Context());
		
		XMMATRIX WVP = World*cam.View()*cam.Proj();
		shaderWorldViewProj->SetMatrix(reinterpret_cast<float*>(&WVP));
		Global::Context()->DrawIndexed(indexCount, 0, 0);
	}
};