#include "tbx/application.h"
#include "tbx/autocreatelistener.h"
#include "tbx/redrawlistener.h"
#include "tbx/offsetgraphics.h"
#include "tbx/slider.h"

#include <stdio.h>
#include <stdlib.h>

struct GRAPH
{
  int fill_A;
  int fill_B;
};
struct GRAPH Graph;

class GRAPHWINDOW:
  public tbx::AutoCreateListener,
  public tbx::RedrawListener,
  public tbx::SliderValueChangedListener
{
  tbx::Window _window;
  tbx::Slider _slider_A;
  tbx::Slider _slider_B;

  void redraw(const tbx::RedrawEvent &e)
  {
    tbx::OffsetGraphics g(e.visible_area());
    
    // tank A
    g.foreground(tbx::Colour(0,0,255));
    g.fill_rectangle(40+0, -40-200, 40+200, -40-200+2*Graph.fill_A);
    g.foreground(tbx::Colour(255,0,0));
    g.line(40+0, -40-200, 40+0, -40-0);
    g.line(40+2, -40-200, 40+2, -40-0);
    g.line(40+200, -40-200, 40+200, -40-0);
    g.line(40+200-2, -40-200, 40+200-2, -40-0);
    g.line(40+0, -40-200, 40+200, -40-200);
    g.line(40+0, -40-200+2, 40+200, -40-200+2);
    
    g.text_colours(tbx::Colour(0,0,0), tbx::Colour(255,255,255));
    g.text(40+0, -40-200-40, "Tank A");

    // tank B
    g.foreground(tbx::Colour(0,0,255));
    g.fill_rectangle(400+40+0, -40-200, 400+40+200, -40-200+2*Graph.fill_B);
    g.foreground(tbx::Colour(255,0,0));
    g.line(400+40+0, -40-200, 400+40+0, -40-0);
    g.line(400+40+2, -40-200, 400+40+2, -40-0);
    g.line(400+40+200, -40-200, 400+40+200, -40-0);
    g.line(400+40+200-2, -40-200, 400+40+200-2, -40-0);
    g.line(400+40+0, -40-200, 400+40+200, -40-200);
    g.line(400+40+0, -40-200+2, 400+40+200, -40-200+2);
    
    g.text_colours(tbx::Colour(0,0,0), tbx::Colour(255,255,255));
    g.text(400+40+0, -40-200-40, "Tank B");
  }
  
  void slider_value_changed(const tbx::SliderValueChangedEvent &e)
  {
    Graph.fill_A = _slider_A.value();
    Graph.fill_B = _slider_B.value();
    
    tbx::BBox bbox;
    
    // tank A redraw
    bbox.min.x = 40+0;
    bbox.min.y = -40-200;
    bbox.max.x = 40+200;
    bbox.max.y = -40-0;
    _window.force_redraw(bbox);

    // tank B redraw
    bbox.min.x = 400+40+0;
    bbox.min.y = -40-200;
    bbox.max.x = 400+40+200;
    bbox.max.y = -40-0;
    _window.force_redraw(bbox);
  }

  void auto_created(std::string template_name, tbx::Object object)
  {
    _window = tbx::Window(object);
    _window.add_redraw_listener(this);
    
    _slider_A = _window.gadget(0);
    _slider_A.add_value_changed_listener(this);
    _slider_B = _window.gadget(1);
    _slider_B.add_value_changed_listener(this);
    
    Graph.fill_A = _slider_A.value();
    Graph.fill_B = _slider_B.value();
  }
};

int main(void)
{
  tbx::Application app("<Tank$Dir>");
  GRAPHWINDOW graphwindow;
  
  app.set_autocreate_listener("Window", &graphwindow);
  app.run();
  return EXIT_SUCCESS;
}
