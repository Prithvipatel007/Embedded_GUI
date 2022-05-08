#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>

struct GRAPH
{
	Fl_Window *window;
	Fl_Slider *slider;
	
	int fill;
};

static GRAPH Graph;

class GRAPHBOX : public Fl_Box
{
    void draw()
	{
		// tank
		fl_color(fl_rgb_color(0, 0, 255));
		fl_rectf(20+0, 20+100-Graph.fill, 100, Graph.fill);
		fl_color(fl_rgb_color(255, 0, 0));
		fl_line(20+0, 20+0, 20+0, 20+100);
		fl_line(20+0, 20+100, 20+100, 20+100);
		fl_line(20+100, 20+0, 20+100, 20+100);

		fl_color(fl_rgb_color(0, 0, 0));
		fl_font(FL_COURIER, 16);
		fl_draw("Tank", 20+0, 20+100+20+20+20);
    }

	public : GRAPHBOX(int x, int y, int w, int h, const char* l=0) : Fl_Box(x, y, w, h, l)
	{
		box(FL_FLAT_BOX);
		color(fl_rgb_color(127, 127, 127));
    }
};

static void Slider_CB(Fl_Widget *, void *)
{
	Graph.fill = Graph.slider->value();

	Graph.window->redraw();
}

int main(void)
{
	Fl_Window window(400, 200, "Graph");
	GRAPHBOX graphbox(10, 10, window.w()-20, window.h()-20);
	Fl_Slider slider(20+0, 20+100+20, 100, 20);
	slider.type(FL_HOR_NICE_SLIDER);
	slider.bounds(0, 100);
	slider.value(50);
	slider.callback(Slider_CB);

	Graph.window = &window;
	Graph.slider = &slider;
	Graph.fill = slider.value();

	window.show();

	while (Fl::wait())
	{
		/* main loop */
	}
	return EXIT_SUCCESS;
}
