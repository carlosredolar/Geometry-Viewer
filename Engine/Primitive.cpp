#include "Primitive.h"
#include "glew/include/glew.h"



Primitive::Primitive() {
}

Primitive::~Primitive() {}

void Primitive::RenderPrimitive(float vertex[], int num_vertex, uint index[], int num_index)
{
	uint vertex_buffer = 0;
	glGenBuffers(1, (GLuint*)&(vertex_buffer));
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3, vertex, GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	uint index_buffer = 0;
	glGenBuffers(1, (GLuint*)&(index_buffer));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_index, index, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glDrawElements(GL_TRIANGLES, num_index, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
}

//CUBE

Cube::Cube() : Primitive() {}

Cube::Cube(float x, float y, float z) {}

Cube::~Cube() {}

void Cube::Render()
{
	float vertex[24] = {
		//Top vertex
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		//Bottom vertex
		0.0f ,0.0f, 0.0f,
		1.0f ,0.0f, 0.0f,
		1.0f ,0.0f, 1.0f,
		0.0f ,0.0f, 1.0f		
	};

	uint index[36] = {
		4,7,6, 6,5,4,	//Top
		0,1,2, 2,3,0,	//Bottom
		3,2,6, 6,7,3,	//Front
		1,0,5, 0,4,5,	//Back
		6,2,1, 1,5,6,	//Right
		4,0,7, 7,0,3	//Left
	};

	RenderPrimitive(vertex, 24, index, 36);
}






//SPHERE

Sphere::Sphere() : Primitive()
{
	float radius = 1;
	unsigned int rings = 12;
	unsigned int sectors = 12;

	float const R = 1.0f / (float)(rings - 1);
	float const S = 1.0f / (float)(sectors - 1);
	int r, s;

	vertex.resize(rings * sectors * 3);
	std::vector<GLfloat>::iterator v = vertex.begin();

	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++)
		{
			float const y = sin(-PI * 0.5f + PI * r * R);
			float const x = cos(2 * PI * s * S) * sin(PI * r * R);
			float const z = sin(2 * PI * s * S) * sin(PI * r * R);

			*v++ = x * radius;
			*v++ = y * radius;
			*v++ = z * radius;
		}
	}

	index.resize(rings * sectors * 4);
	std::vector<GLushort>::iterator i = index.begin();

	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++)
		{
			*i++ = r * sectors + s;
			*i++ = r * sectors + (s + 1);
			*i++ = (r + 1) * sectors + (s + 1);
			*i++ = (r + 1) * sectors + s;
		}
	}
}

Sphere::~Sphere()
{
	vertex.clear();
	index.clear();
}

void Sphere::Render()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &vertex[0]);
	glDrawElements(GL_QUADS, index.size(), GL_UNSIGNED_SHORT, &index[0]);
}





//PLANE

Plane::Plane() : Primitive() {}

Plane::~Plane() {}

void Plane::Render()
{
	float vertex[12] = {
		0.0f ,0.0f, 0.0f,
		1.0f ,0.0f, 0.0f,
		1.0f ,0.0f, 1.0f,
		0.0f ,0.0f, 1.0f,
	};

	uint index[6]{
		0, 3, 2,
		2, 1 ,0
	};

	RenderPrimitive(vertex, 12, index, 6);
}




//CYLINDER

Cylinder::Cylinder() : Primitive(), radius(1), height(2), sides(16) {
	CalculateGeometry();
}

Cylinder::Cylinder(float radius, float height, int sides) : Primitive(), radius(radius), height(height), sides(sides) {
	CalculateGeometry();
}

Cylinder::~Cylinder()
{
	delete[] vertex;
	delete[] index;
}

void Cylinder::CalculateGeometry()
{
	float current_angle = 0;
	float angle_increase = 2 * PI / sides;

	std::vector<float> vertex_vector;

	//Top center
	vertex_vector.push_back(0);
	vertex_vector.push_back(height * 0.5f);
	vertex_vector.push_back(0);

	//Top
	for (int i = 0; i < sides; i++)
	{
		vertex_vector.push_back(radius * cos(current_angle));
		vertex_vector.push_back(height * 0.5f);				
		vertex_vector.push_back(radius * sin(current_angle));

		current_angle -= angle_increase;
	}

	current_angle = 0;

	//Bot Center
	vertex_vector.push_back(0);
	vertex_vector.push_back(-height * 0.5f);
	vertex_vector.push_back(0);

	//Bottom
	for (int i = 0; i < sides; i++)
	{
		vertex_vector.push_back(radius * cos(current_angle));
		vertex_vector.push_back(-height * 0.5f);			     
		vertex_vector.push_back(radius * sin(current_angle));

		//clockwise
		current_angle -= angle_increase;
	}


	std::vector<uint> index_vector;

	//Top
	for (int i = 1; i < sides; i++)
	{
		index_vector.push_back(0);
		index_vector.push_back(i);
		index_vector.push_back(i + 1);
	}

	index_vector.push_back(0);
	index_vector.push_back(sides);
	index_vector.push_back(1);

	//Sides
	for (int i = 1; i < sides; i++)
	{
		//triangrle 1
		index_vector.push_back(i);
		index_vector.push_back(sides + i + 1);
		index_vector.push_back(sides + i + 2);
		//triangle 2
		index_vector.push_back(i + sides + 2);
		index_vector.push_back(i + 1);
		index_vector.push_back(i);
	}

	index_vector.push_back(sides);
	index_vector.push_back(2 * sides + 1);
	index_vector.push_back(sides + 2);

	index_vector.push_back(sides + 2);
	index_vector.push_back(1);
	index_vector.push_back(sides);

	//Bottom
	int k = sides + 1;
	for (int i = 1; i < sides; i++)
	{
		index_vector.push_back(k);
		index_vector.push_back(k + i + 1);
		index_vector.push_back(k + i);
	}

	index_vector.push_back(k);
	index_vector.push_back(sides + 2);
	index_vector.push_back(2 * sides + 1);

	num_vertex = vertex_vector.size();
	vertex = new float[num_vertex]();

	for (size_t i = 0; i < num_vertex; i++)
	{
		vertex[i] = vertex_vector[i];
	}

	num_index = index_vector.size();
	index = new uint[num_index]();

	for (size_t i = 0; i < num_index; i++)
	{
		index[i] = index_vector[i];
	}

	vertex_vector.clear();
	index_vector.clear();
}

void Cylinder::Render()
{
	RenderPrimitive(vertex, num_vertex, index, num_index);
}



//PYRAMID

Pyramid::Pyramid() : Primitive() {}

Pyramid::~Pyramid() {}

void Pyramid::Render()
{
	float vertex[15] = {
		//Top
		0.5f, 0.85f, 0.5f,
		//Bottom 
		0.0f ,0.0f, 0.0f,
		1.0f ,0.0f, 0.0f,
		1.0f ,0.0f, 1.0f,
		0.0f ,0.0f, 1.0f
	};

	uint index[20]{
		//Bottom
		0, 1, 2,
		2, 3, 0,

		3, 2, 4,	// Front
		1, 0, 4,	// Back
		2, 1, 3,	// Right
		0, 3, 4		// Left
	};

	RenderPrimitive(vertex, 15, index, 18);
}




//GRID

Grid::Grid(int grid_size) {
	if ((grid_size % 2) != 0)
		grid_size++;

	size = grid_size;
}

Grid::~Grid() {}

void Grid::Render()
{
	glBegin(GL_LINES);

	//Vertical Lines
	for (float x = -size * 0.5f; x <= size * 0.5f; x++)
	{
		glVertex3f(x, 0, -size * 0.5f);
		glVertex3f(x, 0, size * 0.5f);
	}

	//Hortiontal Lines
	for (float z = -size * 0.5f; z <= size * 0.5f; z++)
	{
		glVertex3f(-size * 0.5f, 0, z);
		glVertex3f(size * 0.5f, 0, z);
	}

	glEnd();
}