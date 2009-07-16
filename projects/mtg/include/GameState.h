#ifndef _GAME_STATE_H_
#define _GAME_STATE_H_

#define FADING_SPEED		350.0f

class JGE;

#include <JSoundSystem.h>
#include <string>
using namespace std;

enum ENUM_GAME_STATE
  {
    GAME_STATE_MENU = 0x01,
    GAME_STATE_DUEL = 0x02,
    GAME_STATE_DECK_VIEWER = 0x03,
    GAME_STATE_SHOP = 0x04,
    GAME_STATE_OPTIONS = 0x05,
  };


class GameApp;
class SimpleMenu;
class Player;

class GameState
{
 protected:
  GameApp* mParent;
  JGE* mEngine;

 public:

  static const char * const menuTexts[];
  GameState(GameApp* parent);
  virtual ~GameState() {}

  virtual void Create() {}
  virtual void Destroy() {}

  virtual void Start() {}
  virtual void End() {}

  virtual void Update(float dt) = 0;
  virtual void Render() = 0;
  static int fillDeckMenu(SimpleMenu * _menu, string path, string smallDeckPrefix = "", Player * statsPlayer = NULL);
};


#endif

