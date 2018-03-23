#include "VisualizerParticle.hpp"
#include <cmath>

#define DATA_SIZE 128


namespace ASCIIPlayer
{
  VisualSystem::VisualSystem(float x, float y)
    : ParticleSystem <std::pair<RConsole::Color, char>>(30, 200, 10, true, std::make_pair(RConsole::GREY, 'p'))
    , index_(0)
  {
    posX_ = x;
    posY_ = y;
  }

  void VisualSystem::OnUpdateEnd()
  {
    for (auto& part : ParticleSystem<std::pair<RConsole::Color, char>>::particles_)
    {
      part.VelY += 0.0008f;

      float life_percentage = part.Life / static_cast<float>(maxLife_);
      if (life_percentage > .45)
        part.Data.first = RConsole::LIGHTCYAN;
      else
        part.Data.first = RConsole::CYAN;

      RConsole::Canvas::Draw(part.Data.second, part.PosX, part.PosY, part.Data.first);
    }
  }

  // Run once
  void VisualSystem::AdjustParticle(Particle<std::pair<RConsole::Color, char>> &p)
  {
    p.VelX = (rand() % 20 - 10) / 160.0f;
    p.VelY = (rand() % 20 - 10) / 160.0f;

    p.Data.second = static_cast<unsigned char>((rand() % (255 - 32)) + 32);
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
    
    float data_avg = (data[0] + data[1]) / 2.0f;
    if (data_avg < 0)
      data_avg = 0;

    float index_0 = data_avg * (width_ / 2.1f);

    // Offset systems
    vs1_.SetPosX(width_ / 2.0f + index_0);
    vs2_.SetPosX(width_ / 2.0f - index_0);
    vs1_.SetSpawnDelay(static_cast<int>((width_ / 2) - index_0 * 1.7f));
    vs2_.SetSpawnDelay(static_cast<int>((width_ / 2) - index_0 * 1.7f));

    // Update and return we're good!
    vs1_.Update();
    vs2_.Update();
    return true;
  }
}
