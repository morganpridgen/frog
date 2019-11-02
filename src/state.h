#ifndef stateh
#define stateh
#include <TEXEL/texel.h>

class GameState {
  public:
    virtual bool init() = 0;
    virtual GameState *update(TXL_Controller*[4]) = 0;
    virtual void render() = 0;
    virtual void end() = 0;
};

class PlayState : public GameState {
  public:
    virtual bool init();
    virtual GameState *update(TXL_Controller*[4]);
    virtual void render();
    virtual void end();
};

class InitState : public GameState {
  public:
    virtual bool init() {return 1;}
    virtual GameState *update(TXL_Controller *ctrls[4]) {return new PlayState;}
    virtual void render() {}
    virtual void end() {}
};

#endif