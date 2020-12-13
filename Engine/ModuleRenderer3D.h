#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "Component_Light.h"

#define MAX_LIGHTS 8

typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef unsigned char GLubyte;
typedef void* SDL_GLContext;
class Component_Camera;
struct ImVec4;

enum RenderMode
{
	SOLID,
	WIREFRAME,
	SOLIDWIRE
};

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	bool LoadConfig(JsonObj& config) override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();
	
	void ScreenResized(int width, int height);
	void UpdateProjectionMatrix(float* projectionMatrix);

	void SetDisplayMode(RenderMode display);
	RenderMode GetDisplayMode();
	void SetMainCamera(Component_Camera* camera);
	Component_Camera* GetMainCamera();

	void SetCap(GLenum cap, bool enabled);
	void SetVSync(bool enabled);
	void DrawAABB(float3* aabb, ImVec4 color);
	void DrawRay();
	
private:
	void GenerateBuffers();
	void DrawDirectModeCube();

public:
	GLuint colorTexture;
	GLuint renderBuffer;
	GLuint depthRenderBuffer;
	GLuint depthTexture;

	Component_Light lights[MAX_LIGHTS];
	SDL_GLContext context;

	RenderMode display_mode;

	LineSegment _ray;

	bool cameraCulling;
	bool drawVertexNormals;
	bool drawFaceFormals;
	bool vsync;

private:
	bool debug;
	Component_Camera* mainCamera;
	GLuint frameBuffer;
};