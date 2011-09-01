/*
 *  Wagic, The Homebrew ?! is licensed under the BSD license
 *  See LICENSE in the Folder's root
 *  http://wololo.net/wagic/
 */

#ifndef _IAPLAYER_H
#define _IAPLAYER_H

#include "Player.h"

#include <queue>
using std::queue;

#define INFO_NBCREATURES 0
#define INFO_CREATURESPOWER 1
#define INFO_CREATURESRANK 2
#define INFO_CREATURESTOUGHNESS 3
#define INFO_CREATURESATTACKINGPOWER 4


class AIStats;
class AIHints;

class AIAction
{
protected:
    int efficiency;
    static int currentId;
public:
    MTGAbility * ability;
    NestedAbility * nability;
    Player * player;
    int id;
    MTGCardInstance * click;
    MTGCardInstance * target; // TODO Improve
    vector<Targetable*>mAbilityTargets;
    Targetable * playerAbilityTarget;
    //player targeting through abilities is handled completely seperate from spell targeting.
    
    AIAction(MTGAbility * a, MTGCardInstance * c, MTGCardInstance * t = NULL)
        : efficiency(-1), ability(a), player(NULL), click(c), target(t),playerAbilityTarget(NULL)
    {
        id = currentId++;
    };

    AIAction(MTGCardInstance * c, MTGCardInstance * t = NULL);

    AIAction(Player * p)//player targeting through spells
        : efficiency(-1), ability(NULL), player(p), click(NULL), target(NULL),playerAbilityTarget(NULL)
    {
    };

    AIAction(MTGAbility * a, MTGCardInstance * c, vector<Targetable*>targetCards)
        : efficiency(-1), ability(a), player(NULL), click(c), mAbilityTargets(targetCards),playerAbilityTarget(NULL)
    {
        id = currentId++;
    };

    AIAction(MTGAbility * a, Player * p, MTGCardInstance * c)//player targeting through abilities.
        : efficiency(-1), ability(a), click(c),target(NULL), playerAbilityTarget(p)
    {
        id = currentId++;
    };
    int getEfficiency();
    int Act();
    int clickMultiAct(vector<Targetable*>&actionTargets);
};

// compares Abilities efficiency
class CmpAbilities
{ 
public:
    bool operator()(const AIAction& a1, const AIAction& a2) const
    {
        AIAction* a1Ptr = const_cast<AIAction*>(&a1);
        AIAction* a2Ptr = const_cast<AIAction*>(&a2);
        int e1 = a1Ptr->getEfficiency();
        int e2 = a2Ptr->getEfficiency();
        if (e1 == e2) return a1Ptr->id < a2Ptr->id;
        return (e1 > e2);
    }
};

typedef std::map<AIAction, int, CmpAbilities> RankingContainer;

class AIPlayer: public Player{
protected:
    //Variables used by Test suite
    MTGCardInstance * nextCardToPlay;
    AIHints * hints;
    queue<AIAction *> clickstream;
    bool payTheManaCost(ManaCost * cost, MTGCardInstance * card = NULL,vector<MTGAbility*> gotPayment = vector<MTGAbility*>());
    int orderBlockers();
    int combatDamages();
    int interruptIfICan();
    int chooseAttackers();
    int chooseBlockers();
    int canFirstStrikeKill(MTGCardInstance * card, MTGCardInstance *ennemy);
    int effectBadOrGood(MTGCardInstance * card, int mode = MODE_PUTINTOPLAY, TargetChooser * tc = NULL);
    int getCreaturesInfo(Player * player, int neededInfo = INFO_NBCREATURES , int untapMode = 0, int canAttack = 0);
    AIStats * getStats();

    // returns 1 if the AI algorithm supports a given cost (ex:simple mana cost), 0 otherwise (ex: cost involves Sacrificing a target)
    int CanHandleCost(ManaCost * cost); 

    //Tries to play an ability recommended by the deck creator
    int selectHintAbility();

public:
    AIStats * stats;
    int agressivity;
    bool forceBestAbilityUse;
    void End(){};
    virtual int displayStack() {return 0;};
    int receiveEvent(WEvent * event);
    void Render();
    ManaCost * getPotentialMana(MTGCardInstance * card = NULL);
    vector<MTGAbility*> canPayMana(MTGCardInstance * card = NULL,ManaCost * mCost = NULL);
    vector<MTGAbility*> canPaySunBurst(ManaCost * mCost = NULL);
    AIPlayer(string deckFile, string deckFileSmall, MTGDeck * deck = NULL);
    virtual ~AIPlayer();
    virtual MTGCardInstance * chooseCard(TargetChooser * tc, MTGCardInstance * source, int random = 0);
    virtual int selectMenuOption();
    virtual int chooseTarget(TargetChooser * tc = NULL, Player * forceTarget =NULL,MTGCardInstance * Choosencard = NULL,bool checkonly = false);
    virtual int clickMultiTarget(TargetChooser * tc,vector<Targetable*>&potentialTargets);
    virtual int clickSingleTarget(TargetChooser * tc,vector<Targetable*>&potentialTargets,int nbtargets = 0,MTGCardInstance * Choosencard = NULL);
    virtual int Act(float dt);
    virtual int affectCombatDamages(CombatStep);
    int isAI(){return 1;};
    int canHandleCost(MTGAbility * ability);
    int selectAbility();
    int createAbilityTargets(MTGAbility * a, MTGCardInstance * c, RankingContainer& ranking);
    int useAbility();
    virtual int getEfficiency(AIAction * action);

};


class AIPlayerBaka: public AIPlayer{
 protected:
  int oldGamePhase;
  float timer;
  MTGCardInstance * FindCardToPlay(ManaCost * potentialMana, const char * type);
 public:
     vector<MTGAbility*>gotPayments;
  int deckId;
  AIPlayerBaka(string deckFile, string deckfileSmall, string avatarFile, MTGDeck * deck = NULL);
  virtual int Act(float dt);
  void initTimer();
  virtual int computeActions();
};

class AIPlayerFactory{
 public:
  AIPlayer * createAIPlayer(MTGAllCards * collection, Player * opponent, int deckid = 0);
};


#endif
