#pragma once

class Post
{
public:
   Post(void);
   float testNear(float point[], float tall, float carNorm[], float postPoint[]);
   void setPosition(float p[]);
   void setHeight(float h);
   void setOn(bool value);
   void getPosition(float p[]);
   float getHeight();
   bool getOn();
   virtual ~Post(void);
private:
   float posX;
   float posY;
   float posZ;
   float height;
   bool on;
};
