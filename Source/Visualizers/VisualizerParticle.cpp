#include "VisualizerParticle.hpp"
#include <cmath>

#define DATA_SIZE 128


namespace ASCIIPlayer
{
  VisualSystem::VisualSystem(float x, float y)
    : ParticleSystem <RConsole::Color>(30, 200, 10, true, RConsole::GREY)
  {
    posX_ = x;
    posY_ = y;
  }

  void VisualSystem::OnUpdateEnd()
  {
    for (auto& part : ParticleSystem<RConsole::Color>::particles_)
    {
      part.VelY += 0.0008f;

      float life_percentage = part.Life / static_cast<float>(maxLife_);
      if (life_percentage > .45)
        part.Data = RConsole::LIGHTCYAN;
      else
        part.Data = RConsole::CYAN;

      RConsole::Canvas::Draw('P', part.PosX, part.PosY, part.Data);
    }
  }

  // Run once
  void VisualSystem::AdjustParticle(Particle<RConsole::Color> &p)
  {
    p.VelX = (rand() % 20 - 10) / 160.0f;
    p.VelY = (rand() % 20 - 10) / 160.0f;
  }

  // Constructor
  VisualizerParticle::VisualizerParticle()
    : ASCIIVisualizer(DATA_SIZE, AUDIODATA_SPECTRUM, "visualizerParticle")
    , width_(RConsole::Canvas::GetConsoleWidth())
    , height_(RConsole::Canvas::GetConsoleHeight())
    , vs1_(width_ / 2.0f, height_ / 4.0f)
    , vs2_(width_ / 2.0f, height_ / 4.0f)
  {
    RConsole::Canvas::SetCursorVisible(false);
  }


  // Primary update loop for particle
  bool VisualizerParticle::Update(float* data)
  {
    if (data[0] + data[10] + data[20] == 0)
      return true;
    
    float data_avg = (data[0] + data[1] + data[2]) / 3.0f;
    if (data_avg < 0)
      data_avg = 0;

    float index_0 = data_avg * 33;

    // Offset systems
    vs1_.SetPosX(width_ / 2.0f + index_0);
    vs2_.SetPosX(width_ / 2.0f - index_0);
    vs1_.SetSpawnDelay(static_cast<int>(40 - index_0 * 2));
    vs2_.SetSpawnDelay(static_cast<int>(40 - index_0 * 2));

    // Update and return we're good!
    vs1_.Update();
    vs2_.Update();
    return true;
  }
}
