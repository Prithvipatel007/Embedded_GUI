#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Output.H>

static Fl_Window Window(400, 140, "Hello");
static Fl_Output Text(100, 40, 200, 40);

int main(void)
{
	Text.value("Hello APP");
	Window.show();
	while (Fl::wait())
	{
		/* main loop */
	}
	return EXIT_SUCCESS;
}
