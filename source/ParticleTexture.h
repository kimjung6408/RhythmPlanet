#pragma once
#include"d3dApp.h"

//파티클에 쓰이는 텍스쳐의 구별 식별자.
class ParticleTexture
{
private:
	ID3D11ShaderResourceView* textureSRV;
	
	//row의 개수
	//단위 텍스쳐 개수는 가로, 세로로 numOfRows*numOfRows
	//(texture Atlas를 이용하기 때문임)
	int numOfRows;
public:
	ParticleTexture(LPCWSTR filePath, int numOfRows)
	{
		//cubemap 이미지를 불러와서 저장한다.
		HR(D3DX11CreateShaderResourceViewFromFile(Global::Device(), filePath, 0, 0, &textureSRV, 0));
		this->numOfRows = numOfRows;
	}

	//해당 텍스쳐의 shader resource view를 반환한다.
	ID3D11ShaderResourceView* getSRV()
	{
		return textureSRV;
	}

	//해당 텍스쳐의 row 개수를 반환한다.
	int getNumOfRows()
	{
		return numOfRows;
	}

	~ParticleTexture()
	{
		ReleaseCOM(textureSRV);
	}
};