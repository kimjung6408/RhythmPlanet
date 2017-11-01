#pragma once
#include"ParticleRenderingSystem.h"
#include"ParticleTexture.h"
#include<map>

//모든 파티클을 저장하고, 수명을 관리하는 클래스.
class ParticleManager
{
private:
	static map<ParticleTexture*, vector<Particle>> particles;
	static ParticleRenderingSystem* renderingSys;

public:
	static void initialize()
	{
		renderingSys = new ParticleRenderingSystem();
	}

	static void update(Camera& cam, float dt)
	{
		for (map < ParticleTexture*, vector<Particle>>::iterator iter = particles.begin(); iter != particles.end(); iter++)
		{
			vector<Particle>& pList = iter->second;
			unsigned int i = 0;
			while (i < pList.size())
			{
				bool alive = pList[i].update(cam, dt);

				//수명이 다하면
				if (!alive)
				{	
					//삭제한다.
					pList.erase(pList.begin() + i);
				}
				else
					i++;
			}
		}
	}

	static void addParticle(Particle particle)
	{
		particles[particle.getTexture()].push_back(particle);
	}

	static void render(ParticleShader* shader, Camera* cam)
	{
		renderingSys->render(shader, *cam, particles);
	}

	static void clean()
	{
		particles.clear();
	}
};

map<ParticleTexture*, vector<Particle>> ParticleManager::particles = map<ParticleTexture*, vector<Particle>>();
ParticleRenderingSystem* ParticleManager::renderingSys = 0;