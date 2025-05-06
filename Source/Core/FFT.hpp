#pragma once
namespace ASCIIPlayer
{
  struct Complex;

  int ReverseNBits(int, int);
  void FFTInPlace(Complex*, int);
  void FFT(const float*, float*, int);
}
