#include <cmath>
#include "VisualizerParticle.hpp"


namespace ASCIIPlayer
{
  // Derived system from base particle system.
  VisualSystem::VisualSystem(float x, float y)
    : ParticleSystem <std::pair<RConsole::Color, char>>(30, 200, 10, true, std::make_pair(RConsole::GREY, 'p'))
    , index_(0)
  {
    posX_ = x;
    posY_ = y;
  }

  // Run at the end of particle systems
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
    : ASCIIVisualizer(FMOD_DATA_SIZE, AudioDataStyle::AUDIODATA_WAVEFORM)
    , width_(RConsole::Canvas::GetConsoleWidth())
    , height_(RConsole::Canvas::GetConsoleHeight())
    , vs1_(width_ / 2.0f, height_ / 4.0f)
    , vs2_(width_ / 2.0f, height_ / 4.0f)
  {
    RConsole::Canvas::SetCursorVisible(false);
  }

  // Handle resizing 
  void VisualizerParticle::OnResize(int newWidth, int newHeight)
  {
    RConsole::Canvas::ReInit(newWidth, newHeight);
    RConsole::Canvas::ForceClearEverything();
    width_ = newWidth;
    height_ = newHeight;
    vs1_.SetPos(width_ / 2.0f, height_ / 4.0f);
    vs2_.SetPos(width_ / 2.0f, height_ / 4.0f);
    RConsole::Canvas::SetCursorVisible(false);
  }

  // Primary update loop for particle
  bool VisualizerParticle::Update(double dt, float* data, bool isActive)
  {
    const float data_avg = (data[0] + data[1] + data[2] + data[3] + data[4] + data[5] + data[6] + data[7]) / 8.0f;
    const float position = data_avg * (width_ / 2.3f);
    const float intensity = data_avg * 8*50;
    const float defaultSpawnDelay = 100;

    // Offset systems
    vs1_.SetPosX(width_ / 2.0f + position);
    vs2_.SetPosX(width_ / 2.0f - position);
    vs1_.SetSpawnDelay(static_cast<int>(defaultSpawnDelay - intensity));
    vs2_.SetSpawnDelay(static_cast<int>(defaultSpawnDelay - intensity));

    // Update and return we're good!
    vs1_.Update();
    vs2_.Update();

    return true;
  }
}
