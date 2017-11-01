#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include"ModelShader.h"
#include"Entity.h"

#include"d3dApp.h"

using namespace std;

struct ModelVertex
{
	// Position
	XMFLOAT3 Position;
	// Normal
	XMFLOAT3 Normal;
	// TexCoords
	XMFLOAT2 TexCoords;
};

struct Texture
{
	ID3D11ShaderResourceView* pSRV;
	string type;
	aiString path;
};

class Mesh
{
public:

	/*  Functions  */
	// Constructor
	Mesh(vector<ModelVertex> vertices, vector<UINT> indices, vector<Texture> textures, int IndexCount)
	{
		VertexBuffer = nsCreator::createVertexBuffer(vertices);
		IndexBuffer = nsCreator::createIndexBuffer(indices);
		this->textures = textures;
		this->IndexCount = IndexCount;
	}

	// Render the mesh
	void Render(ModelShader* shader, XMFLOAT3 lightPosition, Entity* entity, Camera& cam, int MeshNumber)
	{
		Global::Context()->IASetInputLayout(shader->InputLayout());
		Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//VertexBuffer와 Index Buffer를 설정한다.
		UINT stride = sizeof(EntityShader::EntityVertex);
		UINT offset = 0;
		ID3D11Buffer* pVB = VertexBuffer;
		ID3D11Buffer* pIB = IndexBuffer;
		Global::Context()->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
		Global::Context()->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);

		// Bind appropriate textures
		UINT diffuseNr = 1;
		UINT specularNr = 1;

		shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());
		XMFLOAT3 Position = entity->GetPosition();
		XMFLOAT3 Rotation = entity->GetRotation();
		XMFLOAT3 Scale = XMFLOAT3(1, 1, 1);
		XMMATRIX TranslationMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);
		XMMATRIX ScaleMatrix = XMMatrixScaling(0.0005f, 0.0004f, 0.0005f);
		XMMATRIX RotMatrix = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
		XMMATRIX WorldMatrix = ScaleMatrix*RotMatrix*TranslationMatrix;
		//쉐이더에 데이터를 로딩한다.
		shader->LoadWorldViewProjMatrix(WorldMatrix*cam.View()*cam.Proj());
		shader->LoadWorldMatrix(WorldMatrix);
		shader->LoadWorldInvTranspose(WorldMatrix);
		shader->LoadLightPosition(lightPosition);
		shader->LoadUVOffset(XMFLOAT2(0.0f, 0.0f));
		shader->LoadAmbientColor(entity->GetAmbientColor());
		shader->LoadMeshNumber(MeshNumber);

		for (int i = 0; i < this->textures.size(); i++)
		{
			int number;
			string name = this->textures[i].type;

			if (name == "texture_diffuse")
			{
				number = diffuseNr++; // Transfer GLuint to stream
			}
			else if (name == "texture_specular")
			{
				number = specularNr++; // Transfer GLuint to stream
			}

			shader->LoadTexture(textures[i].pSRV, name, number);
		}

		//그린다.
		Global::Context()->DrawIndexed(IndexCount, 0, 0);

		//렌더 상태를 원상복귀 시킨다.
		Global::finishRender();
	}

private:
	/*  Mesh Data  */
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	UINT IndexCount;
	vector<Texture> textures;
};