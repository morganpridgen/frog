#ifndef levelh
#define levelh

class Level;

#include "frog.h"

struct TileSpan {
  char type;
  int len;
};

class Level {
  private:
    TileSpan *terrain;
    int length, depth;
    bool solidTop;
    bool inTile(float, float, char);
    char typeAt(int, int);
    int cloudScroll;
  public:
    bool init(const char*, Frog&);
    void update();
    void render(float, float);
    void end();
    
    void modCam(float&, float&, float, float);
    bool inFloor(float, float);
};

#endif