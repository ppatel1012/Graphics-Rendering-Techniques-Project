#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float size;
    float lifetime;
    float maxLifetime;
    float opacity;
    bool active;
    
    Particle() : position(0.0f), velocity(0.0f), size(0.05f), 
                 lifetime(1.0f), maxLifetime(1.0f), opacity(1.0f), active(false) {}
    
    void init(const glm::vec3& pos, const glm::vec3& vel, float sz, float life) {
        position = pos;
        velocity = vel;
        size = sz;
        maxLifetime = life;
        lifetime = life;
        opacity = 0.6f;
        active = true;
    }
    
    void update(float deltaTime) {
        if (!active) return;
        
        position += velocity * deltaTime;
        lifetime -= deltaTime;
       // opacity = lifetime / maxLifetime;  // Fade out as snowflake ages
       // opacity = 0.8f;  // Constant opacity (no fade)
        opacity = 0.6f;  // Fully opaque
        
        if (lifetime <= 0.0f) {
            active = false;
        }
    }
};

class ParticleSystem {
private:
    std::vector<Particle> particles;
    int maxParticles;
    float spawnRate;
    float spawnTimer;
    glm::vec3 spawnAreaMin;
    glm::vec3 spawnAreaMax;
    glm::vec3 gravity;
    
public:
    ParticleSystem() : maxParticles(5500), spawnRate(0.02f), spawnTimer(0.0f) {
        spawnAreaMin = glm::vec3(-15.0f, 5.0f, -15.0f);
        spawnAreaMax = glm::vec3(15.0f, 12.0f, 15.0f);
        gravity = glm::vec3(0.0f, -2.0f, 0.0f);
        particles.resize(maxParticles);
    }
    
    void setSpawnArea(const glm::vec3& min, const glm::vec3& max) {
        spawnAreaMin = min;
        spawnAreaMax = max;
    }
    
    void setGravity(const glm::vec3& g) {
        gravity = g;
    }
    
    void update(float deltaTime) {
        // Spawn new particles
        spawnTimer += deltaTime;
        while (spawnTimer >= spawnRate) {
            spawnTimer -= spawnRate;
            spawnParticle();
        }
        
        // Update existing particles
        for (auto& particle : particles) {
            if (particle.active) {
                particle.velocity += gravity * deltaTime;
                particle.update(deltaTime);
                
                // Remove particles that hit the ground (y <= -0.5)
                if (particle.position.y <= -0.5f) {
                    particle.active = false;
                }
            }
        }
    }
    
    void spawnParticle() {
        // Find inactive particle
        for (auto& particle : particles) {
            if (!particle.active) {
                // Random position within spawn area
                
                float x = spawnAreaMin.x + (float)rand() / RAND_MAX * (spawnAreaMax.x - spawnAreaMin.x);
                float y = spawnAreaMin.y + (float)rand() / RAND_MAX * (spawnAreaMax.y - spawnAreaMin.y);
                float z = spawnAreaMin.z + (float)rand() / RAND_MAX * (spawnAreaMax.z - spawnAreaMin.z);
                
                // Random velocity (with slight horizontal drift)
                float vx = ((float)rand() / RAND_MAX - 0.5f) * 0.5f;
                float vz = ((float)rand() / RAND_MAX - 0.5f) * 0.5f;
                float vy = -1.0f - (float)rand() / RAND_MAX * 1.0f;
                
                // Random size
            //    float size = 1.8f + (float)rand() / RAND_MAX * 0.5f;  
            //    float size = 0.4f + (float)rand() / RAND_MAX * 0.5f;  // Much larger (0.4 to 0.9)
                float size = 0.12f + (float)rand() / RAND_MAX * 0.1f;  // Original size (0.12 to 0.22)
                
                // Random lifetime
                float lifetime = 2.0f + (float)rand() / RAND_MAX * 2.0f;
                
                particle.init(glm::vec3(x, y, z), glm::vec3(vx, vy, vz), size, lifetime);
                break;
            }
        }
    }
    
    const std::vector<Particle>& getParticles() const {
        return particles;
    }
    
    int getActiveParticleCount() const {
        int count = 0;
        for (const auto& p : particles) {
            if (p.active) count++;
        }
        return count;
    }
};