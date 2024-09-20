#pragma once

#include <string>
#include <vector>

struct Journal {
  std::string eissn;
  std::string title;
  std::string jcr_abbr;
  std::string iso_abbr;
  std::string med_abbr;
};

extern std::vector<Journal> db_journals;

void load_all_journals();

std::vector<Journal> search_journal(const std::string& query);

std::string lowercase(const std::string& str);