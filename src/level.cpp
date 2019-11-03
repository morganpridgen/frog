#include "level.h"
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <TEXEL/texel.h>
#include "frog.h"

#define tileSize 32.0f

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
  char path[256];
  sprintf(path, "levels/%s/terrain.txt", name);
  if (!f.init(TXL_DataPath(path), 'r')) return 0;
  length = nextInt(&f);
  depth = nextInt(&f);
  terrain = new int[length * depth];
  for (int i = 0; i < length * depth; i++) terrain[i] = nextInt(&f);
  f.close();
  return 1;
}

void Level::update() {

}

void Level::render(float cX, float cY) {
  for (int i = cX / tileSize; i < (cX + 640.0f) / tileSize; i++) {
    bool terrainRender = 1;
    int height = 0;
    for (int j = 0; j < depth; j++) {
      terrainRender = !terrainRender;
      if (terrainRender) {
        height += terrain[i * depth + j];
        continue;
      }
      //TXL_RenderQuad({i * tileSize - cX, 360.0f - ((terrain[i * depth + j] - height) * tileSize) - cY, tileSize, (terrain[i * depth + j] - height) * tileSize}, {0.0f, 1.0f, 0.0f, 1.0f});
      for (int k = 0; k < terrain[i * depth + j]; k++) {
        if (terrain[i * depth + j] - k == 1) TXL_RenderQuad({i * tileSize - cX, 360.0f - ((k - height + 1) * tileSize) - cY, tileSize, tileSize}, {0.0f, 1.0f, 0.0f, 1.0f});
        else TXL_RenderQuad({i * tileSize - cX, 360.0f - ((k - height + 1) * tileSize) - cY, tileSize, tileSize}, {0.5f, 0.25f, 0.0f, 1.0f});
      }
      height += terrain[i * depth + j];
    }
    if (depth % 2 == 0) {
      //TXL_RenderQuad({i * tileSize - cX, 0, tileSize, 360.0f - height * tileSize - cY}, {0.0f, 1.0f, 0.0f, 1.0f});
      while (height * tileSize - cX < 360) {
        TXL_RenderQuad({i * tileSize - cX, 360.0f - ((height + 1) * tileSize) - cY, tileSize, tileSize}, {0.5f, 0.25f, 0.0f, 1.0f});
        height++;
      }
    }
  }
}

void Level::end() {
  delete [] terrain;
  terrain = nullptr;
}

void Level::modCam(float &cX, float &cY) {
  if (cX < 0.0f) cX = 0.0f;
  if (cX > (length - 20) * tileSize) cX = (length - 20) * tileSize;
  
  int scanStart = cX / tileSize;
  int wCamTarget = 0;
  for (int i = 0; i < 20; i++) wCamTarget += terrain[(i + scanStart) * depth];
  float camTarget = 90.0f - (wCamTarget / 20) * tileSize;
  cY += (camTarget - cY) / 8.0f;
  if (cY > 0.0f) cY = 0.0f;
}

float Level::getBelowHeight(float x, float y) {
  int t = x / tileSize;
  int h = 0;
  bool isSolid = 1;
  for (int i = 0; i < depth; i++) {
    if (isSolid) {
      if (terrain[t * depth + i] > y) break;
      h = terrain[t * depth + i];
    }
    isSolid = !isSolid;
  }
  return 360.0f - (h * tileSize);
}

float Level::getAboveHeight(float x, float y) {
  int t = x / tileSize;
  int h = 0;
  bool isSolid = 1;
  for (int i = 0; i < depth; i++) {
    if (isSolid) {
      h = terrain[t * depth + i];
      if (h > y) break;
    }
    isSolid = !isSolid;
  }
  return 360.0f - (h * tileSize);
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