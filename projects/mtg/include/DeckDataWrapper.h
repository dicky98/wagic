#ifndef _DECKDATAWRAPPER_H_
#define _DECKDATAWRAPPER_H_

#include "../include/MTGDefinitions.h"
#include "../include/MTGCard.h"
#include <map>
#include <string>
using std::map;
using std::string;

class MTGDeck;


class Cmp1 { // compares cards by their name
   public:
      bool operator()(MTGCard * card1, MTGCard * card2) {
				if (!card2) return true;
				if (!card1) return false;
				string name1 = card1->name;
				string name2 = card2->name;
				int result = name1.compare(name2);
				if (!result) return card1->getMTGId() < card2->getMTGId();
				return ( result < 0);
			}
};

class DeckDataWrapper{
public:
	int colors[MTG_NB_COLORS+1];
	int currentColor;
	map<MTGCard *, int,Cmp1> cards;
	int currentposition;
	MTGDeck * parent;

	DeckDataWrapper(MTGDeck * deck);
	~DeckDataWrapper();

	int Add(MTGCard * card);
	int Remove(MTGCard * card);
	MTGCard * getNext(MTGCard * previous = NULL, int color = -1);
	MTGCard * getPrevious(MTGCard * next = NULL, int color = -1);
	void updateCounts(MTGCard * card = NULL, int removed = 0);
	void updateCurrentPosition(MTGCard * currentCard,int color = -1);
	int getCount(int color = -1);
	void save();
};

#endif