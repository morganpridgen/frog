#include "level.h"
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <TEXEL/texel.h>
#include "frog.h"
#include "fly.h"

#define tileSize 32.0f

TXL_Texture groundTex;

int nextInt(TXL_File *f) {
  int out = 0;
  char tmp = 0;
  bool readNum = 0;
  while (1) {
    if (!f->read(&tmp, sizeof(tmp))) {
      if (readNum) return out;
      else return -1;
    }
    if (tmp >= '0' && tmp <= '9') readNum = 1;
    else if (readNum) return out;
    if (readNum) {
      out *= 10;
      out += tmp - '0';
    }
  }
}

bool Level::init(const char *name, Frog &frog) {
  TXL_File f;
  char path[256], root[256];
  sprintf(root, "levels/%s", name);
  sprintf(path, "%s/terrain.txt", root);
  if (!f.init(TXL_DataPath(path), 'r')) return 0;
  length = nextInt(&f);
  depth = nextInt(&f);
  terrain = new int[length * depth];
  for (int i = 0; i < length * depth; i++) terrain[i] = nextInt(&f);
  f.close();
  sprintf(path, "%s/flies.txt", root);
  if (!f.init(TXL_DataPath(path), 'r')) return 0;
  int flyCount = nextInt(&f);
  setFlyCount(flyCount);
  for (int i = 0; i < flyCount; i++) {
    int fX = nextInt(&f);
    int fY = nextInt(&f);
    addFly({fX * tileSize + tileSize / 2.0f, 360.0f - (fY * tileSize + tileSize / 2.0f)});
  }
  sprintf(path, "%s/frog.txt", root);
  if (!f.init(TXL_DataPath(path), 'r')) return 0;
  int fX, fY;
  fX = nextInt(&f);
  fY = nextInt(&f);
  frog.setPos(fX * tileSize + tileSize / 2.0f, 360.0f - (fY * tileSize));
  
  if (!groundTex.load(TXL_DataPath((depth % 2) ? "terrain/ground.png" : "terrain/stone.png"), 96, 32)) return 0;
  return 1;
}

void Level::update() {
  cloudScroll = (cloudScroll + 1) % 512;
}

void Level::render(float cX, float cY) {
  if (depth % 2) {
    for (int i = 0; i < 45; i++) {
      TXL_RenderQuad({0.0f, 8.0f * i, 640.0f, 8.0f}, {fmin(-cY / 1280.0f, 0.5f), (90 - i) / 90.0f, (90 - i) / 90.0f, 1.0f});
    }
    
    for (int i = cX / 256.0f; i < (cX + 2560.0f) / 256.0f + 2; i++) {
      for (int j = 0; j < (270.0f - cY) / 128.0f + 1; j++) {
        TXL_RenderQuad(i * 64.0f + (float(j % 2) * 32.0f) - (float(cloudScroll) / 8.0f) - cX / 4.0f, j * -32.0f + 90.0f - cY / 4.0f, 16, 8, {1.0f, 1.0f, 1.0f, 0.5f});
      }
    }
    
    groundTex.setColorMod(0.75f, 0.75f, 0.75f);
    for (int i = cX / tileSize; i < (cX + 1280.0f) / tileSize + 1; i++) {
      for (int j = 0; j < 10 + (i / 4) % 2; j++) {
        groundTex.setClip(32 * (j == 9 + (i / 4) % 2), 32 * (j == 9 + (i / 4) % 2) + 32, 0, 32);
        groundTex.render(i * (tileSize / 2.0f) - cX / 2.0f, 360.0f - (j * tileSize / 2.0f) - cY / 2.0f, 0.5f, 0.5f);
      }
    }
  } else {
    groundTex.setClip(0, 32, 0, 32);
    
    groundTex.setColorMod(0.25f, 0.25f, 0.25f);
    for (int i = cX / tileSize; i < (cX + 2560.0f) / tileSize + 1; i++) {
      for (int j = cY / tileSize; j < (1440.0f - cY) / tileSize + 1; j++) {
        groundTex.render(i * (tileSize / 4.0f) - cX / 4.0f, 360.0f - (j * tileSize / 4.0f) - cY / 4.0f, 0.25f, 0.25f);
      }
    }
    
    groundTex.setColorMod(0.5f, 0.5f, 0.5f);
    for (int i = cX / tileSize; i < (cX + 1280.0f) / tileSize + 1; i++) {
      for (int j = cY / tileSize; j < (720.0f - cY) / tileSize + 1; j++) {
        if ((i / 2) % 2 == 0 || (j / 2) % 2 == 0) groundTex.render(i * (tileSize / 2.0f) - cX / 2.0f, 360.0f - (j * tileSize / 2.0f) - cY / 2.0f, 0.5f, 0.5f);
      }
    }
  }
  
  groundTex.setColorMod(1.0f, 1.0f, 1.0f);
  for (int i = cX / tileSize; i < (cX + 640.0f) / tileSize; i++) {
    bool terrainRender = 1;
    int height = 0;
    for (int j = 0; j < depth; j++) {
      terrainRender = !terrainRender;
      if (terrainRender) {
        height += terrain[i * depth + j];
        continue;
      }
      groundTex.setClip(0, 32, 0, 32);
      for (int k = 0; k < terrain[i * depth + j]; k++) {
        if (terrain[i * depth + j] - k == 1) {
          if (((i + height + k) & 0b1001) == 0) {
            int fId = ((i + height + k) >> 1) & 0b11;
            groundTex.setClip(64 + 8 * fId, 72 + 8 * fId, 0, 16);
            groundTex.render(i * tileSize + 16 - cX, 360.0f - ((k + height + 1) * tileSize) - 8 - cY);
          }
          groundTex.setClip(32, 64, 0, 32);
        }
        groundTex.render(i * tileSize + 16 - cX, 360.0f - ((k + height + 1) * tileSize) + 16 - cY);
      }
      height += terrain[i * depth + j];
    }
    if (depth % 2 == 0) {
      groundTex.setClip(0, 32, 0, 32);
      while ((height * tileSize) + cY < 360) { // height * tileSize - cY < 360
        groundTex.render(i * tileSize + 16 - cX, 360.0f - ((height + 1) * tileSize) + 16 - cY);
        height++;
      }
    }
  }
}

void Level::end() {
  groundTex.free();
  delete [] terrain;
  terrain = nullptr;
}

void Level::modCam(float &cX, float &cY, float pX, float pY) {
  int wPX = pX / tileSize;
  int wPY = (360.0f - pY) / tileSize;
  if (cX < 0.0f) cX = 0.0f;
  if (cX > (length - 20) * tileSize) cX = (length - 20) * tileSize;
  
  int wPH = 0;
  for (int i = 0; i < depth; i++) {
    if (wPH + terrain[wPX * depth + i] > wPY) break;
    wPH += terrain[wPX * depth + i];
  }
  if (wPY - wPH < 8) {
    float camTarget = 90.0f - wPH * tileSize;
    cY += (camTarget - cY) / 8.0f;
    if (cY > 0.0f) cY = 0.0f;
  }
}

bool Level::inFloor(float x, float y) {
  int pX = x / tileSize;
  int pY = (360.0f - y) / tileSize + 1;
  int h = 0, lH = 0;
  bool isSolid = 1;
  
  if (pX < 0 || pX >= length || pY < 0) return 1;
  
  for (int i = 0; i < depth; i++) {
    h += terrain[pX * depth + i];
    if (pY <= h && pY >= lH) return isSolid;
    lH = h;
    isSolid = !isSolid;
  }
  return isSolid;
}