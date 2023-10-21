#include <fstream>
using namespace std;

#pragma once

class Text {
public:
   Text();
   void update();
   void reset();
   void setPosition(float pX, float pY);
   void setText(string t);
   float getPosX();
   float getPosY();
   string getText();
   int getTimer();
   virtual ~Text(void);
private:
   float posX;
   float posY;
   string text;
   int timer;
};
