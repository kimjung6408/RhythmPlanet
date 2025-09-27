#pragma once
#include"Utils.hpp"
#include<string>
using namespace std;

	virtual ~ShaderProgram()
	{
		ReleaseCOM(pEffectShader);
	}

};
//shader들은 반드시 이것을 상속받아서 구현해야 함.
class ShaderProgram
{
private:
	ID3DX11Effect* pEffectShader;

protected:

	//shader와 attribute들을 연결
	virtual void getAllAttributeLocations() = 0;

	//input layout을 작성함.
	virtual void setInputLayout() = 0;
public:
	ShaderProgram(LPCWSTR FilePath)
	{
		nsShaderUtils::Compile(FilePath, &pEffectShader);
	}

	ID3DX11Effect* Shader() { return pEffectShader; }

};