#pragma once
#include "ASCIIVisualizer.hpp"
#include "Particles/ParticleSystem.hpp"
#include "Particles/Particle.hpp"
#include <vector>


namespace ASCIIPlayer
{
  class VisualSystem : public ParticleSystem<std::pair<RConsole::Color, char>>
  {
  public:
    VisualSystem(float x, float y);

    void OnUpdateEnd();
    void AdjustParticle(Particle<std::pair<RConsole::Color, char>> &p);

    int index_;
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
