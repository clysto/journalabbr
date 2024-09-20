from typing import NamedTuple
from thefuzz import fuzz
import re


class PubMedJounal(NamedTuple):
    jr_id: str
    journal_title: str
    med_abbr: str
    issn_print: str
    issn_online: str
    iso_abbr: str
    nlm_id: str


def load_journals():
    journals = []
    with open("J_Medline.txt") as f:
        jounal = dict()
        for line in f:
            if (line.startswith("----") or line == "") and len(jounal) > 0:
                journals.append(PubMedJounal(**jounal))
                continue
            cols = line.split(": ")
            if cols[0] == "JrId":
                jounal["jr_id"] = cols[1].strip()
            elif cols[0] == "JournalTitle":
                jounal["journal_title"] = cols[1].strip()
            elif cols[0] == "MedAbbr":
                jounal["med_abbr"] = cols[1].strip()
            elif cols[0] == "ISSN (Print)":
                jounal["issn_print"] = cols[1].strip()
            elif cols[0] == "ISSN (Online)":
                jounal["issn_online"] = cols[1].strip()
            elif cols[0] == "IsoAbbr":
                jounal["iso_abbr"] = cols[1].strip()
            elif cols[0] == "NlmId":
                jounal["nlm_id"] = cols[1].strip()

    return journals


class PubMedJounalDatabase:
    def __init__(self) -> None:
        self.all_journals: list[PubMedJounal] = load_journals()

    def fetch(self, issn: str):
        for jounal in self.all_journals:
            if jounal.issn_print == issn or jounal.issn_online == issn:
                return jounal
        return None

    def search(self, title: str):
        results = []

        for jounal in self.all_journals:
            journal_title = re.sub(r"\(.*\)", "", jounal.journal_title.lower()).strip()
            r = max(
                fuzz.ratio(journal_title, title.lower()),
                fuzz.ratio(jounal.med_abbr.lower(), title.lower()),
                fuzz.ratio(jounal.iso_abbr.lower(), title.lower()),
            )
            if r == 100:
                return [jounal]
            elif r > 80:
                results.append(jounal)
        return results
