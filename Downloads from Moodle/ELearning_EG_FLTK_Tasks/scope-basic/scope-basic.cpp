#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Slider.H>
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
	Fl_Slider *slider;

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
			Graph.scale = Graph.slider->value();
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

static void Toggle_CB(Fl_Widget *, void *)
{
	Graph.toggle = 1 - Graph.toggle;
	// redraw
	Graph.xk = 0;
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

#define WINDOW_X	530
#define WINDOW_Y	610
#define	AREA		500
#define	MENU_Y		 25

int main(void)
{
	Fl_Double_Window window(WINDOW_X, WINDOW_Y+MENU_Y, "Scope");
	window.callback(Quit_CB);

	Fl_Slider slider(10, WINDOW_Y-10, AREA+10, MENU_Y, "Scale");
	slider.type(FL_HOR_NICE_SLIDER);
	slider.align(FL_ALIGN_TOP);
	slider.labelfont(FL_COURIER);
	slider.labelsize(16);
	slider.bounds(5, 100);
	slider.value(100);

	Graph.window = &window;
	Graph.slider = &slider;

	Graph.xk = 0;
	Graph.yk[0] = 0;
	Graph.scale = slider.value();
	Graph.toggle = true;
	Graph.done = false;

	GRAPHBOX graphbox(10, 10+MENU_Y, AREA+10, AREA+10+MENU_Y);
	graphbox.show();

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
