#include "Application.h"
#include "ModuleRenderer3D.h"

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled), context(nullptr), cube()
{}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	bool ret = true;

	// OPENGL - SDL CONFIGURATION
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		ret = false;
	}

	if (ret == true)
	{
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
			LOG("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
		}

		LOG("Love me!");

		// Vsync
		SDL_GL_SetSwapInterval(1);

		// OPENGL INITIALIZATION

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			ret = false;
		}

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

		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	std::vector<float> vertices = { 0.0f, 0.0f, 0.0f,   0.0f, 2.0f, 0.0f,   2.0f, 2.0f, 0.0f,   2.0f, 0.0f, 0.0f,   0.0f, 0.0f, 2.0f,   2.0f, 2.0f, 2.0f,   0.0f, 2.0f, 2.0f,   2.0f, 0.0f, 2.0f };


	std::vector<unsigned int> index = { 0, 1, 2,   3, 0, 2,	// Front
								4, 5, 6,   5, 4, 7,	// Back
								2, 1, 6,   5, 2, 6, // Up
								3, 4, 0,   7, 4, 3,	// Down
								4, 6, 0,   6, 1, 0,	// Right
								7, 3, 5,   5, 3, 2	// Left
	};

	cube.SetVertexVector(vertices);
	cube.SetIndexVector(index);

	glGenBuffers(1, (GLuint*)&cube.idVertex);
	glBindBuffer(GL_ARRAY_BUFFER, cube.idVertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cube.sizeVertexVector, cube.GetVertexVector().data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, (GLuint*)&cube.idIndex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.idIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * cube.sizeIndexVector, cube.GetIndexVector().data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// Clear z-buffer
	glClearDepth(1.0f);

	// Clear screen color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	// Geometry


	// Debug

	return update_status::UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	update_status ret = update_status::UPDATE_CONTINUE;

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, cube.idVertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.idIndex);

	glDrawElements(GL_TRIANGLES, cube.sizeIndexVector, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

	return ret;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 1024.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



