#pragma once
#include <vector>
#include "ASCIIVisualizer.hpp"
#include "Particles/ParticleSystem.hpp"
#include "Particles/Particle.hpp"


namespace ASCIIPlayer
{
  // Custom particle system class for current visualizer
  class VisualSystem : public ParticleSystem<std::pair<RConsole::Color, char>>
  {
  public:
    // Constructor
    VisualSystem(float x, float y);

    // Methods
    void OnUpdateEnd();
    void AdjustParticle(Particle<std::pair<RConsole::Color, char>> &p);

    // Variables
  private:
    int index_;
  };


  // A visualizer with a particle system driven by the music
  class VisualizerParticle : public ASCIIVisualizer
  {
  public:
    // Constructor
    VisualizerParticle();

    // Methods
    bool Update(float* data, bool isActive);
    void OnResize(int newWidth, int newHeight);

  private:
    // Variables
    int width_;
    int height_;
    VisualSystem vs1_;
    VisualSystem vs2_;
  };
}
