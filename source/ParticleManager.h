#pragma once

#include "ParticleRenderingSystem.h"
#include "ParticleTexture.h"
#include <map>
#include <memory>
#include <vector>

class ParticleManager {
private:
    static std::map<ParticleTexture*, std::vector<Particle>> particles;
    static std::unique_ptr<ParticleRenderingSystem> renderingSys;

public:
    static void initialize();
    static void update(Camera& cam, float dt);
    static void addParticle(Particle particle);
    static void render(ParticleShader* shader, Camera* cam);
    static void clean();
};

