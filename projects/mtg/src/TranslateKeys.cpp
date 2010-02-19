#include <map>
#include "../include/Translate.h"
#include "../include/WResourceManager.h"
#include "../include/TranslateKeys.h"

using std::string;
using std::map;

static map<const LocalKeySym, KeyRep const*> fattable;
static map<const JButton, KeyRep const*> slimtable;

KeyRep::KeyRep(JQuad* icon, string text) : icon(icon), text(text) {}

#ifdef LINUX
const KeyRep& translateKey(LocalKeySym key) {
  {
    map<const LocalKeySym, KeyRep const*>::iterator res;
    if ((res = fattable.find(key)) != fattable.end())
      return *(res->second);
  }

 char* str = XKeysymToString(key);
  if (!str)
    {
      str = new char[11];
      sprintf(str, "%lu", key);
    }
  KeyRep* k = new KeyRep(NULL, str);
  fattable[key] = k;
  return *k;
}
#else
#ifdef WIN32
const KeyRep& translateKey(LocalKeySym key) {
  {
    map<const LocalKeySym, KeyRep const*>::iterator res;
    if ((res = fattable.find(key)) != fattable.end())
      return *(res->second);
  }

  char* str = new char[11];
  sprintf(str, "%d", key);
  KeyRep* k = new KeyRep(NULL, str);
  fattable[key] = k;
  return *k;
}
#else // PSP

const KeyRep& translateKey(LocalKeySym key) {
  map<const LocalKeySym, KeyRep const*>::iterator res;
  if ((res = fattable.find(key)) != fattable.end())
    return *(res->second);

//  if (fattable.end() == fattable.find(PSP_CTRL_SELECT))
//    {
//      fattable[PSP_CTRL_SELECT]   = new KeyRep(resources.RetrieveQuad("iconspsp.png", (float)10*32, 0, 64, 32, "PSP_CTRL_SELECT", RETRIEVE_NORMAL), _("Select"));
//      fattable[PSP_CTRL_START]    = new KeyRep(resources.RetrieveQuad("iconspsp.png", (float)8*32,  0, 64, 32, "PSP_CTRL_START", RETRIEVE_NORMAL), _("Start"));
//      fattable[PSP_CTRL_UP]       = new KeyRep(resources.RetrieveQuad("iconspsp.png", (float)0*32,  0, 32, 32, "PSP_CTRL_UP", RETRIEVE_NORMAL), _("Up"));
//      fattable[PSP_CTRL_RIGHT]    = new KeyRep(resources.RetrieveQuad("iconspsp.png", (float)3*32,  0, 32, 32, "PSP_CTRL_RIGHT", RETRIEVE_NORMAL), _("Right"));
//      fattable[PSP_CTRL_DOWN]     = new KeyRep(resources.RetrieveQuad("iconspsp.png", (float)1*32,  0, 32, 32, "PSP_CTRL_DOWN", RETRIEVE_NORMAL), _("Down"));
//      fattable[PSP_CTRL_LEFT]     = new KeyRep(resources.RetrieveQuad("iconspsp.png", (float)2*32,  0, 32, 32, "PSP_CTRL_LEFT", RETRIEVE_NORMAL), _("Left"));
//      fattable[PSP_CTRL_LTRIGGER] = new KeyRep(resources.RetrieveQuad("iconspsp.png", (float)14*32, 0, 64, 32, "PSP_CTRL_LTRIGGER", RETRIEVE_NORMAL), _("Left trigger"));
//      fattable[PSP_CTRL_RTRIGGER] = new KeyRep(resources.RetrieveQuad("iconspsp.png", (float)16*32, 0, 64, 32, "PSP_CTRL_RTRIGGER", RETRIEVE_NORMAL), _("Right trigger"));
//      fattable[PSP_CTRL_TRIANGLE] = new KeyRep(resources.RetrieveQuad("iconspsp.png", (float)5*32,  0, 32, 32, "PSP_CTRL_TRIANGLE", RETRIEVE_NORMAL), _("Triangle"));
//      fattable[PSP_CTRL_CIRCLE]   = new KeyRep(resources.RetrieveQuad("iconspsp.png", (float)4*32,  0, 32, 32, "PSP_CTRL_CIRCLE", RETRIEVE_NORMAL), _("Circle"));
//      fattable[PSP_CTRL_CROSS]    = new KeyRep(resources.RetrieveQuad("iconspsp.png", (float)7*32,  0, 32, 32, "PSP_CTRL_CROSS", RETRIEVE_NORMAL), _("Cross"));
//      fattable[PSP_CTRL_SQUARE]   = new KeyRep(resources.RetrieveQuad("iconspsp.png", (float)6*32,  0, 32, 32, "PSP_CTRL_SQUARE", RETRIEVE_NORMAL), _("Square"));
//      fattable[PSP_CTRL_HOLD]     = new KeyRep(resources.RetrieveQuad("iconspsp.png", (float)4*32,  0, 32, 32, "PSP_CTRL_HOLD", RETRIEVE_NORMAL), _("Hold"));
//    }
//  if ((res = fattable.find(key)) != fattable.end())
//    return *(res->second);
  char* str = new char[11];
  sprintf(str, "%d", key);
  KeyRep* k = new KeyRep(NULL, str);
  fattable[key] = k;
  return *k;
}
#endif
#endif

const KeyRep& translateKey(JButton key) {
  {
    map<const JButton, KeyRep const*>::iterator res;
    if ((res = slimtable.find(key)) != slimtable.end())
      return *(res->second);
  }

  slimtable[JGE_BTN_NONE] =   new KeyRep(NULL, _("None"));
  slimtable[JGE_BTN_QUIT] =   new KeyRep(NULL, _("Quit"));
  slimtable[JGE_BTN_MENU] =   new KeyRep(NULL, _("Menu"));
  slimtable[JGE_BTN_CTRL] =   new KeyRep(NULL, _("Control"));
  slimtable[JGE_BTN_POWER] =  new KeyRep(NULL, _("Power"));
  slimtable[JGE_BTN_SOUND] =  new KeyRep(NULL, _("Sound"));
  slimtable[JGE_BTN_RIGHT] =  new KeyRep(NULL, _("Right"));
  slimtable[JGE_BTN_LEFT] =   new KeyRep(NULL, _("Left"));
  slimtable[JGE_BTN_UP] =     new KeyRep(NULL, _("Up"));
  slimtable[JGE_BTN_DOWN] =   new KeyRep(NULL, _("Down"));
  slimtable[JGE_BTN_OK] =     new KeyRep(NULL, _("Ok"));
  slimtable[JGE_BTN_CANCEL] = new KeyRep(NULL, _("Cancel"));
  slimtable[JGE_BTN_PRI] =    new KeyRep(NULL, _("Primary"));
  slimtable[JGE_BTN_SEC] =    new KeyRep(NULL, _("Secondary"));
  slimtable[JGE_BTN_PREV] =   new KeyRep(NULL, _("Next phase/Previous item"));
  slimtable[JGE_BTN_NEXT] =   new KeyRep(NULL, _("Open hand/Next item"));

  return *slimtable[key];
}
