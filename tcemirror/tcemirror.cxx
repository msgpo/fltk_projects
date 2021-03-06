// generated by Fast Light User Interface Designer (fluid) version 1.0110

#include "tcemirror.h"
// (c) Robert Shingledecker 2008
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <FL/fl_message.H>
using namespace std;
static string url, command; 

void btnCallback(Fl_Widget*, void* userdata) {
  const string userdatastr = userdata ? (char *) userdata : "";

if (userdatastr == "cancel" )
   exit(1);
if (userdatastr == "ok")
{
    url = urlInput->value();
    ofstream fout("/opt/tcemirror", ios::out|ios::out);
    if (! fout.is_open())
    {
       cerr << "Can't open /opt/tcemirror for output!" << endl;
       exit(EXIT_FAILURE);
    }
    fout << url << endl;
    fout.close();
    exit(0);
}
}

void mirrorBrwCallback(Fl_Widget*, void* userdata) {
  const string userdatastr = userdata ? (char *) userdata : "";

if (userdatastr == "select")
{
   string selected = mirrorBrw->text(mirrorBrw->value());
   urlInput->value(selected.c_str());    
}
}

Fl_Input *urlInput=(Fl_Input *)0;

Fl_Browser *mirrorBrw=(Fl_Browser *)0;

int main(int argc, char **argv) {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(430, 135, "Select Download Mirror");
    w = o;
    { urlInput = new Fl_Input(50, 6, 375, 24, "Mirror:");
      urlInput->value("www.tinycorelinux.com/files");
    } // Fl_Input* urlInput
    { mirrorBrw = new Fl_Browser(50, 35, 375, 70);
      mirrorBrw->type(1);
      mirrorBrw->callback((Fl_Callback*)mirrorBrwCallback, (void*)("select"));
    } // Fl_Browser* mirrorBrw
    { Fl_Button* o = new Fl_Button(290, 110, 64, 20, "&OK");
      o->callback((Fl_Callback*)btnCallback, (void*)("ok"));
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(360, 110, 64, 20, "&Cancel");
      o->callback((Fl_Callback*)btnCallback, (void*)("cancel"));
    } // Fl_Button* o
    o->end();
  } // Fl_Double_Window* o
  ifstream fin("/opt/tcemirror");
getline(fin,url);
fin.close();
urlInput->value(url.c_str());
command = "busybox wget -cq ftp://distro.ibiblio.org/pub/linux/distributions/tinycorelinux/2.x/tcz/mirrors.lst";
int results = system(command.c_str());
if ( results == 0)
{
   mirrorBrw->load("mirrors.lst");
   system("rm mirrors.lst");
} else
   fl_message("Connection error, check network or ibiblio");
  w->show(argc, argv);
  return Fl::run();
}
