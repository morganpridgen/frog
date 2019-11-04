#ifndef flyh
#define flyh

struct Fly {
  float x, y;
};

void setFlyCount(int);
void addFly(Fly);
void updateFlies();
void renderFlies(float, float);
void endFlies();

#endif