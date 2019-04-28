#include "VisualizerPineapple.hpp"
#include <memory>
#include <cstdlib>
#include <chrono>
#define NORMALIZED_TIME_MS 15


namespace ASCIIPlayer
{
  VisualizerPineapple::VisualizerPineapple()
    : ASCIIVisualizer(PINEAPPLE_SIZE, AUDIODATA_WAVEFORM)
    , width_(CONSOLE_WIDTH_FUNC)
    , height_(CONSOLE_HEIGHT_FUNC)
    , radius_(PINEAPPLE_SIZE / 4)
    , lastTimeMS_(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
    , lastTimeSecond_(lastTimeMS_)
    , lastTimes_(std::vector<long long>())
  {
    memset(smoothed_, 0, PINEAPPLE_SIZE * sizeof(float));
  }

	void VisualizerPineapple::OnResize(int newWidth, int newHeight)
	{
		RConsole::Canvas::ReInit(newWidth, newHeight);
		RConsole::Canvas::ForceClearEverything();
		width_ = newWidth;
		height_ = newHeight;
		RConsole::Canvas::SetCursorVisible(false);
	}

	bool VisualizerPineapple::Update(float* data)
	{
		const char symbol = '/';
    const char symbolAlt = '.';
		const int centerX = width_ / 2;
		const int centerY = height_ / 2;
    const int PINEAPPLE_SIZE_Y = PINEAPPLE_SIZE / 2;
    const float threshold = 1.0f;
    const float smoothRate = 0.965f;
    const float edgeCurve = 5;

    long long current = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // Smooth in new data
    if (current > lastTimeMS_ + NORMALIZED_TIME_MS)
    {
      for (int i = 0; i < PINEAPPLE_SIZE; ++i)
        smoothed_[i] += data[i];
    }

    // Display
		for (int y = 0; y < PINEAPPLE_SIZE_Y; ++y)
		{
			for (int x = 0; x < PINEAPPLE_SIZE; ++x)
			{
        // Define a limit radius. Because characters are taller than wide in most console stuff, this cases the pineapple shape.
        const float xDif = static_cast<float>(PINEAPPLE_SIZE / 2 - x);
        const float yDif = static_cast<float>(PINEAPPLE_SIZE_Y / 2 - y);
				
        float value;
        value = smoothed_[x] + smoothed_[y];
        float amt = value;
        if (amt < 0)
          amt = 0;
        float radius = radius_ + amt / 2;

        if(xDif * xDif + yDif * yDif > radius * radius)
          continue;

        // Acquire color
				const int posOffsetX = centerX - PINEAPPLE_SIZE / 2;
				const int posOffsetY = centerY - PINEAPPLE_SIZE_Y / 2;
        RConsole::Color color;

        color = RConsole::YELLOW;
        
        value *= 4;
        if (value > threshold)
					RConsole::Canvas::Draw(symbol, x + posOffsetX, y + posOffsetY, color);
        else if (value > threshold / 2)
          RConsole::Canvas::Draw(symbolAlt, x + posOffsetX, y + posOffsetY, color);
			}
		}

    // Debug
    float total = 0;
    for (long long val : lastTimes_)
      total += val;
    
    total /= lastTimes_.size();
    
    RConsole::Canvas::DrawString(std::to_string(1000.0 / total).c_str(), 0, 0, RConsole::WHITE);

    // Smooth out old data
    if (current > lastTimeMS_ + NORMALIZED_TIME_MS)
    {
      for (int i = 0; i < PINEAPPLE_SIZE; ++i)
        smoothed_[i] *= smoothRate;

      lastTimes_.insert(lastTimes_.begin(), current - lastTimeMS_);
      lastTimeMS_ = current;
    }

    // FPS tracking
    if (current > lastTimeSecond_ + 1000)
      lastTimeSecond_ = current;

    if (lastTimes_.size() > 100)
      lastTimes_.pop_back();

    return true;
	}

}

#undef PINEAPPLE_SIZE
#undef PINEAPPLE_INTERIOR
#undef NORMALIZED_TIME_MS