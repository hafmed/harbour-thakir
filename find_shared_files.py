import sqlite3, io, sys

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
conn = sqlite3.connect('data/quran.db')

rows = conn.execute("""
    SELECT surah_number, hafs_ayah_start, COUNT(*) as c, GROUP_CONCAT(warsh_ayah) 
    FROM warsh_to_hafs_map 
    WHERE hafs_ayah_start = hafs_ayah_end
    GROUP BY surah_number, hafs_ayah_start 
    HAVING c > 1
""").fetchall()

print(f"Total Hafs files shared by multiple Warsh verses: {len(rows)}")
for s, h, c, w_list in rows:
    s_name = conn.execute("SELECT name_ar FROM surahs WHERE number=?", (s,)).fetchone()[0]
    print(f"Surah {s:3d} ({s_name:12s}): Hafs File {h:3d} shared by Warsh ayahs: {w_list}")
