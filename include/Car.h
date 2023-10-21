#pragma once

class Car
{
public:
   Car(void);
   void draw();
   void updateMovement(int keyDown[]);
   void updateWall(float wPoint[], float distance);
   void updateOthers(float push[], float mag);
   void updatePosition();
   void setPosition(float pos[]);
   void setVelocity(float vel[]);
   void setAngle(float ang);
   void setOmega(float omg);
   void setRadius(float rad);
   void setTall(float t);
   void setElevation(float elev);
   void setNormal(float n[]);
   void setId(int i);
   void setTackle(int tack);
   void setDamage(float dam);
   void setGroundOld(float gold);
   void setAirAngle(float aang);
   void getPosition(float pos[]);
   void getVelocity(float vel[]);
   float getAngle();
   float getOmega();
   float getRadius();
   float getTall();
   float getElevation();
   void getNormal(float n[]);
   int getId();
   int getTackle();
   float getDamage();
   float getGroundOld();
   float getAirAngle();
   virtual ~Car(void);
public:
   float posX;
   float posY;
   float posZ;
   float velX;
   float velY;
   float velZ;
   float angle;
   float omega;
   float radius;
   float tall;
   float elevation; // how high the ground is below the car
   float norm[3]; // normal to the plane of the car (normalized)
   int id;
   int tackle;
   float damage;
   float groundOld;
   float airAngle;
};
