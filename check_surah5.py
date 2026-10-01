import sqlite3, io, sys

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
conn = sqlite3.connect('data/quran.db')

print('=== HAFS SURAH 5 ===')
for a, t in conn.execute('SELECT ayah_number, text_uthmani FROM ayahs WHERE riwayah_id=1 AND surah_number=5 AND ayah_number <= 5').fetchall():
    print(f"Hafs {a}: {t[:60]}...")

print('\n=== WARSH SURAH 5 ===')
for a, t in conn.execute('SELECT ayah_number, text_uthmani FROM ayahs WHERE riwayah_id=2 AND surah_number=5 AND ayah_number <= 5').fetchall():
    print(f"Warsh {a}: {t[:60]}...")

print('\n=== CURRENT MAP SURAH 5 ===')
for w, h in conn.execute('SELECT warsh_ayah, hafs_ayah FROM warsh_to_hafs_map WHERE surah_number=5 AND warsh_ayah <= 5').fetchall():
    print(f"Warsh {w} -> Hafs {h}")
