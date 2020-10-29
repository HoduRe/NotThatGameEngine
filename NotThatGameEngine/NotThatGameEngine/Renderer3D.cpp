#include "Application.h"
#include "Mesh.h"

Renderer3D::Renderer3D(Application* app, bool start_enabled) : Module(app, start_enabled), context(nullptr), frameBufferId(0), sceneTextureId(0), depthStencilId(0)
{}

// Destructor
Renderer3D::~Renderer3D()
{}

// Called before render is available
bool Renderer3D::Init()
{

	bool ret = true;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL) { ret = false; }

	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) { LOG("Error initializing GLEW! %s\n", glewGetErrorString(glewError)); }

	// Vsync
	SDL_GL_SetSwapInterval(1);
	ret = SetOpenGL();

	if (ret) {

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		lights[0].Active(true);

		// Projection matrix for
		OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

		LOG("Love me!\n");
	}

	return ret;
}

// PreUpdate: clear buffer
update_status Renderer3D::PreUpdate(float dt)
{

	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	glClearDepth(1.0f);
	glClearColor(0.f, 0.f, 0.f, 1.f);

	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
	for (uint i = 0; i < MAX_LIGHTS; ++i) { lights[i].Render(); }

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return update_status::UPDATE_CONTINUE;
}


update_status Renderer3D::PostUpdate(float dt)
{
	update_status ret = update_status::UPDATE_CONTINUE;

	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

	DrawGrid();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return ret;
}

// Called before quitting
bool Renderer3D::CleanUp()
{
	glDeleteTextures(1, &sceneTextureId);
	glDeleteFramebuffers(1, &frameBufferId);

	SDL_GL_DeleteContext(context);

	return true;
}


void Renderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 1024.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glGenFramebuffers(1, &frameBufferId);	// TODO: WHY AM I CREATING A FRAMEBUFFER ON EVERY RESIZE
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

	glGenTextures(1, &sceneTextureId);
	glBindTexture(GL_TEXTURE_2D, sceneTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenRenderbuffers(1, &depthStencilId);
	glBindRenderbuffer(GL_RENDERBUFFER, depthStencilId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilId);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTextureId, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { LOG("Scene buffer is not loaded properly."); }

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


void Renderer3D::DrawGrid() {
	glLineWidth(1.0f);

	glBegin(GL_LINES);
	glColor4f(0.8f, 0.8f, 0.8f, 0.8f);

	float d = 40.0f;

	for (float i = -d; i <= d; i += 2.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}


void Renderer3D::SetFrameBuffer(GLuint buffer) { glBindFramebuffer(GL_FRAMEBUFFER, buffer); }




