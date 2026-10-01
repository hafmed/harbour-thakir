import sqlite3, io, sys

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
conn = sqlite3.connect('data/quran.db')

for s in range(1, 115):
    h1 = conn.execute("SELECT text_uthmani FROM ayahs WHERE riwayah_id=1 AND surah_number=? AND ayah_number=1", (s,)).fetchone()
    h2 = conn.execute("SELECT text_uthmani FROM ayahs WHERE riwayah_id=1 AND surah_number=? AND ayah_number=2", (s,)).fetchone()
    w1 = conn.execute("SELECT text_uthmani FROM ayahs WHERE riwayah_id=2 AND surah_number=? AND ayah_number=1", (s,)).fetchone()
    
    if not h1 or not w1:
        continue
        
    h1_txt = h1[0].strip()
    w1_txt = w1[0].strip()
    
    # Check if w1 is longer than h1 and contains parts of h2
    if h2:
        h2_txt = h2[0].strip()
        # If w1 starts with h1 and contains h2
        if len(w1_txt) > len(h1_txt) + 3:
            s_name = conn.execute("SELECT name_ar FROM surahs WHERE number=?", (s,)).fetchone()[0]
            print(f"Surah {s:3d} ({s_name}):")
            print(f"  Warsh 1: {w1_txt}")
            print(f"  Hafs  1: {h1_txt}")
            print(f"  Hafs  2: {h2_txt}")
            print()
