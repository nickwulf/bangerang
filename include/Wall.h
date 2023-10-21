#pragma once

// Walls are assumed to be flat and four sided.  Their normals are parallel to the xz-plane.
//   Their left and right sides are parallel to the y-axis.  The first two vertices are the
//   top vertices and the last two are the bottom two.  The first vertex is the center point.
class Wall {
public:
	Wall(void);
	void draw();
	float testInFrontOf(float point[], float tall, float carNorm[], float wallPoint[], int i);
	void setVertices(float v[][3]);
	void setOn(bool value);
	void getVertices(float v[][3]);
	void getNormal(float n[]);
	bool getOn();
	virtual ~Wall(void);
private:
	float verts[4][3]; // first vertex is center, then 4 vertices in clockwise order
	float norm[4]; // the normal vector to the plane and the D value of the plane equation
	bool on;
};
