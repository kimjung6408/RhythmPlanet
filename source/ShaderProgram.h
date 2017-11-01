#pragma once
#include"Utils.hpp"
#include<string>
using namespace std;

//shader���� ������ ���¸� ���� �ϱ� ���� virtual Ŭ����
//shader���� �ݵ�� �̰��� ��ӹ޾Ƽ� �����ؾ� ��.
class ShaderProgram
{
private:
	ID3DX11Effect* pEffectShader;

protected:

	//shader�� attribute���� ����
	virtual void getAllAttributeLocations() = 0;

	//input layout�� �ۼ���.
	virtual void setInputLayout() = 0;
public:
	ShaderProgram(LPCWSTR FilePath)
	{
		nsShaderUtils::Compile(FilePath, &pEffectShader);
	}

	ID3DX11Effect* Shader() { return pEffectShader; }

};