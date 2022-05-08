#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>

#include <math.h>

#define	GRAPH_MAX	36+1

struct GRAPH
{
	Fl_Window *window;

	double x[GRAPH_MAX+1];
	int n;
};

static struct GRAPH Graph;

class GRAPHBOX : public Fl_Box
{
    void draw()
	{
		// redraw background

		Fl_Box::draw();

		fl_color(fl_rgb_color(0, 0, 0));
		fl_line(x(), y()+h()/2, x()+w(), y()+h()/2);
		fl_line(x(), y(), x(), y()+h());

		// redraw graph

		int n;
		for (n=0; n<Graph.n; n++)
		{
			int xk = x() + 10*(n + 1);
			int yk = y() + h()/2 - 25*Graph.x[n];
			
			fl_color(fl_rgb_color(127, 0, 0));
			fl_begin_polygon();
			fl_arc(xk, yk, 5, 0, 360);
			fl_end_polygon();

			fl_color(fl_rgb_color(0, 0, 0));
			fl_begin_line();
			fl_circle(xk, yk, 5);
			fl_end_line();
		}

		fl_font(FL_COURIER, 16);
		char str[256];
		n = Graph.n-1;
		sprintf(str, "x(%2d)=% 1.4lf", n, Graph.x[n]);
		fl_draw(str, x()+w()/2, y()+h()-20);
    }

	public:
	GRAPHBOX(int x, int y, int w, int h, const char *l=0) : Fl_Box(x, y, w, h, l)
	{
		box(FL_FLAT_BOX);
		color(fl_rgb_color(127, 127, 127));
    }
};

static void Timer_CB(void *)
{
	if (Graph.n < GRAPH_MAX)
	{
		Graph.x[Graph.n+1] = Graph.x[Graph.n]/2.0 + 1.0/Graph.x[Graph.n];
		Graph.n++;

		if (Graph.window)
			Graph.window->redraw();
		Fl::repeat_timeout(0.5, Timer_CB);
	}
}

int main(void)
{
	Fl_Window window(400, 140, "Graph");
	GRAPHBOX graphbox(10, 10, window.w()-20, window.h()-20);
	window.show();

	Graph.window = &window;

	Graph.x[0] = 0.0;
	Graph.x[1] = 2.0;
	Graph.n = 1;

	Fl::add_timeout(0, Timer_CB);

	while (Fl::wait())
	{
		/* main loop */
	}
	return EXIT_SUCCESS;
}
