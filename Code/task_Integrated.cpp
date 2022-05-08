//---------------------------------------------------- FLTK includes
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
//----------------------------------------------------

//---------------------------------------------------- SPS_input includes
#include <fcntl.h>
#include <termios.h>

#include <sys/poll.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;
//----------------------------------------------------

//---------------------------------------------------- FLTK defines and global variables
#define WINDOW_X 600
#define WINDOW_Y 410
#define CONTROL_H 60
#define MARGIN 10
#define CHART_X 580
#define CHART_Y 330

//#define GRAPH_MAX 500
#define GRAPH_MAX 192

double t = 0;
//----------------------------------------------------

//---------------------------------------------------- SPS_input defines and global variables
#define INMSG_MAX_LEN 15
#define OUTMSG_MAX_LEN 15

static char SPS_msg[] = "(------,------)X";

static int SPS_fd = -1;

static const double VCOM_READ_DELAY = 0.050;
//----------------------------------------------------

//---------------------------------------------------- SPS_input Methods
static void VCOM_read(int fd, char *str)
{
    struct pollfd pollevents;
    pollevents.fd = fd;
    pollevents.events = POLLIN;

    int idx, num;
    char ch;

    for (idx = 0; idx < INMSG_MAX_LEN; idx++)
    {
        // read time-out 500 msec
        if ((poll(&pollevents, 1, 1500)) == 0)
        {
            printf("read() time-out\n");
            break;
        }
        else
            num = read(fd, &ch, 1);

        if (num <= 0)
            break;

        str[idx] = ch;
    }

    // clear input buffer
    usleep(VCOM_READ_DELAY * 1000 * 1000);
    tcflush(fd, TCIFLUSH);

    printf("<-%s\n", str);
}

struct GRAPH
{
    Fl_Double_Window *window;
    Fl_Button *btn_start_stop;
    Fl_Output *out_status;
    Fl_Output *out_data_1;
    Fl_Output *out_data_2;
    Fl_Output *out_time_e;
    Fl_Timer *timer_e;
    Fl_Input *Time_Input;

    int i;
    int i_Idle;
    int yk[GRAPH_MAX + 2];
    int xk[GRAPH_MAX + 2];
    double y;

    int scale;
    bool done;

    bool running;

    const char IDLE_Time = 0;
};
static GRAPH Graph;

// (------,------)X
static void VCOM_write(int fd, const char *str)
{
    int idx, num;
    char ch;

    printf("->%s\n", str);

    for (idx = 0; idx < OUTMSG_MAX_LEN; idx++)
    {
        num = write(fd, &str[idx], 1);

        if (num <= 0)
            break;
    }

    // clear output buffer
    tcflush(fd, TCOFLUSH);
}

static void VCOM_close(int fd)
{
    tcflush(fd, TCIOFLUSH);

    close(fd);
}

static int VCOM_open(void)
{
    int fd;

    if ((fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY)) == -1)
        return -1;

    fcntl(fd, F_SETFL, O_NONBLOCK);

    struct termios tty;
    memset(&tty, 0, sizeof tty);
    struct termios tty_prev;
    memset(&tty_prev, 0, sizeof tty_prev);

    // error ?
    if (tcgetattr(fd, &tty) != 0)
        return -1;
    tty_prev = tty;

    cfsetospeed(&tty, (speed_t)B9600);
    cfsetispeed(&tty, (speed_t)B9600);

    tty.c_cflag &= ~PARENB; // 8N1
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    tty.c_cflag &= ~CRTSCTS; // no flow control

    tty.c_cflag |= CREAD | CLOCAL; // read on - ignore ctrl lines
    tty.c_cc[VMIN] = 1;            // read no block
    tty.c_cc[VTIME] = 5;           // read time-out 500 msec

    cfmakeraw(&tty);

    tcflush(fd, TCIOFLUSH);

    // error ?
    if (tcsetattr(fd, TCSANOW, &tty) != 0)
        return -1;

    return fd;
}

// (------,------)X
char SPS_str(int idx)
{
    return SPS_msg[idx];
}

void SPS_response(void)
{
    VCOM_read(SPS_fd, SPS_msg);
}

double SPS_request(const char *msg)
{
    VCOM_write(SPS_fd, msg);

    return VCOM_READ_DELAY;
}

void SPS_close(void)
{
    VCOM_close(SPS_fd);
}

int SPS_ready(void)
{
    return (SPS_fd != -1);
}

void SPS_open(void)
{
    SPS_fd = VCOM_open();
}

void IDLE_setCycle(void)
{
    double sps_read_delay = SPS_request("(IDLE--,------)X");

    usleep((1.0 - sps_read_delay) * 1000 * 1000);

    SPS_response();

    //     string data = (string)Graph.Time_Input->value();
    //     int data_x = stoi(data)*2.89;
    //     char *dataNew = data_x.c_str();
    //     string dataNewNew = (string)dataNew;

    // char *name = ("(SET---,+" + dataNewNew + ")X").c_str();

    //     sps_read_delay = SPS_request(name);
    sps_read_delay = SPS_request("(SET---,+00029)X");

    usleep((1.0 - sps_read_delay) * 1000 * 1000);

    SPS_response();
}

void SetIdleMode(void)
{
    double sps_read_delay = SPS_request("(IDLE--,------)X");

    usleep((1.0 - sps_read_delay) * 1000 * 1000);

    SPS_response();
}

void BUSY_read(void)
{
    double sps_read_delay = SPS_request("(BUSY--,------)X");

    usleep((1.0 - sps_read_delay) * 1000 * 1000);

    SPS_response();

    int n;
    for (n = 0; n < 50; n++)
    {
        double sps_read_delay = SPS_request("(READ--,------)X");

        usleep((1.0 - sps_read_delay) * 1000 * 1000);

        SPS_response();

        string data = (string)SPS_msg;
        string dataX = data.substr(2, 6);
        int data_x = stoi(dataX);
        printf("\n");
        cout << "integer is: " << data_x;
        printf("\n");
    }
}
//----------------------------------------------------

//---------------------------------------------------- FLTK Enums, Struct, Class, Static Methods

// struct GRAPH
// {
//     Fl_Double_Window *window;
//     Fl_Button *btn_start_stop;
//     Fl_Output *out_status;
//     Fl_Output *out_data_1;
//     Fl_Output *out_data_2;
//     Fl_Output *out_time_e;
//     Fl_Timer *timer_e;
//     Fl_Input *Time_Input;

//     int i;
//     int i_Idle;
//     int yk[GRAPH_MAX + 2];
//     int xk[GRAPH_MAX + 2];
//     double y;

//     int scale;
//     bool done;

//     bool running;

//     const char IDLE_Time = 0;
// };
// static GRAPH Graph;

class POINT
{
public:
    int previous_x = 0;

public:
    int previous_y = 0;

public:
    int current_x = 0;

public:
    int current_y = 0;

public:
    POINT(int x_val_previous, int y_val_previous, int x_val_current, int y_val_current)
    {
        previous_x = x_val_previous;
        previous_y = y_val_previous;
        current_x = y_val_current;
        current_y = y_val_current;
    }
};
POINT last_Point(0, 0, 0, 0);

class GRAPHBOX : public Fl_Box
{

    double sps_read_delay;

    void draw(void)
    {
        Fl_Box::draw();

        fl_font(FL_COURIER, 16);
        fl_color(fl_rgb_color(0, 255, 0));
        char str[256];
        static long start = time(NULL);
        long tick = time(NULL) - start;
        sprintf(str, "Time: %02ld:%02ld", tick / 60, tick % 60);
        //        fl_draw(str, 410, WINDOW_Y - (CONTROL_H / 2));
        fl_draw(str, x() + w() - 90, y() + h() - 10);

        fl_color(fl_rgb_color(255, 0, 0));
        for (int i = 0; i < Graph.i; i++)
        {

            if (i == 0)
            {
                fl_line(x() + i * 3, CHART_Y - (Graph.yk[i]), x() + i * 3 + 1, y() + CHART_Y - (Graph.yk[i]));
            }
            else
            {
                fl_line(x() + i * 3, y() + CHART_Y - (Graph.yk[i - 1]), x() + i * 3 + 1, y() + CHART_Y - (Graph.yk[i]));
            }
        }

        fl_color(fl_rgb_color(0, 255, 0));
        for (int i = 0; i < Graph.i; i++)
        {

            if (i == 0)
            {
                fl_line(x() + i * 3, y() + CHART_Y - (Graph.xk[i]), x() + i * 3 + 1, y() + CHART_Y - (Graph.xk[i]));
            }
            else
            {
                fl_line(x() + i * 3, y() + CHART_Y - (Graph.xk[i - 1]), x() + i * 3 + 1, y() + CHART_Y - (Graph.xk[i]));
            }
        }
    }

    static void Timer_CB(void *)
    {
        if (Graph.running == true)
        {

            try
            {
                printf("--------------------\n");
                printf(Graph.Time_Input->value());
                // New
                double sps_read_delay = SPS_request("(READ--,------)X");

                //                usleep((1.0 - sps_read_delay) * 1000 * 1000);

                SPS_response();

                string data = (string)SPS_msg;
                string dataX = data.substr(2, 5);
                string dataY = data.substr(9, 5);

                if (dataX == "-----" && dataY == "-----")
                {
                    Graph.i_Idle++;

                    if (Graph.i_Idle >= 3)
                    {
                        Graph.out_status->value("IDLE");
                        Graph.running = false;
                        Graph.btn_start_stop->label("Start");

                        IDLE_setCycle();
                    }
                }

                int data_x = stoi(dataX);
                int data_y = stoi(dataY);
                Graph.xk[Graph.i] = data_x * 3;
                Graph.yk[Graph.i] = data_y * 3;

                // const char *data_x_c = dataX.c_str();
                // const char *data_y_c = dataY.c_str();

                const char *data_x_c = ("Data1 : " + dataX).c_str();
                const char *data_y_c = ("Data2 : " + dataY).c_str();

                Graph.out_data_1->value(data_x_c);
                Graph.out_data_2->value(data_y_c);

                Graph.i_Idle++;
                Graph.i++;
                if (Graph.i > GRAPH_MAX)
                    Graph.i = 0;

                Graph.window->redraw();
            }
            catch (exception e)
            {
                Fl::repeat_timeout(0.01, Timer_CB);
            }
        }

        if (!Graph.done)
        {
            Fl::repeat_timeout(0.01, Timer_CB);
        }
    }

public:
    GRAPHBOX(int x, int y, int w, int h, const char *l = 0) : Fl_Box(x, y, w, h, l)
    {
        box(FL_FLAT_BOX);
        color(fl_rgb_color(0, 0, 0));

        Fl::add_timeout(0, Timer_CB);
    }
};

static void Start_Stop_CB(Fl_Widget *, void *)
{
    if (!Graph.running)
    {
        // Start Graph
        Graph.out_status->value("BUSY");
        Graph.running = true;
        Graph.btn_start_stop->label("Stop");

        double sps_read_delay = SPS_request("(BUSY--,------)X");
        usleep((1.0 - sps_read_delay) * 1000 * 1000);
        SPS_response();
        printf("--------------------\n");

        //        cout << Graph.IDLE_Time;
    }
    else
    {
        // Stop Graph
        Graph.out_status->value("IDLE");
        Graph.running = false;
        Graph.btn_start_stop->label("Start");

        SetIdleMode();
    }
}

//----------------------------------------------------

//---------------------------------------------------- Main Methods
int main(void)
{

    //---------------------------------------------------- SPS_input Port opening
    printf("COM Test for SPS ...\n");

    SPS_open();

    if (!SPS_ready())
    {
        printf("Error: SPS not ready\n");
        return EXIT_FAILURE;
    }

    usleep(1.0 * 1000 * 1000);

    IDLE_setCycle();

    //----------------------------------------------------

    //---------------------------------------------------- FLTK Windowing
    //Window
    Fl_Double_Window window(WINDOW_X, WINDOW_Y, "Embedded GUI");

    //Chart
    GRAPHBOX chart(10, 10, WINDOW_X - 20, WINDOW_Y - CONTROL_H - 20);

    //Start Button
    Fl_Button btn_start_stop(0, WINDOW_Y - CONTROL_H, 120, CONTROL_H, "Start");
    btn_start_stop.box(FL_FLAT_BOX);
    btn_start_stop.color(fl_rgb_color(127 - 64, 127 - 32, 127));
    btn_start_stop.align(FL_ALIGN_INSIDE | FL_ALIGN_CENTER);
    btn_start_stop.labelfont(FL_HELVETICA);
    btn_start_stop.labelsize(32);
    btn_start_stop.labelcolor(fl_rgb_color(255, 255, 255));
    btn_start_stop.callback(Start_Stop_CB);

    //Status of the machine
    Fl_Output out_status(120, WINDOW_Y - CONTROL_H, 180, CONTROL_H);
    out_status.value("Idle");
    out_status.color(fl_rgb_color(0, 255, 0));
    out_status.textsize(32);
    out_status.textcolor(fl_rgb_color(255, 0, 0));

    //Data 1 out_temp
    Fl_Output out_data_1(300, WINDOW_Y - CONTROL_H, 100, CONTROL_H / 2);
    out_data_1.value("Data 1: Null");
    out_data_1.color(fl_rgb_color(255, 255, 255));
    out_data_1.textsize(32);
    out_data_1.textcolor(fl_rgb_color(0, 255, 0));

    //Data 2 out_temp
    Fl_Output out_data_2(300, WINDOW_Y - (CONTROL_H / 2), 100, CONTROL_H / 2);
    out_data_2.value("Data 2: Null");
    out_data_2.color(fl_rgb_color(255, 255, 255));
    out_data_2.textsize(32);
    out_data_2.textcolor(fl_rgb_color(255, 0, 0));

    // //Elasped Time
    // Fl_Output out_time_e(400, WINDOW_Y - CONTROL_H, 200, CONTROL_H);
    // out_time_e.value("04.07.2021");
    // out_time_e.color(fl_rgb_color(0, 0, 0));
    // out_time_e.textsize(32);
    // out_time_e.textcolor(fl_rgb_color(255, 255, 255));
    // out_time_e.value();

    Fl_Output out_time_e(400, WINDOW_Y - CONTROL_H, 100, CONTROL_H / 2);
    out_time_e.value("IDLE Time Set");
    out_time_e.color(fl_rgb_color(0, 0, 0));
    out_time_e.textsize(32);
    out_time_e.textcolor(fl_rgb_color(255, 255, 255));
    out_time_e.value();

    Fl_Input in_time(500, WINDOW_Y - CONTROL_H, 200, CONTROL_H / 2, &Graph.IDLE_Time);

    //Data 2 out_temp
    Fl_Output out_time_Busy(400, WINDOW_Y - (CONTROL_H / 2), 200, CONTROL_H / 2);
    out_time_Busy.value("Data 2: Null");
    out_time_Busy.color(fl_rgb_color(255, 255, 255));
    out_time_Busy.textsize(32);
    out_time_Busy.textcolor(fl_rgb_color(255, 0, 0));

    //Initialization
    Graph.window = &window;
    Graph.btn_start_stop = &btn_start_stop;
    Graph.out_status = &out_status;
    Graph.out_data_1 = &out_data_1;
    Graph.out_data_2 = &out_data_2;
    Graph.out_time_e = &out_time_e;
    // Graph.timer_e = &time_e;
    Graph.Time_Input = &in_time;
    Graph.running = false;
    Graph.i = 0;
    window.show();
    //----------------------------------------------------

    Fl::run();
    return EXIT_SUCCESS;
}