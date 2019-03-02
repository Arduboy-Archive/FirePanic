#include "PlayGameState.h"

#include "../utils/Arduboy2Ext.h"
#include "../images/Images.h"


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void PlayGameState::activate(StateMachine & machine) {

  (void)machine;

  this->victims[0].init();
  this->angel.setEnabled(false);

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void PlayGameState::update(StateMachine & machine) { 

	auto & arduboy = machine.getContext().arduboy;
  auto & gameStats = machine.getContext().gameStats;
	auto pressed = arduboy.pressedButtons();


  // Update victim positions ..
  {
    uint8_t playerXCentre = this->player.getX() + PLAYER_WIDTH_HALF;

    if (arduboy.everyXFrames(5)) {

      for (uint8_t i = 0; i < VICTIMS_MAX_NUMBER; i++) {

        if (this->victims[i].getEnabled()) {

          uint8_t victimX = this->victims[i].getX();

          if (victimX == VICTIM_IN_AMBULANCE) {
            gameStats.score++;
          }
          else {

            uint8_t victimXCentre = victimX + VICTIM_WIDTH_HALF;
            uint8_t delta = absT(victimXCentre - playerXCentre);

            if (this->victims[i].getY() == VICTIM_BOUNCE_HEIGHT && delta > ACCURACY_TOLERANCE) {

              this->victims[i].setAlive(VICTIM_MISSED_TRAMPOLINE);

              switch (this->victims[i].getX()) {

                case PLAYER_MIN_X_POS ... PLAYER_MID_X_POS - 1:
                  this->angel.init(0);
                  break;

                case PLAYER_MID_X_POS ... PLAYER_MAX_X_POS - 1:
                  this->angel.init(1);
                  break;

                case PLAYER_MAX_X_POS ... WIDTH:
                  this->angel.init(2);
                  break;
                  

              }

              
            }  

          }

          this->victims[i].move();
          
        }

      }

    }

  }

  if (arduboy.everyXFrames(15)) {

    for (uint8_t i = 0; i < VICTIMS_MAX_NUMBER; i++) {

      if (this->victims[i].getEnabled()) {
        this->victims[i].rotate();
      }

    }

  }


  // Update player position ..

  if ((pressed & LEFT_BUTTON) && this->player.canMoveLeft())      { this->player.setPlayerDirection(PlayerDirection::Left); }
  if ((pressed & RIGHT_BUTTON) && this->player.canMoveRight())    { this->player.setPlayerDirection(PlayerDirection::Right); }
  
  if (arduboy.everyXFrames(2)) {

    this->player.move();

  }


  // Update angel ..
  
  if (this->angel.getEnabled()) {
    
    if (arduboy.everyXFrames(6)) {

      this->angel.move();

    }

  }



}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void PlayGameState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
  auto & gameStats = machine.getContext().gameStats;

  Sprites::drawExternalMask(0, 28, Images::Grass, Images::Grass_Mask, 0, 0);
  Sprites::drawExternalMask(0, 51, Images::Ground, Images::Ground_Mask, 0, 0);
  Sprites::drawExternalMask(0, 0, Images::Building, Images::Building_Mask, 0, 0);
  Sprites::drawExternalMask(89, 0, Images::Scoreboard, Images::Scoreboard_Mask, 0, 0);
  

  // Render misses ..

  if (gameStats.misses >= 1) { Sprites::drawExternalMask(76, 2, Images::Misses, Images::Misses_Mask, 0, 0); }
  if (gameStats.misses >= 2) { Sprites::drawExternalMask(63, 2, Images::Misses, Images::Misses_Mask, 0, 0); }


  // Render score ..

	uint8_t digits[6] = {};
	extractDigits(digits, gameStats.score);
	
	for (uint8_t j = 6; j > 0; --j) {
    Sprites::drawSelfMasked(124 - (j*5), 3, Images::Scoreboard_Numbers, digits[j]);
	}


  uint8_t i = 0;

  Sprites::drawExternalMask(96, 31, Images::Ambulance, Images::Ambulance_Mask, 0, 0);
  Sprites::drawExternalMask(this->player.getX(), this->player.getY(), (uint8_t *)pgm_read_word_near(&Images::Firemen[i]), (uint8_t *)pgm_read_word_near(&Images::Firemen_Mask[i]), 0, 0);
  
  // Render Foreground grass ..
  Sprites::drawExternalMask(0, 59, Images::Grass, Images::Grass_Mask, 0, 0);


  // Render victims ..

  for (uint8_t i = 0; i < VICTIMS_MAX_NUMBER; i++) {

    Victim &victim = this->victims[i];

    if (this->victims[i].getEnabled()) {


Serial.print(victim.getX());
Serial.print(" ");
Serial.println(victim.getY());
      Sprites::drawExternalMask(victim.getX(), victim.getY(), (uint8_t *)pgm_read_word_near(&Images::Victims[victim.getRotation()]), (uint8_t *)pgm_read_word_near(&Images::Victims_Mask[victim.getRotation()]), 0, 0);

      uint8_t isAlive = this->victims[i].getAlive();

      if (isAlive >= 2) {
        
        uint8_t haloIndex = victim.getHaloIndex();
        Sprites::drawExternalMask(victim.getX(), victim.getY() - 5, (uint8_t *)pgm_read_word_near(&Images::Victim_Halos[haloIndex]), (uint8_t *)pgm_read_word_near(&Images::Victim_Halos_Mask[haloIndex]), 0, 0);

      }

    }

  }


  // Render angel if required ..

  if (this->angel.getEnabled() && this->angel.renderImage()) {

    uint8_t imageIndex = this->angel.getImageIndex();
    Sprites::drawExternalMask(this->angel.getX(), this->angel.getY(), Images::Angels, Images::Angels_Mask, imageIndex, imageIndex);

  }

  arduboy.displayWithBackground(TimeOfDay::Day);

}
