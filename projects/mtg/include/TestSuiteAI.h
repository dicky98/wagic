#ifndef _TESTSUITE_AI_H_
#define _TESTSUITE_AI_H_

#define MAX_TESTSUITE_ACTIONS 100
#define MAX_TESTUITE_CARDS 100

#include "../include/AIPlayer.h"

class TestSuiteActions{
public:
	int nbitems;
	string actions[MAX_TESTSUITE_ACTIONS];
	void add(string action);
	TestSuiteActions();
	void cleanup();
};

class TestSuitePlayerZone{
public:
	int cards[MAX_TESTUITE_CARDS];
	int nbitems;
	void add(int cardid);
	TestSuitePlayerZone();
	void cleanup();
};

class TestSuitePlayerData{
public:
	int life;
	ManaCost * manapool;
	TestSuitePlayerZone zones[5];
	TestSuitePlayerData();
	~TestSuitePlayerData();
	void cleanup();

};



class TestSuiteState{
public:
	int phase;
	void parsePlayerState(int playerId, string s);
	TestSuiteState();
	TestSuitePlayerData playerData[2];
	void cleanup();
};
class TestSuite{
public:
	int currentAction;
	TestSuiteState initState;
	TestSuiteState endState;
	TestSuiteActions actions;
	string files[128];
	int nbfiles;
	int currentfile;
	void load(const char * filename);
	TestSuite(const char * filename);
	void initGame();
	int assertGame();
	MTGPlayerCards * buildDeck(MTGAllCards * collection, int playerId);
	string getNextAction();
	int phaseStrToInt(string s);
	MTGCardInstance * getCardByMTGId(int mtgid);
	Interruptible * getActionByMTGId(int mtgid);
	int loadNext();
	void cleanup();
	int Log(const char * text);

};

class TestSuiteAI:public AIPlayer{
public:
	TestSuite * suite;
	int timer;
	TestSuiteAI(MTGAllCards * collection,TestSuite * suite, int playerId);	
	virtual int Act();
	virtual int displayStack(){return 1;}
	
};



#endif