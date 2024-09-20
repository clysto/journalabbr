#include "app.hpp"

#include <FL/Fl_File_Chooser.H>
#include <FL/fl_draw.H>

#include <fstream>
#include <iostream>
#include <sstream>

#include "journal.hpp"

ResultTable::ResultTable(int x, int y, int w, int h, const char* l)
    : Fl_Table(x, y, w, h, l), headers({"E-ISSN", "Title", "JCR Abbr", "ISO Abbr", "Med Abbr"}) {
  col_header(1);
  cols(5);
  col_width_all(80);
  col_width(1, 180);
  col_resize(1);
  end();
}

void ResultTable::draw_cell(TableContext context, int ROW, int COL, int X, int Y, int W, int H) {
  switch (context) {
    case CONTEXT_COL_HEADER:
      fl_push_clip(X, Y, W, H);
      fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
      fl_color(FL_BLACK);
      fl_draw(headers[COL].c_str(), X, Y, W, H, FL_ALIGN_CENTER);
      fl_pop_clip();
      return;
    case CONTEXT_CELL:
      if (ROW >= data.size()) return;
      fl_push_clip(X, Y, W, H);
      fl_color(FL_WHITE);
      fl_rectf(X, Y, W, H);
      fl_color(FL_GRAY0);
      switch (COL) {
        case 0:
          fl_draw(data[ROW].eissn.c_str(), X, Y, W, H, FL_ALIGN_CENTER);
          break;
        case 1:
          fl_draw(data[ROW].title.c_str(), X, Y, W, H, FL_ALIGN_CENTER);
          break;
        case 2:
          fl_draw(data[ROW].jcr_abbr.c_str(), X, Y, W, H, FL_ALIGN_CENTER);
          break;
        case 3:
          fl_draw(data[ROW].iso_abbr.c_str(), X, Y, W, H, FL_ALIGN_CENTER);
          break;
        case 4:
          fl_draw(data[ROW].med_abbr.c_str(), X, Y, W, H, FL_ALIGN_CENTER);
          break;
        default:
          break;
      }
      fl_color(color());
      fl_rect(X, Y, W, H);
      fl_pop_clip();
      return;
    default:
      return;
  }
}

void ResultTable::clear_data() {
  data.clear();
  rows(data.size());
  redraw();
}

void ResultTable::append_data(const Journal& journal) {
  data.push_back(journal);
  rows(data.size());
  redraw();
}

const std::vector<Journal>& ResultTable::get_data() { return data; }

App::App() : querys({}), terms_window(nullptr), terms_table(nullptr) {
  window = new Fl_Window(340, 180, "Journal Abbr");
  auto g = new Fl_Group(10, 35, 320, 25);
  search_bar = new Fl_Input(10, 35, 230, 25);
  search_button = new Fl_Button(250, 35, 80, 25, "Search");
  g->end();
  g->resizable(search_bar);
  menu_bar = new Fl_Menu_Bar(0, 0, 340, 25);
  search_result = new ResultTable(10, 70, 320, 100);
  window->end();
  search_button->callback(App::search, this);
  search_bar->when(FL_WHEN_ENTER_KEY_ALWAYS);
  search_bar->callback(App::search, this);
  menu_bar->add("Tool/Generate Endnote Terms", 0, App::open_file, this);
  menu_bar->add("About", 0, App::about, this);
  window->resizable(search_result);
}

App::~App() {}

void App::show() {
  window->resize(0, 0, 600, 300);
  window->position((Fl::w() - window->w()) / 2, (Fl::h() - window->h()) / 2);
  window->show();
}

void App::open_file(Fl_Widget* w, void* self) {
  Fl_File_Chooser chooser(".", "*", Fl_File_Chooser::SINGLE, "Open File");
  chooser.show();
  while (chooser.shown()) Fl::wait();
  if (chooser.value()) {
    std::ifstream file(chooser.value());
    ((App*)self)->terms_window = new Fl_Window(600, 300, "Endnote Terms");
    ((App*)self)->terms_table = new ResultTable(10, 10, 580, 245);

    std::string line;
    ((App*)self)->querys.clear();
    while (std::getline(file, line)) {
      std::string query;
      std::istringstream iss(line);
      std::getline(iss, query, '\t');
      auto journals = search_journal(query);
      if (!journals.empty()) {
        Journal journal = journals[0];
        ((App*)self)->terms_table->append_data(journal);
        ((App*)self)->querys.push_back(query);
      }
    }

    auto g = new Fl_Group(10, 265, 580, 25);
    auto box = new Fl_Box(10, 265, 400, 25);
    auto close_button = new Fl_Button(420, 265, 80, 25, "Close");
    auto save_button = new Fl_Button(510, 265, 80, 25, "Save");
    g->end();
    g->resizable(box);

    save_button->callback(App::save_terms, self);
    close_button->callback([](Fl_Widget* w, void* self) { ((App*)self)->terms_window->hide(); }, self);

    ((App*)self)->terms_window->end();
    ((App*)self)->terms_window->resizable(((App*)self)->terms_table);
    ((App*)self)->terms_window->set_modal();
    ((App*)self)->terms_window->show();
  }
}

void App::search(Fl_Widget* w, void* self) {
  auto query = ((App*)self)->search_bar->value();
  auto journals = search_journal(std::string(query));
  ((App*)self)->search_result->clear_data();
  for (const auto& journal : journals) {
    ((App*)self)->search_result->append_data(journal);
  }
}

void App::save_terms(Fl_Widget* w, void* self) {
  Fl_File_Chooser chooser(".", "*", Fl_File_Chooser::CREATE, "Save File");
  chooser.show();
  while (chooser.shown()) Fl::wait();
  if (chooser.value()) {
    std::ofstream file(chooser.value());
    int i = 0;
    for (const auto& journal : ((App*)self)->terms_table->get_data()) {
      file << journal.title << '\t' << journal.iso_abbr << '\t' << journal.med_abbr << '\t' << ((App*)self)->querys[i++]
           << '\n';
    }
    ((App*)self)->terms_window->hide();
    delete ((App*)self)->terms_window;
    fl_alert("File saved successfully!");
  }
}

void App::about(Fl_Widget* w, void* self) {
  auto about_window = new Fl_Window(200, 100, "About");
  auto about_text = new Fl_Box(10, 10, 180, 80, "Journal Abbr\n\nVersion: 1.0\nAuthor: Yachen Mao\nMade for Wuxian");
  about_window->set_modal();
  about_window->show();
}