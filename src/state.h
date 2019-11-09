#ifndef stateh
#define stateh
#include <TEXEL/texel.h>
#include "frog.h"
#include "level.h"

class GameState {
  public:
    virtual bool init() = 0;
    virtual GameState *update(TXL_Controller*[4]) = 0;
    virtual void render() = 0;
    virtual void end() = 0;
};

class PlayState : public GameState {
  private:
    Frog frog;
    Level lvl;
    float cX, cY;
    int endTimer;
  public:
    virtual bool init();
    virtual GameState *update(TXL_Controller*[4]);
    virtual void render();
    virtual void end();
};

class LevelSelectState : public GameState {
  private:
    char **levelList;
    int selectedLevel, lvlCount;
    float lJX;
    TXL_Texture frogTex, groundTex;
  public:
    virtual bool init();
    virtual GameState *update(TXL_Controller*[4]);
    virtual void render();
    virtual void end();
};

class InitState : public GameState {
  public:
    virtual bool init() {return 1;}
    virtual GameState *update(TXL_Controller *ctrls[4]) {return new LevelSelectState;}
    virtual void render() {}
    virtual void end() {}
};

#endif