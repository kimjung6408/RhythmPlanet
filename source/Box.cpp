#include "Box.h"
#include "d3dUtil.h"
#include <vector>

Sphere::Sphere(LPCWSTR filePath)
    : mVB(nullptr), mIB(nullptr), shaderWorldViewProj(nullptr), shader(nullptr), inputLayout(nullptr), indexCount(0)
{
    nsShaderUtils::Compile(filePath, &shader);
    shaderWorldViewProj = shader->GetVariableByName("worldViewProj")->AsMatrix();

    D3D11_INPUT_ELEMENT_DESC temp1[1] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    D3DX11_PASS_DESC passDesc;
    shader->GetTechniqueByName("boxTech")->GetPassByIndex(0)->GetDesc(&passDesc);
    HR(Global::Device()->CreateInputLayout(temp1, 1, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &inputLayout));

    GeometryGenerator geoGen;
    GeometryGenerator::MeshData mesh;
    geoGen.CreateSphere(5.0f, 40, 40, mesh);

    std::vector<XMFLOAT3> vertices;
    vertices.reserve(mesh.Vertices.size());
    for (size_t i = 0; i < mesh.Vertices.size(); ++i)
    {
        vertices.push_back(mesh.Vertices[i].Position);
    }
    std::vector<UINT> indices;
    indices.reserve(mesh.Indices.size());
    for (size_t i = 0; i < mesh.Indices.size(); ++i)
    {
        indices.push_back(mesh.Indices[i]);
    }

    indexCount = static_cast<UINT>(mesh.Indices.size());

    mVB = nsCreator::createVertexBuffer(vertices);
    mIB = nsCreator::createIndexBuffer(indices);
}

Sphere::~Sphere()
{
    ReleaseCOM(mVB);
    ReleaseCOM(mIB);
    ReleaseCOM(inputLayout);
    ReleaseCOM(shader);
}

void Sphere::Render(Camera& cam, XMMATRIX& World)
{
    Global::Context()->IASetInputLayout(inputLayout);
    Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(XMFLOAT3);
    UINT offset = 0;
    Global::Context()->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
    Global::Context()->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

    shader->GetTechniqueByName("boxTech")->GetPassByIndex(0)->Apply(0, Global::Context());

    XMMATRIX WVP = World * cam.View() * cam.Proj();
    shaderWorldViewProj->SetMatrix(reinterpret_cast<float*>(&WVP));
    Global::Context()->DrawIndexed(indexCount, 0, 0);
}
