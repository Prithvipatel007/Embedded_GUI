#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>

struct GRAPH
{
	Fl_Double_Window *window;
	Fl_Slider *slider_A;
	Fl_Slider *slider_B;
	
	int fill_A;
	int fill_B;
};

static GRAPH Graph;

class GRAPHBOX : public Fl_Box
{
    void draw()
	{
		// tank
		fl_color(fl_rgb_color(0, 0, 255));
		fl_rectf(20+0, 20+100-Graph.fill_A, 100, Graph.fill_A);
		fl_color(fl_rgb_color(255, 0, 0));
		fl_line(20+0, 20+0, 20+0, 20+100);
		fl_line(20+0, 20+100, 20+100, 20+100);
		fl_line(20+100, 20+0, 20+100, 20+100);

		// tank
		fl_color(fl_rgb_color(0, 0, 255));
		fl_rectf(200+20+0, 20+100-Graph.fill_B, 100, Graph.fill_B);
		fl_color(fl_rgb_color(255, 0, 0));
		fl_line(200+20+0, 20+0, 200+20+0, 20+100);
		fl_line(200+20+0, 20+100, 200+20+100, 20+100);
		fl_line(200+20+100, 20+0, 200+20+100, 20+100);

		fl_color(fl_rgb_color(0, 0, 0));
		fl_font(FL_COURIER, 16);
		fl_draw("Tank A", 20+0, 20+100+20+20+20);
		fl_draw("Tank B", 200+20+0, 20+100+20+20+20);
    }

	public:
	GRAPHBOX(int x, int y, int w, int h, const char* l=0) : Fl_Box(x, y, w, h, l)
	{
		box(FL_FLAT_BOX);
		color(fl_rgb_color(127, 127, 127));
    }
};

static void Slider_B_CB(Fl_Widget *, void *)
{
	Graph.fill_B = Graph.slider_B->value();

	Graph.window->redraw();
}

static void Slider_A_CB(Fl_Widget *, void *)
{
	Graph.fill_A = Graph.slider_A->value();

	Graph.window->redraw();
}

int main(void)
{
	Fl_Double_Window window(400, 200, "Graph");
	GRAPHBOX graphbox(10, 10, window.w()-20, window.h()-20);
	Fl_Slider slider_A(20+0, 20+100+20, 100, 20);
	slider_A.type(FL_HOR_NICE_SLIDER);
	slider_A.bounds(0, 100);
	slider_A.value(50);
	slider_A.callback(Slider_A_CB);
	Fl_Slider slider_B(200+20+0, 20+100+20, 100, 20);
	slider_B.type(FL_HOR_NICE_SLIDER);
	slider_B.bounds(0, 100);
	slider_B.value(50);
	slider_B.callback(Slider_B_CB);

	Graph.window = &window;
	Graph.slider_A = &slider_A;
	Graph.slider_B = &slider_B;
	Graph.fill_A = slider_A.value();
	Graph.fill_B = slider_B.value();

	window.show();

	while (Fl::wait())
	{
		/* main loop */
	}
	return EXIT_SUCCESS;
}
