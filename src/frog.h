#ifndef frogh
#define frogh

#include <TEXEL/texel.h>

class Frog;

#include "level.h"

struct FrogInfo {
  float x, y, xV, yV;
  bool grounded;
};

class Frog {
  private:
    FrogInfo info;
    float mX, mY, lCX, lCY;
    void motionCalc();
    void colCalc(Level&);
  public:
    bool init();
    void update(TXL_Controller*, Level&);
    void render(float, float);
    void end();
    
    void modCam(float&, float&);
};

#endif