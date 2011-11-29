#ifndef _PHASERING_H_
#define _PHASERING_H_

#include <list>
#include <string>
#include "MTGDefinitions.h"
using namespace std;

/*
 The class that handles the phases of a turn
 */

class Player;
class GameObserver;

typedef enum
{
    BLOCKERS,
    TRIGGERS,
    ORDER,
    FIRST_STRIKE,
    END_FIRST_STRIKE,
    DAMAGE,
    END_DAMAGE
} CombatStep;

class Phase
{
public:
    GamePhase id;
    Player * player;
    Phase(GamePhase id, Player *player) :
        id(id), player(player)
    {
    }
    ;
};

class PhaseRing
{
private:
    bool extraDamagePhase(int id);
    GameObserver* observer;
public:
    list<Phase *> ring;
    list<Phase *>::iterator current;
    Phase * getCurrentPhase();
    Phase * forward(bool sendEvents = true);
    Phase * goToPhase(int id, Player * player, bool sendEvents = true);
    PhaseRing(GameObserver* observer);
    ~PhaseRing();
    int addPhase(Phase * phase);
    int addPhaseBefore(GamePhase id, Player* player, int after_id, Player * after_player, int allOccurences = 1);
    int removePhase(int id, Player * player, int allOccurences = 1);
    const char * phaseName(int id);
    static GamePhase phaseStrToInt(string s);

};

#endif
