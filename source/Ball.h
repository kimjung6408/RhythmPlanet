#pragma once
#include "d3dApp.h"
#include "GeometryGenerator.h"
#include "RenderState.h"
#include <xnacollision.h>
#include "Entity.h"
#include "EntityShader.h"

class Ball : public Entity
{
private:
    float radius;
    XMFLOAT3 centerPos;
    XMFLOAT3 Direction;
    XMFLOAT3 Rotation;
    float velocity;

    XMFLOAT2 UVOffset;
    XMFLOAT4 Ambient;

public:
    Ball(float radius, XMFLOAT3 center, float velocity, XMFLOAT3 Direction);

    float GetRadius();

    void Render(EntityShader* shader, Camera& cam, XMFLOAT3 lightPosition) override;
    void Update(float dt) override;
    void SetPosition(XMFLOAT3 position) override;
    void SetRotation(float xAngle, float yAngle, float zAngle);
    void SetVelocity(float velocity) override;
    void SetDirection(XMFLOAT3 direction) override;
    void SetUVOffset(XMFLOAT2 UVOffset) override;
    void Move(XMFLOAT3 Direction, float Velocity, float dt) override;
    XMFLOAT3 GetPosition() override;
    XMFLOAT3 GetRotation() override;
    XMFLOAT3 GetDirection() override;
    float GetVelocity() override;
    XMFLOAT2 GetUVOffset() override;
    void SetAmbientColor(XMFLOAT3 color, float Alpha) override;
    XMFLOAT4 GetAmbientColor() override { return Ambient; }
};
