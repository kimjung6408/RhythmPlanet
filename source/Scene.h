#pragma once
#include"Camera.h"
#include"Utils.hpp"
#include"SoundSystem.h"

class Scene
{
private:
	

public:
	virtual SceneStatus Update(float dt) = 0;
	virtual void Render() = 0;
};