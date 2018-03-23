#pragma once



#define PARTICLE_LIFE_DEFAULT 100

template <typename T> class Particle
{
public:
  Particle(const T &d, float x = 0, float y = 0, float vx = 0, float vy = 0, int life = PARTICLE_LIFE_DEFAULT)
    : Data(d)
    , PosX(x)
    , PosY(y)
    , VelX(vx)
    , VelY(vy)
    , Life(life)
    , Active(true)
  {  }

  T Data;
  float PosX;
  float PosY;
  float VelX;
  float VelY;
  int Life;
  bool Active;
};


