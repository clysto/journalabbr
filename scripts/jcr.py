import requests

URL_BASE = "https://jcr.clarivate.com/api/jcr3/journalprofile/v1"
URL_ISSN = URL_BASE + "/journal-informationByIssnEssn"
URL_INFO = URL_BASE + "/journal-information"


class JCR:

    def __init__(self) -> None:
        self.session = requests.Session()

    def login(self, sid):
        self.session.headers.update({"x-1p-inc-sid": sid})

    def fetch(self, issn):
        response = self.session.post(URL_ISSN, json={"issnEissn": issn})
        response.raise_for_status()
        info = response.json()
        if "data" not in info:
            raise Exception(info["error"])
        jcr_abbr = info["data"]["abbreviatedTitle"]
        response = self.session.post(URL_INFO, json={"journal": jcr_abbr, "year": 2019})
        response.raise_for_status()
        info = response.json()
        if "data" not in info:
            raise Exception(info["error"])
        return info["data"]


if __name__ == "__main__":
    jcr = JCR()
    jcr.login(
        "H2-69f2f5b0-73c3-11ec-b80a-a98879b20997-d8ee327c-9da8-4c8b-9d13-6e3dfd53f9ed"
    )
    journal_info = jcr.fetch("1468-0491")
    print(journal_info)
