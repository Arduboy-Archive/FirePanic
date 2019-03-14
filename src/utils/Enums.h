#pragma once

#include <stdint.h>
#include "Utils.h"

//#define DEBUG
//#define DEBUG_RACE
// #define SOUND

#define ANGEL_MISS_1_LEFT 76
#define ANGEL_MISS_2_LEFT 63
#define ANGEL_MISS_TOP 1

#define VICTIMS_MAX_NUMBER 10

#define VICTIM_ON_GROUND_1 43
#define VICTIM_ON_GROUND_2 50
#define VICTIM_HALO_DELAY 4
#define VICTIM_HALO_MAX 60
#define VICTIM_MISSED_TRAMPOLINE 1
#define VICTIM_BOUNCE_HEIGHT 37
#define VICTIM_WIDTH_HALF 8
#define VICTIM_IN_AMBULANCE 102


#define VICTIM_DELAY_0_MIN 150
#define VICTIM_DELAY_0_MAX 200
#define VICTIM_DELAY_1_MIN 120
#define VICTIM_DELAY_1_MAX 170
#define VICTIM_DELAY_2_MIN 80
#define VICTIM_DELAY_2_MAX 130
#define VICTIM_COUNTDOWN 6

#define PLAYER_MIN_X_POS 18
#define PLAYER_MID_X_POS 39
#define PLAYER_MAX_X_POS 60
#define PLAYER_WIDTH_HALF 20

#define ACCURACY_TOLERANCE 10

#define RACE_AMBULANCE_WIDTH 31
#define RACE_OTHERCAR_WIDTH 30
#define RACE_OTHERCARS_MAX 3

#define PLAYER_RANGE1_X_LEFT_START_POS PLAYER_MIN_X_POS
#define PLAYER_RANGE1_X_LEFT_END_POS PLAYER_RANGE1_X11_POS + 3
#define PLAYER_RANGE1_X_MID_START_POS PLAYER_MIN_X_POS
#define PLAYER_RANGE1_X_MID_END_POS PLAYER_RANGE1_X11_POS + 3
#define PLAYER_RANGE1_X_RIGHT_START_POS PLAYER_MID_X_POS - 4
#define PLAYER_RANGE1_X_RIGHT_END_POS PLAYER_MID_X_POS - 1

#define PLAYER_RANGE1_X_21_POS PLAYER_MID_X_POS
#define PLAYER_RANGE1_X_22_POS PLAYER_RANGE1_X21_POS + 3
#define PLAYER_RANGE1_X_23_POS PLAYER_MAX_X_POS - 4
#define PLAYER_RANGE1_X_24_POS PLAYER_MAX_X_POS - 1

#define PLAYER_Y_POS 42
#define PLAYER_STEP_INC 6 

#define NAME_LENGTH 3
#define NAME_LENGTH_PLUS_TERM (NAME_LENGTH + 1)

#define HS_NAME_LEFT 37
#define HS_SCORE_LEFT 73
#define HS_CHAR_TOP 21
#define HS_CHAR_V_SPACING 9
#define HS_PRESS_A_DELAY 100

#define NO_WINNER 255
// #define NUMBER_OF_FIREWORKS 5

#define EEPROM_START                  100
#define EEPROM_START_C1               101
#define EEPROM_START_C2               102
#define EEPROM_HS_NAME_1              103
#define EEPROM_HS_NAME_2              107
#define EEPROM_HS_NAME_3              111
#define EEPROM_HS_SCORE_1             115
#define EEPROM_HS_SCORE_2             117
#define EEPROM_HS_SCORE_3             119
#define EEPROM_END                    121

#define FLASH_FRAME_COUNT_2 56

const int8_t edgePos[] = { 5, 0, 22, 1, 5, 16, 22, 17, 5, 31, 22, 32 };
const uint8_t cloud_X_Pos[] = { 16, 16, 18, 18, 46 };
const uint8_t cloud_Y_Pos[] = { 10, 7, 0, 0, 0 };

enum class TimeOfDay : uint8_t {
  Day,
  Night,
};

enum class GameStateType : uint8_t {
  None,
	SplashScreen,
	TitleScreen,
  GameIntroScreen,
  GameIntroScreen_ChangeDay,
  PlayGameScreen,
  PlayRaceScreen,
  HighScoreScreen
};
  
enum class LightsState : uint8_t {
  Lights_1,
  Lights_2
};


enum class Direction : uint8_t {
  Up,     // 00
  Down,   // 01
  Left,   // 02
  Right,  // 03
  None
};

struct GameStats {

  public: 
    
    GameStats() { };

    uint16_t score = 0;
    uint16_t misses = 0;
    uint8_t xPosition = 0;
    TimeOfDay timeOfDay = TimeOfDay::Day;

    void resetGame() {

      score = 0;
      misses = 0;
      timeOfDay = TimeOfDay::Day;

    }

};