#pragma once
#include "Globals.h"
#include <vector>;

typedef float GLfloat;
typedef unsigned short GLushort;

class Primitive {
public:
	Primitive();
	virtual ~Primitive();

	virtual void Render() = 0;

public:

protected:
	void RenderPrimitive(float vertex[], int num_vertex, uint index[], int num_index);
};

class Cube : public Primitive {
public:
	Cube();
	Cube(float x, float y, float z);
	~Cube();

	void Render();
};

class Sphere : public Primitive {
public:
	Sphere();
	~Sphere();

	std::vector<GLfloat>* GetVerts();

	void Render();

private:
	std::vector<GLfloat> vertex;
	std::vector<GLushort> index;
};

class Plane : public Primitive {
public:
	Plane();
	~Plane();

	void Render();
};

class Cylinder : public Primitive {
public:
	Cylinder();
	Cylinder(float radius, float height, int sides);
	~Cylinder();

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

class Pyramid : public Primitive {
public:
	Pyramid();
	~Pyramid();

	void Render();
};

class Grid {
public:
	Grid(int size);
	~Grid();

	void Render();

private:
	int size;

};