#pragma once
#pragma comment(lib, "assimp.lib")
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include"d3dApp.h"
#include"globalDeviceContext.h"
#include"Utils.hpp"
#include"Camera.h"
#include"ModelShader.h"
#include<vector>
#include<string>
#include"Mesh.h"
using namespace std;


class Model
{
public:

	Model(const string& path);
	~Model();

	void Render(ModelShader* shader, XMFLOAT3 lightPosition, Entity* entity, Camera& cam)
	{
		for (int i = 0; i < this->meshes.size(); i++)
		{
			this->meshes[i].Render(shader, lightPosition, entity, cam, i+1);
		}
	}
private:

	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;
	bool loadModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	bool InitMaterials(const aiScene* pScene, const std::string& Filename);
};

Model::Model(const string& path)
{
	if (loadModel(path))
	{
		//Model successfully loaded
		OutputDebugString(L"Model Successfully loaded");
	}
	else
	{
		//Model loading failed
		OutputDebugString(L"Model loading failed");
	}
}

Model::~Model()
{
	for (int i = 0; i < textures_loaded.size(); i++)
	{
		ReleaseCOM(textures_loaded[i].pSRV);
	}
}

bool Model::loadModel(string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene)
		return false;

	InitMaterials(scene, path);

	this->directory = path.substr(0, path.find_last_of('/'));

	this->processNode(scene->mRootNode, scene);

	return true;
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<ModelVertex> vertices;
	vector<UINT> indices;
	vector<Texture> textures;

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		ModelVertex vertex;

		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;

		vertex.Normal.x = mesh->mNormals[i].x;
		vertex.Normal.y = mesh->mNormals[i].y;
		vertex.Normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0]) //만약 텍스쳐가 존재하면
		{
			vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.TexCoords = XMFLOAT2(0, 0);
		}

		vertices.push_back(vertex);
	}

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return Mesh(vertices, indices, textures_loaded, indices.size());
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;

	for (int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;

		for (int j = 0; j < textures_loaded.size(); j++)
		{
			if (textures_loaded[j].path == str)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)

				break;
			}
		}

		if (!skip)
		{   // If texture hasn't been loaded already, load it
			Texture texture;
			string fullPath = directory + "/" + str.C_Str();
			std::wstring stemp = std::wstring(fullPath.begin(), fullPath.end());
			LPCWSTR sw = stemp.c_str();
			D3DX11CreateShaderResourceViewFromFileW(Global::Device(), sw, 0, 0, &texture.pSRV, 0);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);

			this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}

	return textures;
}

bool Model::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
	// Extract the directory part from the file name
	std::string::size_type SlashIndex = Filename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = Filename.substr(0, SlashIndex);
	}

	bool Ret = true;

	// Initialize the materials
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];
			aiString Path;
			OutputDebugString(L"Texture Loading. . .\n");
			if (pMaterial->GetTexture(aiTextureType_REFLECTION, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string FullPath = Dir + "/" + Path.data;
				Texture texture;
				texture.path = FullPath;
				texture.type="texture_diffuse";
				std::wstring stemp = std::wstring(FullPath.begin(), FullPath.end());
				LPCWSTR sw = stemp.c_str();
				OutputDebugString(sw);
				D3DX11CreateShaderResourceViewFromFileW(Global::Device(), sw, 0, 0, &texture.pSRV, 0);
					textures_loaded.push_back(texture);
				}
			else
			{
				OutputDebugString(L"NoTexture");
			}
		}

	return true;
	}