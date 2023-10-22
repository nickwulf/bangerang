#include "Post.h"
#include "Wall.h"
#include "Car.h"
#include "Ground.h"
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

// Definitions
#define DEG_TO_RAD 0.0174532925
#define RAD_TO_DEG 57.29577951
#define DETECT_HEIGHT 1.15

Post::Post(void) {
}

float Post::testNear(float point[], float tall, float carNorm[], float postPoint[]) {
   float distance = 1000;
   if (on) {
      int negative = 1;
      float norm[3] = {point[0]-posX, point[1]-posY, point[2]-posZ};
      if (norm[0]*point[0]+norm[1]*point[1]+norm[2]*point[2]+norm[3] < 0) negative = -1; // if Ax+By+Cz+D=0 then y=-(Ax+Cz+D)/B
      float mag = negative*sqrt(pow(norm[0],2) + pow(norm[1],2) + pow(norm[2],2));
      float newNorm[3] = {norm[0]/mag, norm[1]/mag, norm[2]/mag};  // normalized normal of wall pointing in direction of point
      // A dot B = |A||B|cos(theta);  A = newNorm;  B = carNorm
      mag = sqrt(pow(carNorm[0],2) + pow(carNorm[1],2) + pow(carNorm[2],2));
      float heightRatio = tan(DEG_TO_RAD*(90 - RAD_TO_DEG*acos((newNorm[0]*carNorm[0] + newNorm[1]*carNorm[1] + newNorm[2]*carNorm[2])/mag)));
      float disTest = sqrt(pow(point[0]-posX,2) + pow(point[2]-posZ,2));

      if ((point[1]<=height-(heightRatio*disTest)+posY+DETECT_HEIGHT) && (point[1]+tall>=posY-(heightRatio*disTest)))
         distance = disTest;
      postPoint[0] = posX;
      postPoint[1] = point[1];
      postPoint[2] = posZ;
   }
   return distance;
}

void Post::setPosition(float p[]) {
   for (int i=0; i<3; i++) {
      posX = p[0];
      posY = p[1];
      posZ = p[2];
   }
}

void Post::setHeight(float h) {
   height = h;
}

void Post::setOn(bool value) {
   on = value;
}

void Post::getPosition(float p[]) {
   for (int i=0; i<3; i++) {
      p[0] = posX;
      p[1] = posY;
      p[2] = posZ;
   }
}

float Post::getHeight() {
   return height;
}

bool Post::getOn() {
   return on;
}

Post::~Post(void) {
}
