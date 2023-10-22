#include "Car.h"
#include "Ground.h"
#include "Wall.h"
#include "Post.h"
#include "Text.h"

#include <stdlib.h>
#include "glm.h"
#include "GL/glut.h"
#include <stdarg.h>
#include <iostream>
#include <memory.h>
#include <time.h>
#include <fstream>
#include <sstream>
#include <cmath>
using namespace std;

#define TIMER_STALL 30.0
#define TIMER_MOVE 75.0
#define TIMER_KILL 105.0

Text::Text() {
   timer = TIMER_KILL;
}
void Text::update() {
   if (timer <= TIMER_KILL) {
      timer++;
      float percent = 0;
      float newY = posY;
      if (timer <= TIMER_STALL) {
         percent = (TIMER_STALL-timer)/(TIMER_STALL);
         newY = posY - 40*pow(percent,2);
      }
      else if (timer >= TIMER_MOVE) {
         percent = (timer-TIMER_MOVE)/(TIMER_KILL-TIMER_MOVE);
         newY = posY + 40*pow(percent,2);
      }
      glEnable(GL_BLEND);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glColor4f(1, 1, 1, 1-percent);
      int length = text.length();
      glRasterPos2f(posX - length*4.2, newY);
      void *font = GLUT_BITMAP_TIMES_ROMAN_24;
      for (int i=0; i<length; i++)
         glutBitmapCharacter(font, text[i]);
      glDisable(GL_BLEND);
   }
}
void Text::reset() {
   timer = 0;
}
void Text::setPosition(float pX, float pY) {
   posX = pX;
   posY = pY;
}
void Text::setText(string t) {
   text = t;
}
float Text::getPosX() {
   return posX;
}
float Text::getPosY() {
   return posY;
}
string Text::getText() {
   return text;
}
int Text::getTimer() {
   return timer;
}
Text::~Text(void) {
}
