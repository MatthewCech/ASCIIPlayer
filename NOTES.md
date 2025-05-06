# Notes
This doc contains a variety of helpful dev links and notes

## FMOD documentation
Link: https://www.fmod.com/docs/2.02/
This includes detailed technical information about fmod and its usage, along with some code examples. The most verbose sample code seems to be in C# for unity, but naming is similar enough that it can be translated.

## Fast Fourier Transforms and Windowing Concepts
Link: https://download.ni.com/evaluation/pxi/Understanding%20FFTs%20and%20Windowing.pdf
Excellent document explaining FFTs and windowing methods by national instruments. Good starting point for knowing what to look for when implementing waveform analysis. 

Link: https://www.youtube.com/watch?v=nmgFG7PUHfo
Nice video discussing FFTs and their development, along with a great series of graphics explaining how data can be re-used.

## FFT Implementation
Link: https://en.wikipedia.org/wiki/Cooley%E2%80%93Tukey_FFT_algorithm
Link: https://www.phys.uconn.edu/~rozman/Courses/m3511_19s/downloads/radix2fft.pdf
Various techniques for implementing fast fourier transforms. Dense, but can be consumed eventually.

## Bit Manupulation
Link: http://graphics.stanford.edu/~seander/bithacks.html
A nice resource for bit manipulation with samples.

## Optimization
Link: https://godbolt.org/
A fantastic c++ compiler explorer. Great for exploring output assembly.