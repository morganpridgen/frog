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
  
  frog.modCam(cX, cY);
  lvl.modCam(cX, cY);
  return nullptr;
}

void PlayState::render() {
  lvl.render(cX, cY);
  frog.render(cX, cY);
}

void PlayState::end() {
  lvl.end();
  frog.end();
}