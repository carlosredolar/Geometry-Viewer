#include "Globals.h"
#include "Application.h"

#include "GameObject.h"
#include "Component_Material.h"
#include "Component_Camera.h"

#include <vector>
#include "FileManager.h"

#include "ModuleJson.h"

#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

#include "Libs/Glew/include/glew.h"
#include "Libs/SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Libs/Devil/include/IL/il.h"

#pragma comment (lib, "glu32.lib")          /* link OpenGL Utility lib */
#pragma comment (lib, "opengl32.lib")     /* link Microsoft OpenGL lib */
#pragma comment (lib, "Libs/Glew/libx86/glew32.lib")		  /* link glew lib */

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled), cameraCulling(true), context(nullptr)
{
	name = "renderer";

	_ray = LineSegment();
	display_mode = SOLID;
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOG_ERROR("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	GLenum error = glewInit();

	if (error != GL_NO_ERROR)
	{
		LOG_ERROR("Error initializing glew library! %s", SDL_GetError());
		ret = false;
	}

	if (ret == true)
	{
		//Use Vsync
		if (vsync && SDL_GL_SetSwapInterval(1) < 0)
			LOG_ERROR("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(App->camera->GetProjectionMatrix());

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG_ERROR("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf(App->camera->GetViewMatrix());		

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG_ERROR("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG_ERROR("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].reference = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPosition(0.0f, 0.0f, 2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glEnable(GL_STENCIL_TEST);
		lights[0].Enable(true);
	}

	GenerateBuffers();

	return ret;
}

bool ModuleRenderer3D::LoadConfig(JsonObj& config)
{
	debug = config.GetBool("debug");
	vsync = config.GetBool("vsync");

	drawVertexNormals = config.GetBool("draw_vertex_normals");
	drawFaceFormals = config.GetBool("draw_face_normals");

	return true;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	Color c = App->camera->background;
	glClearColor(c.r, c.g, c.b, c.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(App->camera->GetViewMatrix());

	float3 cameraPosition = App->camera->GetPosition();

	lights[0].SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i) lights[i].Render();

	return UPDATE_CONTINUE;
}

update_status ModuleRenderer3D::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;
	DrawRay();
	return ret;
}

update_status ModuleRenderer3D::PostUpdate(float dt)
{
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	App->gui->Draw();

	SDL_GL_SwapWindow(App->window->window);

	if (App->scene->selectedGameObject != nullptr && App->scene->selectedGameObject->toDelete)
	{
		App->scene->DeleteGameObject(App->scene->selectedGameObject);
		App->scene->selectedGameObject = nullptr;
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteTextures(1, &colorTexture);

	SDL_GL_DeleteContext(context);
	mainCamera = nullptr;

	return true;
}


void ModuleRenderer3D::ScreenResized(int width, int height)
{
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(App->camera->GetProjectionMatrix());
}

void ModuleRenderer3D::UpdateProjectionMatrix(float* projectionMatrix)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(App->camera->GetViewMatrix());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(App->camera->GetProjectionMatrix());
}

void ModuleRenderer3D::SetDisplayMode(RenderMode display) 
{ 
	GLenum face = GL_FRONT;

	display_mode = display;

	if (!glIsEnabled(GL_CULL_FACE_MODE))
		face = GL_FRONT_AND_BACK;

	switch (display)
	{
	case SOLID:
		glPolygonMode(face, GL_FILL);
		glDisable(GL_POLYGON_SMOOTH);
		break;
	case WIREFRAME:
		glPolygonMode(face, GL_LINE);
		glDisable(GL_POLYGON_SMOOTH);
		break;
	case SOLIDWIRE:
		glPolygonMode(face, GL_FILL);
		glEnable(GL_POLYGON_SMOOTH);
		break;
	default:
		break;
	}
}

RenderMode ModuleRenderer3D::GetDisplayMode()
{
	return display_mode;
}

void ModuleRenderer3D::SetMainCamera(Component_Camera* camera)
{
	mainCamera = camera;
}

Component_Camera* ModuleRenderer3D::GetMainCamera()
{
	return mainCamera;
}

void ModuleRenderer3D::SetCap(GLenum cap, bool enabled)
{
	if (enabled)
		glEnable(cap);
	else
		glDisable(cap);
}

void ModuleRenderer3D::SetVSync(bool enabled)
{
	if (enabled)
	{
		if (SDL_GL_SetSwapInterval(1) == -1)
		{
			LOG_ERROR("VSync not supported");
		}
		else
		{
			LOG("VSync enabled");
		}
	}
	else
	{
		if (SDL_GL_SetSwapInterval(0) == -1)
		{
			LOG_ERROR("VSync not supported");
		}
		else
		{
			LOG("VSync disabled");
		}
	}

}

void ModuleRenderer3D::DrawRay()
{
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.85f, 0.85f);
	glVertex3f(_ray.a.x, _ray.a.y, _ray.a.z);
	glVertex3f(_ray.b.x, _ray.b.y, _ray.b.z);
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();
}

void ModuleRenderer3D::GenerateBuffers()
{
	//FrameBuffer
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	//Color Texture Buffer ===========================================================================================================
	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, App->window->width, App->window->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	//RenderBuffer
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->window->width, App->window->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	//Depth RenderBuffer =============================================================================================================
	//glGenRenderbuffers(1, &depthRenderBuffer);
	//glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App->window->width, App->window->height);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, App->window->width, App->window->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	//==================================================================================================================================

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	/*GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);*/

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG_ERROR("Famebuffer is not complete");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ModuleRenderer3D::DrawDirectModeCube()
{
	int CHECKERS_WIDTH = 64;
	int CHECKERS_HEIGHT = 64;
	GLubyte checkerImage[64][64][4];

	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	GLuint textureID;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	{
		glBegin(GL_TRIANGLES);
		//Bottom Face
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 0.f, 0.f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.f, 0.f, 0.f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 0.f, 1.f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 0.f, 1.f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(0.f, 0.f, 1.f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 0.f, 0.f);

		//Front Face
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 0.f, 1.f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.f, 0.f, 1.f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 1.f, 1.f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 1.f, 1.f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(0.f, 1.f, 1.f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 0.f, 1.f);

		//Left Face
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 0.f, 0.f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.f, 0.f, 1.f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(0.f, 1.f, 1.f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(0.f, 1.f, 1.f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(0.f, 1.f, 0.f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 0.f, 0.f);

		//Right Face
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(1.f, 0.f, 0.f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.f, 1.f, 0.f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 1.f, 1.f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 1.f, 1.f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(1.f, 0.f, 1.f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(1.f, 0.f, 0.f);

		//Back Face
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 0.f, 0.f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.f, 1.f, 0.f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 1.f, 0.f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 1.f, 0.f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(1.f, 0.f, 0.f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 0.f, 0.f);

		//Top Face
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 1.f, 0.f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.f, 1.f, 1.f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 1.f, 1.f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 1.f, 1.f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(1.f, 1.f, 0.f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 1.f, 0.f);
		glEnd();
	}

	glDeleteTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, 0);
	ilBindImage(0);
}


void ModuleRenderer3D::DrawAABB(float3* cornerPoints, ImVec4 color)
{
	glDisable(GL_LIGHTING);
	glColor3f(color.x, color.y, color.z);
	glBegin(GL_LINES);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);

	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);

	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);
	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);

	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);

	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
}