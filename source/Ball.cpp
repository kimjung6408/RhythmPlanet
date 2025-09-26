#include "Ball.h"
#include <cmath>

Ball::Ball(float radius, XMFLOAT3 center, float velocity, XMFLOAT3 Direction)
    : radius(radius), centerPos(center), Direction(Direction), Rotation(0.0f, 0.0f, 0.0f), velocity(velocity),
      UVOffset(0.0f, 0.0f), Ambient(0.0f, 0.0f, 0.0f, 0.0f)
{
}

float Ball::GetRadius()
{
    return radius;
}

void Ball::Render(EntityShader* shader, Camera& cam, XMFLOAT3 lightPosition)
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

    XMMATRIX TranslationMatrix = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);
    XMMATRIX ScaleMatrix = XMMatrixScaling(radius, radius, radius);
    XMMATRIX RotMatrix = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
    XMMATRIX WorldMatrix = ScaleMatrix * RotMatrix * TranslationMatrix;
    shader->LoadWorldViewProjMatrix(WorldMatrix * cam.View() * cam.Proj());
    shader->LoadWorldMatrix(WorldMatrix);
    shader->LoadWorldInvTranspose(WorldMatrix);
    shader->LoadLightPosition(lightPosition);
    shader->LoadUVOffset(XMFLOAT2(0.0f, 0.0f));
    shader->LoadAmbientColor(Ambient);

    Global::Context()->DrawIndexed(shader->getIndexCount(), 0, 0);
    Global::finishRender();
}

void Ball::Update(float dt)
{
    Move(Direction, velocity, dt);
}

void Ball::SetPosition(XMFLOAT3 position)
{
    centerPos = position;
}

void Ball::SetRotation(float xAngle, float yAngle, float zAngle)
{
    Rotation = XMFLOAT3(xAngle, yAngle, zAngle);
}

void Ball::SetVelocity(float velocityValue)
{
    velocity = velocityValue;
}

void Ball::SetDirection(XMFLOAT3 direction)
{
    Direction = direction;
    (void)std::sqrt(Direction.x * Direction.x + Direction.y * Direction.y + Direction.z * Direction.z);
}

void Ball::SetUVOffset(XMFLOAT2 uvOffset)
{
    UVOffset = uvOffset;
}

void Ball::Move(XMFLOAT3 direction, float Velocity, float dt)
{
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

    if (length > 1e-12f)
    {
        direction = XMFLOAT3(direction.x / length, direction.y / length, direction.z / length);
    }

    centerPos.x += direction.x * Velocity * dt;
    centerPos.y += direction.y * Velocity * dt;
    centerPos.z += direction.z * Velocity * dt;
}

XMFLOAT3 Ball::GetPosition()
{
    return centerPos;
}

XMFLOAT3 Ball::GetRotation()
{
    return Rotation;
}

XMFLOAT3 Ball::GetDirection()
{
    return Direction;
}

float Ball::GetVelocity()
{
    return velocity;
}

XMFLOAT2 Ball::GetUVOffset()
{
    return UVOffset;
}

void Ball::SetAmbientColor(XMFLOAT3 color, float Alpha)
{
    Ambient.x = color.x;
    Ambient.y = color.y;
    Ambient.z = color.z;
    Ambient.w = Alpha;
}
