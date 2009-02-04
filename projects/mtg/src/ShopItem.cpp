#include "../include/config.h"
#include "../include/ShopItem.h"
#include "../include/CardGui.h"


ShopItem::ShopItem(int id, JLBFont *font, char* text, JQuad * _quad,JQuad * _thumb,  int x, int y, bool hasFocus, int _price): JGuiObject(id), mFont(font), mText(text), mX(x), mY(y), quad(_quad), thumb(_thumb), price(_price)
{
  quantity = 10;
  card = NULL;
  mHasFocus = hasFocus;

  mScale = 1.0f;
  mTargetScale = 1.0f;

  if (hasFocus)
    Entering();
}

ShopItem::ShopItem(int id, JLBFont *font, int _cardid, int x, int y, bool hasFocus, MTGAllCards * collection, int _price): JGuiObject(id), mFont(font), mX(x), mY(y), price(_price){
  mHasFocus = hasFocus;

  mScale = 1.0f;
  mTargetScale = 1.0f;

  if (hasFocus)
    Entering();

  card = collection->getCardById(_cardid);
  quantity = 1;
  if (card->getRarity() == Constants::RARITY_L) quantity = 50;
  quad = NULL;
  thumb = NULL;
}


ShopItem::~ShopItem(){

}

const char * ShopItem::getText(){
  return mText.c_str();
}


void ShopItem::Render(){
  JRenderer * renderer = JRenderer::GetInstance();
  renderer->FillRect(mX-5, mY-5,230,35, ARGB(128,0,0,0));
  if (card){
    thumb = card->getThumb();
    mText= card->name;
  }

  if (thumb){
    renderer->RenderQuad(thumb,mX,mY - (mScale > 1 ? 4 : 0),0,mScale * 0.45,mScale * 0.45);
  }else{
    //NOTHING
  }
  if (mHasFocus){
    if (card){
      quad = card->getQuad();
    }
    if (quad){
      renderer->RenderQuad(quad,mX + SCREEN_WIDTH/2 + 20,5,0, 0.9f,0.9f);
    }else{
      if (card) CardGui::alternateRender(card,mFont,NULL,mX + SCREEN_WIDTH/2 + 100 + 20,133,0, 0.9f);
      //TODO
    }
    mFont->SetColor(ARGB(255,255,255,0));
  }else{
    mFont->SetColor(ARGB(255,255,255,255));
  }
  if (!quantity){
    mFont->SetColor(ARGB(255,128,128,128));
  }
  mFont->DrawString(mText.c_str(), mX + 30, mY + 8);
}




void ShopItem::Update(float dt)
{
  if (mScale < mTargetScale)
    {
      mScale += 8.0f*dt;
      if (mScale > mTargetScale)
	mScale = mTargetScale;
    }
  else if (mScale > mTargetScale)
    {
      mScale -= 8.0f*dt;
      if (mScale < mTargetScale)
	mScale = mTargetScale;
    }
}




void ShopItem::Entering()
{

  mHasFocus = true;
  mTargetScale = 1.2f;
}


bool ShopItem::Leaving(u32 key)
{
  mHasFocus = false;
  mTargetScale = 1.0f;
  return true;
}


bool ShopItem::ButtonPressed()
{
  return (quantity >0);
}


ShopItems::ShopItems(int id, JGuiListener* listener, JLBFont* font, int x, int y, MTGAllCards * _collection, int _setId): JGuiController(id, listener), mX(x), mY(y), mFont(font), collection(_collection), setId(_setId){
  mHeight = 0;
  showPriceDialog = -1;
  dialog = NULL;
  pricelist = NEW PriceList(RESPATH"/settings/prices.dat",_collection);
  playerdata = NEW PlayerData(_collection);
  display = NULL;
}



void ShopItems::Add(int cardid){
  int rnd = (rand() % 20);
  int price = pricelist->getPrice(cardid);
  price = price + price * (rnd -10)/100;
  JGuiController::Add(NEW ShopItem(mCount, mFont, cardid, mX + 10, mY + 10 + mHeight,  (mCount == 0),collection, price));
  mHeight += 40;
}

void ShopItems::Add(char * text, JQuad * quad,JQuad * thumb, int price){
  JGuiController::Add(NEW ShopItem(mCount, mFont, text, quad, thumb, mX + 10, mY + 10 + mHeight,  (mCount == 0), price));
  mHeight += 40;
}

void ShopItems::Update(float dt){
  if (display){
    while (u32 key = JGE::GetInstance()->ReadButton()) display->CheckUserInput(key);
    if (display) display->Update(dt);
  }else{
    if (showPriceDialog!=-1){
      ShopItem * item =  ((ShopItem *)mObjects[showPriceDialog]);
      int price = item->price;
      char buffer[4096];
      sprintf(buffer,"%s : %i credits",item->getText(),price);
      if(!dialog){
	dialog = NEW SimpleMenu(1,this,GameApp::CommonRes->GetJLBFont(Constants::MENU_FONT),SCREEN_WIDTH-300,SCREEN_HEIGHT/2,buffer);
	dialog->Add(1,"Yes");
	dialog->Add(2,"No");
      }
      else{
	dialog->Update(dt);
      }
    }else{
      SAFE_DELETE(dialog);
      JGuiController::Update(dt);
    }
  }

}


void ShopItems::Render(){
  JGuiController::Render();
  if (showPriceDialog==-1){

  }else{
    if(dialog){
      dialog->Render();
    }
  }
  char credits[512];
  sprintf(credits,"credits: %i", playerdata->credits);
  unsigned int len = 4 + mFont->GetStringWidth(credits);
  mFont->SetColor(ARGB(200,0,0,0));
  mFont->DrawString(credits, SCREEN_WIDTH - len + 2, SCREEN_HEIGHT - 13);
  mFont->SetColor(ARGB(255,255,255,255));
  mFont->DrawString(credits, SCREEN_WIDTH - len, SCREEN_HEIGHT - 15);
  if (display) display->Render();
}

void ShopItems::pricedialog(int id, int mode){
  if (mode){
    showPriceDialog = id;
  }else{
    showPriceDialog = -1;
  }
}

void ShopItems::ButtonPressed(int controllerId, int controlId){
  if (controllerId == 12){
    safeDeleteDisplay();
    return;
  }

  ShopItem * item =  ((ShopItem *)mObjects[showPriceDialog]);
  int price = item->price;
  switch(controlId){
  case 1:
    if (playerdata->credits >= price){
      playerdata->credits -= price;
      if (item->card){
	int rnd = (rand() % 5);
	price = price + (rnd * price)/100;
	pricelist->setPrice(item->card->getMTGId(),price);
	playerdata->collection->add(item->card);
	item->quantity--;
      }else{
	safeDeleteDisplay();
	display = new CardDisplay(12,NULL, SCREEN_WIDTH - 200, SCREEN_HEIGHT/2,this,NULL,5);
	int curNbcards = playerdata->collection->totalCards();
	if (showPriceDialog == 0){
	  //Starter Deck
	  playerdata->collection->addRandomCards(3,setId,Constants::RARITY_R,NULL);
	  playerdata->collection->addRandomCards(9, setId,Constants::RARITY_U,NULL);
	  playerdata->collection->addRandomCards(48, setId,Constants::RARITY_C,NULL);
	}else{
	  //Booster
	  playerdata->collection->addRandomCards(1, setId,Constants::RARITY_R);
	  playerdata->collection->addRandomCards(3, setId,Constants::RARITY_U);
	  playerdata->collection->addRandomCards(11, setId,Constants::RARITY_C);
	}
	int newNbCards = playerdata->collection->totalCards();;
	for (int i = curNbcards; i < newNbCards ; i++){
	  MTGCardInstance * card = NEW MTGCardInstance(playerdata->collection->_(i), NULL);
	  displayCards[i-curNbcards] = card;
	  display->AddCard(card);
	}
      }
      //Remove(showPriceDialog);
      showPriceDialog = -1;
    }else{
      //error not enough money
    }
    break;
  case 2:
    if (item->card){
      int rnd = (rand() % 5);
      price = price - (rnd * price)/100;
      pricelist->setPrice(item->card->getMTGId(),price);
    }
    showPriceDialog = -1;
    break;
  }
}


void ShopItems::safeDeleteDisplay(){
  if (!display) return;
  for (int i = 0; i < display->mCount; i++){
    delete displayCards[i];
  }
  SAFE_DELETE(display);
}

void ShopItems::saveAll(){
  savePriceList();
  playerdata->save();
}

void ShopItems::savePriceList(){
  pricelist->save();
}

ShopItems::~ShopItems(){
  SAFE_DELETE(pricelist);
  SAFE_DELETE(playerdata);
  SAFE_DELETE(dialog);
  safeDeleteDisplay();
}
