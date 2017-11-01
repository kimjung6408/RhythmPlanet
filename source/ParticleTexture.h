#pragma once
#include"d3dApp.h"

//��ƼŬ�� ���̴� �ؽ����� ���� �ĺ���.
class ParticleTexture
{
private:
	ID3D11ShaderResourceView* textureSRV;
	
	//row�� ����
	//���� �ؽ��� ������ ����, ���η� numOfRows*numOfRows
	//(texture Atlas�� �̿��ϱ� ������)
	int numOfRows;
public:
	ParticleTexture(LPCWSTR filePath, int numOfRows)
	{
		//cubemap �̹����� �ҷ��ͼ� �����Ѵ�.
		HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), filePath, 0, 0, &textureSRV, 0));
		this->numOfRows = numOfRows;
	}

	//�ش� �ؽ����� shader resource view�� ��ȯ�Ѵ�.
	ID3D11ShaderResourceView* getSRV()
	{
		return textureSRV;
	}

	//�ش� �ؽ����� row ������ ��ȯ�Ѵ�.
	int getNumOfRows()
	{
		return numOfRows;
	}

	~ParticleTexture()
	{
		ReleaseCOM(textureSRV);
	}
};