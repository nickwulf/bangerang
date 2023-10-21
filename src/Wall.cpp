#include ".\wall.h"
#include ".\post.h"
#include ".\car.h"
#include ".\ground.h"
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
#define DETECT_HEIGHT 1.15

Wall::Wall(void) {
}

void Wall::draw() {
   if (on) {
      glPushMatrix();
         glEnable(GL_COLOR_MATERIAL);
         GLfloat diffuseMat[] = {2, 2, 2, 1};
         glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseMat);
         GLfloat ambientMat[] = {0, 0, 0, 1};
         glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuseMat);
         for (int j=0; j<10; j++) {
         float Start1X,Start1Y,Start1Z,End1X,End1Y,End1Z;
         float Start2X,Start2Y,Start2Z,End2X,End2Y,End2Z;
            Start1X = j*(verts[3][0]-verts[0][0])/10 + verts[0][0];
            Start1Y = j*(verts[3][1]-verts[0][1])/10 + verts[0][1];
            Start1Z = j*(verts[3][2]-verts[0][2])/10 + verts[0][2];
            End1X = j*(verts[2][0]-verts[1][0])/10 + verts[1][0];
            End1Y = j*(verts[2][1]-verts[1][1])/10 + verts[1][1];
            End1Z = j*(verts[2][2]-verts[1][2])/10 + verts[1][2];
            Start2X = (j+1)*(verts[3][0]-verts[0][0])/10 + verts[0][0];
            Start2Y = (j+1)*(verts[3][1]-verts[0][1])/10 + verts[0][1];
            Start2Z = (j+1)*(verts[3][2]-verts[0][2])/10 + verts[0][2];
            End2X = (j+1)*(verts[2][0]-verts[1][0])/10 + verts[1][0];
            End2Y = (j+1)*(verts[2][1]-verts[1][1])/10 + verts[1][1];
            End2Z = (j+1)*(verts[2][2]-verts[1][2])/10 + verts[1][2];
            for (int i=0; i<10; i++) {
               float point[4][3]; // point[pointNum][pointCoordinate], points are in clockwise order
               point[0][0] = i*(End1X-Start1X)/10 + Start1X;
               point[0][1] = i*(End1Y-Start1Y)/10 + Start1Y;
               point[0][2] = i*(End1Z-Start1Z)/10 + Start1Z;
               point[1][0] = (i+1)*(End1X-Start1X)/10 + Start1X;
               point[1][1] = (i+1)*(End1Y-Start1Y)/10 + Start1Y;
               point[1][2] = (i+1)*(End1Z-Start1Z)/10 + Start1Z;
               point[2][0] = (i+1)*(End2X-Start2X)/10 + Start2X;
               point[2][1] = (i+1)*(End2Y-Start2Y)/10 + Start2Y;
               point[2][2] = (i+1)*(End2Z-Start2Z)/10 + Start2Z;
               point[3][0] = i*(End2X-Start2X)/10 + Start2X;
               point[3][1] = i*(End2Y-Start2Y)/10 + Start2Y;
               point[3][2] = i*(End2Z-Start2Z)/10 + Start2Z;
               if ((i+j)%2 == 1) glColor3f(.1333,.6941,.2980);
               else glColor3f(.8980,.6667,.4784);
               glNormal3f(norm[0],norm[1],norm[2]);
               glBegin(GL_POLYGON);
                  glVertex3f(point[0][0], point[0][1], point[0][2]);
                  glVertex3f(point[1][0], point[1][1], point[1][2]);
                  glVertex3f(point[2][0], point[2][1], point[2][2]);
                  glVertex3f(point[3][0], point[3][1], point[3][2]);
               glEnd();
            }
         }
      glPopMatrix();
   }
}

float Wall::testInFrontOf(float point[], float tall, float carNorm[], float wallPoint[], int num) {
   float distance = 1000;
   if (on) {
      int negative = 1;
      if (norm[0]*point[0]+norm[1]*point[1]+norm[2]*point[2]+norm[3] < 0) negative = -1; // if Ax+By+Cz+D=0 then y=-(Ax+Cz+D)/B
      float mag = negative*sqrt(pow(norm[0],2) + pow(norm[1],2) + pow(norm[2],2));
      float newNorm[3] = {norm[0]/mag, norm[1]/mag, norm[2]/mag};  // normalized normal of wall pointing in direction of point
      // A dot B = |A||B|cos(theta);  A = newNorm;  B = carNorm
      float mag2 = sqrt(pow(carNorm[0],2) + pow(carNorm[1],2) + pow(carNorm[2],2));
      float heightRatio = tan(DEG_TO_RAD*(90 - RAD_TO_DEG*acos((newNorm[0]*carNorm[0] + newNorm[1]*carNorm[1] + newNorm[2]*carNorm[2])/mag2)));
      if (verts[0][0] == verts[1][0]) {  // if slope = infinite
         float meetZ = point[2];
         float meetX = verts[0][0];
         float disTest = sqrt(pow(point[0]-meetX,2) + pow(point[2]-meetZ,2));
         float percent = (meetZ-verts[0][2])/(verts[1][2]-verts[0][2]);   // find the percent of the Z dis between meetZ & v1 out of the dis between v1 & v2
         float topY = percent*(verts[1][1]-verts[0][1]) + verts[0][1];   // to find the height of the top side at the intersection point
         float botY = percent*(verts[2][1]-verts[3][1]) + verts[3][1];   // to find the height of the bottom side at the intersection point
         if (((meetZ<=verts[0][2])^(meetZ<=verts[1][2])) && ((point[1]<=topY-(heightRatio*disTest)+DETECT_HEIGHT) && (point[1]+tall>=botY-(heightRatio*disTest))))
            distance = disTest;
         wallPoint[0] = meetX;
         wallPoint[1] = point[1];
         wallPoint[2] = meetZ;
      }
      else if (verts[0][2] == verts[1][2]) {  // if slope = 0
         float meetZ = verts[0][2];
         float meetX = point[0];
         float disTest = sqrt(pow(point[0]-meetX,2) + pow(point[2]-meetZ,2));
         float percent = (meetX-verts[0][0])/(verts[1][0]-verts[0][0]);   // find the percent of the X dis between meetX & v1 out of the dis between v1 & v2
         float topY = percent*(verts[1][1]-verts[0][1]) + verts[0][1];   // to find the height of the top side at the intersection point
         float botY = percent*(verts[2][1]-verts[3][1]) + verts[3][1];   // to find the height of the bottom side at the intersection point
         if (((meetX<=verts[0][0])^(meetX<=verts[1][0])) && ((point[1]<=topY-(heightRatio*disTest)+DETECT_HEIGHT) && (point[1]+tall>=botY-(heightRatio*disTest))))
            distance = disTest;
         wallPoint[0] = meetX;
         wallPoint[1] = point[1];
         wallPoint[2] = meetZ;
      }
      else {
         float slope = (verts[1][2]-verts[0][2])/(verts[1][0]-verts[0][0]);   // slope = (z2-z1)/(x2-x1)
         float zIntercept = verts[0][2]-(slope*verts[0][0]);            // intercept = z1 - slope*x1
         float slopePerp = -1/slope;                           // Find slope and Y-intercept of line perpendicular to ground
         float zInterPerp = point[2] - slopePerp*point[0];         // and that goes through the point (bX,bY)
         float meetX = (zInterPerp-zIntercept)/(slope-slopePerp);   // Find the point on the ground where
         float meetZ = meetX*slope + zIntercept;                  // the perpendicular line intersects it
         float disTest = sqrt(pow(point[0]-meetX,2) + pow(point[2]-meetZ,2));
         float percent = (meetX-verts[0][0])/(verts[1][0]-verts[0][0]);   // find the percent of the X dis between meetX & v1 out of the dis between v1 & v2
         float topY = percent*(verts[1][1]-verts[0][1]) + verts[0][1];   // to find the height of the top side at the intersection point
         float botY = percent*(verts[2][1]-verts[3][1]) + verts[3][1];   // to find the height of the bottom side at the intersection point
         if (((meetX<=verts[0][0])^(meetX<=verts[1][0])) && ((point[1]<=topY-(heightRatio*disTest)+DETECT_HEIGHT) && (point[1]+tall>=botY-(heightRatio*disTest))))
            distance = disTest;
         wallPoint[0] = meetX;
         wallPoint[1] = point[1];
         wallPoint[2] = meetZ;
      }
   }
   return distance;
}

void Wall::setVertices(float v[][3]) {
   for (int i=0; i<4; i++) {
      for (int j=0; j<3; j++) {
            verts[i][j] = v[i][j];
      }
   }
   float x3 = v[0][0];  // find the normal
   float y3 = v[0][1];
   float z3 = v[0][2];
   float x2 = v[1][0];
   float y2 = v[1][1];
   float z2 = v[1][2];
   float x1 = (v[2][0]+v[3][0])/2;
   float y1 = (v[2][1]+v[3][1])/2;
   float z1 = (v[2][2]+v[3][2])/2;
   norm[0] = y1*(z2-z3) + y2*(z3-z1) + y3*(z1-z2);
   norm[1] = z1*(x2-x3) + z2*(x3-x1) + z3*(x1-x2);
   norm[2] = x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);
   norm[3] = -(x1*(y2*z3-y3*z2) + x2*(y3*z1-y1*z3) + x3*(y1*z2-y2*z1));
}

void Wall::setOn(bool value) {
   on = value;
}

void Wall::getVertices(float v[][3]) {
   for (int i=0; i<4; i++) {
      for (int j=0; j<3; j++) {
         v[i][j] = verts[i][j];
      }
   }
}

void Wall::getNormal(float n[]) {
   for (int i=0; i<3; i++) {
      n[i] = norm[i];
   }
}

bool Wall::getOn() {
   return on;
}

 Wall::~Wall(void) {
}
