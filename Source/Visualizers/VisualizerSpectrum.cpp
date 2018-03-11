#include "VisualizerSpectrum.hpp"
#include <cmath>

#define DATA_SIZE 256
#define OSCILATION_CAP 2048.0f

namespace ASCIIPlayer
{
  VisualizerSpectrum::VisualizerSpectrum() 
    : ASCIIVisualizer(DATA_SIZE, AUDIODATA_SPECTRUM, "visualizerSpectrum")
    , width_(RConsole::Canvas::GetConsoleWidth())
    , height_(RConsole::Canvas::GetConsoleHeight())
    , side_offset_(2)
    , pos_1_x_(static_cast<float>(side_offset_))
    , pos_1_y_(static_cast<float>(height_ - 1))
    , pos_2_x_(static_cast<float>(width_ - side_offset_))
    , pos_2_y_(static_cast<float>(0))
    , oscilation_location_(-128)
  { 
    RConsole::Canvas::SetCursorVisible(false);
  }

  // Draw vertical spectrum based on frequencies
  bool VisualizerSpectrum::Update(float* data)
  {
    // Establish offsets and constants
    const int vertical_offset = height_ / 2;
    const int dropped_bars = 2;
    const int vertical_scalar = 700;
    
    // Shift y positions of both Ys
    pos_1_y_ = vertical_offset + sin(static_cast<float>(oscilation_location_) / (OSCILATION_CAP / 2) * 3.14159f) * vertical_offset;
    pos_2_y_ = height_ - pos_1_y_;

    // Characters to draw!
    const unsigned char symbol_up = static_cast<unsigned>(174);// 'u';
    const unsigned char symbol_down = static_cast<unsigned>(175);// 'n';
    
    // Slope calculation
    const float ydiff = pos_2_y_ - pos_1_y_;
    float xdiff = pos_2_x_ - pos_1_x_;
    if (xdiff < .1 && xdiff > -0.1)
      xdiff = 0.1f;

    // Slope and current offset calculation
    const float slope =  ydiff / xdiff;
    const int width = static_cast<int>(abs(pos_1_x_ - pos_2_x_));
    float y_pos = pos_1_y_;

    // Draw the lines
    for (int i = 0; i < width && i < DATA_SIZE; ++i)
    {
      const float value = data[i];
      float height = roundf(log(value * vertical_scalar) * (value * 200) / 20.0f);
      if (height < 0)
        height = 0;

      RConsole::Color color = RConsole::LIGHTBLUE;

      // Goes a certain distance in either direction!
      for (int j = 0; j < height; ++j)
      {
        if (j > height - (height / 1.5))
          color = RConsole::CYAN;

        if (j > height - (height / 4))
          color = RConsole::GREY;

        RConsole::Canvas::Draw(symbol_up, pos_1_x_ + i, y_pos + j, color);
        RConsole::Canvas::Draw(symbol_down, pos_2_x_ - i, height_ - y_pos - j, color);
      }

      y_pos += slope;
    }
      
    if (++oscilation_location_ > OSCILATION_CAP)
      oscilation_location_ = 0;

    return true;
  }
}
