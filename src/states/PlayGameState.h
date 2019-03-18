#pragma once

#include "../utils/GameContext.h"
#include "../utils/GameState.h"
#include "../utils/Enums.h"
#include "../entities/Entities.h"
#include "BaseState.h"

class PlayGameState : public BaseState { //GameState<GameContext, GameStateType> {

  private:
 
    Victim victims[VICTIMS_MAX_NUMBER];
    Player player;
    Angel angel;

    uint8_t counter = 0;
    uint8_t puffIndex = 0;
    uint8_t victimCountdown = 200;
    uint8_t victimDelay = 1;
    uint8_t victimLevel = 0;

    bool gameOver = false;
    bool transitionToRace = false;

    uint8_t getNextAvailable(uint8_t gap);
    bool allVictimsDisabled();

  public:
    void activate(StateMachine & machine) override;
    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;

};