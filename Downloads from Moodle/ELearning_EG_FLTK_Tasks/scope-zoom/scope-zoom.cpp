#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Simple_Terminal.H>
//#include <FL/fl_Simple_Terminal.H>

#include <math.h>
#include <time.h>

#define DEBUG

#ifdef DEBUG
Fl_Simple_Terminal *TERMINAL;
#endif

#define	GRAPH_MAX	500

struct GRAPH
{
	Fl_Double_Window *window;

	int xk;
	int yk[GRAPH_MAX+2];
	double y;

	int scale;
	bool toggle;
	bool done;
};
static GRAPH Graph;

class GRAPHBOX : public Fl_Box
{
	void draw(void)
	{
		// redraw background

		Fl_Box::draw();

		fl_color(fl_rgb_color(255, 255, 255));
		fl_line(x(), y()+h()/2, x()+w(), y()+h()/2);
		fl_line(x(), y(), x(), y()+h());
		fl_draw("t", x()+w()-20, y()+h()/2+20);

		fl_font(FL_COURIER, 16);
		char str[256];
		
		if (Graph.toggle)
			sprintf(str, "Channel: A");
		else
			sprintf(str, "Channel: B");
		fl_draw(str, x()+10, y()+20);

		int val = (int) (Graph.scale*Graph.y);
		if (val < 0)
			sprintf(str, "x = -%4d", -1*val);
		else
			sprintf(str, "x = +%4d", val);
		fl_draw(str, x()+w()-100, y()+20);

		static long start = time(NULL);
		long tick = time(NULL) - start;
		sprintf(str, "Time: %02ld:%02ld", tick/60, tick%60);
		fl_draw(str, x()+10, y()+h()-10);

		sprintf(str, "(%3d)", Graph.scale);
		fl_draw(str, x()+w()-55, y()+h()-10);

		// redraw graph

		if (Graph.toggle)
			fl_color(fl_rgb_color(0, 255, 0));
		else
			fl_color(fl_rgb_color(255, 0, 0));
		int xk;
		for (xk=0; xk<Graph.xk; xk++)
			fl_line(x()+xk, y()+Graph.yk[xk]+h()/2, x()+xk+1, y()+Graph.yk[xk+1]+h()/2);
	}

	static void Timer_CB(void *)
	{
		// aquire every 10 msec
		
		if (Graph.toggle)
			Graph.y = (1.0*sin((Graph.xk+1)/20.0)+2.0*((rand()/((double) RAND_MAX))-0.5))/2.0;
		else
			Graph.y = (1.0*sin((Graph.xk+1)/20.0)+0.2*((rand()/((double) RAND_MAX))-0.5))/1.1;

		Graph.yk[Graph.xk+1] = -1.0*Graph.scale*Graph.y;

		// redraw every 100 msec

		if (Graph.xk%10 == 0)
		{
#ifdef DEBUG
			TERMINAL->printf("scale = %3d\n", Graph.scale);
#endif
			Graph.window->redraw();
		}

		Graph.xk++;
		if (Graph.xk > GRAPH_MAX) Graph.xk = 0;

		if (! Graph.done)
			Fl::repeat_timeout(0.01, Timer_CB);
	}

	public : GRAPHBOX(int x, int y, int w, int h, const char* l=0) : Fl_Box(x, y, w, h, l)
	{
		box(FL_FLAT_BOX);
		color(fl_rgb_color(0, 0, 0));

		Fl::add_timeout(0, Timer_CB);
	}
};

static void Zoom_out_CB(Fl_Widget *, void *data)
{
	Graph.scale -= 5;

	if (Graph.scale < 5)
		Graph.scale = 5;
}

static void Zoom_in_CB(Fl_Widget *, void *data)
{
	Graph.scale += 5;

	if (Graph.scale > 100)
		Graph.scale = 100;
}

static void Toggle_CB(Fl_Widget *, void *data)
{
	Graph.toggle = 1 - Graph.toggle;
	// redraw
	Graph.xk = 0;

	Fl_Button *button_toggle = (Fl_Button *) data;

	if (data)
	{
		if (Graph.toggle)
		{
			// button red
			button_toggle->color(fl_rgb_color(255-32-16, 32, 0));
		}
		else
		{
			// button green
			button_toggle->color(fl_rgb_color(127, 127+64, 127-16));
		}
	}
}

static void WAIT(double time)
{
	int cnt = clock();
	int ticks = (int) (time*CLOCKS_PER_SEC);
	while (true)
	{
		if (clock() > (cnt + ticks))
			return;
		else
			(void) Fl::check();
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

static void Lock_CB(Fl_Widget *, void *)
{
	/* window cannot be closed */
}

#define WINDOW_X	530
#define WINDOW_Y	590
#define	AREA		500

int main(void)
{
	Fl_Double_Window window(WINDOW_X, WINDOW_Y, "Scope");
	window.callback(Lock_CB);

	Graph.window = &window;

	Graph.xk = 0;
	Graph.yk[0] = 0;
	Graph.scale = 100;
	Graph.toggle = true;
	Graph.done = false;

	GRAPHBOX graphbox(10, 10, AREA+10, AREA+10);
	graphbox.show();

	Fl_Button button_quit(10, 10+AREA+10, 120, 60, "Quit");
	button_quit.box(FL_FLAT_BOX);
	button_quit.color(fl_rgb_color(127-64, 127-32, 127));
	button_quit.align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
	button_quit.labelfont(FL_HELVETICA);
	button_quit.labelsize(32);
	button_quit.labelcolor(fl_rgb_color(255, 255, 255));
	button_quit.callback(Quit_CB);

	Fl_Button button_toggle(10+120, 10+AREA+10, 120, 60, "Toggle");
	button_toggle.box(FL_FLAT_BOX);
	button_toggle.color(fl_rgb_color(255-32-16, 32, 0));
	button_toggle.align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
	button_toggle.labelfont(FL_HELVETICA);
	button_toggle.labelsize(32);
	button_toggle.labelcolor(fl_rgb_color(255, 255, 255));
	button_toggle.callback(Toggle_CB, &button_toggle);

	Fl_Button button_zoom_in(WINDOW_X-120-120-10, 10+AREA+10, 120, 60, "Zoom+");
	button_zoom_in.box(FL_FLAT_BOX);
	button_zoom_in.color(fl_rgb_color(127-64-32, 127-64-32, 127-64-32));
	button_zoom_in.align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
	button_zoom_in.labelfont(FL_HELVETICA);
	button_zoom_in.labelsize(32);
	button_zoom_in.labelcolor(fl_rgb_color(255-32-16, 32, 0));
	button_zoom_in.callback(Zoom_in_CB);

	Fl_Button button_zoom_out(WINDOW_X-120-10, 10+AREA+10, 120, 60, "Zoom-");
	button_zoom_out.box(FL_FLAT_BOX);
	button_zoom_out.color(fl_rgb_color(127-64-16, 127-64-16, 127-64-16));
	button_zoom_out.align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
	button_zoom_out.labelfont(FL_HELVETICA);
	button_zoom_out.labelsize(32);
	button_zoom_out.labelcolor(fl_rgb_color(255, 255, 255));
	button_zoom_out.callback(Zoom_out_CB);

	// center window
	window.position((Fl::w()-window.w())/2, (Fl::h()-window.h())/2);
	window.show();

#ifdef DEBUG
	Fl_Double_Window console(300, 200, "Console");

	Fl_Simple_Terminal terminal(10, 10, 300-20, 200-20);

	TERMINAL = &terminal;

	// move window
	console.position(Fl::w(), Fl::h()-200);
	console.show();

	terminal.printf("Scope APP started ...\n");
#endif

	Fl::run();
	return EXIT_SUCCESS;
}
