#include "effect.h"
#include "rendering.h"

Effect::Effect(vec3f &origin, unsigned int maxParticleCount, float particleSize, vec3f &color)
{
    m_origin = origin;
    m_particles = new Particles(maxParticleCount, 1.0, particleSize, color);
}

Effect::~Effect()
{
    delete m_particles;
}

UpgradeEffect::UpgradeEffect(vec3f &origin, vec3f &color)
    :Effect(origin, 200, 2.0f, color)
{

}

void UpgradeEffect::Start()
{
    for(int i = 0; i < 200; i++) {
        //vec3f origin = vec3f(0,0,0);
        vec3f dir = vec3f((random_float() - 0.5) * 0.1f,random_float() * 2.0f, (random_float() - 0.5) * 0.1f);
        m_particles->Add(m_origin, dir, 100);
    }
}

void UpgradeEffect::Update()
{
    Effect::Update();
}

void UpgradeEffect::Render()
{
    Effect::Render();
}

BombermanEffect::BombermanEffect(vec3f &origin, vec3f &color)
    :Effect(origin, 20, 1.0f, color)
{

}

void BombermanEffect::Start()
{
    m_particleSpawnTimer.start();
}

void BombermanEffect::Update(vec3f &newOrigin)
{
    m_origin = newOrigin;
    Effect::Update();

    if(m_particleSpawnTimer.look() < wall_time_timer::msec(s_particleSpawnRate)) return;

    m_particleSpawnTimer.restart();

    vec3f dir = vec3f((random_float() - 0.5) * 0.1f,random_float() * -0.2f, (random_float() - 0.5) * 0.1f);
    m_particles->Add(m_origin, dir, 100);
}

void BombermanEffect::Render()
{
    Effect::Render();
}
