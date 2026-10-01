import sqlite3

conn = sqlite3.connect('data/quran.db')

# Add start_ms and end_ms columns if they don't exist
try:
    conn.execute("ALTER TABLE warsh_to_hafs_map ADD COLUMN start_ms INTEGER DEFAULT 0")
    conn.execute("ALTER TABLE warsh_to_hafs_map ADD COLUMN end_ms INTEGER DEFAULT 0")
except Exception as e:
    print("Columns may already exist:", e)

# Surah 5: Ayah 1 ends at 4800ms, Ayah 2 starts at 4800ms
conn.execute("UPDATE warsh_to_hafs_map SET start_ms=0, end_ms=4800 WHERE surah_number=5 AND warsh_ayah=1")
conn.execute("UPDATE warsh_to_hafs_map SET start_ms=4800, end_ms=0 WHERE surah_number=5 AND warsh_ayah=2")

# Surah 5: Ayah 16 ends at 17500ms, Ayah 17 starts at 17500ms
conn.execute("UPDATE warsh_to_hafs_map SET start_ms=0, end_ms=17500 WHERE surah_number=5 AND warsh_ayah=16")
conn.execute("UPDATE warsh_to_hafs_map SET start_ms=17500, end_ms=0 WHERE surah_number=5 AND warsh_ayah=17")

# Surah 1: Ayah 6 ends at 4000ms, Ayah 7 starts at 4000ms
conn.execute("UPDATE warsh_to_hafs_map SET start_ms=0, end_ms=4000 WHERE surah_number=1 AND warsh_ayah=6")
conn.execute("UPDATE warsh_to_hafs_map SET start_ms=4000, end_ms=0 WHERE surah_number=1 AND warsh_ayah=7")

conn.commit()

print("Verification:")
print("Surah 5 Ayahs 1-3:", conn.execute("SELECT * FROM warsh_to_hafs_map WHERE surah_number=5 AND warsh_ayah<=3").fetchall())
print("Surah 5 Ayahs 16-17:", conn.execute("SELECT * FROM warsh_to_hafs_map WHERE surah_number=5 AND warsh_ayah IN (16, 17)").fetchall())
print("Surah 1 Ayahs 6-7:", conn.execute("SELECT * FROM warsh_to_hafs_map WHERE surah_number=1 AND warsh_ayah>=6").fetchall())

conn.close()
