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
#include <sstream>

//---------------------------------------------------- SPS_input includes
#include <fcntl.h>
#include <termios.h>

#include <sys/poll.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;

//---------------------------------------------------- FLTK defines and global variables
#define WINDOW_X 600
#define WINDOW_Y 500
#define CONTROL_H 60
#define MARGIN 10
#define CHART_0_X 10
#define CHART_0_Y 430

#define X_MAG 3
#define GRAPH_MAX 192

double t = 0;

//---------------------------------------------------- SPS_input defines and global variables
#define INMSG_MAX_LEN 15
#define OUTMSG_MAX_LEN 15

#define ARDUINO_USB_PORT "/dev/ttyUSB0"

static char SPS_msg[] = "(------,------)X";

static int SPS_fd = -1;

static const double VCOM_READ_DELAY = 0.050;

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
        // read time-out 1500 msec
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

    if ((fd = open(ARDUINO_USB_PORT, O_RDWR | O_NOCTTY)) == -1)
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

void IDLE_setCycle(string cycle)
{
    double sps_read_delay = SPS_request("(IDLE--,------)X");

    usleep((1.0 - sps_read_delay) * 1000 * 1000);

    SPS_response();

    //Setting the cycles to the SPS
    int time = 0;
    try
    {
        time = stoi(cycle);
        time = time * 2.9;
        cycle = to_string(time);
    }
    catch (const std::exception &e)
    {
        printf("!!!Invalid input in measurment time");
        std::cerr << e.what() << '\n';
    }
    if (time > 9999)
        cycle = "(SET---,+9999)X";
    else if (time > 999)
        cycle = "(SET---,+0" + cycle + ")X";
    else if (time > 99)
        cycle = "(SET---,+00" + cycle + ")X";
    else if (time > 9)
        cycle = "(SET---,+000" + cycle + ")X";

    const char *setCycle = cycle.c_str();

    //Set Cycle
    sps_read_delay = SPS_request(setCycle);

    usleep((1.0 - sps_read_delay) * 1000 * 1000);

    SPS_response();
}

void IDLE_Cycle(void)
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

//---------------------------------------------------- FLTK Enums, Struct, Class, Static Methods

struct GRAPH
{
    Fl_Double_Window *window;
    Fl_Button *btn_start_stop;
    Fl_Output *out_status;
    Fl_Output *out_Data_1;
    Fl_Output *out_Data_2;
    Fl_Input *out_time_e;

    long startTime = time(NULL);
    bool isRunning;

    int data_1[GRAPH_MAX + 2];
    int data_2[GRAPH_MAX + 2];

    int i;
    int i_Idle;
    bool done;
};
static GRAPH Graph;

class GRAPHBOX : public Fl_Box
{
    void draw(void)
    {
        Fl_Box::draw();

        fl_font(FL_COURIER, 16);
        fl_color(fl_rgb_color(0, 255, 0));
        char str[256];
        long tick = time(NULL) - Graph.startTime;
        //Elapsed Time
        sprintf(str, "Elapsed Time : %02ld:%02ld", tick / 60, tick % 60);
        fl_draw(str, x() + w() - 160, y() + 20);

        for (size_t i = 0; i < Graph.i; i++)
        {
            fl_color(fl_rgb_color(255, 0, 0));
            fl_line(CHART_0_X + i * X_MAG, CHART_0_Y - Graph.data_1[i - 1], CHART_0_X + (i + 1) * X_MAG, CHART_0_Y - Graph.data_1[i]);

            fl_color(fl_rgb_color(0, 255, 0));
            fl_line(CHART_0_X + i * X_MAG, CHART_0_Y - Graph.data_2[i - 1], CHART_0_X + (i + 1) * X_MAG, CHART_0_Y - Graph.data_2[i]);
        }
    }

    static void Timer_CB(void *)
    {
        if (Graph.isRunning)
        {
            //Read the data from SPS/Arduino
            double sps_read_delay = SPS_request("(READ--,------)X");
            SPS_response();

            //Convert string to char array
            string data = (string)SPS_msg;
            string data1 = data.substr(2, 5);
            string data2 = data.substr(9, 5);
            if (data1 == "-----" && data2 == "-----")
            {
                Graph.i_Idle++;
                if (Graph.i_Idle >= 3)
                {
                    Graph.out_status->value("IDLE");
                    Graph.out_status->textcolor(fl_rgb_color(127 - 64, 127 - 32, 127));
                    Graph.isRunning = false;
                    Graph.btn_start_stop->label("START");
                    Graph.btn_start_stop->color(fl_rgb_color(127 - 64, 127 - 32, 127));

                    IDLE_setCycle((string)Graph.out_time_e->value());
                }
            }
            try
            {
                Graph.data_1[Graph.i] = stoi(data1) * 3;
                Graph.data_2[Graph.i] = stoi(data2) * 3;
                Graph.i_Idle = 0;
            }
            catch (const std::exception &e)
            {
                printf("-------------------------------- \n");
                printf("Invalid values from the SPS: NaN \n");
                printf("-------------------------------- \n");
                Fl::repeat_timeout(0.01, Timer_CB);
            }

            //Show the current value of Data1 and Data2
            const char *data_1_c = ("Data 1: " + data1).c_str();
            const char *data_2_c = ("Data 2: " + data2).c_str();

            Graph.out_Data_1->value(data_1_c);
            Graph.out_Data_2->value(data_2_c);

            Graph.i++;
            if (Graph.i > GRAPH_MAX)
                Graph.i = 0;

            Graph.window->redraw();
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

void set_BUSY_mode(void)
{
    double sps_read_delay = SPS_request("(BUSY--,------)X");
    usleep((1.0 - sps_read_delay) * 1000 * 1000);
    SPS_response();
}

static void Start_Stop_Button_Callback(Fl_Widget *, void *)
{
    if (!Graph.isRunning)
    {
        IDLE_setCycle((string)Graph.out_time_e->value());

        set_BUSY_mode();

        Graph.out_status->value("BUSY");
        Graph.out_status->textcolor(fl_rgb_color(255, 0, 0));
        Graph.isRunning = true;
        Graph.btn_start_stop->label("STOP");
        Graph.btn_start_stop->color(fl_rgb_color(255, 0, 0));
        Graph.startTime = time(NULL);
    }
    else
    {
        IDLE_Cycle();
        Graph.out_status->value("IDLE");
        Graph.out_status->textcolor(fl_rgb_color(127 - 64, 127 - 32, 127));
        Graph.isRunning = false;
        Graph.btn_start_stop->label("START");
        Graph.btn_start_stop->color(fl_rgb_color(127 - 64, 127 - 32, 127));
    }
}

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

    //---------------------------------------------------- FLTK Windowing
    //Window
    Fl_Double_Window window(WINDOW_X, WINDOW_Y, "Embedded GUI for SPS");

    //Chart
    GRAPHBOX chart(10, 10, WINDOW_X - 20, WINDOW_Y - CONTROL_H - 20);

    //Start Button
    Fl_Button btn_start_stop(0, WINDOW_Y - CONTROL_H, 120, CONTROL_H, "Start");
    btn_start_stop.box(FL_FLAT_BOX);
    btn_start_stop.color(fl_rgb_color(127 - 64, 127 - 32, 127));
    btn_start_stop.labelsize(32);
    btn_start_stop.labelcolor(fl_rgb_color(255, 255, 255));
    btn_start_stop.callback(Start_Stop_Button_Callback);

    //Status of the machine
    Fl_Output out_status(120, WINDOW_Y - CONTROL_H, 180, CONTROL_H);
    out_status.value("IDLE");
    out_status.color(fl_rgb_color(0, 255, 0));
    out_status.textsize(32);
    out_status.textcolor(fl_rgb_color(127 - 64, 127 - 32, 127));

    //Data 1
    Fl_Output out_Data_1(300, WINDOW_Y - CONTROL_H, 120, CONTROL_H / 2);
    out_Data_1.color(fl_rgb_color(255, 255, 255));
    out_Data_1.textsize(16);
    out_Data_1.textcolor(fl_rgb_color(255, 0, 0));
    out_Data_1.value("Data 1: NULL");

    //Data 2
    Fl_Output out_Data_2(300, WINDOW_Y - CONTROL_H / 2, 120, CONTROL_H / 2);
    out_Data_2.color(fl_rgb_color(255, 255, 255));
    out_Data_2.textsize(16);
    out_Data_2.textcolor(fl_rgb_color(20, 255, 0));
    out_Data_2.value("Data 2: NULL");

    //Elasped Time
    Fl_Input out_time_e(420, WINDOW_Y - CONTROL_H + 10, 180, 50);
    out_time_e.label("Set time in sec :");
    out_time_e.value("30");
    out_time_e.align(FL_ALIGN_TOP | FL_ALIGN_CENTER);
    out_time_e.color(fl_rgb_color(0, 0, 0));
    out_time_e.textsize(32);
    out_time_e.textcolor(fl_rgb_color(255, 255, 255));
    out_time_e.value();

    //Initialization
    Graph.window = &window;
    Graph.btn_start_stop = &btn_start_stop;
    Graph.out_status = &out_status;
    Graph.out_Data_1 = &out_Data_1;
    Graph.out_Data_2 = &out_Data_2;
    Graph.out_time_e = &out_time_e;
    Graph.isRunning = false;
    Graph.i = 0;

    window.show();
    Fl::run();
    return EXIT_SUCCESS;
}