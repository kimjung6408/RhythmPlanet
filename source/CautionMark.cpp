#include "CautionMark.h"
#include "RenderState.h"
#include "MathHelper.h"

CautionMark::CautionMark()
    : lifeLength(30.0f / 150.0f), Position(0.0f, 1.0f, 0.0f), rotationAngle(0.0f), Alpha(0.0f), RotAxis(0.0f, 1.0f, 0.0f),
      lifeTimeSum(0.0f), Ambient(0.0f, 0.0f, 0.0f, 0.0f), frontType(CAUTION_TYPE_FRONT)
{
}

CautionMark::CautionMark(float BPM, XMFLOAT3 Position, XMFLOAT3 RotationAxis)
    : lifeLength(120.0f / BPM), Position(Position), rotationAngle(0.0f), Alpha(0.0f), RotAxis(RotationAxis),
      lifeTimeSum(0.0f), Ambient(0.0f, 0.0f, 0.0f, 0.0f), frontType(CAUTION_TYPE_FRONT)
{
}

bool CautionMark::Update(float dt)
{
    bool isAlive = true;
    lifeTimeSum += dt;

    float lifeRatio = lifeTimeSum / lifeLength;

    rotationAngle = MathHelper::Pi * lifeRatio;
    Alpha = 1.0f;

    return isAlive;
}

void CautionMark::Render(EntityShader* shader, Camera& cam, XMFLOAT3 lightPosition)
{
    Global::Context()->IASetInputLayout(shader->InputLayout());
    Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Global::Context()->RSSetState(RenderState::NoCullRS);

    UINT stride = sizeof(EntityShader::EntityVertex);
    UINT offset = 0;
    ID3D11Buffer* pVB = shader->VB();
    ID3D11Buffer* pIB = shader->IB();
    Global::Context()->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
    Global::Context()->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);

    shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());

    XMMATRIX TranslationMatrix = XMMatrixTranslation(Position.x, Position.y + 1.0f, Position.z);
    XMMATRIX ScaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
    XMMATRIX PreRot;

    if (RotAxis.x >= 0.5f)
    {
        PreRot = XMMatrixRotationRollPitchYaw(0, 0, MathHelper::Pi / 2.0f);
    }
    else if (RotAxis.z >= 0.5f)
    {
        PreRot = XMMatrixRotationRollPitchYaw(MathHelper::Pi / 2.0f, 0, 0);
    }
    else
    {
        PreRot = XMMatrixIdentity();
    }

    XMFLOAT4 tmpRotAxis = XMFLOAT4(RotAxis.x, RotAxis.y, RotAxis.z, 1.0f);
    XMMATRIX RotMatrix = XMMatrixRotationAxis(XMLoadFloat4(&tmpRotAxis), rotationAngle);
    XMMATRIX WorldMatrix = ScaleMatrix * PreRot * RotMatrix * TranslationMatrix;
    shader->LoadWorldViewProjMatrix(WorldMatrix * cam.View() * cam.Proj());
    shader->LoadWorldMatrix(WorldMatrix);
    shader->LoadWorldInvTranspose(WorldMatrix);
    shader->LoadLightPosition(lightPosition);
    shader->LoadUVOffset(XMFLOAT2(0.0f, 0.0f));
    shader->LoadAmbientColor(Ambient);

    Global::Context()->DrawIndexed(shader->getIndexCount(), 0, 0);
    Global::finishRender();
}

bool CautionMark::isDead()
{
    if (lifeLength <= lifeTimeSum)
        return true;
    else
        return false;
}
