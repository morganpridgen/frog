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
  for (int i = 0; i < length; i++) {
    bool terrainRender = 1;
    int height = 0;
    for (int j = 0; j < depth; j++) {
      terrainRender = !terrainRender;
      if (terrainRender) {
        height += terrain[i * depth + j];
        continue;
      }
      TXL_RenderQuad({i * tileSize - cX, 360.0f - ((terrain[i * depth + j] - height) * tileSize) - cY, tileSize, (terrain[i * depth + j] - height) * tileSize}, {0.0f, 1.0f, 0.0f, 1.0f});
      height += terrain[i * depth + j];
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
  for (int i = 0; i < 21; i++) wCamTarget += terrain[(i + scanStart) * depth];
  float camTarget = (wCamTarget / 21) * tileSize;
  cY += (wCamTarget - cY) / 8.0f;
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
  int t = x / tileSize;
  int pY = y / tileSize;
  int h = 0, lH = 0;
  bool isSolid = 1;
  for (int i = 0; i < depth; i++) {
    h += terrain[t * depth + i];
    if (pY <= h && pY >= lH) return isSolid;
    lH = h;
    isSolid = !isSolid;
  }
}