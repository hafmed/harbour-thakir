import sqlite3, io, sys

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
conn = sqlite3.connect('data/quran.db')

for a in range(1, 6):
    h = conn.execute("SELECT text_uthmani FROM ayahs WHERE riwayah_id=1 AND surah_number=5 AND ayah_number=?", (a,)).fetchone()
    w = conn.execute("SELECT text_uthmani FROM ayahs WHERE riwayah_id=2 AND surah_number=5 AND ayah_number=?", (a,)).fetchone()
    print(f"Hafs  {a}: {h[0] if h else 'None'}\n")
    print(f"Warsh {a}: {w[0] if w else 'None'}\n")
    print("-" * 50)
