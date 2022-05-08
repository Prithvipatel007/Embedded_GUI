#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Simple_Terminal.H>
#include <FL/Fl_Pixmap.H>

#include <math.h>
#include <time.h>

#define DEBUG

#ifdef DEBUG
Fl_Simple_Terminal *TERMINAL;
#endif

#define GRAPH_MAX 500

struct GRAPH
{
    Fl_Double_Window *window;
    Fl_Slider *slider;

    int xk;
    int yk[GRAPH_MAX + 2];
    double y;

    int scale;
    bool toggle;
    bool done;
};
static GRAPH Graph;

class GRAPHBOX : public Fl_Box
{
};

static void WAIT(double time)
{
    int cnt = clock();
    int ticks = (int)(time * CLOCKS_PER_SEC);
    while (true)
    {
        if (clock() > (cnt + ticks))
            return;
        else
            (void)Fl::check();
    }
}

static void Quit_CB(Fl_Widget *, void *)
{
#ifdef DEBUG
    TERMINAL->printf("... done\n");
#endif
    Graph.done = true;

    fl_cursor(FL_CURSOR_WAIT);
    WAIT(1.0);

    exit(EXIT_SUCCESS);
}

#define WINDOW_X 400
#define WINDOW_Y 200
#define AREA 500


int main(void)
{
    Fl_Window window(WINDOW_X, WINDOW_Y, "Task");
    window.callback(Quit_CB);

	Fl_Button status_btn(0, WINDOW_Y - 60, 120, 60, "Stop");
	status_btn.box(FL_FLAT_BOX);
	status_btn.color(fl_rgb_color(127-64, 127-32, 127));
//	button_quit.align(FL_ALIGN_BOTTOM_LEFT);
   	status_btn.align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
	status_btn.labelfont(FL_HELVETICA);
	status_btn.labelsize(32);
	status_btn.labelcolor(fl_rgb_color(255, 255, 255));
	status_btn.callback(Quit_CB);


    Fl_Output status_output(120, WINDOW_Y - 60, 120, 60);
    status_output.value("Running");
	status_output.box(FL_FLAT_BOX);
   	status_output.color(fl_rgb_color(255-32-16, 32, 0));
    status_output.align(FL_ALIGN_INSIDE | FL_ALIGN_CENTER);
    status_output.labelfont(FL_HELVETICA);
	status_output.labelsize(32);
	status_output.labelcolor(fl_rgb_color(255, 255, 255));

    Fl_Output temperature_output(240, WINDOW_Y - 60, 120, 60);
    temperature_output.value("Running");
	temperature_output.box(FL_FLAT_BOX);
   	temperature_output.color(fl_rgb_color(255-32-16, 32, 0));
    temperature_output.align(FL_ALIGN_INSIDE | FL_ALIGN_CENTER);
    temperature_output.labelfont(FL_HELVETICA);
//	temperature_output.labelsize(32);
    temperature_output.textsize(64);
	temperature_output.labelcolor(fl_rgb_color(255, 255, 255));


	// center window
	window.position((Fl::w()-window.w())/2, (Fl::h()-window.h())/2);
	window.show();


    Fl::run();
    return EXIT_SUCCESS;
}
