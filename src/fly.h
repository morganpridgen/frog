#ifndef flyh
#define flyh

struct Fly {
  float x, y;
};

bool initFlies();
void setFlyCount(int);
void addFly(Fly);
void updateFlies();
void renderFlies(float, float);
void endFlies();

#endif