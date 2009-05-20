#include "../include/config.h"
#include "../include/PriceList.h"



PriceList::PriceList(const char * _filename, MTGAllCards * _collection):collection(_collection){
  filename = _filename;
  std::ifstream file(_filename);
  std::string cardid;
  std::string price;
  if(file){
    while(std::getline(file,cardid)){
      std::getline(file,price);
      prices[atoi(cardid.c_str())]= atoi(price.c_str());
    }
    file.close();
  }
}


PriceList::~PriceList(){
}

int PriceList::save(){
  std::ofstream file(filename.c_str());
  char writer[20];
  if (file){
      map<int,int>::iterator it=prices.begin();
      while(it != prices.end()){
        sprintf(writer,"%i\n%i\n", (*it).first, (*it).second);
        it++;
        file<<writer;
    }
    file.close();
  }

  return 1;
}
int PriceList::getPrice(int cardId){
  map<int,int>::iterator it = prices.find(cardId);
  if (it != prices.end()) return (*it).second;

  char rarity = collection->getCardById(cardId)->getRarity();
  switch(rarity){
  case Constants::RARITY_M:
    return 3000;
    break;
  case Constants::RARITY_R:
    return 500;
    break;
  case Constants::RARITY_U:
    return 100;
    break;
  case Constants::RARITY_C:
    return 20;
    break;
  case Constants::RARITY_L:
    return 5;
    break;
  default:
    return 20;
    break;

  }

}

int PriceList::setPrice(int cardId, int price){
  prices[cardId] = price;
  return price;
}
