#include "GameIntroState.h"
#include "../utils/Arduboy2Ext.h"
#include "../images/Images.h"


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void GameIntroState::activate(StateMachine & machine) {
  	
  auto & gameStats = machine.getContext().gameStats;

  if (machine.getContext().nextState == GameStateType::PlayGameScreen) {

    this->xAmbulance = 156;
    this->xPlayer = PLAYER_MIN_X_POS + 6 + (30 * 3);

  }
  else {

    this->xAmbulance = 96;
    this->xPlayer = gameStats.xPosition;

  }

  this->counter = 0;
  this->ambulanceDoor = false;
  this->playerImageIndex = false;
  this->speedInc = 0;

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void GameIntroState::update(StateMachine & machine) { 

	auto & arduboy = machine.getContext().arduboy;
	auto justPressed = arduboy.justPressedButtons();
    
  if (arduboy.everyXFrames(2)) {

    if (machine.getContext().nextState == GameStateType::PlayGameScreen) {

      switch (counter) {

        case 0 ... 59:
          this->xAmbulance--;
          break;

        case 60 ... 75:
          break;

        case 76 ... 90:
          this->ambulanceDoor = true;
          break;

        case 91 ... 161:

          this->ambulanceDoor = true;

          if (arduboy.everyXFrames(4)) {

            switch (this->xPlayer) {

              case PLAYER_MIN_X_POS : break;

              case PLAYER_MIN_X_POS + 1 ... PLAYER_MIN_X_POS + 3:
                this->xPlayer = this->xPlayer - 1;
                break;

              case PLAYER_MIN_X_POS + 4 ... PLAYER_MIN_X_POS + 8:
                this->xPlayer = this->xPlayer - 2;
                break;

              default:
                this->xPlayer = this->xPlayer - 3;
                break;

            }

          }

          break;

        case 162 ... 190:
          this->ambulanceDoor = false;
          break;

        case 191:
          machine.changeState(machine.getContext().nextState, GameStateType::None); 
          break;

      }

      this->counter++;

    }
    else {

      switch (counter) {

        case 0 ... 20:
          break;

        case 21 ... 40:
          this->ambulanceDoor = true;
          break;

        case 41 ... 115:

          this->ambulanceDoor = true;

          if (arduboy.everyXFrames(4)) {

            switch (this->speedInc) {

              case 0 ... 1:
                this->xPlayer = this->xPlayer + 1;
                break;

              case 2 ... 3:
                this->xPlayer = this->xPlayer + 2;
                break;

              default:
                this->xPlayer = this->xPlayer + 3;
                break;

            }

            this->speedInc++;

          }

          break;

        case 116 ... 140:
          this->ambulanceDoor = false;
          break;

        case 141 ... 179:
          this->xAmbulance++;
          break;

        case 180:
          machine.changeState(machine.getContext().nextState, GameStateType::None); 
          break;
      }

      this->counter++;

    }

  }

  // Skip intro ..
  
  if (justPressed & A_BUTTON) machine.changeState(machine.getContext().nextState, GameStateType::None); 

  if (arduboy.everyXFrames(12) && this->xPlayer != PLAYER_MIN_X_POS) {
    this->playerImageIndex = !this->playerImageIndex;
  }
  

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void GameIntroState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & gameStats = machine.getContext().gameStats;

  BaseState::renderCommonScenery(machine, true, false);

  if (this->xPlayer < 100) {

    uint8_t image = this->playerImageIndex ? 1 : 0;
    SpritesB::drawExternalMask(xPlayer, PLAYER_Y_POS, Images::FireMen, Images::FireMen_Mask, image, image);

  }

  SpritesB::drawExternalMask(104, 28, Images::Grass, Images::Grass_Mask, 0, 0);
  SpritesB::drawOverwrite(104, 38, Images::Ground_RHS, 0);
  BaseState::renderLowerGrass(machine);


  // Draw Ambulance with lights ..

  BaseState::renderAmbulance(machine, this->xAmbulance, 31, this->ambulanceDoor);

  arduboy.displayWithBackground(gameStats.timeOfDay);

}
