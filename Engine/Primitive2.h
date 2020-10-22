#pragma once
#include "Globals.h"
#include <vector>;

typedef float GLfloat;
typedef unsigned short GLushort;

class Primitive2 {
public:
	Primitive2();
	virtual ~Primitive2();

	virtual void Render() = 0;

public:

protected:
	void RenderPrimitive2(float vertex[], int num_vertex, uint index[], int num_index);
};

class Cube2 : public Primitive2 {
public:
	Cube2();
	Cube2(float x, float y, float z);
	~Cube2();

	void Render();
};

class Sphere2 : public Primitive2 {
public:
	Sphere2();
	~Sphere2();

	void Render();

private:
	std::vector<GLfloat> vertex;
	std::vector<GLushort> index;
};

class Plane3 : public Primitive2 {
public:
	Plane3();
	~Plane3();

	void Render();
};

class Cylinder2 : public Primitive2 {
public:
	Cylinder2();
	Cylinder2(float radius, float height, int sides);
	~Cylinder2();

	void CalculateGeometry();
	void Render();
private:
	float radius;
	float height;
	unsigned int sides;

	float* vertex;
	int num_vertex;

	uint* index;
	int num_index;
};

class Pyramid2 : public Primitive2 {
public:
	Pyramid2();
	~Pyramid2();

	void Render();
};