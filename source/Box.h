#pragma once
#include "d3dApp.h"
#include "Utils.hpp"
#include "globalDeviceContext.h"
#include "GeometryGenerator.h"
#include "Camera.h"

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
    explicit Sphere(LPCWSTR filePath);
    ~Sphere();

    void Render(Camera& cam, XMMATRIX& World);
};
