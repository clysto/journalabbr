#include "journal.hpp"

#include <fstream>
#include <rapidfuzz/fuzz.hpp>
#include <sstream>

#include "data.hpp"

std::vector<Journal> db_journals;

std::string lowercase(const std::string& str) {
  std::string result = str;
  for (char& c : result) {
    c = std::tolower(c);
  }
  return result;
}

void load_all_journals() {
  std::istringstream file(std::string((char*)journals_csv, journals_csv_len));
  std::string line;
  std::getline(file, line);
  while (std::getline(file, line)) {
    std::string eissn;
    std::string title;
    std::string jcr_abbr;
    std::string iso_abbr;
    std::string med_abbr;
    std::istringstream iss(line);
    std::getline(iss, eissn, ',');
    std::getline(iss, title, ',');
    std::getline(iss, jcr_abbr, ',');
    std::getline(iss, iso_abbr, ',');
    std::getline(iss, med_abbr, '\n');
    Journal journal = {eissn, title, jcr_abbr, iso_abbr, med_abbr};
    db_journals.push_back(journal);
  }
}

std::vector<Journal> search_journal(const std::string& query) {
  auto query_ = lowercase(query);
  std::vector<Journal> results;
  std::vector<double> scores;
  double title_score = 0;
  double jcr_abbr_score = 0;
  double iso_abbr_score = 0;
  double med_abbr_score = 0;
  double score = 0;
  for (const auto& journal : db_journals) {
    title_score = rapidfuzz::fuzz::ratio(lowercase(journal.title), query_);
    jcr_abbr_score = rapidfuzz::fuzz::ratio(lowercase(journal.jcr_abbr), query_);
    iso_abbr_score = rapidfuzz::fuzz::ratio(lowercase(journal.iso_abbr), query_);
    med_abbr_score = rapidfuzz::fuzz::ratio(lowercase(journal.med_abbr), query_);
    score = std::max({title_score, jcr_abbr_score, iso_abbr_score, med_abbr_score});
    if (score > 70) {
      results.push_back(journal);
      scores.push_back(score);
    }
  }
  for (size_t i = 0; i < results.size(); i++) {
    for (size_t j = i + 1; j < results.size(); j++) {
      if (scores[i] < scores[j]) {
        std::swap(results[i], results[j]);
        std::swap(scores[i], scores[j]);
      }
    }
  }
  return results;
}
