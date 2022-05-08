#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Chart.H>
#include <FL/Fl_Timer.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Output.H>

#include <unistd.h>
#include <chrono>

#include <math.h>
#include <time.h>

#define WINDOW_X 900
#define WINDOW_Y 980
#define CONTROL_H 60
#define MARGIN 10
#define CHART_X 580
#define CHART_Y 400

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

    bool run;
    Status status;
};

//---------------------------------------------------- Global Variables

static GRAPH Graph;
double t = 0;
int last_x = 0;
int last_y = 0;
int current_x;
int current_y;

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

//---------------------------------------------------- Class

class CHART : public Fl_Box
{
    void draw()
    {

        Fl_Box::draw();
        fl_color(fl_rgb_color(255, 255, 255));

        // fl_line(0, 0, 100, 100);
        // drawChart(0, 0);
    }

    static void Timer_CB(void *)
    {

        fl_color(fl_rgb_color(255, 255, 255));
        
//        fl_line(t+11, t+11, t + 12, t + 12);

        last_x = current_x;
        last_y = current_y;

        if (Graph.run)
        {
            t = t + 1;
            Fl::repeat_timeout(0.01, Timer_CB);
        }
    }

public:
    CHART(int x, int y, int w, int h, const char *l = 0) : Fl_Box(x, y, w, h, l)
    {
        box(FL_FLAT_BOX);
        color(fl_rgb_color(127, 127, 127));

        Fl::add_timeout(0, Timer_CB);
    }
};

//---------------------------------------------------- Main Methods

int main(void)
{
    //Window
    Fl_Double_Window window(WINDOW_X, WINDOW_Y, "Embedded GUI");

    //Chart
    CHART chart(10, 10, WINDOW_X - 20, WINDOW_Y - 80);

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
    Graph.run = false;
    Graph.status = idle;

    window.show();
    Fl::run();
    return -1;
}