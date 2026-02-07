# include <Siv3D.hpp>
#include "Application.h"

void Main()
{
	Application app;

	if (app.SystemInit())
	{
		app.Run();
	}
	
}
