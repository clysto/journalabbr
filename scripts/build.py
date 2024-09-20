import csv
from concurrent.futures import ThreadPoolExecutor, as_completed

from rich.progress import track

from jcr import JCR
from medline import PubMedJounalDatabase

f = open("journals.csv", "w")
writer = csv.writer(f)
writer.writerow(["EISSN", "Title", "JCR Abbr", "ISO Abbr", "Med Abbr"])

db = PubMedJounalDatabase()


def fetch_journal_info(journal):
    jcr = JCR()
    jcr.login(
        "H2-69f2f5b0-73c3-11ec-b80a-a98879b20997-d8ee327c-9da8-4c8b-9d13-6e3dfd53f9ed"
    )
    if journal.issn_online == "":
        return None
    try:
        info = jcr.fetch(journal.issn_online)
        return [
            info["eissn"],
            info["title"],
            info["jcrAbbreviation"],
            info["isoAbbreviation"],
            journal.med_abbr,
        ]
    except Exception as e:
        print(f"Failed to fetch {journal.issn_online}: {e}")
        return None


with ThreadPoolExecutor(max_workers=50) as executor:
    futures = {
        executor.submit(fetch_journal_info, journal): journal
        for journal in db.all_journals
    }

    for future in track(as_completed(futures), total=len(futures)):
        result = future.result()
        if result:
            writer.writerow(result)

f.close()
