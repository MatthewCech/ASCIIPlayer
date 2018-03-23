#pragma once
#include "Particle.hpp"
#include "Defines.hpp"
#include <list>


template <typename T> 
class ParticleSystem
{
public:
  // Constructor
  ParticleSystem(int max, int life, int spawn_delay, bool loops, const T &def)
    : posX_(0)
    , posY_(0)
    , maxParticles_(max)
    , maxLife_(life)
    , spawnDelay_(spawn_delay)
    , spawnCounter_(0)
    , isLooping_(loops)
    , default_(def)
    , particles_()
  {  }


  // Primary update, should be called every loop.
  void Update()
  {
    // Pre-update call
    OnUpdateStart();

    // If necessary, add new particles.
    // If there is no delay, add them all immediately.
    if (isLooping_)
    {
      if (spawnDelay_ == 0)
        while (particles_.size() < maxParticles_)
          AddParticle();

      else if (++spawnCounter_ >= spawnDelay_)
      {
        AddParticle();
        spawnCounter_ = 0;
      }
    }

    // Update all
    for (auto&& p : particles_)
    {
      if (--p.Life < 0)
        p.Active = false;

      p.PosX += p.VelX;
      p.PosY += p.VelY;
    }

    // Remove every particle no longer marked as active.
    particles_.remove_if([](const Particle<T> &p) 
    { 
      return p.Active == false; 
    });

    // Post-update call
    OnUpdateEnd();
  }


  // Adds a particle
  void AddParticle()
  {
    Particle<T> p1 = Particle<T>(default_, posX_, posY_, 0, 1, maxLife_);
    AdjustParticle(p1);
    particles_.push_back(p1);
  }


  // Override to customize how particles are created.
  virtual void AdjustParticle(Particle<T> &p) { }

  // Can be implemented to add behavior to the start and end of the update.
  virtual void OnUpdateStart() { }
  virtual void OnUpdateEnd() { }

  // Allows access to things
  std::list<Particle<T>> &Particles() { return particles_; }
  float GetPosX() { return posX_; }
  float GetPosY() { return posY_; }
  void SetPosX(float x) { posX_ = x; }
  void SetPosY(float x) { posY_ = y; }

protected:
  // Variables
  float posX_;
  float posY_;
  size_t maxParticles_;
  int maxLife_;
  int spawnDelay_;
  int spawnCounter_;
  bool isLooping_;
  const T default_;
  std::list<Particle<T>> particles_;
};