#ifndef EFFECT_H
#define EFFECT_H

#include "wall-timer.h"
#include "particles.h"

class Effect
{   
protected:
    Particles *m_particles;

    unsigned int m_duration; // in msec
    wall_time_timer m_timer;
    bool m_isRunning;

    vec3f m_origin;

public:
    Effect(vec3f origin, unsigned int maxParticleCount, float particleSize, vec3f color);
    ~Effect();

    virtual void Update() { m_particles->Update(); }
    virtual void Render() { m_particles->Render(); }

    virtual void Start() {}
    bool IsFinished() { return !m_isRunning; }
    void ChangeParticleColor(vec3f color) { m_particles->ChangeColor(color); }
};

class UpgradeEffect : public Effect
{

public:
    UpgradeEffect(vec3f origin, vec3f color);

    void Start();

    void Update();
    void Render();
};

class BombermanEffect : public Effect
{
    const static unsigned int s_particleSpawnRate = 100; // in msec
    wall_time_timer m_particleSpawnTimer;

public:
    BombermanEffect(vec3f origin, vec3f color);

    void Start();

    void Update(vec3f newOrigin);
    void Render();
};

class SnowEffect : public Effect
{
    const static unsigned int s_particleSpawnRate = 100; // in msec
    wall_time_timer m_particleSpawnTimer;

    float m_radius;

public:
    SnowEffect(vec3f mapCenter, float radius);

    void Start();

    void Update();
    void Render();
};

#endif // EFFECT_H
