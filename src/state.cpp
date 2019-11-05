#include "state.h"
#include <cstring>
#include <cmath>
#include <TEXEL/texel.h>
#include "fly.h"

bool PlayState::init() {
  if (!frog.init()) return 0;
  if (!lvl.init("lvl1", frog)) return 0;
  if (!initFlies()) return 0;
  cX = 0.0f, cY = 0.0f;
  return 1;
}

GameState *PlayState::update(TXL_Controller *ctrls[4]) {
  frog.update(ctrls[0], lvl);
  lvl.update();
  updateFlies();
  
  frog.modCam(cX, cY, lvl);
  return nullptr;
}

void PlayState::render() {
  for (int i = 0; i < 45; i++) {
    TXL_RenderQuad({0.0f, 8.0f * i, 640.0f, 8.0f}, {0.0f, (90 - i) / 90.0f, (90 - i) / 90.0f, 1.0f});
  }
  lvl.render(cX, cY);
  renderFlies(cX, cY);
  frog.render(cX, cY);
  
  char gui[16];
  sprintf(gui, "Flies: %i/%i", liveFlies(), totalFlies());
  TXL_Texture *guiTex = TXL_RenderText(gui, 1.0f, 1.0f, 1.0f);
  guiTex->setColorMod(0.0f, 0.0f, 0.0f);
  for (int i = 0; i < 9; i++) guiTex->render(16.0f + guiTex->width() / 2.0f - ((i % 3) - 1), 344.0f - guiTex->height() / 2.0f - ((i / 3) - 1));
  guiTex->setColorMod(1.0f, 1.0f, 1.0f);
  guiTex->render(16.0f + guiTex->width() / 2.0f, 344.0f - guiTex->height() / 2.0f);
  delete guiTex;
}

void PlayState::end() {
  endFlies();
  lvl.end();
  frog.end();
}