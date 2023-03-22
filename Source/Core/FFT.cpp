#define _USE_MATH_DEFINES // enables defines like M_PI
#include <math.h>         // sin, cos
#include "Defines.hpp"    // data size
#include "Core\FFT.hpp"


namespace ASCIIPlayer
{
  /// <summary>
  /// A float-centeric implementation of std::complex to help improve performance
  /// </summary>
  struct Complex
  {
    float real;
    float imag;

    // (a + ib) (c + id) = (ac - bd) + i(ad + bc)
    static Complex ComplexMul(Complex lhs, Complex rhs)
    {
      Complex multiplied;
      multiplied.real = (lhs.real * rhs.real) - (lhs.imag * rhs.imag);
      multiplied.imag = (lhs.real * rhs.imag) + (lhs.imag * rhs.real);

      return multiplied;
    }

    // (a + ib) + (c + id) = ((a + c) + i(b + d))
    static Complex ComplexAdd(Complex lhs, Complex rhs)
    {
      return { (lhs.real + rhs.real), (lhs.imag + rhs.imag) };
    }

    // (a + ib) - (c + id) = ((a + c) + i(b - d))
    static Complex ComplexSub(Complex lhs, Complex rhs)
    {
      return { (lhs.real - rhs.real), (lhs.imag - rhs.imag) };
    }
  };

  // Scratch workspace. We are assuming we're using stereo settings, so we only care about one channel at a time.
  Complex buffer[FMOD_DATA_SIZE / 2] = {}; 

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
  /// Wrapper for raw float real number inputs.
  /// We can operate under the assumption that all data inside output is garbage.
  /// </summary>
  /// <param name="input">input array that remains unmodified</param>
  /// <param name="output">written over output array</param>
  /// <param name="size">precomputed size of both</param>
  void FFT(const float* input, float* output, int size)
  {
    for (int i = 0; i < size; ++i)
    {
      buffer[i].real = input[i];
      buffer[i].imag = 0;
    }

    FFTInPlace(buffer, size);

    // Output the magnitude of everything
    for (int i = 0; i < size; ++i)
    {
      const float real = buffer[i].real;
      const float imag = buffer[i].imag;
      output[i] = sqrt(real * real + imag * imag);
    }
  }

  /// <summary>
  /// Based on Radix-2 DIT case of Cooley-Tukey FFT algorithm. Size must be a multiple of 2.
  /// https://en.wikipedia.org/wiki/Cooley%E2%80%93Tukey_FFT_algorithm
  /// https://www.phys.uconn.edu/~rozman/Courses/m3511_19s/downloads/radix2fft.pdf
  /// 
  /// Digestible explanation: https://www.youtube.com/watch?v=nmgFG7PUHfo
  /// </summary>
  /// <param name="to_transform">a vector that MUST have a size that's a multiple of 2</param>
  void FFTInPlace(Complex* to_transform, int size)
  {
    const int bits = static_cast<int>(log2(size));

    for (int i = 1; i < size; ++i)
    {
      const int swap_pos = ReverseNBits(i, bits);

      if (swap_pos <= i)
      {
        continue;
      }

      std::swap(to_transform[i], to_transform[swap_pos]);
    }

    for (int n = 2; n <= size; n <<= 1)
    {
      for (int i = 0; i < size; i += n)
      {
        for (int k = 0; k < n / 2; ++k)
        {
          const int index_even = i + k;
          const int index_odd = i + k + (n / 2);
          Complex even{ to_transform[index_even].real , to_transform[index_even].imag };
          Complex odd{ to_transform[index_odd].real, to_transform[index_odd].imag };

          const double term = -2 * M_PI * k / n;
          Complex exp{ static_cast<float>(std::cos(term)), static_cast<float>(std::sin(term)) };
          exp = Complex::ComplexMul(exp, odd);

          to_transform[index_even] = Complex::ComplexAdd(even, exp);
          to_transform[index_odd] = Complex::ComplexSub(even, exp);
        }
      }
    }
  }
}

#undef _USE_MATH_DEFINES