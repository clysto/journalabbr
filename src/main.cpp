#include <FL/Fl.H>

#include "app.hpp"
#include "journal.hpp"

int main(int argc, char **argv) {
  load_all_journals();
  auto app = App();
  app.show();
  Fl::run();
}