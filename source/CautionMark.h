#pragma once
#include "d3dApp.h"
#include "Camera.h"
#include "EntityShader.h"

#define CAUTION_TYPE_LEFT 0
#define CAUTION_TYPE_RIGHT 1
#define CAUTION_TYPE_FRONT 2
#define CAUTION_TYPE_FLOOR 3

class CautionMark
{
private:
    float lifeLength;
    XMFLOAT3 Position;
    float rotationAngle;
    float Alpha;
    XMFLOAT3 RotAxis;
    float lifeTimeSum;
    XMFLOAT4 Ambient;
    int frontType;

public:
    CautionMark();
    CautionMark(float BPM, XMFLOAT3 Position, XMFLOAT3 RotationAxis);

    bool Update(float dt);
    void Render(EntityShader* shader, Camera& cam, XMFLOAT3 lightPosition);
    bool isDead();
};
