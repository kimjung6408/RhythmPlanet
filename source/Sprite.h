#pragma once
#include"d3dApp.h"
#include"Camera.h"
#include"ShaderProgram.h"
class Sprite
{
private:
	ID3D11Buffer* mVB;
	ID3D11ShaderResourceView* mTextureSRV;
public:
	Sprite(LPCTSTR TextureFilePath);
	~Sprite(void);
	void Update(float dt);
	void Render(Camera& cam, ShaderProgram* shader);

};

Sprite::Sprite(LPCTSTR TextureFilePath)
{}