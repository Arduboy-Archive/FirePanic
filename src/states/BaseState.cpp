#include "BaseState.h"

#include "../images/Images.h"
#include "../utils/Utils.h"
#include "../utils/Enums.h"

void BaseState::renderScore(StateMachine & machine, TimeOfDay timeOfDay, uint16_t score, uint8_t x, uint8_t y) {

	auto & arduboy = machine.getContext().arduboy;

	uint8_t digits[6] = {};
	extractDigits(digits, score);

	for (uint8_t j = 6; j > 0; --j) {

    if (timeOfDay == TimeOfDay::Day) {
      SpritesB::drawErase(x - (j*5), y, Images::Scoreboard_Numbers, digits[j - 1]);
    }
    else {
      SpritesB::drawSelfMasked(x - (j*5), y, Images::Scoreboard_Numbers, digits[j - 1]);
    }

	}

}


void BaseState::drawCommonScenery(StateMachine & machine, bool incSmoke) {

	auto & arduboy = machine.getContext().arduboy;

  for (uint8_t i = 0; i <= 120; i = i + 8) {
    SpritesB::drawExternalMask(i, 28, Images::Grass, Images::Grass_Mask, 0, 0);
  }

  SpritesB::drawExternalMask(0, 51, Images::Ground, Images::Ground_Mask, 0, 0);
  SpritesB::drawExternalMask(0, 0, Images::Building, Images::Building_Mask, 0, 0);


  // Draw smoke if specified ..

  #ifndef DEBUG

  uint8_t x = cloud_X_Pos[this->smokeIndex];
  uint8_t y = cloud_Y_Pos[this->smokeIndex];
  SpritesB::drawOverwrite(x, y, pgm_read_word_near(&Images::Smoke_Day[this->smokeIndex]), 0);

  if (arduboy.everyXFrames(16)) {
    this->smokeIndex++;
    if (this->smokeIndex >= 5) this->smokeIndex = 0;
  }

  #endif

}


void BaseState::drawLowerGrass(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;

  for (uint8_t i = 0; i <= 120; i = i + 8) {
    SpritesB::drawExternalMask(i, 59, Images::Grass, Images::Grass_Mask, 0, 0);
  }

}
