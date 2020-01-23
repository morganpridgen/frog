#include "state.h"
#include <cstring>
#include <cmath>
#include <TEXEL/texel.h>
#include "fly.h"

char level[64];

bool PlayState::init() {
  if (!frog.init()) return 0;
  if (!lvl.init(level, frog)) return 0;
  if (!initFlies()) return 0;
  frog.getPos(cX, cY);
  cX -= 320.0f;
  cY -= 180.0f;
  endTimer = 0;
  return 1;
}

GameState *PlayState::update(TXL_Controller *ctrls[4]) {
  frog.update(ctrls[0], lvl);
  lvl.update();
  updateFlies();
  if (!liveFlies()) endTimer++;
  if (endTimer >= 150) return new LevelSelectState;
  
  frog.modCam(cX, cY, lvl);
  return nullptr;
}

void PlayState::render() {
  lvl.render(cX, cY);
  renderFlies(cX, cY);
  frog.render(cX, cY);
  
  char gui[16];
  sprintf(gui, "Flies: %i/%i", totalFlies() - liveFlies(), totalFlies());
  TXL_Texture guiTex;
  TXL_RenderText(&guiTex, gui, 1.0f, 1.0f, 1.0f);
  guiTex.setColorMod(0.625f, 0.625f, 0.625f);
  for (int i = 0; i < 9; i++) guiTex.render(16.0f + guiTex.width() / 2.0f - ((i % 3) - 1), 344.0f - guiTex.height() / 2.0f - ((i / 3) - 1));
  guiTex.setColorMod(1.0f, 1.0f, 1.0f);
  guiTex.render(16.0f + guiTex.width() / 2.0f, 344.0f - guiTex.height() / 2.0f);
  guiTex.free();
  
  if (endTimer > 60) TXL_RenderQuad(320, 180, 640, 360, {1.0f, 1.0f, 1.0f, fmin(endTimer - 60, 60) / 60.0f});
}

void PlayState::end() {
  endFlies();
  lvl.end();
  frog.end();
}



bool LevelSelectState::init() {
  char tmpLevels[64][64];
  TXL_File f;
  selectedLevel = 0, lvlCount = 0;
  if (!f.init(TXL_DataPath("levels/levellist.txt"), 'r')) return 0;
  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 64; j++) {
      tmpLevels[i][j] = 0;
      char tmp;
      if (!f.read(&tmp, sizeof(tmp)) || tmp < ' ') break;
      tmpLevels[i][j] = tmp;
    }
    if (tmpLevels[i][0] == 0) break;
    lvlCount++;
  }
  levelList = new char*[lvlCount];
  for (int i = 0; i < lvlCount; i++) {
    levelList[i] = new char[strlen(tmpLevels[i]) + 1];
    strcpy(levelList[i], tmpLevels[i]);
  }
  
  lBX = 160.0f, rBX = 480.0f;
  
  if (!frogTex.load(TXL_DataPath("frog.png"), 64, 16)) return 0;
  frogTex.setClip(0, 16, 0, 16);
  if (!groundTex.load(TXL_DataPath("terrain/ground.png"), 96, 32)) return 0;
  if (!arrowTex.load(TXL_DataPath("menuarrow.png"), 16, 16)) return 0;
  return 1;
}

GameState *LevelSelectState::update(TXL_Controller *ctrls[4]) {
  if (ctrls[0]->leftJoyX() < -0.5f && lJX >= -0.5f) selectedLevel--;
  if (ctrls[0]->leftJoyX() > 0.5f && lJX <= 0.5f) selectedLevel++;
  lJX = ctrls[0]->leftJoyX();
  mX = ctrls[0]->mouseX();
  mY = ctrls[0]->mouseY();
  mOnLB = fabs(lBX - mX) < 8.0f && fabs(180.0f - mY) < 8.0f;
  mOnRB = fabs(rBX - mX) < 8.0f && fabs(180.0f - mY) < 8.0f;
  
  if (mOnLB && ctrls[0]->buttonRelease(CtrlM)) selectedLevel--;
  if (mOnRB && ctrls[0]->buttonRelease(CtrlM)) selectedLevel++;
  
  while (selectedLevel < 0) selectedLevel += lvlCount;
  while (selectedLevel >= lvlCount) selectedLevel -= lvlCount;
  if (ctrls[0]->buttonRelease(CtrlA)) {
    strcpy(level, levelList[selectedLevel]);
    return new PlayState;
  }
  
  return nullptr;
}

void LevelSelectState::render() {
  for (int i = 0; i < 45; i++) {
    TXL_RenderQuad({0.0f, 8.0f * i, 640.0f, 8.0f}, {0.0f, (90 - i) / 90.0f, (90 - i) / 90.0f, 1.0f});
  }
  for (int i = 0; i < 20; i++) {
    groundTex.setClip(0, 32, 0, 32);
    groundTex.render(i * 32 + 16, 344);
    groundTex.setClip(32, 64, 0, 32);
    groundTex.render(i * 32 + 16, 312);
  }
  frogTex.render(48, 288);
  
  TXL_Texture levelTex;
  TXL_RenderText(&levelTex, levelList[selectedLevel], 1.0f, 1.0f, 1.0f);
  levelTex.setColorMod(0.625f, 0.625f, 0.625f);
  for (int i = 0; i < 9; i++) levelTex.render(321.0f - (i % 3), 181.0f - (i / 3));
  levelTex.setColorMod(1.0f, 1.0f, 1.0f);
  levelTex.render(320.0f, 180.0f);
  
  lBX = fmin(320.0f - 16.0f - float(levelTex.width()) / 2.0f, 160.0f), rBX = fmax(320.0f + 16.0f + float(levelTex.width()) / 2.0f, 480.0f);
  arrowTex.render(lBX, 180.0f, 1.0f + 0.25f * float(mOnLB), 1.0f + 0.25f * float(mOnLB));
  arrowTex.render(rBX, 180.0f, 1.0f + 0.25f * float(mOnRB), 1.0f + 0.25f * float(mOnRB), 180.0f);
  levelTex.free();
  
  TXL_RenderQuad(mX, mY, 18, 4, {0.625f, 0.625f, 0.625f, 1.0f});
  TXL_RenderQuad(mX, mY, 4, 18, {0.625f, 0.625f, 0.625f, 1.0f});
  TXL_RenderQuad(mX, mY, 16, 2, {1.0f, 1.0f, 1.0f, 1.0f});
  TXL_RenderQuad(mX, mY, 2, 16, {1.0f, 1.0f, 1.0f, 1.0f});
}

void LevelSelectState::end() {
  for (int i = 0; i < lvlCount; i++) delete levelList[i];
  delete levelList;
  levelList = nullptr;
  frogTex.free();
  groundTex.free();
  arrowTex.free();
}