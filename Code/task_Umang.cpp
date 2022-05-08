#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Chart.H>
#include <FL/Fl_Timer.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Output.H>

#include <unistd.h>

#include <math.h>
#include <time.h>

#define WINDOW_X 600
#define WINDOW_Y 480
#define CONTROL_H 60
#define MARGIN 10
#define CHART_X 580
#define CHART_Y 400

#define GRAPH_MAX 500

//---------------------------------------------------- Enums
enum Status
{
    idle,
    running
};

//---------------------------------------------------- Struct

struct GRAPH
{
    Fl_Double_Window *window;
    Fl_Button *btn_start_stop;
    Fl_Output *out_status;
    Fl_Output *out_temp;
    Fl_Output *out_time_e;
    Fl_Timer *timer_e;

    int xk;
    int yk[GRAPH_MAX + 2];
    double y;

    int scale;
    bool done;

    bool run;
    Status status;
};

//---------------------------------------------------- Global Variables

static GRAPH Graph;
double t = 0;

//---------------------------------------------------- Class

class POINT
{
public:
    int x = 0;

public:
    int y = 0;

public:
    POINT(int x_val, int y_val)
    {
        x = x_val;
        y = y_val;
    }
};
POINT last_Point(0, 0);

class GRAPHBOX : public Fl_Box
{
    void draw(void)
    {
        Fl_Box::draw();

        fl_color(fl_rgb_color(255, 0, 0));

        int xk;
        for (xk = 0; xk < Graph.xk; xk++)
        {
            fl_line(x() + xk, y() + xk + h() / 2, x() + xk + 1, y() + xk + 1 + h() / 2);
        }
    }

    static void Timer_CB(void *)
    {
        // aquire every 10 msec

        Graph.y = (1.0 * sin((Graph.xk + 1) / 20.0) + 0.2 * ((rand() / ((double)RAND_MAX)) - 0.5)) / 1.1;

        Graph.yk[Graph.xk + 1] = -1.0 * Graph.scale * Graph.y;

        // redraw every 100 msec

        if (Graph.xk % 10 == 0)
        {
            Graph.window->redraw();
        }

        Graph.xk++;
        if (Graph.xk > GRAPH_MAX)
            Graph.xk = 0;

        if (!Graph.done)
            Fl::repeat_timeout(0.01, Timer_CB);
    }

public:
    GRAPHBOX(int x, int y, int w, int h, const char *l = 0) : Fl_Box(x, y, w, h, l)
    {
        box(FL_FLAT_BOX);
        color(fl_rgb_color(0, 0, 0));

        Fl::add_timeout(0, Timer_CB);
    }
};

//---------------------------------------------------- Static Methods

static void Start_Stop_CB(Fl_Widget *, void *)
{
    if (!Graph.run)
    {
        Graph.out_status->value("Running");
        Graph.run = true;
        Graph.status = running;
        Graph.btn_start_stop->label("Stop");
    }
    else
    {
        Graph.out_status->value("Idle");
        Graph.run = false;
        Graph.status = idle;
        Graph.btn_start_stop->label("Start");
    }
}

//---------------------------------------------------- Main Methods

int main(void)
{
    //Window
    Fl_Double_Window window(WINDOW_X, WINDOW_Y, "Embedded GUI");

    //Chart
    GRAPHBOX chart(10, 10, WINDOW_X - 20, WINDOW_Y - 80);

    //Start Button
    Fl_Button btn_start_stop(0, WINDOW_Y - CONTROL_H, 120, 60, "Start");
    btn_start_stop.box(FL_FLAT_BOX);
    btn_start_stop.color(fl_rgb_color(127 - 64, 127 - 32, 127));
    btn_start_stop.align(FL_ALIGN_INSIDE | FL_ALIGN_CENTER);
    btn_start_stop.labelfont(FL_HELVETICA);
    btn_start_stop.labelsize(32);
    btn_start_stop.labelcolor(fl_rgb_color(255, 255, 255));
    btn_start_stop.callback(Start_Stop_CB);

    //Status of the machine
    Fl_Output out_status(120, WINDOW_Y - CONTROL_H, 180, 60);
    out_status.value("Idle");
    out_status.color(fl_rgb_color(0, 255, 0));
    out_status.textsize(32);
    out_status.textcolor(fl_rgb_color(255, 0, 0));

    //Temperature
    Fl_Output out_temp(300, WINDOW_Y - CONTROL_H, 100, 60);
    out_temp.value("70°C");
    out_temp.color(fl_rgb_color(255, 255, 255));
    out_temp.textsize(32);
    out_temp.textcolor(fl_rgb_color(255, 0, 0));

    //Elasped Time
    Fl_Output out_time_e(400, WINDOW_Y - CONTROL_H, 200, 60);
    // Fl_Timer time_e(FL_VALUE_TIMER, 400, WINDOW_Y - CONTROL_H, 200, 60, &x);
    out_time_e.value("04.07.2021");
    out_time_e.color(fl_rgb_color(0, 0, 0));
    out_time_e.textsize(32);
    out_time_e.textcolor(fl_rgb_color(255, 255, 255));
    out_time_e.value();

    //Initialization
    Graph.window = &window;
    Graph.btn_start_stop = &btn_start_stop;
    Graph.out_status = &out_status;
    Graph.out_temp = &out_temp;
    Graph.out_time_e = &out_time_e;
    // Graph.timer_e = &time_e;
    Graph.run = 0;
    Graph.status = idle;

    window.show();
    Fl::run();
    return EXIT_SUCCESS;
}