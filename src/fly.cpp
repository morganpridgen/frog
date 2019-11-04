#include "fly.h"
#include <cmath>
#include <TEXEL/texel.h>

Fly *flies = nullptr;
int numFlies = 0;
int animTimer = 0;
TXL_Texture flyTex;

bool initFlies() {
  if (!flyTex.load(TXL_DataPath("fly.png"), 16, 8)) return 0;
  return 1;
}

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
  animTimer++;
}

void renderFlies(float cX, float cY) {
  for (int i = 0; i < numFlies; i++) {
    if (flies[i].x < 0.0f) continue;
    //TXL_RenderQuad(flies[i].x - cX + 4.0f * cos(float(animTimer) * 3.14f / 48.0f), flies[i].y - cY, 8, 8, {0.0f, 0.0f, 0.0f, 1.0f});
    flyTex.setClip(8 * ((animTimer / 2) % 2), 8 * ((animTimer / 2) % 2) + 8, 0, 8);
    flyTex.render(flies[i].x - cX + 4.0f * cos(float(animTimer) * 3.14f / 48.0f), flies[i].y - cY + 2.0f * sin(float(animTimer) * 3.14f / 24.0f));
  }
}

void endFlies() {
  flyTex.free();
  numFlies = 0;
  delete [] flies;
  flies = nullptr;
}