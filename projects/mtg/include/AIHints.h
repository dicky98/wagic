#ifndef _AIHINTS_H_
#define _AIHINTS_H_

#include <string>
#include <vector>
using std::string;
using std::vector;

#include "AIPlayerBaka.h"
#include "AllAbilities.h"

class ManaCost;
class MTGAbility;

class AIHint
{
public:
    string mCondition;
    string mAction;
    string mCombatAttackTip;
    vector<string>castOrder;
    vector<string>combos;
    //for preformance we disect the combo on first run.
    vector<string>partOfCombo;
    vector<string>hold;
    vector<string>until;
    vector<string>restrict;
    map<string,string>cardTargets;
    string manaNeeded;
    int mSourceId;
    AIHint(string line);
};


class AIHints
{
protected:
    AIPlayerBaka * mPlayer;
    vector<AIHint *> hints;
    AIHint * getByCondition (string condition);
    AIAction * findAbilityRecursive(AIHint * hint, ManaCost * potentialMana);
    vector<MTGAbility *> findAbilities(AIHint * hint);
    RankingContainer findActions(AIHint * hint);
    string constraintsNotFulfilled(AIAction * a, AIHint * hint, ManaCost * potentialMana);
    bool findSource(int sourceId);
    bool abilityMatches(MTGAbility * a, AIHint * hint);
public:
    AIHints (AIPlayerBaka * player);
    AIAction * suggestAbility(ManaCost * potentialMana);
    bool HintSaysDontAttack(GameObserver* observer,MTGCardInstance * card = NULL);
    bool HintSaysItsForCombo(GameObserver* observer,MTGCardInstance * card = NULL);
    bool canWeCombo(GameObserver* observer,MTGCardInstance * card = NULL,AIPlayerBaka * Ai = NULL);
    vector<string> mCastOrder();
    void add(string line);
    ~AIHints();
};

#endif
