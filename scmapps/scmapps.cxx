// generated by Fast Light User Interface Designer (fluid) version 1.0110

#include <libintl.h>
#include "scmapps.h"
// (c) Robert Shingledecker 2012
#include <iostream>
#include <fstream>
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
static int results, locales_set=0; 
static string repository, tce_dir, download_dir, scmbootList; 
static string select_extn, select_results, report_type, appserr; 
static string hilite, mode, command, msg, mirror, err_extn; 
static Fl_Text_Buffer *txtBuffer = new Fl_Text_Buffer; 
static void errhandler(const string &str); 

static char * mygettext(const char *msgid) {
  if (!locales_set) {

setlocale(LC_ALL, "");
bindtextdomain("tinycore","/usr/local/share/locale");
textdomain("tinycore");

locales_set=1;

}

return gettext(msgid);
}

static void localView() {
  grpUpdates->deactivate();
grpUpdates->hide();
txtBuffer->loadfile("");
tabs->hide();
grpSearch->deactivate();
grpResults->show();
grpResults->activate();
boxResults->label("Results");
brwResults->activate();
brwResults->clear();
grpInstall->activate();
grpInstall->show();
choiceInstall->deactivate();
btnGo->show();
brwExtensions->activate();
brwExtensions->show();
}

static void remoteView() {
  grpResults->hide();
grpUpdates->deactivate();
grpUpdates->hide();
tabs->show();
tabs->deactivate();
grpInstall->activate();
grpInstall->show();
choiceInstall->activate();
btnGo->show();
grpSearch->activate();
brwResults->deactivate();
txtBuffer->loadfile("");
brwExtensions->activate();
brwExtensions->show();
}

static void multiView() {
  brwExtensions->hide();
brwExtensions->deactivate();
tabs->hide();
grpInstall->hide();
grpInstall->deactivate();
grpSearch->deactivate();
grpUpdates->show();
grpUpdates->activate();
grpResults->show();
grpResults->activate();  
brwMulti->activate();
brwMulti->show();
brwMulti->clear();
brwResults->activate();
brwResults->show();
brwResults->clear();
}

static void displayTabData() {
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

void fetch_extension() {
  outputStatus->color(FL_WHITE);
outputStatus->value(command.c_str());
cursor_wait();
msg = select_extn;
command = "aterm -fg black -bg white +tr -g 80x5 -e " + command;
system(command.c_str());

command = "busybox md5sum -cs " + select_extn + ".md5.txt";
results = system(command.c_str());

if (results == 0 )
{
  outputStatus->color(175);
  msg += + " OK.";
} else 
  msg += " Failed.";
  
outputStatus->value(msg.c_str());  
cursor_normal();
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
  string cmd = "zsync " + mirror + "4.x/scm/" + err_extn + ".zsync";

int ret = system(cmd.c_str());
ret = WEXITSTATUS(ret);

if (ret == 0) fetch_extension();
else {

	// If zsync also fails, it's a bad hair
	// day for the network. Nothing we can do.

	outputStatus->value("Zsync failed.");

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

static void menuCB(Fl_Widget *, void* userdata) {
  if (userdata == "scm")
{
   mode = "scm";
   remoteView();   
   repository = (const char*) userdata;
   cursor_wait();
   unlink("scm.lst");
   command = "scm-fetch.sh scm.lst.gz";
   int results = system(command.c_str());
   cursor_normal();
   if (results == 0 )
   {
      system("gunzip -c scm.lst.gz > scm.lst");
      brwExtensions->load("scm.lst");
      brwExtensions->remove(brwExtensions->size());
      btnGo->deactivate();
      boxLeftSide->label("Select Extension");
      boxLeftSide->activate();
      grpSearch->activate();
//      search_field->activate();                                              
    } else
      fl_message("Connection error, check network or mirror.");
} else if (userdata == "install")
{
   mode = "install";
   localView();   
   boxResults->label("Results");
   cursor_wait();
   command = "scm -i";
   int results = system(command.c_str());
   cursor_normal();
   if (results == 0 )
   {   
      brwExtensions->load("/tmp/scm.lst");
      unlink("/tmp/scm.lst");
      brwExtensions->remove(brwExtensions->size());
      btnGo->deactivate();
      boxLeftSide->label("Select scm to Install");
      boxLeftSide->activate();
    } else
      fl_message("No local scm extensions found.");   
} else if (userdata == "uninstall")
{
   mode = "uninstall";
   localView();   
   boxResults->label("Results");
   cursor_wait();
   command = "scm -u";
   int results = system(command.c_str());
   cursor_normal();
   if (results == 0 )
   {   
      brwExtensions->load("/tmp/scm.lst");
      unlink("/tmp/scm.lst");
      brwExtensions->remove(brwExtensions->size());
      btnGo->deactivate();
      boxLeftSide->label("Select scm to Uninstall");
      boxLeftSide->activate();
    } else
      fl_message("No installed scm extensions found.");
} else if (userdata == "delete")
{
   mode = "delete";
   localView();   
   boxResults->label("Results");
   cursor_wait();
   command = "scm -i";
   int results = system(command.c_str());
   cursor_normal();
   if (results == 0 )
   {   
      brwExtensions->load("/tmp/scm.lst");
      unlink("/tmp/scm.lst");
      brwExtensions->remove(brwExtensions->size());
      btnGo->deactivate();
      boxLeftSide->label("Select scm to Delete");
      boxLeftSide->activate();
    } else
      fl_message("No local scm extensions found.");   
} else if (userdata == "updates") 
{
   string line;
   mode = "updates";
   multiView();
   boxLeftSide->label("Select SCM to Update");
   command = "version -c >/tmp/VerChk";
   results = system(command.c_str());
   if ( results == 0 )
   {
      brwResults->load("/tmp/VerChk");
      brwResults->bottomline(brwResults->size());
   }
   brwResults->add("Please Standby... Now checking your SCM extensions.");
   //   

   command = "scm-update -l " + tce_dir;
   FILE *pipe = popen(command.c_str(),"r");
   char *mbuf = (char *)calloc(PATH_MAX,sizeof(char));
   if (pipe)
   {
      while(fgets(mbuf,PATH_MAX,pipe))
      {
         string line(mbuf);
         if (line.find("Error ") == string::npos ) {
            brwMulti->add(line.c_str());
            brwMulti->bottomline(brwMulti->size());
         } else { 
            brwResults->add(line.c_str());
            brwResults->bottomline(brwResults->size());
         }
         Fl::flush();
      }
      pclose(pipe);
      free(mbuf);
   }
   Fl::flush();      
   brwResults->add("Scan for updates completed.");    
   if ( brwMulti->size() >= 1 ) {
      btnMulti->show();
      btnMulti->activate();
   } else {
      if ( brwResults->size() == 1 )
         brwResults->add("Extensions are current. No updates required.");
   }
   cursor_normal();
   Fl::flush();      
} else if (userdata == "boot")
{
   mode = "boot";
   localView();   
   boxResults->label("Boot List");
   cursor_wait();
   command = "scm -b";
   int results = system(command.c_str());
   cursor_normal();
   if (results == 0 )
   {   
      brwExtensions->load("/tmp/scm.lst");
      unlink("/tmp/scm.lst");
      brwExtensions->remove(brwExtensions->size());
      btnGo->deactivate();
      boxLeftSide->label("Local scms:");
      boxLeftSide->activate();
      brwResults->load(scmbootList.c_str());
    } else
      fl_message("No local scm extensions found.");   
} else if (userdata == "setdrive")
{
   cursor_wait();
   command = "tce-setdrive -l";
   int results = system(command.c_str());
   cursor_normal();
   if (results == 0 )
   {
      mode = "setdrive";
      brwExtensions->load("/tmp/tcesetdev");
      brwExtensions->remove(brwExtensions->size());
      boxLeftSide->label("Select for TCE dir.");
      boxLeftSide->activate();
      unlink("/tmp/tcesetdev");
   } else
     fl_message("No available drives found!");
     
} else if (userdata == "search")
{
  if (choiceSearch->text() == "Search")
     command = "scm-search.sh";
  else if (choiceSearch->text() == "Tags")
     command = "scm-search.sh -t";
  else
     command = "provides.sh";
  tabs->deactivate();
  txtBuffer->loadfile("");
  brwExtensions->load("");
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
        brwExtensions->add(line.c_str());
        brwExtensions->bottomline(brwExtensions->size());
        Fl::flush();
     }
     pclose(pipe);
     free(mbuf);
  }
  search_field->value("");
  cursor_normal();
  btnGo->deactivate();
  choiceSearch->activate();
  search_field->activate();                                              
} else if (userdata == "md5s") 
{
   mode = "md5s";
   multiView();
   cursor_wait();
   command = "cd " + download_dir + " && ls *.scm.md5.txt > /tmp/apps_upd.lst";
   system(command.c_str());
   boxLeftSide->label(download_dir.c_str());
   brwMulti->load("/tmp/apps_upd.lst");
   brwMulti->remove(brwMulti->size());
   btnMulti->show();
   btnMulti->activate();
   cursor_normal();
      
} else if (userdata == "quit")
  {
    command = "quit\n";
    unlink("scm.lst");
    unlink("scm.lst.gz");
    exit(0);

  }
}

static void brwExtensionsCB(Fl_Widget *, void *) {
  if (brwExtensions->value())
{
   select_extn = brwExtensions->text(brwExtensions->value());
   if ( mode == "scm" )
   {
      string select_extn_file = select_extn + (string)".info";
      string info_line;
      command = "scm-fetch.sh " + select_extn_file;
      int results = system(command.c_str());
      if (results == 0)
      {
         txtBuffer->loadfile(select_extn_file.c_str());

         unlink(select_extn_file.c_str());
         btnGo->activate();
         
         tabs->activate();
         infoTab->activate();
         filesTab->activate();
         dependsTab->activate();
         infoTab->show();
      } 
   }
   if ( mode == "install" or mode == "uninstall" or mode == "delete" )
   {
      btnGo->activate();
   }
   if ( mode == "boot" )
   {
      cursor_wait();
      command = "echo " + select_extn + " >> " + scmbootList;
      system(command.c_str());
      brwResults->load(scmbootList.c_str());
      brwResults->remove(brwResults->size());
      
      command = "scm -b";
      int results = system(command.c_str());
      if (results == 0 )
      {   
         brwExtensions->load("/tmp/scm.lst");
         unlink("/tmp/scm.lst");
         brwExtensions->remove(brwExtensions->size());
      }
      cursor_normal();
   }
   if ( mode == "mirror" )
   {
     mirror = select_extn;
     uri->value(mirror.c_str());
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
         brwExtensions->clear();
         boxLeftSide->label("");
         download_dir += "/optional";
         outputStatus->color(FL_WHITE);
         outputStatus->value((download_dir).c_str());
         btn_tce->deactivate();         
      }
   }
}
}

static void btnMirrorCB(Fl_Widget*, void*) {
  mode = "mirror";
remoteView();
tabs->deactivate();
grpSearch->deactivate();
system("cat /opt/localmirrors /usr/local/share/mirrors > /tmp/mirrors 2>/dev/null");
brwExtensions->load("/tmp/mirrors");
if ( brwExtensions->size() == 1)
  fl_message("Must load mirrors.tcz extension or have /opt/localmirrors in order to use this feature.");
else {
   brwExtensions->remove(brwExtensions->size());
   boxLeftSide->label("Select Mirror");
   boxLeftSide->activate();
}
}

void brwMultiCB(Fl_Widget *, void *) {
  cursor_wait();
brwResults->clear();
if ( mode == "updates" ) { 
   for (int t=0; t<=brwMulti->size(); t++) {
      if (brwMulti->selected(t) ) {
         select_extn = brwMulti->text(t);
         string info_file(select_extn,0,select_extn.size()-1);
         info_file = info_file + ".info";
         command = "scm-fetch.sh " + info_file;
         Fl::flush();
         int results = system(command.c_str());
         if (results == 0) {
            boxResults->label(select_extn.c_str());
            brwResults->load(info_file.c_str());
            unlink(info_file.c_str());
         }   
         continue;
      }
   }
}   
cursor_normal();
}

void btnMultiCB(Fl_Widget *, void *) {
  cursor_wait();
brwResults->clear();
for ( int t=0; t<=brwMulti->size(); t++ )
{
   if ( brwMulti->selected(t) )
   {
      select_extn = brwMulti->text(t);
      if ( mode == "md5s" )
      {
         command = "cd " + download_dir +"/ && busybox md5sum -c " + select_extn;
         results = system(command.c_str());
         if ( results == 0 ) {
            msg = " OK";
            hilite = "";
         } else {
            msg = " FAILED";
            hilite = "@B17";
         }   
             
         brwResults->add((hilite + select_extn + msg).c_str());
         Fl::flush();      
      
      } else {
         boxResults->label(("Fetching " + select_extn).c_str());
         boxResults->redraw();
         Fl::flush();
         
         string md5_file(select_extn,0,select_extn.size()-1);
         md5_file = md5_file + ".md5.txt";
         command = "scm-update -u " + download_dir +"/" + md5_file;
         
         
         
         FILE *pipe = popen(command.c_str(),"r");
         char *mbuf = (char *)calloc(PATH_MAX,sizeof(char));
         if (pipe)
         {
            while(fgets(mbuf,PATH_MAX,pipe))
            {
               string line(mbuf);
               brwResults->add(line.c_str());
               brwResults->bottomline(brwResults->size());
               Fl::flush();
            }
            pclose(pipe);
            free(mbuf);
         }
      }
   }
}
brwMulti->deselect();
if (mode == "updates" )
	boxResults->label("Updates complete.");
cursor_normal();
}

static void tabsGroupCB(Fl_Widget*, void*) {
  if (brwExtensions->value())
{
   int results;
   select_extn = brwExtensions->text(brwExtensions->value());
   
   if (infoTab->visible())
   {
     string select_extn_file = select_extn + (string)".info";
     command = "scm-fetch.sh -O " + select_extn_file;
     displayTabData();
   }
   
   if (filesTab->visible())
   {
     string select_extn_file = select_extn + (string)".list";
     command = "scm-fetch.sh -O " + select_extn_file;
     displayTabData();
   }
   
   if (dependsTab->visible())
   {
     cursor_wait();
     txtBuffer->loadfile("");
     string select_extn_file = select_extn + (string)".dep";
     command = "scm-fetch.sh -O " + select_extn_file;
     displayTabData();
   }
   
}
}

void brwResultsCB(Fl_Widget *, void *) {
  if (brwResults->value())
{
   select_results = brwResults->text(brwResults->value());
   if (report_type == "delete")
   {
     string target = select_results.substr(select_results.find_last_of("/")+1);
     command = "sed -i '/" + target + "/d' /tmp/audit_marked.lst";
     system(command.c_str());
     brwResults->load("/tmp/audit_marked.lst");
     brwResults->remove(brwResults->size());
   }
   if (mode == "boot")
   {
     cursor_wait();
     command = "sed -i '/" + select_results + "/d' " + scmbootList;
     system(command.c_str());
  
     brwResults->load(scmbootList.c_str());
     brwResults->remove(brwResults->size());
     
     command = "scm -b";
     int results = system(command.c_str());
     if (results == 0 )
     {   
        brwExtensions->load("/tmp/scm.lst");
        unlink("/tmp/scm.lst");
        brwExtensions->remove(brwExtensions->size());
       cursor_normal();
     }
   }  

}
}

static void btnGoCB(Fl_Widget *, void* userdata) {
  if (mode == "scm")
{
   outputStatus->value("");
   outputStatus->label("Status");
   int action = choiceInstall->value();
   string action_type;
   switch(action)
   {
      case 0 : action_type="wi";
               break;
      case 1 : action_type="w";
               break;
      default: mode="w";
   }
   command = "scm-load -" + action_type + " " + select_extn;
   fetch_extension();
   if ( action_type == "wi" && results == 0 )
   {
      command = "echo " + select_extn.substr(0,(select_extn.length()-4)) + " >>"+scmbootList;
      system(command.c_str());
      Fl::flush();
   }
} else if (mode == "install")
{
  command = "/usr/bin/scm-load -i " + select_extn;
  cursor_wait();
  results = system(command.c_str());
  if (results == 0 )
  {
     msg = "Successfully installed " + select_extn;
     brwExtensions->remove(brwExtensions->value());
  } else
     msg = "Failed.";
  
  brwResults->add(msg.c_str());  
  cursor_normal();

} else if (mode == "uninstall")
{
  command = "/usr/bin/scm-load -r " + select_extn;
  cursor_wait();
  results = system(command.c_str());
  if (results == 0 )
  {
     msg = "Successfully uninstalled " + select_extn;
     brwExtensions->remove(brwExtensions->value());
  } else
     msg = "Failed.";
  
  brwResults->add(msg.c_str());  
  cursor_normal();
} else if ( mode == "delete" )
{
  command = "rm -f " + download_dir + "/" + select_extn + ".scm*";
  results = system(command.c_str());
  if (results == 0)
  {
    command = "sed -i '/" + select_extn + "/d' " + scmbootList;
    system(command.c_str());
    msg = select_extn + " deleted.";
    brwExtensions->remove(brwExtensions->value());
  } else
    msg = "Failed";
    
  brwResults->add(msg.c_str());  
  cursor_normal();
}
}

Fl_Double_Window *window=(Fl_Double_Window *)0;

Fl_Menu_Bar *menuBar=(Fl_Menu_Bar *)0;

Fl_Menu_Item menu_menuBar[] = {
 {gettext("   Apps"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {gettext("Cloud (Remote)"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {gettext("Browse"), 0,  (Fl_Callback*)menuCB, (void*)("scm"), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {gettext("Select Mirror"), 0,  (Fl_Callback*)btnMirrorCB, (void*)("mirror"), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {gettext("Local"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {gettext("Install"), 0,  (Fl_Callback*)menuCB, (void*)("install"), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {gettext("Uninstall"), 0,  (Fl_Callback*)menuCB, (void*)("uninstall"), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {gettext("Delete"), 0,  (Fl_Callback*)menuCB, (void*)("delete"), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {gettext("Maintenance"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {gettext("Md5 Checking"), 0,  (Fl_Callback*)menuCB, (void*)("md5s"), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {gettext("Check for Updates"), 0,  (Fl_Callback*)menuCB, (void*)("updates"), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {gettext("Boot List Maintenance"), 0,  (Fl_Callback*)menuCB, (void*)("boot"), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {gettext("Quit"), 0,  (Fl_Callback*)menuCB, (void*)("quit"), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Group *grpSearch=(Fl_Group *)0;

Fl_Choice *choiceSearch=(Fl_Choice *)0;

Fl_Menu_Item menu_choiceSearch[] = {
 {gettext("Search"), 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {gettext("Tags"), 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Input *search_field=(Fl_Input *)0;

Fl_Box *boxLeftSide=(Fl_Box *)0;

Fl_Browser *brwExtensions=(Fl_Browser *)0;

Fl_Group *grpUpdates=(Fl_Group *)0;

Fl_Browser *brwMulti=(Fl_Browser *)0;

Fl_Button *btnMulti=(Fl_Button *)0;

Fl_Tabs *tabs=(Fl_Tabs *)0;

Fl_Group *infoTab=(Fl_Group *)0;

Fl_Text_Display *infoDisplay=(Fl_Text_Display *)0;

Fl_Group *filesTab=(Fl_Group *)0;

Fl_Text_Display *filesDisplay=(Fl_Text_Display *)0;

Fl_Group *dependsTab=(Fl_Group *)0;

Fl_Text_Display *dependsDisplay=(Fl_Text_Display *)0;

Fl_Group *grpResults=(Fl_Group *)0;

Fl_Box *boxResults=(Fl_Box *)0;

Fl_Browser *brwResults=(Fl_Browser *)0;

Fl_Group *grpInstall=(Fl_Group *)0;

Fl_Choice *choiceInstall=(Fl_Choice *)0;

Fl_Menu_Item menu_choiceInstall[] = {
 {gettext("Install"), 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {gettext("Download Only"), 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Button *btnGo=(Fl_Button *)0;

Fl_Output *outputStatus=(Fl_Output *)0;

Fl_Button *btn_tce=(Fl_Button *)0;

Fl_Output *uri=(Fl_Output *)0;

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
tce_dir = strdup(buffer);

download_dir = tce_dir + "/optional";
scmbootList = tce_dir + "/scmboot.lst";
chdir(download_dir.c_str()); // we go there to more easily handle errors (delete, zsync)
  { window = new Fl_Double_Window(685, 433, gettext("ScmApps: Self Contained Appllications (scm)"));
    window->callback((Fl_Callback*)menuCB, (void*)("quit"));
    { menuBar = new Fl_Menu_Bar(0, 7, 85, 20);
      menuBar->menu(menu_menuBar);
    } // Fl_Menu_Bar* menuBar
    { grpSearch = new Fl_Group(90, 7, 595, 20);
      grpSearch->deactivate();
      { choiceSearch = new Fl_Choice(90, 7, 93, 20);
        choiceSearch->down_box(FL_BORDER_BOX);
        choiceSearch->menu(menu_choiceSearch);
      } // Fl_Choice* choiceSearch
      { search_field = new Fl_Input(185, 7, 500, 20);
        search_field->labeltype(FL_NO_LABEL);
        search_field->callback((Fl_Callback*)menuCB, (void*)("search"));
        search_field->when(FL_WHEN_ENTER_KEY);
      } // Fl_Input* search_field
      grpSearch->end();
    } // Fl_Group* grpSearch
    { boxLeftSide = new Fl_Box(5, 30, 190, 19);
    } // Fl_Box* boxLeftSide
    { brwExtensions = new Fl_Browser(0, 52, 200, 325);
      brwExtensions->type(2);
      brwExtensions->textfont(4);
      brwExtensions->callback((Fl_Callback*)brwExtensionsCB);
    } // Fl_Browser* brwExtensions
    { grpUpdates = new Fl_Group(0, 52, 205, 348);
      grpUpdates->hide();
      grpUpdates->deactivate();
      { brwMulti = new Fl_Browser(0, 52, 200, 325);
        brwMulti->type(3);
        brwMulti->textfont(4);
        brwMulti->callback((Fl_Callback*)brwMultiCB);
      } // Fl_Browser* brwMulti
      { btnMulti = new Fl_Button(3, 380, 180, 20, gettext("Process Selected Item(s)"));
        btnMulti->callback((Fl_Callback*)btnMultiCB);
      } // Fl_Button* btnMulti
      grpUpdates->end();
    } // Fl_Group* grpUpdates
    { tabs = new Fl_Tabs(205, 27, 480, 355);
      tabs->callback((Fl_Callback*)tabsGroupCB);
      { infoTab = new Fl_Group(205, 52, 475, 325, gettext("Info"));
        infoTab->when(FL_WHEN_CHANGED);
        infoTab->deactivate();
        { infoDisplay = new Fl_Text_Display(210, 57, 470, 318);
          infoDisplay->textfont(4);
          infoDisplay->buffer(txtBuffer);
        } // Fl_Text_Display* infoDisplay
        infoTab->end();
      } // Fl_Group* infoTab
      { filesTab = new Fl_Group(205, 52, 475, 325, gettext("Files"));
        filesTab->when(FL_WHEN_CHANGED);
        filesTab->hide();
        filesTab->deactivate();
        { filesDisplay = new Fl_Text_Display(210, 57, 470, 318);
          filesDisplay->textfont(4);
          filesDisplay->buffer(txtBuffer);
        } // Fl_Text_Display* filesDisplay
        filesTab->end();
      } // Fl_Group* filesTab
      { dependsTab = new Fl_Group(210, 57, 475, 325, gettext("Depends"));
        dependsTab->hide();
        dependsTab->deactivate();
        { dependsDisplay = new Fl_Text_Display(215, 62, 470, 318);
          dependsDisplay->labelfont(1);
          dependsDisplay->textfont(4);
          dependsDisplay->buffer(txtBuffer);
        } // Fl_Text_Display* dependsDisplay
        dependsTab->end();
      } // Fl_Group* dependsTab
      tabs->end();
    } // Fl_Tabs* tabs
    { grpResults = new Fl_Group(205, 25, 480, 352);
      grpResults->hide();
      grpResults->deactivate();
      { boxResults = new Fl_Box(205, 25, 475, 27);
      } // Fl_Box* boxResults
      { brwResults = new Fl_Browser(210, 52, 475, 325);
        brwResults->type(1);
        brwResults->textfont(4);
        brwResults->callback((Fl_Callback*)brwResultsCB);
      } // Fl_Browser* brwResults
      grpResults->end();
    } // Fl_Group* grpResults
    { grpInstall = new Fl_Group(3, 380, 180, 20);
      { choiceInstall = new Fl_Choice(3, 380, 140, 20);
        choiceInstall->down_box(FL_BORDER_BOX);
        choiceInstall->menu(menu_choiceInstall);
      } // Fl_Choice* choiceInstall
      { btnGo = new Fl_Button(145, 380, 30, 20, gettext("Go"));
        btnGo->callback((Fl_Callback*)btnGoCB, (void*)("go"));
        btnGo->deactivate();
      } // Fl_Button* btnGo
      grpInstall->end();
    } // Fl_Group* grpInstall
    { outputStatus = new Fl_Output(225, 380, 420, 20, gettext("Status"));
      outputStatus->color((Fl_Color)55);
      outputStatus->value(download_dir.c_str());
      outputStatus->label("  TCE:");
    } // Fl_Output* outputStatus
    { btn_tce = new Fl_Button(645, 380, 34, 20, gettext("Set"));
      btn_tce->callback((Fl_Callback*)menuCB, (void*)("setdrive"));
      btn_tce->deactivate();
      if (download_dir.compare(0,8,"/tmp/tce") == 0 ){btn_tce->activate();outputStatus->color(9);};
    } // Fl_Button* btn_tce
    { uri = new Fl_Output(35, 405, 655, 20, gettext("URI:"));
      uri->color((Fl_Color)29);
      uri->value(mirror.c_str());
    } // Fl_Output* uri
    window->end();
    window->resizable(window);
  } // Fl_Double_Window* window
  window->show(argc, argv);
  return Fl::run();
}
