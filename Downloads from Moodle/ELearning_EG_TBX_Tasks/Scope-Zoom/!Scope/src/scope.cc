#include "tbx/application.h"
#include "tbx/autocreatelistener.h"
#include "tbx/abouttobeshownlistener.h"
#include "tbx/redrawlistener.h"
#include "tbx/actionbutton.h"
#include "tbx/command.h"
#include "tbx/displayfield.h"
#include "tbx/buttonselectedlistener.h"
#include "tbx/offsetgraphics.h"
#include "tbx/timer.h"

#define DEBUG
#undef DEBUG

#ifdef DEBUG
#include "tbx/taskwindow.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PATH  600

struct GRAPH
{
  int xk;
  int yk;
  double y;

  tbx::Point path[PATH+2];

  int scale;

  bool started;
  bool toggle;
  bool redraw;
};
GRAPH Graph;

#define BUT_TOGGLE       0
#define DISP_LEFT        1
#define DISP_RIGHT       2
#define LABEL_LEFT       3
#define LABEL_RIGHT      4

#define LABEL_SLIDER     6
#define BUT_GROUND       7
#define DISP_SLIDER      8

#define BUT_ZOOM_PLUS    9
#define BUT_ZOOM_MINUS  10

class GRAPHWINDOW:
  public tbx::AutoCreateListener,
  public tbx::AboutToBeShownListener,
  public tbx::RedrawListener,
  public tbx::Command,
  public tbx::ButtonSelectedListener,
  public tbx::Timer
{
  tbx::Window _window;

  tbx::ActionButton _but_toggle;
  tbx::DisplayField _disp_left;
  tbx::DisplayField _disp_right;
  tbx::ActionButton _but_ground;
  tbx::DisplayField _disp_slider;

  tbx::ActionButton _but_zoom_plus;
  tbx::ActionButton _but_zoom_minus;

  public:
  virtual void auto_created(std::string template_name, tbx::Object object);
  virtual void about_to_be_shown(tbx::AboutToBeShownEvent &e);
  virtual void redraw(const tbx::RedrawEvent &e);
  virtual void execute();
  virtual void button_selected(tbx::ButtonSelectedEvent &event);
  virtual void timer(unsigned int elapsed);
};

void GRAPHWINDOW::timer(unsigned int elapsed)
{
  if (Graph.started == true)
  {
    // calculate graph every 10 msec
    if (Graph.toggle)
      Graph.y = (1.0*sin((Graph.xk+1)/20.0)+2.0*((rand()/((double) RAND_MAX))-0.5))/2.0;
    else
      Graph.y = (1.0*sin((Graph.xk+1)/20.0)+0.2*((rand()/((double) RAND_MAX))-0.5))/1.1;

    Graph.yk = Graph.scale*Graph.y;

    Graph.path[Graph.xk+1].x = 10+Graph.xk+1; Graph.path[Graph.xk+1].y = Graph.yk-300;

    // show coordinates every 100 msec
    if (Graph.xk%10 == 0)
    {
      char str[256];
      sprintf(str, "%d", Graph.xk);
      _disp_left.text(str);
      sprintf(str, "%d", (int) (100.0*Graph.y));
      _disp_right.text(str);
      sprintf(str, "%d", Graph.scale);
      _disp_slider.text(str);
    }

    Graph.xk++;

    // redraw graph every 100 msec
    if (Graph.xk%10 == 0)
    {
      tbx::BBox bbox;

      bbox.min.x = 10+Graph.xk-10;
      bbox.min.y = -450;
      bbox.max.x = 10+Graph.xk;
      bbox.max.y = -150;
      _window.force_redraw(bbox);
    }

#ifdef DEBUG
    // console every 500 msec
    if (Graph.xk%50 == 0)
    {
      tbx::TaskWindow terminal;
      terminal.options(0x4);
      char str[256];
      sprintf(str, "echo Graph.scale = %i", Graph.scale);
      terminal.command(str);
      terminal.run();
    }
#endif

    // clipping or full redraw required
    if ((Graph.xk > PATH) || (Graph.redraw))
    {
      Graph.redraw = false;

      Graph.xk = 0;
      Graph.yk = 0;
      Graph.y  = 0;
      for (int idx=0; idx<=PATH; idx++)
      { Graph.path[idx  ].x = 20+idx  ; Graph.path[idx  ].y = -300; }

      tbx::BBox bbox;

      bbox.min.x = 10+0;
      bbox.min.y = -450;
      bbox.max.x = 10+PATH;
      bbox.max.y = -150;
      _window.force_redraw(bbox);
    }
  }
}

void GRAPHWINDOW::redraw(const tbx::RedrawEvent &e)
{
  tbx::OffsetGraphics g(e.visible_area());

  // show graph
  g.foreground(tbx::Colour(0,0,0));
  g.fill_rectangle(10+0, -150, 10+PATH, -450);
  if (Graph.toggle)
    g.foreground(tbx::Colour(0,255,0));
  else
    g.foreground(tbx::Colour(255,0,0));
  g.path(Graph.path, PATH);

  // show axes
  g.foreground(tbx::Colour(255,255,255));
  g.line(10+0, -150, 10+PATH, -150);
  g.line(10+0, -300, 10+PATH, -300);
  g.line(10+0, -450, 10+PATH, -450);
  g.line(10+0, -150, 10+0, -450);
  g.line(10+PATH, -150, 10+PATH, -450);

  g.text_colours(tbx::Colour(0,0,0), tbx::Colour(255,255,255));
  g.text(10+10+PATH, -300, "t");
  g.text(10+10, -140, "x(t)");
}

// buttons ground and zoom plus and zoom minus
void GRAPHWINDOW::button_selected(tbx::ButtonSelectedEvent &event)
{
  tbx::IdBlock id_block = event.id_block();
  tbx::Component component = id_block.self_component();
  tbx::ComponentId component_id = component.id();
  
  if (component_id == BUT_GROUND)
  {
    Graph.scale = 5;
  }

  if (component_id == BUT_ZOOM_PLUS)
  {
    Graph.scale += 5;

    if (Graph.scale > 100)
      Graph.scale = 100;
  }

  if (component_id == BUT_ZOOM_MINUS)
  {
    Graph.scale -= 5;

    if (Graph.scale < 5)
      Graph.scale = 5;
  }
}

// button toggle
void GRAPHWINDOW::execute()
{
  Graph.toggle = 1-Graph.toggle;

  Graph.redraw = true;
}

void GRAPHWINDOW::about_to_be_shown(tbx::AboutToBeShownEvent &e)
{
  // window opened then start drawing
  Graph.started = true;

#ifdef DEBUG
  tbx::TaskWindow terminal;
  terminal.options(0x4);
  terminal.command("echo Scope APP started ...");
  terminal.run();
#endif
}

void GRAPHWINDOW::auto_created(std::string template_name, tbx::Object object)
{
  _window = tbx::Window(object);
  _window.add_about_to_be_shown_listener(this);
  _window.add_redraw_listener(this);

  _but_toggle = _window.gadget(BUT_TOGGLE);
  _but_toggle.add_selected_command(this);
  _disp_left = _window.gadget(DISP_LEFT);
  _disp_right = _window.gadget(DISP_RIGHT);
  _but_ground = _window.gadget(BUT_GROUND);
  _but_ground.add_selected_listener(this);
  _disp_slider = _window.gadget(DISP_SLIDER);
  
  _but_zoom_plus = _window.gadget(BUT_ZOOM_PLUS);
  _but_zoom_plus.add_selected_listener(this);
  _but_zoom_minus = _window.gadget(BUT_ZOOM_MINUS);
  _but_zoom_minus.add_selected_listener(this);
}

int main()
{
  tbx::Application app("<Scope$Dir>");
  GRAPHWINDOW graphwindow;
  app.set_autocreate_listener("Window", &graphwindow);
  
  Graph.xk = 0;
  Graph.yk = 0;
  Graph.y  = 0.0;
  for (int idx=0; idx<=PATH; idx++)
  { Graph.path[idx  ].x = 10+idx  ; Graph.path[idx  ].y = -300; }
  Graph.scale = 100;
  Graph.started = false;
  Graph.toggle = true;
  Graph.redraw = false;

  app.add_timer(1, &graphwindow);
  app.run();
  return EXIT_SUCCESS;
}
