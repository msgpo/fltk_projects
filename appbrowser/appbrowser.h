// generated by Fast Light User Interface Designer (fluid) version 1.0110

#ifndef appbrowser_h
#define appbrowser_h
#include <FL/Fl.H>
void displayTabData();
#include <FL/Fl_Double_Window.H>
extern Fl_Double_Window *errwindow;
#include <FL/Fl_Box.H>
extern Fl_Box *errlabel;
#include <FL/Fl_Button.H>
extern Fl_Double_Window *window;
#include <FL/Fl_Choice.H>
extern Fl_Choice *search_choices;
#include <FL/Fl_Input.H>
extern Fl_Input *search_field;
extern Fl_Box *box_select;
#include <FL/Fl_Browser.H>
extern Fl_Browser *brwSelect;
#include <FL/Fl_Tabs.H>
extern Fl_Tabs *tabs;
#include <FL/Fl_Group.H>
extern Fl_Group *infoTab;
#include <FL/Fl_Text_Display.H>
extern Fl_Text_Display *infoDisplay;
extern Fl_Group *filesTab;
extern Fl_Text_Display *filesDisplay;
extern Fl_Group *dependsTab;
extern Fl_Text_Display *dependsDisplay;
extern Fl_Group *sizeTab;
extern Fl_Text_Display *sizeDisplay;
extern Fl_Choice *install_choices;
extern Fl_Button *btn_go;
#include <FL/Fl_Output.H>
extern Fl_Output *status_out;
extern Fl_Button *btn_tce;
extern Fl_Menu_Item menu_search_choices[];
extern Fl_Menu_Item menu_install_choices[];
#endif
