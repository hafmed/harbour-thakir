import sqlite3

conn = sqlite3.connect('data/quran.db')
rows = conn.execute("SELECT surah_number, warsh_ayah, hafs_ayah_start, hafs_ayah_end FROM warsh_to_hafs_map ORDER BY surah_number, warsh_ayah").fetchall()

prev_s = 0
prev_start = 0
errors = []

for s, w, start, end in rows:
    if s != prev_s:
        prev_s = s
        prev_start = 0
        
    if start < prev_start:
        errors.append((s, w, start, end, prev_start))
        
    prev_start = start

print("Total sequence errors:", len(errors))
if errors:
    for e in errors[:10]:
        print("Sequence error:", e)
