#include "Application.h"

Application::Application() : userDt(false), msVecCounter(0), framerateVecCounter(0), framerate(0), consoleVecSize(0)
{
	window = new Window(this);
	input = new Input(this);
	audio = new Audio(this, true);
	engineScene = new Scene(this);
	renderer3D = new Renderer3D(this);
	camera = new Camera3D(this);
	physics = new Physics3D(this);
	player = new Player(this);
	reset = new ResetModule(this);
	imGui = new ManagerImGui(this);
	eventManager = new ManagerEvent(this);
	primitives = new ManagerPrimitives(this);
	fileLoad = new FileSystem(this);
	modelManager = new ManagerModel(this);
	editorScene = new EditorScene(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(reset);
	AddModule(eventManager);
	AddModule(window);
	AddModule(fileLoad);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(physics);
	AddModule(engineScene);
	AddModule(editorScene);
	AddModule(player);
	AddModule(modelManager);

	// Renderer last!
	AddModule(renderer3D);
	AddModule(primitives);
	AddModule(imGui);
}

Application::~Application()
{
	modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	int size = modules.size();

	for (int i = 0; i < size; i++)
	{
		modules[i]->Init();
	}

	for (int i = 0; i < size; i++)
	{
		modules[i]->Start();
	}

	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	float currentSec = ms_timer.Read() / 1000.0f;

	if (userDt) {
		framerate = imGui->sliderDt;
		dt = 1 / imGui->sliderDt;
	}
	else {
		dt = currentSec;
		framerate = 1 / dt;
	}

	// Dt data storing

	framerateVec.push_back(framerate);
	framerateVecCounter++;
	if (framerateVecCounter > 100) {
		framerateVec.erase(framerateVec.begin());
		framerateVecCounter--;
	}

	msVec.push_back(ms_timer.Read());
	msVecCounter++;
	if (msVecCounter > 100) {
		msVec.erase(msVec.begin());
		msVecCounter--;
	}

	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (userDt) {
		float timer = ms_timer.Read();
		float dtTemp = dt * 1000;
		if (dtTemp > timer) { SDL_Delay(dtTemp - timer); }
	}
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = update_status::UPDATE_CONTINUE;
	PrepareUpdate();

	int size = modules.size();

	for (int i = 0; i < size && ret == update_status::UPDATE_CONTINUE; i++)
	{
		ret = modules[i]->PreUpdate(dt);
		if (ret != update_status::UPDATE_CONTINUE) { i = size; }
	}

	for (int i = 0; i < size && ret == update_status::UPDATE_CONTINUE; i++)
	{
		ret = modules[i]->Update(dt);
		if (ret != update_status::UPDATE_CONTINUE) { i = size; }
	}

	for (int i = 0; i < size && ret == update_status::UPDATE_CONTINUE; i++)
	{
		ret = modules[i]->PostUpdate(dt);
		if (ret != update_status::UPDATE_CONTINUE) { i = size; }
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	int size = modules.size();

	for (int i = size - 1; i > -1; i--)
	{
		ret = modules[i]->CleanUp();
		if (ret == false) { i = -1; }
	}

	framerateVec.clear();
	msVec.clear();
	framerateVecCounter = 0;
	msVecCounter = 0;

	return ret;
}

void Application::AddModule(Module* mod)
{
	modules.push_back(mod);
}


void Application::UserDt(bool dt) {
	userDt = dt;
}


void Application::log(const char file[], int line, const char* format, ...)
{
	char tmp_string[4096];
	char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	consoleVec.push_back(tmp_string2);
	consoleVecSize++;

	if (consoleVecSize >= 50) {
		consoleVec.erase(consoleVec.begin());
		consoleVecSize--;
	}
}

