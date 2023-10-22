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

// Definitions
#define DEG_TO_RAD 0.0174532925
#define RAD_TO_DEG 57.29577951

Ground::Ground(void){
}

void Ground::draw() {
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

bool Ground::testAbove(float point[], float rad) {
   if (on) {
      bool testInside = true;
      if (!testInsideLine(point, verts[0], verts[1])) testInside = false;
      if (!testInsideLine(point, verts[1], verts[2])) testInside = false;
      if (!testInsideLine(point, verts[2], verts[3])) testInside = false;
      if (!testInsideLine(point, verts[3], verts[0])) testInside = false;
      
      bool testNear = false;
      if (testNearVertex(point, rad, verts[0]) && !conCorner[0]) testNear = true;
      if (testNearVertex(point, rad, verts[1]) && !conCorner[1]) testNear = true;
      if (testNearVertex(point, rad, verts[2]) && !conCorner[2]) testNear = true;
      if (testNearVertex(point, rad, verts[3]) && !conCorner[3]) testNear = true;
      if (testNearLine(point, rad, verts[0], verts[1]) && !conSide[0]) testNear = true;
      if (testNearLine(point, rad, verts[1], verts[2]) && !conSide[1]) testNear = true;
      if (testNearLine(point, rad, verts[2], verts[3]) && !conSide[2]) testNear = true;
      if (testNearLine(point, rad, verts[3], verts[0]) && !conSide[3]) testNear = true;
      
      return testInside || testNear;
   }
   else return false;
}

float Ground::getHeight(float point[]) {
   float height = -1000;
   if (norm[1] != 0) height = -((norm[0]*point[0])+(norm[2]*point[2])+(norm[3]))/norm[1]; // if Ax+By+Cz+D=0 then y=-(Ax+Cz+D)/B
   return height;
}

void Ground::setVertices(float v[][3]) {
   for (int i=0; i<4; i++) {
      for (int j=0; j<3; j++) {
            verts[i][j] = v[i][j];
      }
   }
   center[0] = (verts[0][0]+verts[1][0]+verts[2][0]+verts[3][0])/4;
   center[1] = (verts[0][1]+verts[1][1]+verts[2][1]+verts[3][1])/4;
   center[2] = (verts[0][2]+verts[1][2]+verts[2][2]+verts[3][2])/4;
   float x3 = v[1][0];  // find the normal
   float y3 = v[1][1];
   float z3 = v[1][2];
   float x2 = v[2][0];
   float y2 = v[2][1];
   float z2 = v[2][2];
   float x1 = v[3][0];
   float y1 = v[3][1];
   float z1 = v[3][2];
   norm[0] = y1*(z2-z3) + y2*(z3-z1) + y3*(z1-z2);
   norm[1] = z1*(x2-x3) + z2*(x3-x1) + z3*(x1-x2);
   norm[2] = x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);
   norm[3] = -(x1*(y2*z3-y3*z2) + x2*(y3*z1-y1*z3) + x3*(y1*z2-y2*z1));
   if (norm[1] < 0) {
      norm[0] = -norm[0];
      norm[1] = -norm[1];
      norm[2] = -norm[2];
      norm[3] = -norm[3];
   }
}

void Ground::setOn(bool value) {
   on = value;
}

void Ground::setConnectedCorners(bool con[]) {
   for (int i=0; i<4; i++) {
      conCorner[i] = con[i];
   }
}

void Ground::setConnectedSides(bool con[]) {
   for (int i=0; i<4; i++) {
      conSide[i] = con[i];
   }
}

void Ground::getVertices(float v[][3]) {
   for (int i=0; i<4; i++) {
      for (int j=0; j<3; j++) {
         v[i][j] = verts[i][j];
      }
   }
}

void Ground::getNormal(float n[]) {
   float magnitude = sqrt(pow(norm[0],2) + pow(norm[1],2) + pow(norm[2],2));  // use to normalize the vector
   for (int i=0; i<3; i++) {
      n[i] = norm[i]/magnitude;
   }
}

bool Ground::getOn() {
   return on;
}

void Ground::getConnectedCorners(bool con[]) {
   for (int i=0; i<4; i++) {
      con[i] = conCorner[i];
   }
}

void Ground::getConnectedSides(bool con[]) {
   for (int i=0; i<4; i++) {
      con[i] = conCorner[i];
   }
}

bool Ground::testInsideLine(float point[], float vert1[], float vert2[]) { // this tests if a point is on the same side of the line as the center
   bool testCenter;
   bool testPoint;
   if ((vert2[0]-vert1[0]) == 0) {  // if slope = infinite
      testCenter = center[0] > vert1[0];
      testPoint = point[0] > vert1[0];
   }
   else {
      float slope = (vert2[2]-vert1[2])/(vert2[0]-vert1[0]); // slope = (z2-z1)/(x2-x1)
      float yIntercept = vert1[2]-(slope*vert1[0]); // intercept = z1 - slope*x1
      testCenter = (center[2] > slope*center[0] + yIntercept);
      testPoint = (point[2] > slope*point[0] + yIntercept);
   }
   bool testInside = testPoint == testCenter;
   return testInside;
}

bool Ground::testNearVertex(float point[], float rad, float vert[]) { // this tests if a point is within a rad distance of the corner
   bool testNear;
   float distanceSq = pow(vert[0]-point[0],2) + pow(vert[2]-point[2],2);
   testNear = (distanceSq <= pow(rad,2));
   return testNear;
}

bool Ground::testNearLine(float point[], float rad, float vert1[], float vert2[]) { // this tests if a point is within a rad distance of the side
   bool testNear = false;
   if (vert1[0] == vert2[0]) {  // if slope = infinite
      if ((point[2]<=vert1[2])^(point[2]<=vert2[2])){
         testNear = ((point[0]<=vert1[0]+rad) && (point[0]>=vert1[0]-rad));
      }
   }
   else if (vert1[2] == vert2[2]) {  // if slope = 0
      if ((point[0]<=vert1[0])^(point[0]<=vert2[0])){
         testNear = ((point[2]<=vert1[2]+rad) && (point[2]>=vert1[2]-rad));
      }
   }
   else {
      float slope = (vert2[2]-vert1[2])/(vert2[0]-vert1[0]);      // slope = (z2-z1)/(x2-x1)
      float zIntercept = vert1[2]-(slope*vert1[0]);            // intercept = z1 - slope*x1
      float slopePerp = -1/slope;                           // Find slope and Y-intercept of line perpendicular to ground
      float zInterPerp = point[2] - slopePerp*point[0];         // and that goes through the point (bX,bY)
      float meetX = (zInterPerp-zIntercept)/(slope-slopePerp);   // Find the point on the ground where
      float meetZ = meetX*slope + zIntercept;                  // the perpendicular line intersects it
      if ((meetX<=vert1[0])^(meetX<=vert2[0])) {               // Only test if the intersection point is between the edges of the line
         float distanceSq = pow(meetX-point[0],2) + pow(meetZ-point[2],2);;   // Test the distance against the radius of the object
         testNear = (distanceSq <= pow(rad,2));
      }
   }
   return testNear;
}

Ground::~Ground(void) {
}
