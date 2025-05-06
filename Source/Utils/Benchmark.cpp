#include <string>
#include <vector>
#include <numeric>
#include <ConsoleUtils/console-utils.hpp>
#include "Benchmark.hpp"
#include "Core/Lobby.hpp"


namespace ASCIIPlayer
{
  // Sloppily determine if we have the benchmark argument
  bool IsBenchmark(int argc, char **argv)
  {
    for (int i = 0; i < argc; ++i)
    {
       std::string arg(argv[i]);
       std::string benchmarkArg("--benchmark");
       const bool isEqual = arg == benchmarkArg;

       if (isEqual)
       {
         return true;
       }
    }

    return false;
  }

  // Spin up and down lobbies enough times to fill the provided vector with loop counts.
  // Assumes vector is appropriately sized already with something like vector.resize(...)
  void RunBenchmark(int argc, char** argv, std::vector<std::uint64_t>& outputResults)
  {
    for (size_t i = 0; i < outputResults.size(); ++i)
    {
      Lobby benchmarkLobby(argc, argv);
      benchmarkLobby.GetDJ()->VolumeSet(0.05f); // Hard-code to impact scaling and also not detonate ears.
      benchmarkLobby.InterpretMultiCharInput("benchmark5s.mp3");
      benchmarkLobby.EnableOneshotPlayback();
      benchmarkLobby.Run();

      outputResults[i] = benchmarkLobby.GetUpdateLoopCount();
    }
  }

  // Attempt to draw out all the content of a benchmark result
  void DrawBenchmarkOutput(const std::vector<std::uint64_t>& loops)
  {
    if (loops.empty())
    {
      return;
    }

    const int iterations = loops.size();
    const int average = static_cast<int>(std::accumulate(loops.begin(), loops.end(), static_cast<std::uint64_t>(0)) / static_cast<double>(iterations));
    int yOffset = 1;

    RConsole::Canvas::ForceClearEverything();
    RConsole::Canvas::DrawString((std::string("  Total = ") + std::to_string(iterations)).c_str(), 1, yOffset++);
    RConsole::Canvas::DrawString((std::string("Average = ") + std::to_string(average)).c_str(), 1, yOffset++);

    for (int i = 0; i < iterations; ++i)
    {
      RConsole::Canvas::DrawString((std::string(" Run ") + std::to_string(i + 1) + std::string(": ") + std::to_string(loops[i])).c_str(), 1, yOffset++);
    }

    RConsole::Canvas::DrawString(" ", 0, yOffset++);
    RConsole::Canvas::DrawString(" ", 0, yOffset++);
    RConsole::Canvas::Update();
  }
}