#ifndef levelh
#define levelh

class Level;

#include "frog.h"

class Level {
  private:
    int *terrain;
    int length, depth;
  public:
    bool init(const char*, Frog&);
    void update();
    void render(float, float);
    void end();
    
    void modCam(float&, float&, float, float);
    bool inFloor(float, float);
};

#endif