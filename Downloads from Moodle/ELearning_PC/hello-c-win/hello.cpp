#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Output.H>

int main(void)
{
	Fl_Window window(400, 140, "Hello");
	Fl_Output text(100, 40, 200, 40);
	text.value("Hello APP");
	window.show();
	while (Fl::wait())
	{
		/* main loop */
	}
	return EXIT_SUCCESS;
}
