// generated by Fast Light User Interface Designer (fluid) version 1.0110

#include <libintl.h>
#include "appbrowser.h"
// (c) Robert Shingledecker 2008-2012
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <FL/fl_message.H>
#include <FL/Fl_File_Chooser.H>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <locale.h>
#include <unistd.h>
#include <string.h>
using namespace std;
static FILE *G_in = NULL; // how we read the child
static int   G_out = 0; // how we write to the child
static string download_dir, last_dir; 
static string select_extn; 
static string repository; 
static ifstream ifaberr; 
static string aberr; 
static string mode, command, msg, mirror, err_extn; 
static Fl_Text_Buffer *txtBuffer = new Fl_Text_Buffer; 
static void errhandler(const string &str); 

void displayTabData() {
  FILE *pipe = popen(command.c_str(),"r");
char *mbuf = (char *)calloc(PATH_MAX,sizeof(char));
if (pipe)
{
   txtBuffer->loadfile("");
   while(fgets(mbuf,PATH_MAX,pipe))
   {
      string line(mbuf);
      txtBuffer->append(line.c_str());
      Fl::flush();
   }
   pclose(pipe);
   free(mbuf);
}
}

static void cursor_normal() {
  window->cursor(FL_CURSOR_DEFAULT);
Fl::flush();
}

static void cursor_wait() {
  window->cursor(FL_CURSOR_WAIT);
Fl::flush();
}

static void HandleInput_CB(int, void *) {
  static int x = 0;
static char s[1024];
char c = fgetc(G_in);            // read one char at a time
if ( c == '\n' || x == (sizeof(s)-1) )
{
  s[x] = 0;
  status_out->value(s);
  string result = s;
  size_t loc = result.find((select_extn + ": OK"));
  if (loc != string::npos)
  {
    status_out->color(175);
  } else { // Error
    loc = result.find("Error"); // Only the md5sum error
    if (loc != string::npos) errhandler(result);
  }
  x = 0;
} else
  { s[x++] = c; }
}

static void fetch_extension() {
  status_out->activate();
status_out->color(FL_WHITE);
status_out->value(command.c_str());
string command2 = command + "\n";
write(G_out, command2.c_str(), command2.length());
Fl::add_fd(fileno(G_in), HandleInput_CB, (void*)&status_out);
}

Fl_Double_Window *errwindow=(Fl_Double_Window *)0;

Fl_Box *errlabel=(Fl_Box *)0;

static void cb_Remove(Fl_Button*, void*) {
  string cmd = "rm -f " + err_extn + "*";

system(cmd.c_str());

fetch_extension();
errwindow->hide();
}

static void cb_Try(Fl_Button*, void*) {
  string cmd = "zsync " + mirror + "3.x/tcz/" + err_extn + ".zsync";

int ret = system(cmd.c_str());
ret = WEXITSTATUS(ret);

if (ret == 0) fetch_extension();
else {

	// If zsync also fails, it's a bad hair
	// day for the network. Nothing we can do.

	status_out->value("Zsync failed.");

	cmd = "rm -f " + err_extn + "*";

	system(cmd.c_str());
}

errwindow->hide();
}

static void cb_Cancel(Fl_Button*, void*) {
  errwindow->hide();
}

static void errhandler(const string &str) {
  { errwindow = new Fl_Double_Window(520, 125, gettext("MD5SUM error"));
    { errlabel = new Fl_Box(162, 25, 195, 30, gettext("Md5sum error on"));
    } // Fl_Box* errlabel
    { Fl_Button* o = new Fl_Button(15, 70, 160, 35, gettext("Remove and try again"));
      o->callback((Fl_Callback*)cb_Remove);
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(200, 70, 160, 35, gettext("Try to finish download"));
      o->callback((Fl_Callback*)cb_Try);
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(385, 70, 115, 35, gettext("Cancel"));
      o->callback((Fl_Callback*)cb_Cancel);
    } // Fl_Button* o
    errwindow->end();
  } // Fl_Double_Window* errwindow
  errlabel->label(str.c_str());

err_extn = str;
err_extn.replace(0,9,""); // Yes, hardcoded magic value. Bad.

errwindow->show();
}

static void btnCallback(Fl_Widget *, void* userdata) {
  if (userdata == "tcz")
{
   mode = "tcz";
   tabs->deactivate();
   txtBuffer->loadfile("");
   repository = (const char*) userdata;
   cursor_wait();
   unlink("info.lst");
   command = "tce-fetch.sh info.lst.gz";
   int results = system(command.c_str());
   cursor_normal();
   if (results == 0 )
   {
      system("gunzip -c info.lst.gz > info.lst");
      brwSelect->load("info.lst");
      brwSelect->remove(brwSelect->size());
      btn_go->deactivate();
      box_select->label("Select Extension");
      box_select->activate();
      search_choices->activate();
      search_field->activate();                                              
    } else
      fl_message("Error, check network, mirror or writable extension directory.");
} else if (userdata == "go")
{
   status_out->value("");
   status_out->label("Status");
   int action = install_choices->value();
   string action_type;
   switch(action)
   {
      case 0 : action_type="wi";
               break;
      case 1 : action_type="wo";
               break;
      case 2 : action_type="wil";
               break;
      case 3 : action_type="w";
               break;
      default: mode="w";
   }
   command = "tce-load -" + action_type + " " + select_extn;
   cout << command << endl;
   fetch_extension();
} else if (userdata == "setdrive")
{
   cursor_wait();
   command = "tce-setdrive -l";
   int results = system(command.c_str());
   cursor_normal();
   cout << results << endl;
   if (results == 0 )
   {
      mode = "setdrive";
      brwSelect->load("/tmp/tcesetdev");
      brwSelect->remove(brwSelect->size());
      box_select->label("Select for TCE dir.");
      box_select->activate();
      unlink("/tmp/tcesetdev");
   } else
     fl_message("No available drives found!");
     
} else if (userdata == "search")
{
  if (search_choices->text() == "Search")
     command = "search.sh";
  else if (search_choices->text() == "Tags")
     command = "search.sh -t";
  else
     command = "provides.sh";
  tabs->deactivate();
  txtBuffer->loadfile("");
  brwSelect->load(""); 
  cursor_wait();
  command = command + " " + (string)search_field->value();
  FILE *pipe = popen(command.c_str(),"r");
  char *mbuf = (char *)calloc(PATH_MAX,sizeof(char));
  if (pipe)
  {
     while(fgets(mbuf,PATH_MAX,pipe))
     {
        string line(mbuf);
        line = line.substr(0,line.length()-1);
        brwSelect->add(line.c_str());
        brwSelect->bottomline(brwSelect->size());
        Fl::flush();
     }
     pclose(pipe);
     free(mbuf);
  }
  search_field->value("");
  cursor_normal();
  btn_go->deactivate();
  search_choices->activate();
  search_field->activate();                                              
      
} else if (userdata == "quit")
  {
    if ( last_dir.compare(0,8,"/tmp/tce") != 0 )
    {
      ofstream last_dir_fout("/opt/.appbrowser",ios::out|ios::out);
      if ( last_dir_fout.is_open())
      {
        last_dir_fout << last_dir << endl;
        last_dir_fout.close();
      }
    }  
    command = "quit\n";
    write(G_out, command.c_str(), command.length());
    unlink("info.lst");
    unlink("info.lst.gz");
    unlink("/tmp/tags.lst");
    unlink("/tmp/ab2tce.fifo");
    exit(0);

  }
}

static void brwSelect_callback(Fl_Widget *, void *) {
  if (brwSelect->value())
{
   select_extn = brwSelect->text(brwSelect->value());
   if ( mode == "tcz" )
   {
      string select_extn_file = select_extn + (string)".info";
      command = "tce-fetch.sh " + select_extn_file;
      int results = system(command.c_str());
      if (results == 0)
      {
         txtBuffer->loadfile(select_extn_file.c_str());

         unlink(select_extn_file.c_str());
         btn_go->activate();
         
         tabs->activate();
         infoTab->activate();
         filesTab->activate();
         dependsTab->activate();
         sizeTab->activate();
         infoTab->show();
      } 
   }
   if ( mode == "mirror" )
   {
     mirror = select_extn;
     window->label(mirror.c_str());
     ofstream fout("/opt/tcemirror", ios::out|ios::out);
     if (! fout.is_open())
     {
       cerr << "Can't open /opt/tcemirror for output!" << endl;
       exit(EXIT_FAILURE);
     }
     fout << mirror << endl;
     fout.close();      
   }
   if ( mode == "setdrive" )
   {
      command = "tce-setdrive -s " + select_extn;
      int results = system(command.c_str());
      if (results == 0)
      {
         download_dir = select_extn + "/tce";
         brwSelect->clear();
         box_select->label("");
         download_dir += "/optional";
         status_out->color(FL_WHITE);
         status_out->value((download_dir).c_str());
         btn_tce->deactivate();         
      }
   }
}
}

static void local_btnCallback(Fl_Widget*, void* userdata) {
  string title = "Install Local Extension";
string selected;
string flags = " -i ";
string target_dir = last_dir;
if (userdata == "File/Optional")
{
   target_dir = last_dir + "/optional";
   title = "Install Optional Extension";
}   
Fl_File_Chooser chooser(target_dir.c_str(),"*.tcz",Fl_File_Chooser::SINGLE,title.c_str());
chooser.show();                                             
// Block until user picks something.
//     (The other way to do this is to use a callback())                                                      
while(chooser.shown())
    { Fl::wait(); }
// User hit cancel?
if ( chooser.value() == NULL )
   return; 
   
selected = (string)chooser.value();

last_dir = chooser.directory();

command = "tce-load" + flags + (string)chooser.value();

status_out->activate();
status_out->color(FL_WHITE);
status_out->value(command.c_str());
command = command + "\n";
write(G_out, command.c_str(), command.length());
Fl::add_fd(fileno(G_in), HandleInput_CB, (void*)&status_out);
}

static void mirror_btnCallback(Fl_Widget*, void*) {
  mode = "mirror";
tabs->deactivate();
search_choices->deactivate();
search_field->deactivate();
system("cat /opt/localmirrors /usr/local/share/mirrors > /tmp/mirrors 2>/dev/null");
brwSelect->load("/tmp/mirrors");
if ( brwSelect->size() == 1)
  fl_message("Must load mirrors.tcz extension or have /opt/localmirrors in order to use this feature.");
else {
   brwSelect->remove(brwSelect->size());
   box_select->label("Select Mirror");
   box_select->activate();
}
}

static void tabsGroupCB(Fl_Widget*, void*) {
  if (brwSelect->value())
{
   cursor_wait();
   int results;
   select_extn = brwSelect->text(brwSelect->value());
   
   if (infoTab->visible())
   {
     string select_extn_file = select_extn + (string)".info";
     command = "tce-fetch.sh -O " + select_extn_file;
     displayTabData();
   }
   
   if (filesTab->visible())
   {
     string select_extn_file = select_extn + (string)".list";
     command = "tce-fetch.sh -O " + select_extn_file;
     displayTabData();
   }
   
   if (dependsTab->visible())
   {
     string select_extn_file = select_extn + (string)".tree";
     command = "tce-fetch.sh -O " + select_extn_file;
     displayTabData();
   }
   if (sizeTab->visible())
   {
     command = "tce-size " + select_extn;
     displayTabData();
   }
   cursor_normal();
}
}

Fl_Double_Window *window=(Fl_Double_Window *)0;

Fl_Choice *search_choices=(Fl_Choice *)0;

Fl_Menu_Item menu_search_choices[] = {
 {gettext("Search"), 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {gettext("Provides"), 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {gettext("Tags"), 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Input *search_field=(Fl_Input *)0;

Fl_Box *box_select=(Fl_Box *)0;

Fl_Browser *brwSelect=(Fl_Browser *)0;

Fl_Tabs *tabs=(Fl_Tabs *)0;

Fl_Group *infoTab=(Fl_Group *)0;

Fl_Text_Display *infoDisplay=(Fl_Text_Display *)0;

Fl_Group *filesTab=(Fl_Group *)0;

Fl_Text_Display *filesDisplay=(Fl_Text_Display *)0;

Fl_Group *dependsTab=(Fl_Group *)0;

Fl_Text_Display *dependsDisplay=(Fl_Text_Display *)0;

Fl_Group *sizeTab=(Fl_Group *)0;

Fl_Text_Display *sizeDisplay=(Fl_Text_Display *)0;

Fl_Choice *install_choices=(Fl_Choice *)0;

Fl_Menu_Item menu_install_choices[] = {
 {gettext("OnBoot"), 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {gettext("OnDemand"), 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {gettext("Download + Load"), 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {gettext("Download Only"), 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Button *btn_go=(Fl_Button *)0;

Fl_Output *status_out=(Fl_Output *)0;

Fl_Button *btn_tce=(Fl_Button *)0;

int main(int argc, char **argv) {
  setlocale(LC_ALL, "");
bindtextdomain("tinycore", "/usr/local/share/locale");
textdomain("tinycore");

ifstream mirror_fin("/opt/tcemirror");
getline(mirror_fin,mirror);
mirror_fin.close();

char buffer[1024];
int length;
length = readlink("/etc/sysconfig/tcedir", buffer, sizeof(buffer));
buffer[length] = '\0';
download_dir = strdup(buffer);
/*
ifstream dd_fin("/etc/sysconfig/tcedir");
getline(dd_fin,download_dir);
dd_fin.close();
*/

download_dir += "/optional";
last_dir = download_dir;   
ifstream last_dir_file("/opt/.appbrowser");
if ( last_dir_file.is_open() )
{
   getline(last_dir_file,last_dir);
   last_dir_file.close();
}

chdir(download_dir.c_str()); // we go there to more easily handle errors (delete, zsync)

// Test writable 
string testfile = download_dir + "/test.test";
ofstream writest(testfile.c_str());
if (writest.fail()) {
   fl_message("Fatal Error: TCE Directory is not writable.");
   exit(1);
}
writest.close();
unlink(testfile.c_str());   

// Make fifo
unlink("/tmp/ab2tce.fifo");
if ( mkfifo("/tmp/ab2tce.fifo", 0666) < 0 ) {
    perror("mkfifo(/tmp/ab2tce.fifo)");
    exit(1);
}

// Popen child for reading, set child to read fifo
if ( ( G_in = popen("ab2tce.sh < /tmp/ab2tce.fifo", "r") ) == NULL ) {
    perror("popen failed");
    exit(1);
}
setbuf(G_in, NULL);                         // disable buffering

// Now open fifo
if ( ( G_out = open("/tmp/ab2tce.fifo", O_WRONLY) ) < 0 ) {
    perror("open(/tmp/ab2tce.fifo) for write failed");
    unlink("/tmp/ab2tce.fifo");
    exit(1);
}
  { window = new Fl_Double_Window(685, 395, gettext("Appbrowser"));
    window->callback((Fl_Callback*)btnCallback, (void*)("quit"));
    { Fl_Button* o = new Fl_Button(0, 0, 68, 20, gettext("Connect"));
      o->callback((Fl_Callback*)btnCallback, (void*)("tcz"));
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(69, 0, 65, 20, gettext("Local"));
      o->callback((Fl_Callback*)local_btnCallback, (void*)("File/Local"));
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(135, 0, 65, 20, gettext("Mirrors"));
      o->callback((Fl_Callback*)mirror_btnCallback, (void*)("mirror"));
    } // Fl_Button* o
    { search_choices = new Fl_Choice(206, 0, 93, 20);
      search_choices->down_box(FL_BORDER_BOX);
      search_choices->deactivate();
      search_choices->menu(menu_search_choices);
    } // Fl_Choice* search_choices
    { search_field = new Fl_Input(300, 0, 385, 20);
      search_field->labeltype(FL_NO_LABEL);
      search_field->callback((Fl_Callback*)btnCallback, (void*)("search"));
      search_field->when(FL_WHEN_ENTER_KEY);
      search_field->deactivate();
    } // Fl_Input* search_field
    { box_select = new Fl_Box(40, 24, 120, 16);
      box_select->deactivate();
    } // Fl_Box* box_select
    { brwSelect = new Fl_Browser(0, 45, 200, 325);
      brwSelect->type(2);
      brwSelect->textfont(4);
      brwSelect->callback((Fl_Callback*)brwSelect_callback);
    } // Fl_Browser* brwSelect
    { tabs = new Fl_Tabs(205, 20, 475, 350);
      tabs->callback((Fl_Callback*)tabsGroupCB);
      { infoTab = new Fl_Group(205, 45, 475, 325, gettext("Info"));
        infoTab->when(FL_WHEN_CHANGED);
        infoTab->hide();
        infoTab->deactivate();
        { infoDisplay = new Fl_Text_Display(210, 50, 470, 318);
          infoDisplay->textfont(4);
          infoDisplay->buffer(txtBuffer);
        } // Fl_Text_Display* infoDisplay
        infoTab->end();
      } // Fl_Group* infoTab
      { filesTab = new Fl_Group(205, 45, 475, 325, gettext("Files"));
        filesTab->when(FL_WHEN_CHANGED);
        filesTab->hide();
        filesTab->deactivate();
        { filesDisplay = new Fl_Text_Display(210, 50, 470, 318);
          filesDisplay->textfont(4);
          filesDisplay->buffer(txtBuffer);
        } // Fl_Text_Display* filesDisplay
        filesTab->end();
      } // Fl_Group* filesTab
      { dependsTab = new Fl_Group(205, 45, 475, 325, gettext("Depends"));
        dependsTab->deactivate();
        { dependsDisplay = new Fl_Text_Display(210, 50, 470, 318);
          dependsDisplay->labelfont(1);
          dependsDisplay->textfont(4);
          dependsDisplay->buffer(txtBuffer);
        } // Fl_Text_Display* dependsDisplay
        dependsTab->end();
      } // Fl_Group* dependsTab
      { sizeTab = new Fl_Group(205, 45, 475, 325, gettext("Size"));
        sizeTab->hide();
        sizeTab->deactivate();
        { sizeDisplay = new Fl_Text_Display(210, 50, 470, 318);
          sizeDisplay->textfont(4);
          sizeDisplay->buffer(txtBuffer);
        } // Fl_Text_Display* sizeDisplay
        sizeTab->end();
      } // Fl_Group* sizeTab
      tabs->end();
    } // Fl_Tabs* tabs
    { install_choices = new Fl_Choice(3, 373, 140, 20);
      install_choices->down_box(FL_BORDER_BOX);
      install_choices->menu(menu_install_choices);
    } // Fl_Choice* install_choices
    { btn_go = new Fl_Button(145, 373, 30, 20, gettext("Go"));
      btn_go->callback((Fl_Callback*)btnCallback, (void*)("go"));
      btn_go->deactivate();
    } // Fl_Button* btn_go
    { status_out = new Fl_Output(225, 373, 420, 20, gettext("Status"));
      status_out->color((Fl_Color)55);
      status_out->value(download_dir.c_str());
      status_out->label("  TCE:");
    } // Fl_Output* status_out
    { btn_tce = new Fl_Button(645, 373, 34, 20, gettext("Set"));
      btn_tce->callback((Fl_Callback*)btnCallback, (void*)("setdrive"));
      btn_tce->deactivate();
      if (download_dir.compare(0,8,"/tmp/tce") == 0 ){btn_tce->activate();status_out->color(9);};
    } // Fl_Button* btn_tce
    window->label(mirror.c_str());
    window->end();
    window->resizable(window);
  } // Fl_Double_Window* window
  window->show(argc, argv);
  return Fl::run();
}
