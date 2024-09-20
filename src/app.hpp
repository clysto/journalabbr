#pragma once

#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Window.H>

#include "journal.hpp"

class ResultTable : public Fl_Table {
 private:
  std::vector<Journal> data;
  std::vector<std::string> headers;

 public:
  ResultTable(int x, int y, int w, int h, const char* l = 0);

  void draw_cell(TableContext context, int ROW = 0, int COL = 0, int X = 0, int Y = 0, int W = 0,
                 int H = 0) FL_OVERRIDE;

  void clear_data();

  void append_data(const Journal& journal);

  const std::vector<Journal>& get_data();
};

class App {
 private:
  Fl_Window* window;
  Fl_Window* terms_window;
  Fl_Button* search_button;
  Fl_Menu_Bar* menu_bar;
  Fl_Input* search_bar;
  ResultTable* search_result;
  ResultTable* terms_table;
  std::vector<std::string> querys;

 public:
  App();
  ~App();
  void show();
  static void open_file(Fl_Widget* w, void* data);
  static void search(Fl_Widget* w, void* data);
  static void save_terms(Fl_Widget* w, void* data);
  static void about(Fl_Widget* w, void* data);
};