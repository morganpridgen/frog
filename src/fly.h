#ifndef flyh
#define flyh

struct Fly {
  float x, y;
};

bool initFlies();
void setFlyCount(int);
void addFly(Fly);
int flyAt(float, float, float);
void removeFly(int);
int liveFlies();
int totalFlies();
float nearFlyAngle(float, float);
void updateFlies();
void renderFlies(float, float);
void endFlies();

#endif