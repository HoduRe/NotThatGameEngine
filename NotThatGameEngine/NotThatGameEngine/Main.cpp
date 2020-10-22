#include <stdlib.h>
#include "Application.h"

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )

enum class main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

int main(int argc, char ** argv)
{
	int main_return = EXIT_FAILURE;
	main_states state = main_states::MAIN_CREATION;
	Application* App = NULL;

	while (state != main_states::MAIN_EXIT)
	{
		switch (state)
		{
		case main_states::MAIN_CREATION:

			App = new Application();
			state = main_states::MAIN_START;
			break;

		case main_states::MAIN_START:

			if (App->Init() == false)
			{
				state = main_states::MAIN_EXIT;
			}
			else
			{
				state = main_states::MAIN_UPDATE;
			}

			break;

		case main_states::MAIN_UPDATE:
		{
			int update_return = (int)App->Update();

			if (update_return == (int)update_status::UPDATE_ERROR)
			{
				state = main_states::MAIN_EXIT;
			}

			else if (update_return == (int)update_status::UPDATE_STOP)
				state = main_states::MAIN_FINISH;
		}
			break;

		case main_states::MAIN_FINISH:

			if (App->CleanUp() == false) {}
			else
				main_return = EXIT_SUCCESS;

			state = main_states::MAIN_EXIT;

			break;

		}
	}

	delete App;
	return main_return;
}