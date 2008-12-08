#ifndef _GAME_STATE_DECK_VIEWER_H_
#define _GAME_STATE_DECK_VIEWER_H_

#include <math.h>
#include <iostream>

#include <JGE.h>

#include "GameState.h"
#include "SimpleMenu.h"
#include "../include/CardGui.h"
#include "../include/GameOptions.h"
#include "../include/PriceList.h"
#include "../include/PlayerData.h"
#include "../include/DeckDataWrapper.h"

#define NO_USER_ACTIVITY_HELP_DELAY 10
#define NO_USER_ACTIVITY_SHOWCARD_DELAY 0.1

enum
  {
    STAGE_TRANSITION_RIGHT = 0,
    STAGE_TRANSITION_LEFT = 1,
    STAGE_WAITING = 2,
    STAGE_TRANSITION_UP = 3,
    STAGE_TRANSITION_DOWN = 4,
    STAGE_ONSCREEN_MENU = 5,
    STAGE_WELCOME = 6,
    STAGE_MENU = 7
  };


#define ALL_COLORS -1

#define ROTATE_LEFT 1;
#define ROTATE_RIGHT 0;

#define HIGH_SPEED 15.0
#define MED_SPEED 5.0
#define LOW_SPEED 1.5


class GameStateDeckViewer: public GameState, public JGuiListener
{
 private:
  JQuad * mIcons[7];
  JQuad * pspIcons[8];
  JTexture * pspIconsTexture;
  float last_user_activity;
  float onScreenTransition;
  float mRotation;
  float mSlide;
  int mAlpha;
  int mStage;

  int colorFilter;
  JMusic * bgMusic;
  JTexture * backTex;
  JQuad * backQuad;
  SimpleMenu * welcome_menu;
  JLBFont * mFont;
  bool showing_user_deck;
  JLBFont * menuFont;
  SimpleMenu * menu;
  SimpleMenu * sellMenu;
  PriceList* pricelist;
  PlayerData * playerdata;
  int price;
  DeckDataWrapper * displayed_deck;
  DeckDataWrapper * myDeck;
  DeckDataWrapper * myCollection;
  MTGCard * currentCard;
  MTGCard *  cardIndex[7];
  int hudAlpha;
  float scrollSpeed;
 public:

 GameStateDeckViewer(GameApp* parent): GameState(parent) {
    bgMusic = NULL;
    scrollSpeed = MED_SPEED;
  }
  virtual ~GameStateDeckViewer() {
    SAFE_DELETE(bgMusic);
  }


  void rotateCards(int direction){
    int maxCards=displayed_deck->getCount(colorFilter);
    if (maxCards==0)
      return;
    int left = direction;
    if (left){
      MTGCard * currentCard = displayed_deck->getNext(cardIndex[6],colorFilter);
      for (int i = 1; i<7; i++){
	cardIndex[i-1] = cardIndex[i];
      }
      cardIndex[6] = currentCard;
    }else{
      MTGCard * currentCard = displayed_deck->getPrevious(cardIndex[0],colorFilter);
      for (int i = 5; i>=0; i--){
	cardIndex[i+1] = cardIndex[i];
      }
      cardIndex[0] = currentCard;
    }
    displayed_deck->updateCurrentPosition(cardIndex[2],colorFilter);
  }

  void loadIndexes(MTGCard * current = NULL){
    for (int i = 0; i < 7; i++){
      cardIndex[i] = NULL;
    }
    MTGCard * _current = current;
    _current = displayed_deck->getNext(NULL,colorFilter);
    for (int i = 0; i < 7; i++){
      cardIndex[i] = _current;
#if defined (WIN32) || defined (LINUX)
      char buf[4096];
      sprintf(buf,"Loadindexes[%i] is NULL\n", i);
      if(_current) sprintf(buf, "LoadIndexes[%i]  : %s\n", i, _current->getName());
      OutputDebugString(buf);
#endif
      _current = displayed_deck->getNext(_current,colorFilter);

    }
    displayed_deck->updateCurrentPosition(cardIndex[2],colorFilter);
  }

  void switchDisplay(){
#if defined (WIN32) || defined (LINUX)
    OutputDebugString("Switching display");
#endif
    if (displayed_deck == myCollection){
      displayed_deck = myDeck;
    }else{
      displayed_deck = myCollection;
    }
    currentCard = NULL;
    loadIndexes();
  }

  virtual void Start()
  {
    hudAlpha = 0;
    pricelist = NEW PriceList("Res/settings/prices.dat",mParent->collection);
    playerdata = NEW PlayerData(mParent->collection);
    sellMenu = NULL;
    myCollection = 	 NEW DeckDataWrapper(NEW MTGDeck("Res/player/collection.dat", mParent->cache,mParent->collection));
    displayed_deck =  myCollection;
    myDeck = NULL;
    menuFont = GameApp::CommonRes->GetJLBFont("graphics/f3");
    mFont = GameApp::CommonRes->GetJLBFont("graphics/magic");




    menu = NEW SimpleMenu(11,this,menuFont,SCREEN_WIDTH/2-100,20,200);
    menu->Add(11,"Save");
    menu->Add(12,"Back to main menu");
    menu->Add(13, "Cancel");


    //icon images
    mIcons[MTG_COLOR_ARTIFACT] = GameApp::CommonRes->GetQuad("c_artifact");
    mIcons[MTG_COLOR_LAND] = GameApp::CommonRes->GetQuad("c_land");
    mIcons[MTG_COLOR_WHITE] = GameApp::CommonRes->GetQuad("c_white");
    mIcons[MTG_COLOR_RED] = GameApp::CommonRes->GetQuad("c_red");
    mIcons[MTG_COLOR_BLACK] = GameApp::CommonRes->GetQuad("c_black");
    mIcons[MTG_COLOR_BLUE] = GameApp::CommonRes->GetQuad("c_blue");
    mIcons[MTG_COLOR_GREEN] = GameApp::CommonRes->GetQuad("c_green");
    for (int i=0; i < 7; i++){
      mIcons[i]->SetHotSpot(16,16);
    }


    pspIconsTexture = JRenderer::GetInstance()->LoadTexture("graphics/iconspsp.png", TEX_TYPE_USE_VRAM);

    for (int i=0; i < 8; i++){
      pspIcons[i] = NEW JQuad(pspIconsTexture, i*32, 0, 32, 32);
      pspIcons[i]->SetHotSpot(16,16);
    }


    backTex = JRenderer::GetInstance()->LoadTexture("sets/back.jpg", TEX_TYPE_USE_VRAM);
    backQuad = NEW JQuad(backTex, 0, 0, 200, 285);		// Create quad for rendering.

    //menuFont = NEW JLBFont("graphics/f3",16);
    menuFont = GameApp::CommonRes->GetJLBFont("graphics/f3");
    welcome_menu = NEW SimpleMenu(10,this,menuFont,20,20,200);
    char buffer[100];
    for (int i=1; i < 6; i++){
      sprintf(buffer, "Res/player/deck%i.txt",i);
      std::ifstream file(buffer);
      if(file){
	welcome_menu->Add(i, GameState::menuTexts[i]);
	file.close();
      }else{
	welcome_menu->Add(i, GameState::menuTexts[0]);
      }

    }
    welcome_menu->Add(10, "Cancel");

    if (GameApp::HasMusic && GameOptions::GetInstance()->values[OPTIONS_MUSICVOLUME] > 0){
      if (GameApp::music){
         JSoundSystem::GetInstance()->StopMusic(GameApp::music);
          SAFE_DELETE(GameApp::music);
      }
      GameApp::music = JSoundSystem::GetInstance()->LoadMusic("sound/track1.mp3");
      if (GameApp::music){
	      JSoundSystem::GetInstance()->PlayMusic(GameApp::music, true);
      }
    }
    colorFilter = ALL_COLORS;

    mStage = STAGE_WELCOME;

    mRotation = 0;
    mSlide = 0;
    mAlpha = 255;
    //mEngine->ResetPrivateVRAM();
    //mEngine->EnableVSync(true);
    currentCard = NULL;
    loadIndexes(currentCard);
    last_user_activity = NO_USER_ACTIVITY_HELP_DELAY + 1;
    onScreenTransition = 0;

    mEngine->ResetInput();
  }


  virtual void End()
  {
    //mEngine->EnableVSync(false);
    if (GameApp::music){
      JSoundSystem::GetInstance()->StopMusic(GameApp::music);
      SAFE_DELETE(GameApp::music);
    }
    SAFE_DELETE(backTex);
    SAFE_DELETE(backQuad);
    SAFE_DELETE(welcome_menu);
    SAFE_DELETE(menu);
    SAFE_DELETE(pspIconsTexture);
    for (int i=0; i < 8; i++){
      SAFE_DELETE(pspIcons[i]);
    }
    SAFE_DELETE(myCollection);
    SAFE_DELETE(myDeck);
    SAFE_DELETE(pricelist);
    SAFE_DELETE(playerdata);
  }


  void addRemove(MTGCard * card){
    if (!card) return;
    if (displayed_deck->Remove(card)){
      if (displayed_deck == myCollection){
	myDeck->Add(card);
      }else{
	myCollection->Add(card);
      }
    }
    //loadIndexes(cardIndex[0]);
  }

  int Remove(MTGCard * card){
    if (!card) return 0;
    int result = displayed_deck->Remove(card);
    loadIndexes(currentCard);
    return result;
  }


  virtual void Update(float dt)
  {
    hudAlpha = 255-(last_user_activity * 500);
    if (hudAlpha < 0) hudAlpha = 0;
    if (sellMenu){
      sellMenu->Update(dt);
      return;
    }
    if (mStage == STAGE_WAITING || mStage == STAGE_ONSCREEN_MENU){
      switch (mEngine->ReadButton())
	{
	case PSP_CTRL_LEFT :
	  last_user_activity = 0;
	  currentCard = displayed_deck->getNext(currentCard,colorFilter);
	  mStage = STAGE_TRANSITION_LEFT;
	  break;
	case PSP_CTRL_RIGHT :
	  last_user_activity = 0;
	  currentCard = displayed_deck->getPrevious(currentCard,colorFilter);
	  mStage = STAGE_TRANSITION_RIGHT;
	  break;
	case PSP_CTRL_UP :
	  last_user_activity = 0;
	  mStage = STAGE_TRANSITION_UP;
	  colorFilter--;
	  if (colorFilter < -1) colorFilter = MTG_COLOR_LAND;
	  break;
	case PSP_CTRL_DOWN :
	  last_user_activity = 0;
	  mStage = STAGE_TRANSITION_DOWN;
	  colorFilter ++;
	  if (colorFilter >  MTG_COLOR_LAND) colorFilter =-1;
	  break;
	case PSP_CTRL_TRIANGLE :
	  if (last_user_activity > 0.2)
	    {
	      last_user_activity = 0;
	      switchDisplay();
	    }
	  break;
	case PSP_CTRL_CIRCLE :
	  last_user_activity = 0;
	  addRemove(cardIndex[2]);
	  break;
	case PSP_CTRL_CROSS :
	  last_user_activity = 0;
	  SAFE_DELETE(sellMenu);
	  char buffer[4096];
	  {
	    MTGCard * card  = cardIndex[2];
	    if (card){
	      int rnd = (rand() % 20);
	      price = pricelist->getPrice(card->getMTGId()) / 2;
	      price = price - price * (rnd -10)/100;
	      sprintf(buffer,"%s : %i credits",card->getName(),price);
	      sellMenu = NEW SimpleMenu(2,this,mFont,SCREEN_WIDTH-300,SCREEN_HEIGHT/2,270,buffer);
	      sellMenu->Add(20,"Yes");
	      sellMenu->Add(21,"No");
	    }
	  }
	  break;
	case PSP_CTRL_SQUARE :
	  if (last_user_activity < NO_USER_ACTIVITY_HELP_DELAY){
	    last_user_activity = NO_USER_ACTIVITY_HELP_DELAY + 1;
	  }else{
	    last_user_activity = 0;
	    mStage = STAGE_WAITING;
	  }
	  break;
	case PSP_CTRL_START :
	  mStage = STAGE_MENU;
	  break;
	case PSP_CTRL_SELECT :
	  if (scrollSpeed == HIGH_SPEED)
	    scrollSpeed = MED_SPEED;
	  else if (scrollSpeed == MED_SPEED)
	    scrollSpeed = LOW_SPEED;
	  else
	    scrollSpeed = HIGH_SPEED;
	  break;
 	default :  // no keypress
	  if (last_user_activity > NO_USER_ACTIVITY_HELP_DELAY){
	    if  (mStage != STAGE_ONSCREEN_MENU){
	      mStage = STAGE_ONSCREEN_MENU;
	      onScreenTransition = 1;
	    }else{
	      if (onScreenTransition >0){
		onScreenTransition-= 0.05f;
	      }else{
		onScreenTransition = 0;
	      }
	    }
	  }else{
	    last_user_activity+= dt;
	  }
	}

    } if (mStage == STAGE_TRANSITION_RIGHT || mStage == STAGE_TRANSITION_LEFT) {
      //mAlpha = 128;
      if (mStage == STAGE_TRANSITION_RIGHT){
	mRotation -= dt * scrollSpeed;
	if (mRotation < -1.0f){
	  do {
	    rotateCards(mStage);
	    mRotation += 1;
	  } while (mRotation < -1.0f);
	  mStage = STAGE_WAITING;
	  mRotation = 0;
	}
      }else if(mStage == STAGE_TRANSITION_LEFT){
	mRotation += dt * scrollSpeed;
	if (mRotation > 1.0f){
	  do {
	    rotateCards(mStage);
	    mRotation -= 1;
	  } while (mRotation > 1.0f);
	  mStage = STAGE_WAITING;
	  mRotation = 0;
	}
      }
    } if (mStage == STAGE_TRANSITION_DOWN || mStage == STAGE_TRANSITION_UP){
      if (mStage == STAGE_TRANSITION_DOWN){
	mSlide -= 0.05f;
	if (mSlide < -1.0f){
	  loadIndexes(currentCard);
	  mSlide = 1;
	}else if (mSlide > 0 && mSlide < 0.05){
	  mStage = STAGE_WAITING;
	  mSlide = 0;
	}
      } if (mStage == STAGE_TRANSITION_UP){
	mSlide += 0.05f;
	if (mSlide > 1.0f){
	  loadIndexes(currentCard);
	  mSlide = -1;
	}else if (mSlide < 0 && mSlide > -0.05){
	  mStage = STAGE_WAITING;
	  mSlide = 0;
	}
      }


    }else if (mStage == STAGE_WELCOME){
      welcome_menu->Update(dt);
    }else if (mStage == STAGE_MENU){
      menu->Update(dt);
    }


  }


  void renderOnScreenBasicInfo(){
    mFont->SetScale(1.0);
    char buffer[30], buffer2[30];

    float y = 0;
    JRenderer::GetInstance()->FillRoundRect(SCREEN_WIDTH-125,y-5,110,15,5,ARGB(128,0,0,0));
    sprintf(buffer, "DECK: %i", myDeck->getCount());
    mFont->DrawString(buffer, SCREEN_WIDTH-120 , y);

    if (colorFilter != ALL_COLORS){
      sprintf(buffer2, "(   %i)", myDeck->getCount(colorFilter));
      mFont->DrawString(buffer2, SCREEN_WIDTH-55 , y);
      JRenderer::GetInstance()->RenderQuad(mIcons[colorFilter], SCREEN_WIDTH-42  , y + 6 , 0.0f,0.5,0.5);
    }

  }


  void renderSlideBar(){
    int currentPos = displayed_deck->currentposition;
    int total = displayed_deck->getCount(colorFilter);
    int filler = 15;
    int y = SCREEN_HEIGHT-25;
    int bar_size  = SCREEN_WIDTH - 2*filler;
    int cursor_pos = bar_size * currentPos / total;
    JRenderer * r = JRenderer::GetInstance();
    r->FillRoundRect(filler + 5,y+5,bar_size,0,3,ARGB(hudAlpha/2,0,0,0));
    r->DrawLine(filler+cursor_pos + 5 ,y+5,filler+cursor_pos + 5,y+10,ARGB(hudAlpha/2,0,0,0));

    r->FillRoundRect(filler,y,bar_size,0,3,ARGB(hudAlpha/2,128,128,128));
    //r->FillCircle(filler+cursor_pos + 3 ,SCREEN_HEIGHT - 15 + 3,6,ARGB(255,128,128,128));
    r->DrawLine(filler+cursor_pos,y,filler+cursor_pos,y+5,ARGB(hudAlpha,255,255,255));
    char buffer[256];
    string deckname = "Collection";
    if (displayed_deck == myDeck){
      deckname = "Deck";
    }
    sprintf(buffer,"%s - %i/%i", deckname.c_str(),currentPos, total);
    mFont->SetColor(ARGB(hudAlpha,255,255,255));
    mFont->DrawString(buffer,SCREEN_WIDTH/2, y+5,JGETEXT_CENTER);


    mFont->SetColor(ARGB(255,255,255,255));
  }

  void renderDeckBackground(){
    int max1 = 0;
    int maxC1 = 4;
    int max2 = 0;
    int maxC2 = 4;

    for (int i= 0; i< MTG_NB_COLORS -1; i++){
      int value = myDeck->getCount(i);
      if (value > max1){
	max2 = max1;
	maxC2 = maxC1;
	max1 = value;
	maxC1 = i;
      }else if (value > max2){
	max2 = value;
	maxC2 = i;
      }
    }
    if (max2 < max1/2){
      maxC2 = maxC1;
    }

    int _a[6] = {255,	255,	255,255,255,255};
    int _r[6] = {75,	0,		0,	255,50,255};
    int _g[6] = {30,	255,	0,	0,	50,255};
    int _b[6] = {20,	0,		255,0,	50,255};


    PIXEL_TYPE colors[] =
      {
	ARGB(255,_r[maxC1],_g[maxC1],_b[maxC1]),
	ARGB(255,_r[maxC1],_g[maxC1],_b[maxC1]),
	ARGB(255,_r[maxC2],_g[maxC2],_b[maxC2]),
	ARGB(255,_r[maxC2],_g[maxC2],_b[maxC2]),
      };

    JRenderer::GetInstance()->FillRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,colors);

  }

  void renderOnScreenMenu(){
    mFont->SetColor(ARGB(255,255,255,255));
    mFont->SetScale(1.0);
    JRenderer * r = JRenderer::GetInstance();
    float pspIconsSize = 0.5;

    float leftTransition = onScreenTransition*84;
    float rightTransition = onScreenTransition*204;
    float leftPspX = 40 - leftTransition;
    float leftPspY = SCREEN_HEIGHT/2 - 30 ;
    float rightPspX = SCREEN_WIDTH-100 + rightTransition;
    float rightPspY = SCREEN_HEIGHT/2 - 30 ;


    //FillRects
    r->FillRect(0-(onScreenTransition*84),0,84,SCREEN_HEIGHT,ARGB(128,0,0,0));
    r->FillRect(SCREEN_WIDTH-204+(onScreenTransition*204),0,200,SCREEN_HEIGHT,ARGB(128,0,0,0));


    //LEFT PSP CIRCLE render
    r->FillCircle(leftPspX,leftPspY,40,ARGB(128,50,50,50));

    r->RenderQuad(pspIcons[0],leftPspX, leftPspY - 20,0,pspIconsSize,pspIconsSize);
    r->RenderQuad(pspIcons[1],leftPspX, leftPspY + 20,0,pspIconsSize,pspIconsSize);
    r->RenderQuad(pspIcons[2],leftPspX - 20, leftPspY,0,pspIconsSize,pspIconsSize);
    r->RenderQuad(pspIcons[3],leftPspX + 20, leftPspY,0,pspIconsSize,pspIconsSize);


    mFont->DrawString("Prev.", leftPspX - 35, leftPspY-15);
    mFont->DrawString("Next", leftPspX + 15, leftPspY-15);
    mFont->DrawString("card", leftPspX - 35, leftPspY);
    mFont->DrawString("card", leftPspX + 15, leftPspY);
    mFont->DrawString("Next color", leftPspX - 33, leftPspY - 35);
    mFont->DrawString("Prev. color", leftPspX -33 , leftPspY +25);

    //RIGHT PSP CIRCLE render
    r->FillCircle(rightPspX+(onScreenTransition*204),rightPspY,40,ARGB(128,50,50,50));
    r->RenderQuad(pspIcons[4],rightPspX+20, rightPspY,0,pspIconsSize,pspIconsSize);
    r->RenderQuad(pspIcons[5],rightPspX, rightPspY - 20,0,pspIconsSize,pspIconsSize);
    r->RenderQuad(pspIcons[6],rightPspX-20, rightPspY,0,pspIconsSize,pspIconsSize);
    r->RenderQuad(pspIcons[7],rightPspX, rightPspY + 20,0,pspIconsSize,pspIconsSize);

    if (displayed_deck == myCollection){
      mFont->DrawString("Add card", rightPspX + 20, rightPspY-15);
      mFont->DrawString("Display Deck", rightPspX - 35, rightPspY - 40);
    }else{
      mFont->DrawString("Remove card", rightPspX + 20, rightPspY-15);
      mFont->DrawString("Display Collection", rightPspX - 35, rightPspY - 40);
    }
    mFont->DrawString("Deck info", rightPspX - 70 , rightPspY-15);
    mFont->DrawString("Sell card", rightPspX - 30 , rightPspY+20);
    //Bottom menus
    mFont->DrawString("menu", SCREEN_WIDTH-35 +rightTransition, SCREEN_HEIGHT-15);



    //Your Deck Information
    char buffer[300];
    int nb_letters = 0;
    for (int j=0; j<MTG_NB_COLORS;j++){
      int value = myDeck->getCount(j);
      if (value > 0){
	sprintf(buffer, "%i", value);
	mFont->DrawString(buffer, SCREEN_WIDTH-190+rightTransition + nb_letters*13, SCREEN_HEIGHT/2 + 40);
	r->RenderQuad(mIcons[j],SCREEN_WIDTH-197+rightTransition + nb_letters*13 , SCREEN_HEIGHT/2 + 46,0,0.5,0.5);
	if (value > 9){nb_letters += 3;}else{nb_letters+=2;}
      }
    }
    int value = myDeck->getCount();
    sprintf(buffer, "Your Deck: %i cards", value);
    mFont->DrawString(buffer, SCREEN_WIDTH-200+rightTransition, SCREEN_HEIGHT/2 + 25);

    //TODO, put back !
    /*int nbCreatures = myDeck->countByType("Creature");
      int nbSpells = myDeck->countByType("Instant") + myDeck->countByType("Enchantment") + myDeck->countByType("Sorcery");

      sprintf(buffer, "Creatures: %i - Spells: %i", nbCreatures, nbSpells);
      mFont->DrawString(buffer, SCREEN_WIDTH-200+rightTransition, SCREEN_HEIGHT/2 + 55);
    */


    mFont->DrawString("You are currently viewing your",  SCREEN_WIDTH-200+rightTransition, 5);
    if (displayed_deck == myCollection){
      mFont->DrawString("collection. Press TRIANGLE",  SCREEN_WIDTH-200+rightTransition, 20);
      mFont->DrawString("to switch to your deck",  SCREEN_WIDTH-200+rightTransition, 35);
    }else{
      mFont->DrawString("deck. Press TRIANGLE to",  SCREEN_WIDTH-200+rightTransition, 20);
      mFont->DrawString("switch to your collection",  SCREEN_WIDTH-200+rightTransition, 35);
    }

  }


  virtual void renderCard(int id, float rotation){
    MTGCard * card = cardIndex[id];


    float max_scale = 0.96;
    float min_scale = 0.2;
    float x_center_0 = 180;
    float right_border = SCREEN_WIDTH - 20 ;

    float x_center = x_center_0 + cos((rotation + 8 - id)*M_PI/12)*(right_border-x_center_0);
    float scale = max_scale/ 1.12 * cos((x_center-x_center_0)*1.5/(right_border - x_center_0) ) + 0.2 * max_scale * cos (cos((x_center-x_center_0)*0.15/(right_border - x_center_0) ));
    float x =  x_center - 100*scale;

    float y = (SCREEN_HEIGHT - 285*scale)/2 + SCREEN_HEIGHT*mSlide*(scale+0.2);

    int alpha = (int) (255 * (scale + 1.0 - max_scale));

    if (!card) return;
    JQuad * quad = backQuad;

    int showName = 1;
    if (mParent->cache->isInCache(card) || last_user_activity > (abs(2-id) + 1)* NO_USER_ACTIVITY_SHOWCARD_DELAY){
      quad = card->getQuad(mParent->cache);
      showName = 0;
    }




    if (quad){
      int quadAlpha = alpha;
      if ( !displayed_deck->cards[card]) quadAlpha /=2;
      quad->SetColor(ARGB(mAlpha,quadAlpha,quadAlpha,quadAlpha));
      JRenderer::GetInstance()->RenderQuad(quad, x   , y , 0.0f,scale,scale);
      if (showName){
	char buffer[4096];
	sprintf(buffer, "%s", card->getName());
	int scaleBackup = mFont->GetScale();
	mFont->SetScale(1*scale);
	mFont->DrawString(buffer,x,y);
	mFont->SetScale(scaleBackup);
      }
    }else{
      CardGui::alternateRender(card, mFont, mIcons, x_center, y + 142.5*scale, 0, scale);
      quad = card->getThumb();
      if (quad){
         float _scale = 285 * scale / quad->mHeight;
         quad->SetColor(ARGB(40,255,255,255));
         JRenderer::GetInstance()->RenderQuad(quad,x,y,0,_scale,_scale);
      }
    }
    if (last_user_activity < 3){
      int fontAlpha = alpha;
      float qtY  = y + 0 * scale;
      float qtX = x + 120*scale;
      char buffer[4096];
      sprintf(buffer, "x%i", displayed_deck->cards[card]);
      JLBFont * font = menuFont;
      int scaleBackup = font->GetScale();
      font->SetScale(1*scale);
      font->SetColor(ARGB(fontAlpha/2,0,0,0));
      JRenderer::GetInstance()->FillRect(qtX, qtY,32*scale,20*scale,ARGB(fontAlpha/2,0,0,0));
      font->DrawString(buffer, qtX + 2, qtY + 2);
      font->SetColor(ARGB(fontAlpha,255,255,255));
      font->DrawString(buffer, qtX, qtY);
      font->SetScale(scaleBackup);
      font->SetColor(ARGB(255,255,255,255));
    }
  }


  virtual void renderCard (int id){
    renderCard(id, 0);
  }

  virtual void Render()
  {
    //	void RenderQuad(JQuad* quad, float xo, float yo, float angle=0.0f, float xScale=1.0f, float yScale=1.0f);

    JRenderer::GetInstance()->ClearScreen(ARGB(0,0,0,0));


    if(displayed_deck == myDeck){
      renderDeckBackground();
    }




    int order[3] = {1,2,3};
    if (mRotation < 0.5 && mRotation > -0.5){
      order[1]=3;
      order[2]=2;
    }else if (mRotation < -0.5){
      order[0] = 3;
      order[2] = 1;
    }

    renderCard(6,mRotation);
    renderCard(5,mRotation);
    renderCard(4,mRotation);
    renderCard(0,mRotation);

    for (int i =0; i< 3; i++){
      renderCard(order[i],mRotation);
    }

    if (displayed_deck->getCount(colorFilter)>0){
      renderSlideBar();
    }else{
      int scaleBackup = mFont->GetScale();
      mFont->SetScale(2);
      mFont->DrawString("No Card", SCREEN_WIDTH/2, SCREEN_HEIGHT/2,JGETEXT_CENTER);
      mFont->SetScale(scaleBackup);
    }

    if (mStage == STAGE_ONSCREEN_MENU){
      renderOnScreenMenu();
    }else if (mStage == STAGE_WELCOME){
      welcome_menu->Render();
    }else{
      renderOnScreenBasicInfo();
      if (sellMenu) sellMenu->Render();
    }
    if (mStage == STAGE_MENU){
      menu->Render();
    }

  }


  int loadDeck(int deckid){
    SAFE_DELETE(myCollection);
    myCollection = 	 NEW DeckDataWrapper(NEW MTGDeck("Res/player/collection.dat", mParent->cache,mParent->collection));
    displayed_deck = myCollection;
    char filename[4096];
    sprintf(filename, "Res/player/deck%i.txt", deckid);
    SAFE_DELETE(myDeck);
    myDeck = NEW DeckDataWrapper(NEW MTGDeck(filename, mParent->cache,mParent->collection));
    MTGCard * current = myDeck->getNext();
    while (current){
      int howmanyinDeck = myDeck->cards[current];
      for (int i = 0; i < howmanyinDeck; i++){
	int deleted = myCollection->Remove(current);
	if (!deleted){
	  myDeck->Remove(current);
	}
      }
      current = myDeck->getNext(current);
    }
    return 1;
  }

  virtual void ButtonPressed(int controllerId, int controlId)
  {
    switch (controlId)
      {
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
	loadDeck(controlId);
	mStage = STAGE_WAITING;
	break;
      case 10:
	mParent->SetNextState(GAME_STATE_MENU);
	break;
      case 11:
	myDeck->save();
	playerdata->save();
	pricelist->save();
	mStage =  STAGE_WAITING;
	break;
      case 12:
	mParent->SetNextState(GAME_STATE_MENU);
	break;
      case 13:
	mStage =  STAGE_WAITING;
	break;
      case 20:
	{
	  MTGCard * card  = cardIndex[2];
	  if (card){
	    int rnd = (rand() % 20);
	    price = price - (rnd * price)/100;
	    playerdata->credits += price;
	    pricelist->setPrice(card->getMTGId(),price*2);
#if defined (WIN32) || defined (LINUX)
	    char buf[4096];
	    sprintf(buf, "CARD'S NAME : %s", card->getName());
	    OutputDebugString(buf);
#endif
	    playerdata->collection->remove(card->getMTGId());
	    Remove(card);
	  }
	}
      case 21:
	SAFE_DELETE(sellMenu); //TODO, stop deleting this object while it's being used !!!
	break;
      }

  }

};


#endif
