#pragma once
#include "ASCIIVisualizer.hpp"
#include "Particles/ParticleSystem.hpp"
#include "Particles/Particle.hpp"
#include <vector>


namespace ASCIIPlayer
{
  class VisualSystem : public ParticleSystem<RConsole::Color>
  {
  public:
    VisualSystem(float x, float y);

    void OnUpdateEnd();
    void AdjustParticle(Particle<RConsole::Color> &p);
  };

  class VisualizerParticle : public ASCIIVisualizer
  {
  public:
    // Constructor
    VisualizerParticle();

    // DrawBars
    bool Update(float* data);
    int width_;
    int height_;
    VisualSystem vs1_;
    VisualSystem vs2_;
  };
}
