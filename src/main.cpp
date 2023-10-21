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

#include ".\car.h"
#include ".\ground.h"
#include ".\wall.h"
#include ".\post.h"
#include ".\text.h"

// Definitions
#define DEG_TO_RAD 0.0174532925
#define RAD_TO_DEG 57.29577951
#define DETECT_HEIGHT 1.15

// Default Image dimensions
int imageWidth=1600;
int imageHeight=1000;

// Mouse variables
bool mouseLeft=false, mouseRight=false;
int mousePosX, mousePosY, mouseVelX, mouseVelY;

// Screen positioning variables
float camThetaX=180, camThetaY=30, camOmegaX=0, camOmegaY=0, camDis=100, camVel=0;
float camPosX, camPosY, camPosZ, camDesX, camDesY, camDesZ;
bool camMove = false;

// Framerate variables
int clockThen=clock();  // used to determine how long a second is
int frameCount=0;  // used to determine how many frames have been in the last second
int frameRateTarget = 60;
float frameRateTargetMs = 1.0 * CLOCKS_PER_SEC / frameRateTarget;
float frameRateCredit = 0;
int frameRateClock = clock();
int frameRateCalc=0;

// Car variables
Car cars[2];
float carPos[3];  // These 3 variables are used for
float carVel[3];  //   grabbing and storing temporary
float carNorm[3]; //   car variables

// Debugging variables
float minY=1000;
float floatValue=3;
float debug;
bool boolValue=false;
float test[3];

// Intro Screen variables
int introTex[10];

// Other object variables
Ground grounds[30];
Wall walls[30];
Post posts[30];
Text text;

// Keyboard Array
int keyDown[256];

//OpenGL Calls
void setupGL();
void CheckGLError();

//Callbacks
void Display();
void Idle();
void Timer(int t);
void Reshape(int w, int h);
void Keyboard (unsigned char key, int , int );
void KeyboardUp (unsigned char key, int , int );
void MouseButton(int button, int state, int x, int y);
void MouseMotion(int x, int y);
void MousePassiveMotion(int x, int y);

// Other Methods
void setupGame();
void doStyle(int cNum, float gNum);
void testCarGround();
void testCarWall();
void testCarCar();
void doLighting();
void updateKeyInput();
void updateCamera();
void displayHUD();
void drawString(float x, float y, void *font, string s, float r, float g, float b);
string intToString(int i);
string floatToString(float f, int decPlace);
void loadTextures();

//font
GLvoid *font_style = GLUT_BITMAP_HELVETICA_12;

// a cleanup function
void quit(int i = 0);


int main(int argc, char **argv) {
   // INITIALIZE THE GLUT WINDOW
   memset(keyDown,0,sizeof(keyDown));
   glutInit(&argc, argv);  
   glutInitWindowSize(imageWidth, imageHeight);
   glutInitDisplayString("rgb double");
   glutInitWindowPosition(0, 0);
   glutCreateWindow("Project #1");

   //SETUP GLUT CALLBACKS
   cout << "Setting up callbacks... ";
   glutDisplayFunc(Display);
   glutKeyboardFunc(Keyboard);
   glutKeyboardUpFunc(KeyboardUp);
   glutMouseFunc(MouseButton);
   glutMotionFunc(MouseMotion);
   glutPassiveMotionFunc(MousePassiveMotion);
   glutReshapeFunc(Reshape);
   // glutIdleFunc(Idle);
   cout << "[completed]\n";
   glEnable(GL_DEPTH_TEST);

   //SETUP MISC GL
   setupGL();
   CheckGLError();

   setupGame(); // setup game parameters

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_NORMALIZE);
   loadTextures();
   glMatrixMode(GL_MODELVIEW);

   //MAIN PROGRAM LOOP
   glutTimerFunc(frameRateTargetMs, Timer, 0);
   glutMainLoop();
}

// This function checks the state of openGL and prints an error if an error has occurred
void CheckGLError() {
     GLenum error;
     error = glGetError();
     if (error!=GL_NO_ERROR)
     {
      cout << "OpenGL reports an error: "<< gluErrorString(error) << endl;
      quit(1);
     }
}

// This function draws the scene
void Display() {
   // clear the screen
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   testCarGround();  // used for collision detection
   for (int i=0; i<2; i++) {
      cars[i].updateMovement(keyDown); // read the key presses and move the car accordingly
   }
   testCarWall();
   testCarCar();
   glPopMatrix();
   for (int i=0; i<2; i++) {
      cars[i].updatePosition(); // read the key presses and move the car accordingly
   }
   updateKeyInput(); // read other key presses
   updateCamera(); // move the camera to the correct position

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
      glLoadIdentity();
      cars[0].getPosition(carPos);
      gluLookAt(camPosX,camPosY,camPosZ,carPos[0],carPos[1]+13,carPos[2],0,1,0);
      doLighting();
      for (int i=0; i<2; i++) {
         cars[i].draw();
      }
      for (int i=0; i<30; i++) {
         grounds[i].draw();
         walls[i].draw();
      }
      glEnable(GL_TEXTURE_2D);
      glDisable(GL_LIGHTING);
      /*glBindTexture(GL_TEXTURE_2D, introTex[0]);
      glBegin(GL_QUADS); // top
         glTexCoord2f(0,0);
         glVertex3f(100,100,100);
         glTexCoord2f(1,0);
         glVertex3f(100,100,-100);
         glTexCoord2f(1,1);
         glVertex3f(-100,100,-100);
         glTexCoord2f(0,1);
         glVertex3f(-100,100,100);
      glEnd();
      glDisable(GL_TEXTURE_2D);
      glEnable(GL_LIGHTING);*/
   glPopMatrix();

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
      glDisable(GL_LIGHTING);
      glLoadIdentity();
      gluOrtho2D(-400,400,-300,300);
      text.update();
      displayHUD();
      glEnable(GL_LIGHTING);
   glPopMatrix();

   // swap the buffers
   glutSwapBuffers();
}

void testCarGround() {
   for (int j=0; j<2; j++) {
      cars[j].getPosition(carPos);  // grabs the car position
      cars[j].getVelocity(carVel);  // grabs the car velocity
      float radius = cars[j].getRadius();
      float tall = cars[j].getTall();
      float height = -1000;  // The new heights are tested against this
      float norm[4];
      for (int i=0; i<30; i++) {
         if (grounds[i].testAbove(carPos, radius))   {  // if the car is above a floor block then
            float newHeight = grounds[i].getHeight(carPos);  // look at the height of the floor directly below the car
            float grdNorm[3];
            grounds[i].getNormal(grdNorm);  // grab the normal of the ground
            float velNorm = carVel[0]*grdNorm[0] + carVel[1]*grdNorm[1] + carVel[2]*grdNorm[2];  // use dot product to get the velocity in the normal direction
            if (newHeight > height && newHeight < carPos[1]-DETECT_HEIGHT-5*velNorm) {  // if this height is bigger than the old height and not too much above the car then
               height = newHeight;  // remember the new height
               if (carPos[1] < height+1.98) {  // if the car is close enough to the ground then
                  grounds[i].getNormal(norm);  // find the normal to the ground plane
                  cars[j].setNormal(norm);  // and tell the car what it is
                  doStyle(j,i);
               }
            }
         }
      }
      cars[j].setElevation(height);  // tell the car how high above the ground it is
   }
}

void doStyle(int cNum, float gNum) {
   cars[cNum].getPosition(carPos);
   cars[cNum].getVelocity(carVel);
   if (gNum == 5) {
      if (carPos[2] >= carPos[0] && carPos[2] >= -carPos[0]) gNum = 5.25;
      else if (carPos[2] >= carPos[0] && carPos[2] <= -carPos[0]) gNum = 5.5;
      else if (carPos[2] <= carPos[0] && carPos[2] <= -carPos[0]) gNum = 5.75;
   }
   if (gNum == 10 && carPos[0] > 400) gNum = 10.25;
   if (gNum == 13 && carPos[0] > 400) gNum = 13.25;
   if (gNum == 11 && carPos[0] < -400) gNum = 11.25;
   if (gNum == 12 && carPos[0] < -400) gNum = 12.25;
   if (gNum == 14) {
      if (carPos[2] < -400) gNum = 14.25;
      else if (carPos[2] > 400) gNum = 14.5;
   }
   if (gNum == 15) {
      if (carPos[2] < -400) gNum = 15.25;
      else if (carPos[2] > 400) gNum = 15.5;
   }

   float speed = sqrt(pow(carVel[0],2)+pow(carVel[1],2)+pow(carVel[2],2));
   float oldGround = cars[cNum].getGroundOld();
   float airAngle = abs(cars[cNum].getAirAngle());
   string newText = "";
   if (oldGround == 0) {
      if (gNum == 6 || gNum == 7) {
         newText = "Straight to the Ramp";
      }
   }
   if (oldGround == 1) {
      if (gNum == 3 || gNum == 5.5) {
         newText = "E.T.";
      }
   }
   if (oldGround == 2) {
      if (gNum == 4 || gNum == 5.75) {
         newText = "E.T.";
      }
   }
   if (oldGround == 3) {
      if (gNum == 1 || gNum == 5.0) {
         newText = "E.T.";
      }
   }
   if (oldGround == 4) {
      if (gNum == 2 || gNum == 5.25) {
         newText = "E.T.";
      }
   }
   if (oldGround >=8 && oldGround <= 15) {
      if (gNum <= 4 ) {
         newText = "Jump to the Mound";
      }
   }
   if (oldGround == 10) {
      if (gNum == 14 && speed >= 2) {
         newText = "Cutting Corners";
      }
   }
   if (oldGround == 11) {
      if (gNum == 15 && speed >= 2) {
         newText = "Cutting Corners";
      }
   }
   if (oldGround == 12) {
      if (gNum == 15 && speed >= 2) {
         newText = "Cutting Corners";
      }
   }
   if (oldGround == 13) {
      if (gNum == 14 && speed >= 2) {
         newText = "Cutting Corners";
      }
   }
   if (oldGround == 14) {
      if ((gNum == 10 || gNum ==13) && speed >= 2) {
         newText = "Cutting Corners";
      }
   }
   if (oldGround == 15) {
      if ((gNum == 11 || gNum == 12) && speed >= 2) {
         newText = "Cutting Corners";
      }
   }
   if (oldGround == 16 || oldGround == 17) {
      if (gNum == 0) {
         newText = "Return to the Beginning";
      }
   }

   string airText = "";
   if (airAngle >= 720) {
      airText = "720!!!";
   }
   else if (airAngle >= 540) {
      airText = "540!!";
   }
   else if (airAngle >= 360) {
      airText = "360!";
   }
   else if (airAngle >= 180 && speed >= 1) {
      airText = "180";
   }
   if (airText != "") {
      if (newText != "") newText += " + ";
      newText += airText;
   }

   if (newText != "") {
      text.setText(newText);
      text.setPosition(0, 150);
      text.reset();
   }

   cars[cNum].setGroundOld(gNum);
}

void testCarWall() {
   for (int j=0; j<2; j++) {
      cars[j].getPosition(carPos);  // grabs the car position
      cars[j].getNormal(carNorm);  // grabs the car normal
      float radius = cars[j].getRadius();
      float tall = cars[j].getTall();
      float wallPoint[3];
      for (int i=0; i<30; i++) {
         float wallDis = walls[i].testInFrontOf(carPos, tall, carNorm, wallPoint, i);
         if (wallDis <= radius) {
            cars[j].updateWall(wallPoint, wallDis);
         }
      }
      float postPoint[3];
      for (int i=0; i<30; i++) {
         float postDis = posts[i].testNear(carPos, tall, carNorm, postPoint);
         if (postDis <= radius) {
            cars[j].updateWall(postPoint, postDis);
         }
      }
   }
}

void testCarCar() {
   for (int j=0; j<2; j++) {
      float pos1[3], pos2[3], vel1[3], vel2[3], rad1, rad2, tall1, tall2;
      cars[j].getPosition(pos1);
      cars[j].getVelocity(vel1);
      rad1 = cars[j].getRadius();
      tall1 = cars[j].getTall();
      for (int i=0; i<2; i++) {
         if (j != i) {
            cars[i].getPosition(pos2);
            cars[i].getVelocity(vel2);
            rad2 = cars[i].getRadius();
            tall2 = cars[i].getTall();
            float distance = sqrt(pow(pos1[0]-pos2[0],2) + 2*pow(pos1[1]-pos2[1],2) + pow(pos1[2]-pos2[2],2));
            if ((distance < (rad1 + rad2))) {
               float overlap = (rad1 + rad2) - distance;
               float pushDir[3] = {pos1[0]-pos2[0], pos1[1]-pos2[1], pos1[2]-pos2[2]};  // vector from 2 to 1
               float magnitude = sqrt(pow(pushDir[0],2) + pow(pushDir[1],2) + pow(pushDir[2],2));  // use to normalize vector;
               pushDir[0] /= magnitude;
               pushDir[1] /= magnitude;
               pushDir[2] /= magnitude;
               cars[j].updateOthers(pushDir, overlap);
               cars[j].setTackle(0);
            }
         }
      }
   }
}

void doLighting() {
   GLfloat ambient0[] = {.7, .7, .7, 1};
   GLfloat diffuse0[] = {.2, .2, .2, 1};
   GLfloat position0[] = {10, 20, 8, 0};
   glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
   glLightfv(GL_LIGHT0, GL_AMBIENT, diffuse0);
   glLightfv(GL_LIGHT0, GL_POSITION, position0);
}

void updateKeyInput() {
   if (keyDown['c'] == 1) {
      keyDown['c'] = 2;
      camMove = !camMove;  // toggle free camera movement by pressing 'c'
   }
}

void updateCamera() {
   if (mouseLeft) {
      camOmegaX = (float)mouseVelX * (0.2);
      camOmegaY = (float)mouseVelY * (-0.2);
   }
   else if (mouseRight) {
      camVel = (float)mouseVelY * (0.5);
   }
   mouseVelX = 0;
   mouseVelY = 0;

   camThetaX += camOmegaX;  // omega is the change in theta over time
   camThetaY += camOmegaY;
   camDis += camVel;
   
   if (camThetaX > 360) camThetaX -= 360;
   else if (camThetaX < -360) camThetaX += 360;
   if (camThetaY >= 90) {
      camThetaY = 90;
      camOmegaY = 0;
   }
   else if (camThetaY <= -90) {
      camThetaY = -90;
      camOmegaY = 0;
   }
   if (camDis >= 2000) {
      camDis = 2000;
      camVel = 0;
   }
   else if (camDis <= 5) {
      camDis = 5;
      camVel = 0;
   }
   cars[0].getPosition(carPos);  // obtain the information about the car
   cars[0].getNormal(carNorm);
   float carAngle = cars[0].getAngle();
   if (camMove) {  // if the user is controlling the car then keep the camera upright and convert the polor coordinates into cartesian
      camDesX = carPos[0]+camDis*cos(camThetaY*DEG_TO_RAD)*cos((camThetaX+carAngle)*DEG_TO_RAD);
      camDesZ = carPos[2]+camDis*cos(camThetaY*DEG_TO_RAD)*sin((camThetaX+carAngle)*DEG_TO_RAD);
      camDesY = carPos[1]+camDis*sin(camThetaY*DEG_TO_RAD);
   }
   else {
      // A dot B = |A||B|cos(theta);  A = carNorm;  B = direction of cam in xz-plane
      float tempX = 40*cos(20*DEG_TO_RAD)*cos((180+carAngle)*DEG_TO_RAD);  // Find where the camera should
      float tempZ = 40*cos(20*DEG_TO_RAD)*sin((180+carAngle)*DEG_TO_RAD);  //   be on the xz-plane with respect to the car
      float magnitude = sqrt(pow(tempX,2)+pow(tempZ,2));  // find the magnitude of its distance to the car along this plane
      float bVector[3] = {tempX/magnitude, 0, tempZ/magnitude};  // create a normalized vector that lies in xz-plane and faces direction of car
      float pivotAngle = RAD_TO_DEG*acos(carNorm[0]*bVector[0] + carNorm[1]*bVector[1] + carNorm[2]*bVector[2]);  // use dot product to find the angle
      pivotAngle = pivotAngle-90;                                             // that the camera should pivot at to line up with ground
      camDesX = carPos[0]+40*cos((20+pivotAngle)*DEG_TO_RAD)*cos((180+carAngle)*DEG_TO_RAD);  // convert from polar to cartesian
      camDesZ = carPos[2]+40*cos((20+pivotAngle)*DEG_TO_RAD)*sin((180+carAngle)*DEG_TO_RAD);
      camDesY = carPos[1]+40*sin((20+pivotAngle)*DEG_TO_RAD);
   }
   camPosX += 0.15*(camDesX-camPosX);  // move to camera closer and closer to where it needs to be
   camPosY += 0.15*(camDesY-camPosY);  // causes a smoothing effect
   camPosZ += 0.15*(camDesZ-camPosZ);
}

void displayHUD() {  // helps to display debuging info
   string text;
   cars[0].getPosition(carPos);
   cars[0].getVelocity(carVel);

   text = "Ground: " + floatToString(cars[0].getGroundOld(),-1);
   drawString(-370, -180, GLUT_BITMAP_HELVETICA_18, text, 1, 1, 1);

   text = "CamThetaY: " + floatToString(camThetaY,-1);
   drawString(-370, -195, GLUT_BITMAP_HELVETICA_18, text, 1, 1, 1);

   text = "Damage 1: " + floatToString(cars[0].getDamage(),-1);
   drawString(-370, -210, GLUT_BITMAP_HELVETICA_18, text, 1, 1, 1);

   text = "Damage 2: " + floatToString(cars[1].getDamage(),-1);
   drawString(-370, -225, GLUT_BITMAP_HELVETICA_18, text, 1, 1, 1);

   text = "Vel Y: " + floatToString(carVel[1],-1);
   drawString(-370, -240, GLUT_BITMAP_HELVETICA_18, text, 1, 1, 1);

   text = "Speed: " + floatToString(sqrt(pow(carVel[0],2)+pow(carVel[1],2)+pow(carVel[2],2)),-1);
   drawString(-370, -255, GLUT_BITMAP_HELVETICA_18, text, 1, 1, 1);

   // show framerate
   frameCount++;
   int clockNow = clock();
   int clockCount = clockNow - clockThen;
   if ((clockCount>=1000) || (clockCount<=-1)) {
      frameRateCalc = (int)(1000*frameCount/clockCount);
      frameCount = 0;
      clockThen = clockNow;
   }
   text = "FPS: " + intToString(frameRateCalc);
   drawString(-370, -270, GLUT_BITMAP_HELVETICA_18, text, 1, 1, 1);
}

void setupGame() {  // Initialize game
   // Create the ground segments; The first vertex needs to be the center; It's used for collision detection
   float groundVerts0[4][3] = {{100,20,-100}, {100,20,100}, {-100,20,100}, {-100,20,-100}};
   bool conCorner0[4] = {1,1,1,1}; // tells which corners are connected to other ground pieces
   bool conSide0[4] = {1,1,1,1}; // tells which sides are connected to other ground pieces
   float groundVerts1[4][3] = {{100,20,-100}, {150,0,-150}, {150,0,150}, {100,20,100}};
   bool conCorner1[4] = {1,1,1,1};
   bool conSide1[4] = {1,1,1,1};
   float groundVerts2[4][3] = {{100,20,100}, {150,0,150}, {-150,0,150}, {-100,20,100}};
   bool conCorner2[4] = {1,1,1,1};
   bool conSide2[4] = {1,1,1,1};
   float groundVerts3[4][3] = {{-100,20,100}, {-150,0,150}, {-150,0,-150}, {-100,20,-100}};
   bool conCorner3[4] = {1,1,1,1};
   bool conSide3[4] = {1,1,1,1};
   float groundVerts4[4][3] = {{-100,20,-100}, {-150,0,-150}, {150,0,-150}, {100,20,-100}};
   bool conCorner4[4] = {1,1,1,1};
   bool conSide4[4] = {1,1,1,1};
   float groundVerts5[4][3] = {{400,0,400}, {-400,0,400}, {-400,0,-400}, {400,0,-400}};
   bool conCorner5[4] = {0,0,0,0};
   bool conSide5[4] = {0,0,0,0};
   float groundVerts6[4][3] = {{100,0,300}, {200,40,400}, {-200,40,400}, {-100,0,300}};
   bool conCorner6[4] = {1,1,1,1};
   bool conSide6[4] = {0,1,0,1};
   float groundVerts7[4][3] = {{100,0,-300}, {200,40,-400}, {-200,40,-400}, {-100,0,-300}};
   bool conCorner7[4] = {1,1,1,1};
   bool conSide7[4] = {0,1,0,1};
   float groundVerts8[4][3] = {{200,40,400}, {200,40,600}, {-200,40,600}, {-200,40,400}};
   bool conCorner8[4] = {0,0,0,0};
   bool conSide8[4] = {1,0,1,1};
   float groundVerts9[4][3] = {{-200,40,-400}, {-200,40,-600}, {200,40,-600}, {200,40,-400}};
   bool conCorner9[4] = {0,0,0,0};
   bool conSide9[4] = {1,0,1,1};
   float groundVerts10[4][3] = {{400,40,400}, {600,40,600}, {200,40,600}, {200,40,400}};
   bool conCorner10[4] = {0,0,0,1};
   bool conSide10[4] = {1,0,1,0};
   float groundVerts11[4][3] = {{-400,40,400}, {-600,40,600}, {-200,40,600}, {-200,40,400}};
   bool conCorner11[4] = {0,0,0,1};
   bool conSide11[4] = {1,0,1,0};
   float groundVerts12[4][3] = {{-400,40,-400}, {-600,40,-600}, {-200,40,-600}, {-200,40,-400}};
   bool conCorner12[4] = {0,0,0,1};
   bool conSide12[4] = {1,0,1,0};
   float groundVerts13[4][3] = {{400,40,-400}, {600,40,-600}, {200,40,-600}, {200,40,-400}};
   bool conCorner13[4] = {0,0,0,1};
   bool conSide13[4] = {1,0,1,0};
   float groundVerts14[4][3] = {{400,40,400}, {600,40,600}, {600,40,-600}, {400,40,-400}};
   bool conCorner14[4] = {0,0,0,0};
   bool conSide14[4] = {1,0,1,0};
   float groundVerts15[4][3] = {{-400,40,400}, {-600,40,600}, {-600,40,-600}, {-400,40,-400}};
   bool conCorner15[4] = {0,0,0,0};
   bool conSide15[4] = {1,0,1,0};
   float groundVerts16[4][3] = {{300,80,100}, {400,40,200}, {400,40,-200}, {300,80,-100}};
   bool conCorner16[4] = {0,1,1,0};
   bool conSide16[4] = {0,1,0,0};
   float groundVerts17[4][3] = {{-300,80,100}, {-400,40,200}, {-400,40,-200}, {-300,80,-100}};
   bool conCorner17[4] = {0,1,1,0};
   bool conSide17[4] = {0,1,0,0};
   float groundVerts18[4][3] = {{600,-200,600}, {-600,-200,600}, {-600,-200,-600}, {600,-200,-600}};
   bool conCorner18[4] = {0,1,1,0};
   bool conSide18[4] = {0,1,0,0};
   grounds[0].setVertices(groundVerts0);
   grounds[0].setConnectedCorners(conCorner0);
   grounds[0].setConnectedSides(conSide0);
   grounds[0].setOn(true);
   grounds[1].setVertices(groundVerts1);
   grounds[1].setConnectedCorners(conCorner1);
   grounds[1].setConnectedSides(conSide1);
   grounds[1].setOn(true);
   grounds[2].setVertices(groundVerts2);
   grounds[2].setConnectedCorners(conCorner2);
   grounds[2].setConnectedSides(conSide2);
   grounds[2].setOn(true);
   grounds[3].setVertices(groundVerts3);
   grounds[3].setConnectedCorners(conCorner3);
   grounds[3].setConnectedSides(conSide3);
   grounds[3].setOn(true);
   grounds[4].setVertices(groundVerts4);
   grounds[4].setConnectedCorners(conCorner4);
   grounds[4].setConnectedSides(conSide4);
   grounds[4].setOn(true);
   grounds[5].setVertices(groundVerts5);
   grounds[5].setConnectedCorners(conCorner5);
   grounds[5].setConnectedSides(conSide5);
   grounds[5].setOn(true);
   grounds[6].setVertices(groundVerts6);
   grounds[6].setConnectedCorners(conCorner6);
   grounds[6].setConnectedSides(conSide6);
   grounds[6].setOn(true);
   grounds[7].setVertices(groundVerts7);
   grounds[7].setConnectedCorners(conCorner7);
   grounds[7].setConnectedSides(conSide7);
   grounds[7].setOn(true);
   grounds[8].setVertices(groundVerts8);
   grounds[8].setConnectedCorners(conCorner8);
   grounds[8].setConnectedSides(conSide8);
   grounds[8].setOn(true);
   grounds[9].setVertices(groundVerts9);
   grounds[9].setConnectedCorners(conCorner9);
   grounds[9].setConnectedSides(conSide9);
   grounds[9].setOn(true);
   grounds[10].setVertices(groundVerts10);
   grounds[10].setConnectedCorners(conCorner10);
   grounds[10].setConnectedSides(conSide10);
   grounds[10].setOn(true);
   grounds[11].setVertices(groundVerts11);
   grounds[11].setConnectedCorners(conCorner11);
   grounds[11].setConnectedSides(conSide11);
   grounds[11].setOn(true);
   grounds[12].setVertices(groundVerts12);
   grounds[12].setConnectedCorners(conCorner12);
   grounds[12].setConnectedSides(conSide12);
   grounds[12].setOn(true);
   grounds[13].setVertices(groundVerts13);
   grounds[13].setConnectedCorners(conCorner13);
   grounds[13].setConnectedSides(conSide13);
   grounds[13].setOn(true);
   grounds[14].setVertices(groundVerts14);
   grounds[14].setConnectedCorners(conCorner14);
   grounds[14].setConnectedSides(conSide14);
   grounds[14].setOn(true);
   grounds[15].setVertices(groundVerts15);
   grounds[15].setConnectedCorners(conCorner15);
   grounds[15].setConnectedSides(conSide15);
   grounds[15].setOn(true);
   grounds[16].setVertices(groundVerts16);
   grounds[16].setConnectedCorners(conCorner16);
   grounds[16].setConnectedSides(conSide16);
   grounds[16].setOn(true);
   grounds[17].setVertices(groundVerts17);
   grounds[17].setConnectedCorners(conCorner17);
   grounds[17].setConnectedSides(conSide17);
   grounds[17].setOn(true);
   grounds[18].setVertices(groundVerts18);
   grounds[18].setConnectedCorners(conCorner18);
   grounds[18].setConnectedSides(conSide18);
   grounds[18].setOn(true);
   // // Create the walls; Start with top two vertices and then bottom two
   float wallVerts0[4][3] = {{100,0,-300}, {200,40,-400}, {200,0,-400}, {100,0,-300}};
   float wallVerts1[4][3] = {{200,40,-400}, {400,40,-400}, {400,0,-400}, {200,0,-400}};
   float wallVerts2[4][3] = {{400,40,-400}, {400,40,-200}, {400,0,-200}, {400,0,-400}};
   float wallVerts3[4][3] = {{400,40,-200}, {300,80,-100}, {300,0,-100}, {400,0,-200}};
   float wallVerts4[4][3] = {{300,80,-100}, {300,80,100}, {300,0,100}, {300,0,-100}};
   float wallVerts5[4][3] = {{300,80,100}, {400,40,200}, {400,0,200}, {300,0,100}};
   float wallVerts6[4][3] = {{400,40,200}, {400,40,400}, {400,0,400}, {400,0,200}};
   float wallVerts7[4][3] = {{400,40,400}, {200,40,400}, {200,0,400}, {400,0,400}};
   float wallVerts8[4][3] = {{100,0,300}, {200,40,400}, {200,0,400}, {100,0,300}};
   float wallVerts9[4][3] = {{-100,0,-300}, {-200,40,-400}, {-200,0,-400}, {-100,0,-300}};
   float wallVerts10[4][3] = {{-200,40,-400}, {-400,40,-400}, {-400,0,-400}, {-200,0,-400}};
   float wallVerts11[4][3] = {{-400,40,-400}, {-400,40,-200}, {-400,0,-200}, {-400,0,-400}};
   float wallVerts12[4][3] = {{-400,40,-200}, {-300,80,-100}, {-300,0,-100}, {-400,0,-200}};
   float wallVerts13[4][3] = {{-300,80,-100}, {-300,80,100}, {-300,0,100}, {-300,0,-100}};
   float wallVerts14[4][3] = {{-300,80,100}, {-400,40,200}, {-400,0,200}, {-300,0,100}};
   float wallVerts15[4][3] = {{-400,40,200}, {-400,40,400}, {-400,0,400}, {-400,0,200}};
   float wallVerts16[4][3] = {{-400,40,400}, {-200,40,400}, {-200,0,400}, {-400,0,400}};
   float wallVerts17[4][3] = {{-100,0,300}, {-200,40,400}, {-200,0,400}, {-100,0,300}};
   float wallVerts18[4][3] = {{600,40,600}, {-600,40,600}, {-600,-200,600}, {600,-200,600}};
   float wallVerts19[4][3] = {{-600,40,600}, {-600,40,-600}, {-600,-200,-600}, {-600,-200,600}};
   float wallVerts20[4][3] = {{-600,40,-600}, {600,40,-600}, {600,-200,-600}, {-600,-200,-600}};
   float wallVerts21[4][3] = {{600,40,-600}, {600,40,600}, {600,-200,600}, {600,-200,-600}};
   walls[0].setVertices(wallVerts0);
   walls[0].setOn(true);
   walls[1].setVertices(wallVerts1);
   walls[1].setOn(true);
   walls[2].setVertices(wallVerts2);
   walls[2].setOn(true);
   walls[3].setVertices(wallVerts3);
   walls[3].setOn(true);
   walls[4].setVertices(wallVerts4);
   walls[4].setOn(true);
   walls[5].setVertices(wallVerts5);
   walls[5].setOn(true);
   walls[6].setVertices(wallVerts6);
   walls[6].setOn(true);
   walls[7].setVertices(wallVerts7);
   walls[7].setOn(true);
   walls[8].setVertices(wallVerts8);
   walls[8].setOn(true);
   walls[9].setVertices(wallVerts9);
   walls[9].setOn(true);
   walls[10].setVertices(wallVerts10);
   walls[10].setOn(true);
   walls[11].setVertices(wallVerts11);
   walls[11].setOn(true);
   walls[12].setVertices(wallVerts12);
   walls[12].setOn(true);
   walls[13].setVertices(wallVerts13);
   walls[13].setOn(true);
   walls[14].setVertices(wallVerts14);
   walls[14].setOn(true);
   walls[15].setVertices(wallVerts15);
   walls[15].setOn(true);
   walls[16].setVertices(wallVerts16);
   walls[16].setOn(true);
   walls[17].setVertices(wallVerts17);
   walls[17].setOn(true);
   walls[18].setVertices(wallVerts18);
   walls[18].setOn(true);
   walls[19].setVertices(wallVerts19);
   walls[19].setOn(true);
   walls[20].setVertices(wallVerts20);
   walls[20].setOn(true);
   walls[21].setVertices(wallVerts21);
   walls[21].setOn(true);
   // // Create the posts;
   float postPosition0[3] = {300,0,-100};
   float postHeight0 = 80;
   float postPosition1[3] = {300,0,-100};
   float postHeight1 = 80;
   float postPosition2[3] = {-300,0,100};
   float postHeight2 = 80;
   float postPosition3[3] = {-300,0,-100};
   float postHeight3 = 80;
   posts[0].setPosition(postPosition0);
   posts[0].setHeight(postHeight0);
   posts[0].setOn(true);
   posts[1].setPosition(postPosition1);
   posts[1].setHeight(postHeight1);
   posts[1].setOn(true);
   posts[2].setPosition(postPosition2);
   posts[2].setHeight(postHeight2);
   posts[2].setOn(true);
   posts[3].setPosition(postPosition3);
   posts[3].setHeight(postHeight3);
   posts[3].setOn(true);
   // Setup cars
   cars[0].setId(1);
   cars[0].getPosition(carPos);
	carPos[0] = -20;
   carPos[1] = 40;
	carPos[2] = 0;
   cars[0].setPosition(carPos);
	carPos[0] = 40;
   cars[1].setId(2);
   cars[1].setPosition(carPos);
   cars[1].setDamage(0);
}

// set up GL stuff
void setupGL() {
   // set the clear color
   glClearColor(.1, .1, .1, 1.0);  // background color
}


// This function is continuously called when events are not being received
// by the window.  This is a good place to update the state of your objects 
// after every frame.
void Idle() {
   glutPostRedisplay();
}
void Timer(int t) {
   // Update display
   glutPostRedisplay();

   // Reset timer
   int clockNew = clock();
   frameRateCredit += (clockNew - frameRateClock) * 1000.0 / CLOCKS_PER_SEC;
   frameRateCredit -= frameRateTargetMs;
   frameRateClock = clockNew;
   int delayMs = (int) round(frameRateTarget - frameRateCredit);
   delayMs = min(int (10*frameRateTargetMs), max(0, delayMs));
   glutTimerFunc(delayMs, Timer, 0);
}

// keyboard handler
void Keyboard (unsigned char key, int , int ) {
   if (keyDown[key] == 0) keyDown[key]=1;
}

void KeyboardUp (unsigned char key, int , int ) {
   keyDown[key]=0;
}

void MouseButton(int button, int state, int x, int y) {
   if (button == GLUT_LEFT_BUTTON) {
      if (state == GLUT_DOWN && !mouseRight) mouseLeft = true;
      else if (state == GLUT_UP) mouseLeft = false;
   }
   else if (button == GLUT_RIGHT_BUTTON) {
      if (state == GLUT_DOWN && !mouseLeft) mouseRight = true;
      else if (state == GLUT_UP) mouseRight = false;
   }
   mousePosX = x;
   mousePosY = y;
}

void MouseMotion(int x, int y) {
   if (mouseLeft || mouseRight) {
      mouseVelX = x - mousePosX;
      mouseVelY = mousePosY - y; // the y-axis is flipped for mouse/screen positioning
   }
   mousePosX = x;
   mousePosY = y;
}

void MousePassiveMotion(int x, int y) {
   mousePosX = x;
   mousePosY = y;
}

// This functions handles what happens when the window is reshaped
void Reshape(int w, int h) {
   imageWidth = w;
   imageHeight = h;

    glViewport(0,0,w,h);
   double aspect = ((double)w) / ((double)h);
   glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
   gluPerspective(60,aspect,0.01, 10000);
}

// a cleanup function.  call this when you want to exit.
void quit(int i) {
   exit(i);
}

void drawString(float x, float y, void *font, string s, float r, float g, float b) {
   glColor3f(r, g, b);
   glRasterPos2f(x, y);
   for (int i=0; i<(signed)s.length(); i++)
      glutBitmapCharacter(font, s[i]);
}

string intToString(int i) {
   stringstream s;
   s << i;
   return s.str();
}

string floatToString(float f, int decNum) {
   stringstream s;
   string final;
   s << f;
   final = s.str();
   if (decNum != -1) {
      int position = final.find('.');
      if (position != string::npos) {
          final = final.substr(0, position+1+decNum);
      }
   }
   return final;
}

void loadTextures() {
   
}