import sqlite3

c = sqlite3.connect('data/quran.db')
c.execute("UPDATE riwayat SET page_url_template = 'https://quranpedia.net/api/page/hafs/{page}' WHERE id = 1")
c.execute("UPDATE riwayat SET page_url_template = 'https://quranpedia.net/api/page/warsh/{page}' WHERE id = 2")
c.commit()

print("Updated riwayat URLs:")
for row in c.execute("SELECT id, code, page_url_template FROM riwayat").fetchall():
    print(row)
