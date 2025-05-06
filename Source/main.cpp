#include <iostream>
#include <vector>
#include <FileIO/fileio.hpp>
#include "Core/Lobby.hpp"
#include "Utils/Benchmark.hpp"


namespace ASCIIPlayer
{
  int Run(int argc, char** argv)
  {
    // Run program
    bool benchmark = IsBenchmark(argc, argv);

    // Benchmark conditions
    if (!benchmark)
    {
      Lobby lounge(argc, argv);
      lounge.Run();
    }
    else
    {
      constexpr int benchmarkRuns = 20;
      std::vector<std::uint64_t> benchmarkResults;
      benchmarkResults.resize(benchmarkRuns);

      RunBenchmark(argc, argv, benchmarkResults);
      DrawBenchmarkOutput(benchmarkResults);

      std::system("pause");
    }

    // Return normally
    return 0;
  }
}

// Run main, catching dumping any errors.
int main(int argc, char** argv) try
{
  return ASCIIPlayer::Run(argc, argv);
}
catch (std::exception &e)
{
  std::cerr << e.what() << std::endl;
  std::cout << e.what() << std::endl;
  throw e;
}
