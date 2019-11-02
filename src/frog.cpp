#include "frog.h"
#include <cmath>

bool Frog::init() {
  if (!frogTex.load(TXL_DataPath("frog.png"), 32, 16)) return 0;
  dir = 0;
  info.x = 320.0f, info.y = 180.0f, info.xV = 0.0f, info.yV = 0.0f;
  info.grounded = 0;
  mX = 0.0f, mY = 0.0f, lCX = 0.0f, lCY = 0.0f;
  return 1;
}

void Frog::update(TXL_Controller *ctrl, Level &lvl) {
  mX = ctrl->mouseX(), mY = ctrl->mouseY();
  
  if (ctrl->buttonClick(CtrlM) && info.grounded) {
    info.grounded = 0;
    float d, r;
    float dX = mX + lCX - info.x, dY = mY + lCY - info.y;
    d = fmin(sqrt(dX * dX + dY * dY), 256.0f);
    r = atan2(dY, dX);
    info.xV = (d * cos(r)) / 8.0f;
    info.yV = (d * sin(r)) / 8.0f;
    ctrl->rumble(d / 512.0f + 0.25f, 250);
    dir = info.xV < 0;
  }
  
  motionCalc();
  for (int i = 0; i < 4; i++) {
    info.x += info.xV / 4.0f;
    info.y += info.yV / 4.0f;
    colCalc(lvl);
  }
}

void Frog::motionCalc() {
  if (!info.grounded) {
    info.yV += 1.0f;
    if (info.xV > 0.0f) info.xV -= 0.5f;
    if (info.xV < 0.0f) info.xV += 0.5f;
    if (fabs(info.xV) < 1.0f) info.xV = 0.0f;
  }
}

void Frog::colCalc(Level &lvl) {
  if (lvl.inFloor(info.x, info.y)) {
    for (int i = 0; i < 128; i++) {
      if (!lvl.inFloor(info.x, info.y - i)) { // floor check
        info.y -= i + 1;
        info.yV = round(info.yV * -0.5f);
        info.xV = round(info.xV * 0.5f);
        if (info.yV > -3.0f) {
          info.xV = 0.0f;
          info.yV = 0.0f;
          info.grounded = 1;
        }
        break;
      }
      if (!lvl.inFloor(info.x - i, info.y)) { // left wall check
        info.x -= i + 1;
        info.xV = round(info.xV * -0.75f);
        break;
      }
      if (!lvl.inFloor(info.x + i, info.y)) { // right wall check
        info.x += i + 1;
        info.xV = round(info.xV * -0.75f);
        break;
      }
      if (!lvl.inFloor(info.x, info.y + i)) { // ceiling check
        info.y += i + 1;
        info.yV = round(info.yV * -0.5f);
        info.xV = round(info.xV * 0.5f);
        break;
      }
    }
  }
}

void Frog::render(float cX, float cY) {
  lCX = cX, lCY = cY;
  
  int xOff = 0, yOff = 0;
  if (!info.grounded) xOff = 16;
  if (dir) frogTex.setClip(xOff + 16, xOff, yOff, yOff + 16);
  else frogTex.setClip(xOff, xOff + 16, yOff, yOff + 16);
  frogTex.render(info.x - cX, info.y - 7 - cY);
  
  TXL_RenderQuad(mX, mY, 16, 2, {1.0f, 1.0f, 1.0f, 1.0f});
  TXL_RenderQuad(mX, mY, 2, 16, {1.0f, 1.0f, 1.0f, 1.0f});
}

void Frog::end() {
  frogTex.free();
}

void Frog::modCam(float &cX, float &cY) {
  cX += (info.x - cX - 320.0f) / 8.0f;
}