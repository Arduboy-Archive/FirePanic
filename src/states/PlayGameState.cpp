#include "PlayGameState.h"

#include "../utils/Arduboy2Ext.h"
#include "../images/Images.h"


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void PlayGameState::activate(StateMachine & machine) {

  (void)machine;

  this->gameOver = false;
  this->angel.setEnabled(false);
  this->transitionToRace = false;

  BaseState::setPaused(false);

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void PlayGameState::update(StateMachine & machine) { 

	auto & arduboy = machine.getContext().arduboy;
  auto & gameStats = machine.getContext().gameStats;
	auto justPressed = arduboy.justPressedButtons();
	auto pressed = arduboy.pressedButtons();

  if (!BaseState::getPaused()) {

    // Update victim positions ..
    {
      uint8_t playerXCentre = this->player.getX() + PLAYER_WIDTH_HALF;

      if (arduboy.everyXFrames(5)) {

        for (auto &victim : victims) {

          if (victim.getEnabled()) {

            uint8_t victimX = victim.getX();

            if (victimX == VICTIM_IN_AMBULANCE) {
              gameStats.score++;
            }
            else {

              uint8_t victimXCentre = victimX + VICTIM_WIDTH_HALF;
              uint8_t delta = absT(victimXCentre - playerXCentre);

              if (victim.getY() == VICTIM_BOUNCE_HEIGHT && delta > ACCURACY_TOLERANCE) {

                victim.setAlive(VICTIM_MISSED_TRAMPOLINE);
                gameStats.misses++;

                switch (victim.getX()) {

                  case PLAYER_MIN_X_POS ... PLAYER_MID_X_POS - 1:
                    if (gameStats.misses < 3) {
                      this->angel.init(0, gameStats.misses);
                    }
                    break;

                  case PLAYER_MID_X_POS ... PLAYER_MAX_X_POS - 1:
                    if (gameStats.misses < 3) {
                      this->angel.init(1, gameStats.misses);
                    }
                    break;

                  case PLAYER_MAX_X_POS ... WIDTH:
                    if (gameStats.misses < 3) {
                      this->angel.init(2, gameStats.misses);
                    }
                    break;

                }
                
              }  

            }

            victim.move();
            
          }

        }

      }


      // If its the end of the game, remove all other victims ..

      if (gameStats.misses == 3) {

        for (auto &victim : victims) {

          if (victim.getEnabled() && victim.getAlive() == 0 && victim.getPuffIndex() == 0) {

            victim.incPuffIndex();

          }

        }

      }

    }


    // Rotate victims ..

    if (arduboy.everyXFrames(15)) {

      for (auto &victim : victims) {

        if (victim.getEnabled()) {
          victim.rotate();
        }

      }

    }


    // Launch a new victim?

    if (arduboy.everyXFrames(2) && gameStats.misses < 3) {

      this->victimDelay--;

      if (this->victimDelay == 0) {

        uint8_t maxDelay = 110;
        uint8_t minDelay = 80;
        uint8_t countdown = 30;
        
        if (gameStats.score < 180)  { maxDelay = 200 - (gameStats.score / 2); }
        if (gameStats.score < 140)  { minDelay = 150 - (gameStats.score / 2); }
        if (gameStats.score < 280)  { countdown = 100 - (gameStats.score / 4); }

        this->victimDelay = random(minDelay, maxDelay);
        this->victimCountdown = countdown;

        switch (gameStats.score) {

          case 0 ... 20:
            this->victimLevel = 0;
            break;

          case 21 ... 40:
            this->victimLevel = random(2);
            break;

          default:
            this->victimLevel = random(3);
            break;

        }

      }

    }


    // Is a victim ready to jump?

    if (!this->transitionToRace) {

      if (this->victimCountdown > 0 && this->victimCountdown < VICTIM_COUNTDOWN_NONE) {

        this->victimCountdown--;

      }

      if (this->victimCountdown == 0) {

        uint8_t gap = (gameStats.score > 120 ? 2 : (180 - gameStats.score) / 30);
        uint8_t nextAvailableVictim = getNextAvailable(gap);

        if (nextAvailableVictim != VICTIM_NONE_AVAILABLE) {
          this->victims[nextAvailableVictim].init();
          this->victimCountdown = VICTIM_COUNTDOWN_NONE;
        }

      }

    }



    // Update player position ..

    if ((pressed & LEFT_BUTTON) && this->player.canMoveLeft())      { this->player.setDirection(Direction::Left); }
    if ((pressed & RIGHT_BUTTON) && this->player.canMoveRight())    { this->player.setDirection(Direction::Right); }
    
    if (arduboy.everyXFrames(2)) {

      player.move();

    }


    // Update angel ..
      
    if (arduboy.everyXFrames(6)) {
    
      if (this->angel.getEnabled()) {

        if (this->angel.move(gameStats.misses)) {

          this->puffIndex++;

          if (this->puffIndex == 8) {

            this->angel.setEnabled(false);
            this->puffIndex = 0;

          }

        }
        else {

          this->puffIndex = 0;

        }
        
      }


      // Update the puff index on any victims mid flight ..

      for (auto &victim : victims) {

        if (victim.getEnabled() && victim.getPuffIndex() > 0) {

          victim.incPuffIndex();

        }

      }

    }

    if (gameStats.misses == 3 && allVictimsDisabled()) {

      this->gameOver = true;

    }

  }


  // Transition to race ..

  if (gameStats.score > gameStats.targetScore) {

    this->transitionToRace = true;

  }

  if (this->transitionToRace) {

    if (allVictimsDisabled()) {
      gameStats.xPosition = this->player.getX();
      gameStats.level++;
      machine.changeState(GameStateType::GameIntroScreen, GameStateType::PlayRaceScreen);
    }

  }


  // Handle other buttons ..

  if (this->gameOver) {

    if (justPressed & A_BUTTON) {
      machine.changeState(GameStateType::HighScoreScreen, GameStateType::None); 
    }

  }
  else {

    BaseState::handlePauseButton(machine);

  }

}


// ----------------------------------------------------------------------------
//  Get index of next available victim.
//
uint8_t PlayGameState::getNextAvailable(uint8_t gap) {

  // Serial.print(" ");
  // for (auto &victim : this->victims) {
  //   Serial.print(victim.getPosIndex());
  //   if (victim.getEnabled()) {
  //     Serial.print("E ");
  //   }
  //   else {
  //     Serial.print("D ");
  //   }
  // }
  // Serial.println(" ");

  for (auto &victim : this->victims) {

    uint8_t posIndex = victim.getPosIndex();

    if ( victim.getEnabled() && 
       (((posIndex < 23) && (absT(posIndex - 23) <= gap)) || ((posIndex < 45) && (absT(posIndex - 45) <= gap))) 
       ) {
      return VICTIM_NONE_AVAILABLE;
    }

  }

  for (uint8_t i = 0; i < VICTIMS_MAX_NUMBER; i++) {

    if (!this->victims[i].getEnabled()) {
      return i;
    }

  }

  return VICTIM_NONE_AVAILABLE;

}


// ----------------------------------------------------------------------------
//  Are all the victims disabled?
//
bool PlayGameState::allVictimsDisabled() {

  for (auto &victim : this->victims) {

    if (victim.getEnabled()) {
      return false;
    }

  }

  return true;

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void PlayGameState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
  auto & gameStats = machine.getContext().gameStats;

  BaseState::renderCommonScenery(machine, true, false);


  // Render misses ..

  switch (gameStats.misses) {

    case 0: break;

    case 1:
      if (!this->angel.getEnabled() || this->puffIndex >= 3) {
        SpritesB::drawExternalMask(ANGEL_MISS_1_LEFT, ANGEL_MISS_TOP, Images::Misses, Images::Misses_Mask, 0, 0); 
      }
      break;

    case 2:
      SpritesB::drawExternalMask(ANGEL_MISS_1_LEFT, ANGEL_MISS_TOP, Images::Misses, Images::Misses_Mask, 0, 0); 
      if (!this->angel.getEnabled() || this->puffIndex >= 3) {
        SpritesB::drawExternalMask(ANGEL_MISS_2_LEFT, ANGEL_MISS_TOP, Images::Misses, Images::Misses_Mask, 0, 0); 
      }
      break;
      
    default: 
      SpritesB::drawExternalMask(ANGEL_MISS_1_LEFT, ANGEL_MISS_TOP, Images::Misses, Images::Misses_Mask, 0, 0); 
      SpritesB::drawExternalMask(ANGEL_MISS_2_LEFT, ANGEL_MISS_TOP, Images::Misses, Images::Misses_Mask, 0, 0); 
      break;

  }


  // Render score ..

  BaseState::renderScore(machine, gameStats.timeOfDay);


  // Render firemen ..

  uint8_t i = this->player.getImageIndex();
  SpritesB::drawExternalMask(this->player.getX(), this->player.getY(), Images::FireMen, Images::FireMen_Mask, i, i);


  // Render foreground grass ..

  BaseState::renderLowerGrass(machine);


  // Render victims ..

  for (auto &victim : this->victims) {

    if (victim.getEnabled()) {

      if (victim.getPuffIndex() < 3) {

        uint8_t imageIndex = victim.getRotation();
        SpritesB::drawExternalMask(victim.getX(), victim.getY(), Images::Victims, Images::Victims_Mask, imageIndex, imageIndex);

        uint8_t isAlive = victim.getAlive();

        if (isAlive >= 2) {
          
          uint8_t haloIndexMask = victim.getHaloIndex();
          uint8_t haloIndex = haloIndexMask * 2;

          if (gameStats.timeOfDay == TimeOfDay::Night) haloIndex++; 
          SpritesB::drawExternalMask(victim.getX(), victim.getY() - 5, Images::Victim_Halos, Images::Victim_Halos_Mask, haloIndex, haloIndexMask);

        }

      }

      if (victim.getPuffIndex() > 0) {

        uint8_t puffIndex_Mask = victim.getPuffIndex() - 1;
        uint8_t puffIndex = (puffIndex_Mask * 2) + (gameStats.timeOfDay == TimeOfDay::Night ? 1 : 0);

        BaseState::renderPuff(victim.getX(), victim.getY(), puffIndex, puffIndex_Mask);

      }

    }

  }


  // Render victim about to jump ..

  if (this->victimCountdown < VICTIM_COUNTDOWN_NONE) {

    SpritesB::drawExternalMask(edgePos[this->victimLevel * 4], edgePos[(this->victimLevel * 4) + 1], Images::Victim_OnEdge_01, Images::Victim_OnEdge_01_Mask, 0, 0);

    if (this->victimCountdown % 30 < 15) {

      SpritesB::drawExternalMask(edgePos[(this->victimLevel * 4) + 2], edgePos[(this->victimLevel * 4) + 3], Images::Victim_OnEdge_02, Images::Victim_OnEdge_02_Mask, 0, 0);

    }

  }


  // Render angel if required ..

  if (this->angel.getEnabled() && this->angel.renderImage() && this->puffIndex <= 3) {

    uint8_t imageIndex = this->angel.getImageIndex();
    SpritesB::drawExternalMask(this->angel.getX(), this->angel.getY(), Images::Angels, Images::Angels_Mask, imageIndex, imageIndex);

  }

  BaseState::renderAmbulance(machine, 96, 31, false);

  if (this->puffIndex > 0) {

    if (gameStats.misses < 3) {

      uint8_t puffIndex_Mask = this->puffIndex - 1;
      uint8_t puffIndex = (puffIndex_Mask * 2) + (gameStats.timeOfDay == TimeOfDay::Night ? 1 : 0);

      BaseState::renderPuff((gameStats.misses == 1 ? ANGEL_MISS_1_LEFT : ANGEL_MISS_2_LEFT) - 1, ANGEL_MISS_TOP, puffIndex, puffIndex_Mask);

    }

  }

  BaseState::renderGameOverOrPause(this->gameOver);
  arduboy.displayWithBackground(gameStats.timeOfDay);

}
