#include <iostream>
#include <chrono>

#define _USE_MATH_DEFINES // Flag that we want the defines in the C math header
#include <math.h>

struct Complex
{
  float real;
  float imag;
};

// (a + ib) (c + id) = (ac - bd) + i(ad + bc)
Complex ComplexMul(Complex lhs, Complex rhs)
{
  Complex multiplied;
  multiplied.real = (lhs.real * rhs.real) - (lhs.imag * rhs.imag);
  multiplied.imag = (lhs.real * rhs.imag) + (lhs.imag * rhs.real);

  return multiplied;
}

// (a + ib) + (c + id) = ((a + c) + i(b + d))
Complex ComplexAdd(Complex lhs, Complex rhs)
{
  return { (lhs.real + rhs.real), (lhs.imag + rhs.imag) };
}

// (a + ib) - (c + id) = ((a + c) + i(b - d))
Complex ComplexSub(Complex lhs, Complex rhs)
{
  return { (lhs.real - rhs.real), (lhs.imag - rhs.imag) };
}

#define SPACER() do { std::cout << "------------------------------------------------------------" << std::endl; } while (false) 
#define REAL 0
#define IMAG 1
#define WAVEFORM_SAMPLES 1024 // Size of waveform array. MUST BE MULTIPLE OF 2 don't you DARE make it not that.
Complex waveform[WAVEFORM_SAMPLES] = {}; // Scratch workspace

// Test config
const int testCount = 5000;             // Number of times to run the test. Generates output average.
const int waveformPreviewSamples = 128; // Arbitrary width cutoff for console display output.
const float majorFrequencyHz = 72;      // Waveform frequency within total sample count. X cycles in N data points.
const float minorFrequencyHz = 128;     // Frequency of half magnitude waveform as major in the same space as major.
const float thirdFrequencyHz = 7;       // A third, cos-based wave for testing
const float majorWeight = 2;            // The major frequency prominence, or weight, of the total. 
const float minorWeight = 1;            // The minor frequency prominence, or weight, of the total. 
const float thirdWeight = 3;            // The weight of the cos-based wave.
const float totalWeight = majorWeight + minorWeight + thirdWeight; // Mmm, tasty addition

/// <summary>
/// Reverse the specified number of bits
/// http://graphics.stanford.edu/~seander/bithacks.html
/// </summary>
/// <param name="input">The target integer to reverse N bits in</param>
/// <param name="n_bits_to_reverse">the number of bits to reverse</param>
int ReverseNBits(int input, int n_bits_to_reverse)
{
  int reversed = input;
  int remaining_to_reverse = n_bits_to_reverse - 1;

  input >>= 1;
  while (input > 0)
  {
    reversed = (reversed << 1) | (input & 1);
    --remaining_to_reverse;
    input >>= 1;
  }

  return (reversed << remaining_to_reverse) & ((1 << n_bits_to_reverse) - 1);
}

/// <summary>
/// Get the magnitude of a complex number
/// </summary>
/// <param name="to_evaluate"></param>
/// <returns></returns>
float ComplexMagnitude(Complex to_evaluate)
{
  const float real = to_evaluate.real;
  const float imaginary = to_evaluate.imag;
  return sqrt(real * real + imaginary * imaginary);
}

/// <summary>
/// Based on Radix-2 DIT case of Cooley-Tukey FFT algorithm, meaning it requires
/// size of input is a multiple of 2.
/// https://en.wikipedia.org/wiki/Cooley%E2%80%93Tukey_FFT_algorithm
/// https://www.phys.uconn.edu/~rozman/Courses/m3511_19s/downloads/radix2fft.pdf
/// 
/// Digestible explanation: https://www.youtube.com/watch?v=nmgFG7PUHfo
/// </summary>
/// <param name="to_transform">a vector that MUST have a size that's a multiple of 2</param>
void InternalFFT(Complex *to_transform, int size)
{
  const int bits = static_cast<int>(log2f(size));

  for (int i = 1; i < size; ++i)
  {
    const int swap_pos = ReverseNBits(i, bits);

    // Avoid dupe swaps
    if (swap_pos <= i)
    {
      continue;
    }

    std::swap(to_transform[i], to_transform[swap_pos]);
  }

  // Constrain to non-repetitive sections.
  for (int n = 2; n <= size; n <<= 1)
  {
    for (int i = 0; i < size; i += n)
    {
      for (int k = 0; k < n / 2; ++k)
      {
        const int index_even = i + k;
        const int index_odd = i + k + (n / 2);
        Complex even { to_transform[index_even].real , to_transform[index_even].imag };
        Complex odd { to_transform[index_odd].real, to_transform[index_odd].imag };

        const double term = -2 * M_PI * k / n;
        Complex exp{ std::cos(term), std::sin(term) };
        exp = ComplexMul(exp, odd);

        to_transform[index_even] = ComplexAdd(even, exp);
        to_transform[index_odd] = ComplexSub(even, exp);
      }
    }
  }
}

/// <summary>
/// Wrapper for raw float real number inputs.
/// We can operate under the assumption that all data exept input is garbage
/// </summary>
/// <param name="input">input array that remains unmodified</param>
/// <param name="output">modified output array</param>
/// <param name="size">precomputed size of both</param>
void FFT(const float* input, float* output, int size)
{
  for (int i = 0; i < size; ++i)
  {
    // Set internal _Val directly since we know we're only real at this point
    waveform[i].real = input[i];
    waveform[i].imag = 0;
  }

  InternalFFT(waveform, size);

  for (int i = 0; i < size; ++i)
  {
    output[i] = ComplexMagnitude(waveform[i]);
  }
}

  /////////////////////////////////////////////////////
 // NOTHING BEYOND THIS POINT NEEDS TO BE OPTIMIZED //
/////////////////////////////////////////////////////


/// <summary>
/// Draw out a waveform in the console that's represented by the provided float array.
/// </summary>
/// <param name="to_print">target to draw</param>
/// <param name="size">size of the data</param>
/// <param name=""></param>
void Dump(float* to_print, int size, int limit)
{
  const float graph_height = 10;

  for (int y = 0; y < graph_height; ++y)
  {
    for (int x = 0; x < size && x < limit; ++x)
    {
      const float cur = to_print[x];
      const float zero_to_one = (cur + 1.0f) / 2.0f;
      const float scaled = graph_height * zero_to_one;
      const float current_row = graph_height - y;
      const float size = 1.0f / graph_height * 10.0f;

      const bool show = (scaled + size >= current_row) && (scaled - size <= current_row);
      std::cout << (show ? static_cast<char>(254) : ' ');
    }

    std::cout << std::endl;
  }
}

/// <summary>
/// Times the FFT function, assuming conventional inputs. 
/// Comes with optional print arg that's off by default.
/// </summary>
/// <returns>The time the FFT function took to execute, in microseconds.</returns>
std::chrono::microseconds Test(float* input, float* output, int samples, bool print = false)
{
  std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
  FFT(input, output, samples);
  std::chrono::steady_clock::time_point finish = std::chrono::high_resolution_clock::now();

  std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
  std::chrono::microseconds us = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
  std::chrono::nanoseconds ns = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start);

  if (print)
  {
    std::cout << "Processed wave with " << samples << " in: " << ms.count() << "ms / " << us.count() << static_cast<char>(230) << "s / " << ns.count() << "ns" << std::endl;
  }

  return us;
}

/// <summary>
/// Based on globals, generates a waveform.
/// </summary>
/// <param name="waveform">Float array to put the generated waveform in. Will be written over.</param>
/// <param name="samples">number of samples in the array</param>
void GenerateWaveform(float* waveform, int samples)
{
  for (int i = 0; i < samples; ++i)
  {
    const float step_in_one_hz = (M_PI * 2.0f / WAVEFORM_SAMPLES) * i;
    const float freq_main = sin(step_in_one_hz * majorFrequencyHz);
    const float freq_alt = sin(step_in_one_hz * minorFrequencyHz);
    const float freq_third = cos(step_in_one_hz * thirdFrequencyHz);
    const float avg = (freq_main * majorWeight + freq_alt * minorWeight + freq_third * thirdWeight) / totalWeight;
    waveform[i] = avg;
  }
}

int main(int argc, char** argv)
{
  const int size = WAVEFORM_SAMPLES;
  float waveform[size] = {};
  float spectrum[size] = {};

  // Set up and display defaults
  GenerateWaveform(waveform, size);
  Dump(waveform, size, waveformPreviewSamples);
  SPACER();

  // Validate output
  std::cout << "Samples: " << size << " (previewed: " << waveformPreviewSamples << ")" << std::endl;
  std::cout << "Waves: " << majorFrequencyHz << "hz, " << minorFrequencyHz << "hz, " << thirdFrequencyHz << "hz" << std::endl;
  Test(waveform, spectrum, size, true);
  std::cout << "Frequencies found: ";
  bool fencepost = true;
  for (int i = 0; i < size / 2; ++i)
  {
    if (spectrum[i] > 0.01f)
    {
      std::cout << (fencepost ? "" : ", ") << i << "hz";
      fencepost = false;
    }
  }
  std::cout << std::endl;
  std::cout << "(Second half of array omitted due to nyquist)" << std::endl;
  SPACER();

  // Run benchmark
  std::cout << "Averaging " << testCount << " runs:" << std::endl;
  std::chrono::microseconds total = std::chrono::microseconds::zero();
  for (int i = 0; i < testCount; ++i)
  {
    if ((i % (testCount / 100)) == 0 && i > 0)
    {
      std::cout << ((i / static_cast<float>(testCount)) * 100 + 1) << "% of the way through...\r";
    }

    total += Test(waveform, spectrum, size, false);
  }

  const int microsec = (total / testCount).count();
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << (microsec / 1000.0f) << " ms avg over " << testCount << " runs " << std::endl;
  SPACER();

  return 0;
}