#include "ParticleManager.h"

#include <utility>

std::map<ParticleTexture*, std::vector<Particle>> ParticleManager::particles;
std::unique_ptr<ParticleRenderingSystem> ParticleManager::renderingSys;

void ParticleManager::initialize() {
    renderingSys = std::make_unique<ParticleRenderingSystem>();
}

void ParticleManager::update(Camera& cam, float dt) {
    for (auto& entry : particles) {
        auto& pList = entry.second;
        unsigned int i = 0;
        while (i < pList.size()) {
            const bool alive = pList[i].update(cam, dt);
            if (!alive) {
                pList.erase(pList.begin() + i);
            } else {
                ++i;
            }
        }
    }
}

void ParticleManager::addParticle(Particle particle) {
    particles[particle.getTexture()].push_back(std::move(particle));
}

void ParticleManager::render(ParticleShader* shader, Camera* cam) {
    if (renderingSys) {
        renderingSys->render(shader, *cam, particles);
    }
}

void ParticleManager::clean() {
    particles.clear();
}

