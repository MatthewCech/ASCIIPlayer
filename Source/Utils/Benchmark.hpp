#pragma once
#include <vector>


// Static benchmarking utility functions
namespace ASCIIPlayer
{
  bool IsBenchmark(int argc, char** argv);

  void RunBenchmark(int argc, char** argv, std::vector<std::uint64_t>& outputResults);

  void DrawBenchmarkOutput(const std::vector<std::uint64_t>& results);
}