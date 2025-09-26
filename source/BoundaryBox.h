#pragma once
#include "d3dApp.h"
#include "GeometryGenerator.h"
#include "RenderState.h"
#include <xnacollision.h>
#include "Entity.h"
#include "EntityShader.h"

class BoundaryBox : public Entity
{
private:
    XMFLOAT3 Position;
    XMFLOAT3 Direction;
    XMFLOAT3 Rotation;
    XMFLOAT4 Ambient;
    XMFLOAT3 Scale;
    float velocity;

    XMFLOAT2 UVOffset;

public:
    BoundaryBox(XMFLOAT3 Position, XMFLOAT3 Scale);

    void Render(EntityShader* shader, Camera& cam, XMFLOAT3 lightPosition) override;
    void Update(float dt) override;
    void SetPosition(XMFLOAT3 position) override;
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
    XMFLOAT4 GetAmbientColor() override;
};
