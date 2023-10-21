#pragma once

class Ground {
public:
   Ground(void);
   void draw();
   bool testAbove(float point[], float rad);
   float getHeight(float point[]);
   void setVertices(float v[][3]);
   void setOn(bool value);
   void setConnectedCorners(bool con[]);
   void setConnectedSides(bool con[]);
   void getVertices(float v[][3]);
   void getNormal(float n[]);
   bool getOn();
   void getConnectedCorners(bool con[]);
   void getConnectedSides(bool con[]);
   virtual ~Ground(void);
private:
   bool testInsideLine(float point[], float vert1[], float vert2[]);
   bool testNearVertex(float point[], float rad, float vert[]);
   bool testNearLine(float point[], float rad, float vert1[], float vert2[]);
   float verts[4][3];
   float center[3];
   float norm[4]; // the normal vector to the plane and the D value of the plane equation
   bool conCorner[4]; // true if a corner is connected to another ground
   bool conSide[4]; // true if a side is connected to another ground
   bool on;
};