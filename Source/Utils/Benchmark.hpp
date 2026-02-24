#pragma once
#include <vector>

#define DEFAULT_WAVEFORM_FOR_BENCHMARK "waveform lite"

// Static benchmarking utility functions
namespace ASCIIPlayer
{
  bool IsBenchmark(int argc, char** argv);

  void RunBenchmark(int argc, char** argv, std::vector<std::uint64_t>& outputResults, std::string visualizer = DEFAULT_WAVEFORM_FOR_BENCHMARK);

  void DrawBenchmarkOutput(const std::vector<std::uint64_t>& results);
}

#undef DEFAULT_WAVEFORM_FOR_BENCHMARK