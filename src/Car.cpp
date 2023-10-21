#include ".\car.h"
#include ".\ground.h"
#include ".\wall.h"
#include ".\post.h"
#include ".\text.h"

#include <stdlib.h>
#include "glm.h"
#include "GL\glut.h"
#include <stdarg.h>
#include <iostream>
#include <memory.h>
#include <time.h>
#include <fstream>
#include <sstream>
#include <cmath>
using namespace std;

// Definitions
#define DEG_TO_RAD 0.0174532925
#define RAD_TO_DEG 57.29577951

Car::Car(void) {
   posX = 0;
   posY = 50;
   posZ = 0;
   velX = 0;
   velY = 0;
   velZ = 0;
   angle = 0;
   omega = 0;
   radius = 5;
   tall = 5;
   norm[0] = 0;
   norm[1] = 1;
   norm[2] = 0;
   id = 0;
   tackle = 0;
   damage = 0;
}

void Car::draw() {
   glPushMatrix();
      glEnable(GL_COLOR_MATERIAL);
      GLfloat diffuseMat[] = {1, 1, 1, 1};
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseMat);
      GLfloat ambientMat[] = {1, 1, 1, 1};
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuseMat);
      glTranslatef(posX, posY, posZ);
      glRotatef(RAD_TO_DEG*acos(norm[1]),norm[2],0,-norm[0]);
      glRotatef(angle, 0, -1, 0);
      if (id == 1) glColor3f(0,0,1);
      if (id == 2) glColor3f(1,0,0);
      glNormal3f(0, 1, 0);
      glBegin(GL_POLYGON); // top
         glVertex3f(-7.5, 5, -5);
         glVertex3f(2.5, 5, -5);
         glVertex3f(2.5, 5, 5);
         glVertex3f(-7.5, 5, 5);
      glEnd();
      glNormal3f(-1, 0, 0);
      glBegin(GL_POLYGON); // back
         glVertex3f(-7.5, 5, -5);
         glVertex3f(-7.5, 5, 5);
         glVertex3f(-7.5, 0, 5);
         glVertex3f(-7.5, 0, -5);
      glEnd();
      glNormal3f(0, 0, 1);
      glBegin(GL_POLYGON); // right
         glVertex3f(-7.5, 5, 5);
         glVertex3f(2.5, 5, 5);
         glVertex3f(2.5, 0, 5);
         glVertex3f(-7.5, 0, 5);
      glEnd();
      glNormal3f(0, 0, -1);
      glBegin(GL_POLYGON); // left
         glVertex3f(-7.5, 5, -5);
         glVertex3f(2.5, 5, -5);
         glVertex3f(2.5, 0, -5);
         glVertex3f(-7.5, 0, -5);
      glEnd();
      glNormal3f(0, -1, 0);
      glBegin(GL_POLYGON); // bottom
         glVertex3f(-7.5, 0, -5);
         glVertex3f(2.5, 0, -5);
         glVertex3f(7.5, 0, 0);
         glVertex3f(2.5, 0, 5);
         glVertex3f(-7.5, 0, 5);
      glEnd();
      glNormal3f(1, 1, 0);
      glBegin(GL_POLYGON); // nose top
         glVertex3f(2.5, 5, -5);
         glVertex3f(7.5, 0, 0);
         glVertex3f(2.5, 5, 5);
      glEnd();
      glNormal3f(1, 0, 1);
      glBegin(GL_POLYGON); // nose left
         glVertex3f(2.5, 5, 5);
         glVertex3f(7.5, 0, 0);
         glVertex3f(2.5, 0, 5);
      glEnd();
      glNormal3f(1, 0, -1);
      glBegin(GL_POLYGON); // nose right
         glVertex3f(2.5, 5, -5);
         glVertex3f(7.5, 0, 0);
         glVertex3f(2.5, 0, -5);
      glEnd();
   glPopMatrix();
}

void Car::updateMovement(int keyDown[]) {
   float xzFriction = 0.995;
   float yFriction = 0.995;
   float acc = 0;
   if ((id == 1) && (tackle == 0)) {
      if (keyDown['w'] == 1 && keyDown['s'] != 1) { // go forwards
         acc = 0.05;
      }
      if (keyDown['s'] == 1 && keyDown['w'] != 1) { // go backwards
         acc = -0.05;
      }
      if (keyDown['a'] == 1 && keyDown['d'] != 1) { // turn left
         omega -= .5;
      }
      if (keyDown['d'] == 1 && keyDown['a'] != 1) { // turn right
         omega += .5;
      }
      if (keyDown['j'] == 1) { // jump
         keyDown['j'] = 2;
         if (posY < elevation + 3.5) {
            float jump = 1;
            velX += jump*norm[0]; // Make car jump up in direction of normal
            velY += jump*norm[1]; //   if the car is close enough to the ground
            velZ += jump*norm[2];
         }
      }
      if (keyDown[' '] == 1) { // jump
         keyDown[' '] = 2;
         if (posY < elevation + 3.5) {
            tackle = 1;
         }
      }
   }

   if (tackle > 0) {
      tackle++;
      if (tackle < 5) acc = 1.5;
      else if (tackle < 60) xzFriction = .92;
      else tackle = 0;
   }

   omega *= 0.85;  // angular friction
   angle += omega;  // increment angle
   airAngle += omega;

   // A x B = (AyBz-ByAz)i + (AzBx-BzAx)j + (AxBy-BxAy)k
   // A = xz-plane vector pointing in same direction as car, B = normal vector
   float Ax = cos(angle*DEG_TO_RAD);
   float Az = sin(angle*DEG_TO_RAD);
   float sideVector[3]; // vector pointing towards right of car, sideVector = A x B
   sideVector[0] = -norm[1]*Az;
   sideVector[1] = Az*norm[0] - norm[2]*Ax;
   sideVector[2] = Ax*norm[1];
   float faceVector[3]; // vector pointing in same direction as car
   // A = normal vector, B = side vector, faceVector = A x B
   faceVector[0] = norm[1]*sideVector[2] - sideVector[1]*norm[2];
   faceVector[1] = norm[2]*sideVector[0] - sideVector[2]*norm[0];
   faceVector[2] = norm[0]*sideVector[1] - sideVector[0]*norm[1];

   velY += -.03;
   if (posY <= -100) {
      posX = 0;
      posY = 100;
      posZ = 0;
      velX = 0;
      velY = 0;
      velZ = 0;
      norm[0] = 0;
      norm[1] = 1;
      norm[2] = 0;
      groundOld = -1;
   }

   // ground collision response
   if (posY < elevation + 3.5) {
      velX += acc*faceVector[0]; // Make car move forward due to input
      velY += acc*faceVector[1]; //   if the car is close enough to the ground
      velZ += acc*faceVector[2];
   }

   float velNorm = velX*norm[0] + velY*norm[1] + velZ*norm[2]; // use dot-product to calculate velocity in direction of plane normal
   float accNorm = 0;
   if (posY < elevation+2) {
      if (velNorm <= -.01) accNorm = velNorm * (1/pow(elevation+3.15-posY,3)) - velNorm;  // inelastic collision: loss of energy when pushing into ground
      if (velNorm <= 2) accNorm += 0.5*(elevation+2-posY);  // elastic collision: bounce the car depending on how much it is depressed into ground (spring model)
      if (velNorm >= 2 || accNorm > (2-velNorm)) accNorm = 2 - velNorm; // don't let the ground accelerate or speed up the car too much
      velX += accNorm*norm[0];  // apply the change in velocity along the normal to the velocity cartesian coordinates
      velY += accNorm*norm[1];  // models a normal force like in physics
      velZ += accNorm*norm[2];
   }
   velX *= xzFriction;  // Air friction (this is actually a model for ground friction since wind resistance is
   velY *= yFriction;  //   proportional to the velocity SQUARED, however its calculations are much more complicated
   velZ *= xzFriction;  //   and don't really add to the experience or make the game better.
}

void Car::updateOthers(float push[], float mag) {
   float accNorm = 0;
   if (mag >= 0) {
      float pushAngle;
      if (push[0] == 0) {
         if (push[2] >= 0) pushAngle = 90;
         else pushAngle = -90;
      }
      else pushAngle = RAD_TO_DEG*atan(push[2]/(push[0]));  // grab angle of push projected to xz-plane
      if (push[0] < 0) pushAngle += 180;
      float pain = 0.8 * pow(mag*(1+cos(DEG_TO_RAD*(pushAngle-angle))),2);  // damage the car the more its back is toward collision
      damage += pain;
      accNorm += 0.4*(mag)*pow((damage+100)/100,2);  // elastic collision: bounce the car depending on how much it is depressed into other (spring model)
      velX += accNorm*push[0];  // apply the change in velocity along the normal to the velocity cartesian coordinates
      velY += accNorm*push[1];  // models a normal force like in physics
      velZ += accNorm*push[2];
      velY += .02*pain*pow((damage+100)/100,2);
   }
}

void Car::updateWall(float wPoint[], float wallDis) {
   float wallNorm[3] = {(posX-wPoint[0])/wallDis, (posY-wPoint[1])/wallDis, (posZ-wPoint[2])/wallDis};
   float velNorm = velX*wallNorm[0] + velY*wallNorm[1] + velZ*wallNorm[2]; // use dot-product to calculate velocity in direction of plane normal
   float accNorm = 0;
   if (wallDis < radius) {
      if (velNorm <= 0) accNorm = velNorm * (1/pow(radius+1.0-wallDis,1)) - velNorm;  // inelastic collision: loss of energy when pushing into wall
      if (velNorm <= 2) accNorm += 0.3*(radius-wallDis);  // elastic collision: bounce the car depending on how much it is depressed into wall (spring model)
      if (velNorm >= 2 || accNorm > (2-velNorm)) accNorm = 2 - velNorm; // don't let the ground accelerate or speed up the car too much
      velX += accNorm*wallNorm[0];  // apply the change in velocity along the normal to the velocity cartesian coordinates
      velY += accNorm*wallNorm[1];  // models a normal force like in physics
      velZ += accNorm*wallNorm[2];
   }
}

void Car::updatePosition() {
   posX += velX;
   posY += velY;   // increment position
   posZ += velZ;
}

void Car::setPosition(float pos[]) {
   posX = pos[0];
   posY = pos[1];
   posZ = pos[2];
}

void Car::setVelocity(float vel[]) {
   velX = vel[0];
   velY = vel[1];
   velZ = vel[2];
}

void Car::setAngle(float ang) {
   angle = ang;
}

void Car::setOmega(float omg) {
   omega = omg;
}

void Car::setRadius(float rad) {
   radius = rad;
}

void Car::setTall(float t) {
   tall = t;
}

void Car::setElevation(float elev) {
   elevation = elev;
}

void Car::setNormal(float n[]) {
   if (n[1] != 0) {  // this line makes sure that the floor is not vertical
      float magnitude = sqrt(pow(n[0],2)+pow(n[1],2)+pow(n[2],2));
      norm[0] = n[0]/magnitude;
      norm[1] = n[1]/magnitude; // divide by magnitudes to normalize a vector
      norm[2] = n[2]/magnitude;
   }
}

void Car::setId(int i) {
   id = i;
}

void Car::setTackle(int tack) {
   tackle = tack;
}

void Car::setDamage(float dam) {
   damage = dam;
}

void Car::getPosition(float pos[]) {
   pos[0] = posX;
   pos[1] = posY;
   pos[2] = posZ;
}

void Car::setGroundOld(float gold) {
   groundOld = gold;
   airAngle = 0;
}

void Car::setAirAngle(float aang) {
   airAngle = aang;
}

void Car::getVelocity(float vel[]) {
   vel[0] = velX;
   vel[1] = velY;
   vel[2] = velZ;
}

float Car::getAngle() {
   return angle;
}

float Car::getOmega() {
   return omega;
}

float Car::getRadius() {
   return radius;
}

float Car::getTall() {
   return tall;
}

float Car::getElevation() {
   return elevation;
}

void Car::getNormal(float n[]) {
   n[0] = norm[0];
   n[1] = norm[1];
   n[2] = norm[2];
}

int Car::getId() {
   return id;
}

int Car::getTackle() {
   return tackle;
}

float Car::getDamage() {
   return damage;
}

float Car::getGroundOld() {
   return groundOld;
}

float Car::getAirAngle() {
   return airAngle;
}

Car::~Car(void) {
}
