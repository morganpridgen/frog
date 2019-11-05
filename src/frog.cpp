#include "frog.h"
#include <cmath>
#include "fly.h"

bool Frog::init() {
  if (!frogTex.load(TXL_DataPath("frog.png"), 32, 16)) return 0;
  if (!toungeTex.load(TXL_DataPath("tounge.png"), 32, 16)) return 0;
  dir = 0;
  info.x = 320.0f, info.y = 180.0f, info.xV = 0.0f, info.yV = 0.0f;
  info.grounded = 0;
  mX = 0.0f, mY = 0.0f, lCX = 0.0f, lCY = 0.0f;
  tR = 0.0f;
  tSegs = 0, tTSegs = 0;
  tState = 0;
  return 1;
}

void Frog::update(TXL_Controller *ctrl, Level &lvl) {
  mX = ctrl->mouseX(), mY = ctrl->mouseY();
  
  if (ctrl->buttonClick(CtrlM) && info.grounded && !tSegs) {
    float wMX = mX + lCX, wMY = mY + lCY;
    int tFly = flyAt(wMX, wMY);
    if (tFly != -1) { // do tounge
      tR = atan2(wMY - info.y, wMX - info.x);
      tState = 1;
      tTSegs = fmin(sqrt((wMX - info.x) * (wMX - info.x) + (wMY - info.y) * (wMY - info.y)) / 16.0f, 16.0f);
      dir = wMX < info.x;
    } else { // do jump
      info.grounded = 0;
      float d, r;
      float dX = wMX - info.x, dY = wMY - info.y;
      d = fmin(sqrt(dX * dX + dY * dY), 256.0f);
      r = atan2(dY, dX);
      info.xV = (d * cos(r)) / 8.0f;
      info.yV = (d * sin(r)) / 8.0f;
      ctrl->rumble(d / 512.0f + 0.25f, 250);
      dir = info.xV < 0;
      TXL_Square hop = {1760, 1.0f, 16.0f, 0};
      TXL_PlaySound(hop);
      hop = {3520, 0.5f, 4.0f, 1};
      TXL_PlaySound(hop);
    }
  }
  
  if (tState) {
    tSegs++;
    if (lvl.inFloor(info.x + 16.0f * tSegs * cos(tR), info.y + 16.0f * tSegs * sin(tR))) tState = 0;
    if (tTSegs == tSegs) tState = 0;
    int tFly = flyAt(info.x + 16.0f * tSegs * cos(tR), info.y + 16.0f * tSegs * sin(tR));
    if (tFly != -1) {
      removeFly(tFly);
      tState = 0;
    }
  } else if (tSegs > 0) tSegs--;
  
  motionCalc();
  for (int i = 0; i < 4; i++) {
    info.x += info.xV / 4.0f;
    info.y += info.yV / 4.0f;
    colCalc(lvl);
  }
}

void Frog::motionCalc() {
  if (!info.grounded) {
    //if (info.yV < 16.0f) info.yV += 1.0f;
    info.yV += 1.0f;
    if (info.xV * info.xV + info.yV * info.yV > 576.0f) {
      float r = atan2(info.yV, info.xV);
      info.xV = 24.0f * cos(r);
      info.yV = 24.0f * sin(r);
    }
    if (info.xV > 0.0f) info.xV -= 0.5f;
    if (info.xV < 0.0f) info.xV += 0.5f;
    if (fabs(info.xV) < 1.0f) info.xV = 0.0f;
  }
}

void Frog::colCalc(Level &lvl) {
  if (lvl.inFloor(info.x, info.y)) {
    bool floor = 0;
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
        floor = 1;
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
    if (floor) {
      TXL_Triangle land = {220, 0.5f, 2.0f};
      TXL_PlaySound(land);
    }
    TXL_Noise bump = {1, 4.0f, 0, 12, 0};
    TXL_PlaySound(bump);
  }
}

void Frog::render(float cX, float cY) {
  lCX = cX, lCY = cY;
  
  if (tSegs > 0) {
    toungeTex.setClip(0, 16, 0, 16);
    for (int i = 0; i < tSegs; i++) {
      if (tSegs - i == 1) toungeTex.setClip(16, 32, 0, 16);
      toungeTex.render(info.x + (16.0f * (float(i) + 0.5f) * cos(tR)) + (dir ? -3 : 3) - cX, info.y + (16.0f * (float(i) + 0.5f) * sin(tR)) - 10 - cY, 1.0f, 1.0f, tR * (180.0f / 3.14f));
    }
  }
  
  int xOff = 0, yOff = 0;
  if (!info.grounded) xOff = 16;
  if (dir) frogTex.setClip(xOff + 16, xOff, yOff, yOff + 16);
  else frogTex.setClip(xOff, xOff + 16, yOff, yOff + 16);
  frogTex.render(info.x - cX, info.y - 7 - cY);
  
  TXL_RenderQuad(mX, mY, 16, 2, {1.0f, 1.0f, 1.0f, 1.0f});
  TXL_RenderQuad(mX, mY, 2, 16, {1.0f, 1.0f, 1.0f, 1.0f});
}

void Frog::end() {
  toungeTex.free();
  frogTex.free();
}

void Frog::modCam(float &cX, float &cY, Level &lvl) {
  cX += (info.x - cX - 320.0f) / 8.0f;
  lvl.modCam(cX, cY, info.x, info.y);
}