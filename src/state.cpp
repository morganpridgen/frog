#include "state.h"
#include <cstring>
#include <cmath>
#include <TEXEL/texel.h>

bool PlayState::init() {
  if (!frog.init()) return 0;
  if (!lvl.init("test", frog)) return 0;
  cX = 0.0f, cY = 0.0f;
  return 1;
}

GameState *PlayState::update(TXL_Controller *ctrls[4]) {
  frog.update(ctrls[0], lvl);
  lvl.update();
  
  frog.modCam(cX, cY, lvl);
  return nullptr;
}

void PlayState::render() {
  for (int i = 0; i < 45; i++) {
    TXL_RenderQuad({0.0f, 8.0f * i, 640.0f, 8.0f}, {0.0f, (90 - i) / 90.0f, (90 - i) / 90.0f, 1.0f});
  }
  lvl.render(cX, cY);
  frog.render(cX, cY);
}

void PlayState::end() {
  lvl.end();
  frog.end();
}