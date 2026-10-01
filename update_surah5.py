import sqlite3

conn = sqlite3.connect('data/quran.db')

# Warsh 1 & 2
conn.execute('UPDATE warsh_to_hafs_map SET hafs_ayah_start=1, hafs_ayah_end=1, start_ms=0, end_ms=4800 WHERE surah_number=5 AND warsh_ayah=1')
conn.execute('UPDATE warsh_to_hafs_map SET hafs_ayah_start=1, hafs_ayah_end=1, start_ms=4800, end_ms=0 WHERE surah_number=5 AND warsh_ayah=2')

# Warsh 3 to 15
for w in range(3, 16):
    h = w - 1
    conn.execute('UPDATE warsh_to_hafs_map SET hafs_ayah_start=?, hafs_ayah_end=?, start_ms=0, end_ms=0 WHERE surah_number=5 AND warsh_ayah=?', (h, h, w))

# Warsh 16 & 17
conn.execute('UPDATE warsh_to_hafs_map SET hafs_ayah_start=15, hafs_ayah_end=15, start_ms=0, end_ms=17500 WHERE surah_number=5 AND warsh_ayah=16')
conn.execute('UPDATE warsh_to_hafs_map SET hafs_ayah_start=15, hafs_ayah_end=15, start_ms=17500, end_ms=0 WHERE surah_number=5 AND warsh_ayah=17')

# Warsh 18 to 122
for w in range(18, 123):
    h = w - 2
    conn.execute('UPDATE warsh_to_hafs_map SET hafs_ayah_start=?, hafs_ayah_end=?, start_ms=0, end_ms=0 WHERE surah_number=5 AND warsh_ayah=?', (h, h, w))

conn.commit()
conn.close()
