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
    TXL_Texture frogTex, toungeTex;
    bool dir;
    FrogInfo info;
    float mX, mY, lCX, lCY;
    float tR;
    int tSegs, tTSegs;
    bool tState;
    void motionCalc();
    void colCalc(Level&);
    bool isInFloor(float, float, Level&);
  public:
    bool init();
    void update(TXL_Controller*, Level&);
    void render(float, float);
    void end();
    
    void getPos(float &x, float &y) {x = info.x, y = info.y;}
    void setPos(float x, float y) {info.x = x, info.y = y;}
    void modCam(float&, float&, Level&);
};

#endif