#include "fly.h"
#include <TEXEL/texel.h>

Fly *flies = nullptr;
int numFlies = 0;

void setFlyCount(int nFlies) {
  numFlies = nFlies;
  flies = new Fly[numFlies];
  for (int i = 0; i < numFlies; i++) flies[i] = {-1.0f, -1.0f};
};

void addFly(Fly newFly) {
  if (!flies) return;
  for (int i = 0; i < numFlies; i++) {
    if (flies[i].x >= 0.0f) continue;
    flies[i] = newFly;
    break;
  }
}

void updateFlies() {
  for (int i = 0; i < numFlies; i++) {
    if (flies[i].x < 0.0f) continue;
  }
}

void renderFlies(float cX, float cY) {
  for (int i = 0; i < numFlies; i++) {
    if (flies[i].x < 0.0f) continue;
    TXL_RenderQuad(flies[i].x - cX, flies[i].y - cY, 8, 8, {0.0f, 0.0f, 0.0f, 1.0f});
  }
}

void endFlies() {
  numFlies = 0;
  delete [] flies;
  flies = nullptr;
}