#include "PlayGameState.h"

#include "../utils/Arduboy2Ext.h"
#include "../images/Images.h"


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void PlayGameState::activate(StateMachine & machine) {

  auto & gameStats = machine.getContext().gameStats;

  this->gameOver = false;
  this->angel.setEnabled(false);
  this->paused = false;
  this->transitionToRace = false;

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void PlayGameState::update(StateMachine & machine) { 

	auto & arduboy = machine.getContext().arduboy;
  auto & gameStats = machine.getContext().gameStats;
	auto justPressed = arduboy.justPressedButtons();
	auto pressed = arduboy.pressedButtons();

  if (!this->paused) {

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

        switch (gameStats.score) {

          case 0 ... 10:
            this->victimDelay = random(VICTIM_DELAY_0_MIN, VICTIM_DELAY_0_MAX);
            this->victimLevel = 0;
            break;

          case 11 ... 20:
            this->victimDelay = random(VICTIM_DELAY_1_MIN, VICTIM_DELAY_1_MAX);
            this->victimLevel = random(2);
            break;

          default:
            this->victimDelay = random(VICTIM_DELAY_2_MIN, VICTIM_DELAY_2_MAX);
            this->victimLevel = random(3);
            break;

        }

        this->victimCountdown = VICTIM_COUNTDOWN;

      }

    }


    // Is a victim ready to jump?

    if (arduboy.everyXFrames(30) && !this->transitionToRace) {

      if (this->victimCountdown > 0) {

        this->victimCountdown--;

        if (this->victimCountdown == 0) {

          uint8_t nextAvailableVictim = getNextAvailable();
          this->victims[nextAvailableVictim].init();

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

    // }


      // Update the puff index on any victims mid flight ..

    // if (arduboy.everyXFrames(6)) {

      for (auto &victim : victims) {

        if (victim.getEnabled() && victim.getPuffIndex() > 0) {

          victim.incPuffIndex();

        }

      }

    }

    if (gameStats.misses == 3 && allVictimsDisabled()) {

      this->gameOver = true;

    }



    // Update ambulance lights ..

    if (arduboy.everyXFrames(8)) {
      this->lights = (this->lights == LightsState::Lights_1 ? LightsState::Lights_2 : LightsState::Lights_1);
    }


    // Update smoke graphic ..

    #ifndef DEBUG
    if (arduboy.everyXFrames(16)) {
      this->smokeIndex++;
      if (this->smokeIndex >= 5) this->smokeIndex = 0;
    }
    #endif

  }


  // Transition to race ..

  if (gameStats.score > 5) {

    this->transitionToRace = true;

  }

  if (this->transitionToRace) {

    if (allVictimsDisabled()) {
      gameStats.xPosition = this->player.getX();
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

    if (justPressed & B_BUTTON) {
      this->paused = !this->paused; 
    }

  }

}


// ----------------------------------------------------------------------------
//  Get index of next available victim.
//
uint8_t PlayGameState::getNextAvailable() {

  for (uint8_t i = 0; i < VICTIMS_MAX_NUMBER; i++) {

    if (!this->victims[i].getEnabled()) {
      return i;
    }

  }

  return 255;

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

  {
    #ifndef DEBUG

      uint8_t x = cloud_X_Pos[this->smokeIndex];
      uint8_t y = cloud_Y_Pos[this->smokeIndex];

      if (gameStats.timeOfDay == TimeOfDay::Day) {
        SpritesB::drawErase(89, 0, Images::Scoreboard, 0);
        SpritesB::drawOverwrite(x, y, pgm_read_word_near(&Images::Smoke_Day[this->smokeIndex]), 0);
      }
      else {
        SpritesB::drawExternalMask(89, 0, Images::Scoreboard, Images::Scoreboard_Mask, 0, 0);
        SpritesB::drawOverwrite(x, y, pgm_read_word_near(&Images::Smoke_Night[this->smokeIndex]), 0);
      }

    #else

      if (gameStats.timeOfDay == TimeOfDay::Day) {
        SpritesB::drawErase(89, 0, Images::Scoreboard, 0);
      }
      else {
        SpritesB::drawExternalMask(89, 0, Images::Scoreboard, Images::Scoreboard_Mask, 0, 0);
      }

    #endif
  }

  BaseState::drawCommonScenery(machine);


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

  renderScore(machine, gameStats.timeOfDay, gameStats.score, 124, 3);


  uint8_t i = this->player.getImageIndex();

  SpritesB::drawExternalMask(this->player.getX(), this->player.getY(), Images::FireMen, Images::FireMen_Mask, i, i);


  // Render foreground grass ..

  BaseState::drawLowerGrass(machine);
  // SpritesB::drawExternalMask(0, 59, Images::Grass, Images::Grass_Mask, 0, 0);


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

        SpritesB::drawExternalMask(victim.getX(), victim.getY(), Images::Puff, Images::Puff_Mask, puffIndex, puffIndex_Mask);

      }

    }

  }


  // Render victim about to jump ..

  if (this->victimCountdown > 0) {

    SpritesB::drawExternalMask(edgePos[this->victimLevel * 4], edgePos[(this->victimLevel * 4) + 1], Images::Victim_OnEdge_01, Images::Victim_OnEdge_01_Mask, 0, 0);

    if (this->victimCountdown % 2 == 0) {

      SpritesB::drawExternalMask(edgePos[(this->victimLevel * 4) + 2], edgePos[(this->victimLevel * 4) + 3], Images::Victim_OnEdge_02, Images::Victim_OnEdge_02_Mask, 0, 0);

    }

  }



  // Render angel if required ..

  if (this->angel.getEnabled() && this->angel.renderImage() && this->puffIndex <= 3) {

    uint8_t imageIndex = this->angel.getImageIndex();
    SpritesB::drawExternalMask(this->angel.getX(), this->angel.getY(), Images::Angels, Images::Angels_Mask, imageIndex, imageIndex);

  }

  SpritesB::drawExternalMask(96, 31, Images::Ambulance, Images::Ambulance_Mask, 0, 0);
  SpritesB::drawExternalMask(114, 31, Images::Ambulance_Lights, Images::Ambulance_Lights_Mask, static_cast<uint8_t>(this->lights), 0);

  if (this->puffIndex > 0) {

    if (gameStats.misses < 3) {

      uint8_t puffIndex_Mask = this->puffIndex - 1;
      uint8_t puffIndex = (puffIndex_Mask * 2) + (gameStats.timeOfDay == TimeOfDay::Night ? 1 : 0);

      SpritesB::drawExternalMask((gameStats.misses == 1 ? ANGEL_MISS_1_LEFT : ANGEL_MISS_2_LEFT) - 1, ANGEL_MISS_TOP, Images::Puff, Images::Puff_Mask, puffIndex, puffIndex_Mask);

    }

  }



  // Game Over?

  if (this->gameOver) {

    SpritesB::drawExternalMask(32, 20, Images::GameOver, Images::GameOver_Mask, 0, 0); 

  }

  // Pause?

  if (this->paused) {

    SpritesB::drawExternalMask(39, 20, Images::Pause, Images::Pause_Mask, 0, 0); 

  }

  arduboy.displayWithBackground(gameStats.timeOfDay);

}
