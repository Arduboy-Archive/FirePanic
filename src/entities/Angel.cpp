#include "Angel.h"
#include "../utils/Physics.h"

Angel::Angel() { }

uint8_t Angel::getX() {

  return this->x;

}

uint8_t Angel::getY() {

  return this->y;

}

uint8_t Angel::getEnabled() {

  return this->enabled;

}

void Angel::setEnabled(bool value) {

  this->enabled = value;

}

void Angel::setSequence(uint8_t sequence) {

  this->sequence = sequence;

}

void Angel::init(uint8_t sequence) {

//  const uint8_t *angelArc = Angel_Arcs[sequence];

  this->sequence = sequence;
  this->enabled = true;
  this->posIndex = 0;  
//(uint8_t *)pgm_read_word_near(&Images::Victims[victim.getRotation()])

  // this->sequenceLen = pgm_read_byte(&angelArc[posIndex]);
  // this->x = pgm_read_byte(&angelArc[(this->posIndex * 2) + 1]);
  // this->y = pgm_read_byte(&angelArc[(this->posIndex * 2) + 2]);

  switch (this->sequence) {

    case 0:
      this->sequenceLen = pgm_read_byte(&Angel_Arc_1[posIndex]);
      this->x = pgm_read_byte(&Angel_Arc_1[(this->posIndex * 2) + 1]);
      this->y = pgm_read_byte(&Angel_Arc_1[(this->posIndex * 2) + 2]);
      break;

    case 1:
      this->sequenceLen = pgm_read_byte(&Angel_Arc_1[posIndex]);
      this->x = pgm_read_byte(&Angel_Arc_2[(this->posIndex * 2) + 1]);
      this->y = pgm_read_byte(&Angel_Arc_2[(this->posIndex * 2) + 2]);
      break;

    case 2:
      this->sequenceLen = pgm_read_byte(&Angel_Arc_1[posIndex]);
      this->x = pgm_read_byte(&Angel_Arc_3[(this->posIndex * 2) + 1]);
      this->y = pgm_read_byte(&Angel_Arc_3[(this->posIndex * 2) + 2]);
      break;

  }


  Serial.print(this->sequenceLen);
  Serial.print(" ");
  Serial.print(this->posIndex);
  Serial.print(" ");
  Serial.print(this->x);
  Serial.print(" ");
  Serial.println(this->y);
  
}

void Angel::move() {

  //const uint8_t *angelArc = Angel_Arcs[this->sequence];
  
  this->posIndex++;  

  if (this->posIndex == this->sequenceLen) {

    this->enabled = false;

  }
  else {

    switch (this->sequence) {

      case 0:
        this->x = pgm_read_byte(&Angel_Arc_1[(this->posIndex * 2) + 1]);
        this->y = pgm_read_byte(&Angel_Arc_1[(this->posIndex * 2) + 2]);
        break;

      case 1:
        this->x = pgm_read_byte(&Angel_Arc_2[(this->posIndex * 2) + 1]);
        this->y = pgm_read_byte(&Angel_Arc_2[(this->posIndex * 2) + 2]);
        break;

      case 2:
        this->x = pgm_read_byte(&Angel_Arc_3[(this->posIndex * 2) + 1]);
        this->y = pgm_read_byte(&Angel_Arc_3[(this->posIndex * 2) + 2]);
        break;

    }



    Serial.print(this->sequenceLen);
    Serial.print(" ");
    Serial.print(this->posIndex);
    Serial.print(" ");
    Serial.print(this->x);
    Serial.print(" ");
    Serial.println(this->y);
    // this->x = pgm_read_byte(&angelArc[(this->posIndex * 2) + 1]);
    // this->y = pgm_read_byte(&angelArc[(this->posIndex * 2) + 2]);


  }

}
