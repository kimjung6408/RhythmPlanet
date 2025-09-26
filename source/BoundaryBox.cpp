#include "BoundaryBox.h"
#include <cmath>

BoundaryBox::BoundaryBox(XMFLOAT3 Position, XMFLOAT3 Scale)
    : Position(Position), Direction(0.0f, 0.0f, 0.0f), Rotation(0.0f, 0.0f, 0.0f), Ambient(0.0f, 0.0f, 0.0f, 0.0f),
      Scale(Scale), velocity(0.0f), UVOffset(0.0f, 0.0f)
{
}

void BoundaryBox::Render(EntityShader* shader, Camera& cam, XMFLOAT3 lightPosition)
{
    Global::Context()->IASetInputLayout(shader->InputLayout());
    Global::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(EntityShader::EntityVertex);
    UINT offset = 0;
    ID3D11Buffer* pVB = shader->VB();
    ID3D11Buffer* pIB = shader->IB();
    Global::Context()->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
    Global::Context()->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);

    shader->getTech()->GetPassByIndex(0)->Apply(0, Global::Context());

    XMMATRIX TranslationMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);
    XMMATRIX ScaleMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
    XMMATRIX WorldMatrix = ScaleMatrix * TranslationMatrix;
    shader->LoadWorldViewProjMatrix(WorldMatrix * cam.View() * cam.Proj());
    shader->LoadWorldMatrix(WorldMatrix);
    shader->LoadWorldInvTranspose(WorldMatrix);
    shader->LoadLightPosition(lightPosition);
    shader->LoadUVOffset(XMFLOAT2(0.0f, 0.0f));
    shader->LoadAmbientColor(Ambient);

    Global::Context()->DrawIndexed(shader->getIndexCount(), 0, 0);
    Global::finishRender();
}

void BoundaryBox::Update(float /*dt*/)
{
}

void BoundaryBox::SetPosition(XMFLOAT3 position)
{
    Position = position;
}

void BoundaryBox::SetVelocity(float velocityValue)
{
    velocity = velocityValue;
}

void BoundaryBox::SetDirection(XMFLOAT3 direction)
{
    Direction = direction;
    (void)std::sqrt(Direction.x * Direction.x + Direction.y * Direction.y + Direction.z * Direction.z);
}

void BoundaryBox::SetUVOffset(XMFLOAT2 UVOffsetValue)
{
    UVOffset = UVOffsetValue;
}

void BoundaryBox::Move(XMFLOAT3 /*Direction*/, float /*Velocity*/, float /*dt*/)
{
}

XMFLOAT3 BoundaryBox::GetPosition()
{
    return Position;
}

XMFLOAT3 BoundaryBox::GetRotation()
{
    return Rotation;
}

XMFLOAT3 BoundaryBox::GetDirection()
{
    return Direction;
}

float BoundaryBox::GetVelocity()
{
    return velocity;
}

XMFLOAT2 BoundaryBox::GetUVOffset()
{
    return UVOffset;
}

void BoundaryBox::SetAmbientColor(XMFLOAT3 color, float Alpha)
{
    Ambient.x = color.x;
    Ambient.y = color.y;
    Ambient.z = color.z;
    Ambient.w = Alpha;
}

XMFLOAT4 BoundaryBox::GetAmbientColor()
{
    return Ambient;
}
