import sqlite3, io, sys

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
conn = sqlite3.connect('data/quran.db')

def get_tafsir(surah, ayah, riwayah_id, edition_id):
    if riwayah_id == 2:
        # Warsh: map to Hafs range
        row = conn.execute("SELECT hafs_ayah_start, hafs_ayah_end FROM warsh_to_hafs_map WHERE surah_number=? AND warsh_ayah=?", (surah, ayah)).fetchone()
        if row:
            h_start, h_end = row
        else:
            h_start, h_end = ayah, ayah
    else:
        h_start, h_end = ayah, ayah
        
    texts = []
    for h in range(h_start, h_end + 1):
        t_row = conn.execute("SELECT text FROM tafsir_translations WHERE edition_id=? AND surah_number=? AND ayah_number=?", (edition_id, surah, h)).fetchone()
        if t_row and t_row[0]:
            texts.append(t_row[0])
            
    return "\n\n".join(texts)

print("--- HAFS 1:1 Tafsir ---")
print(get_tafsir(1, 1, 1, 'ar.muyassar')[:200])

print("\n--- WARSH 2:1 (الم ذلك الكتاب) Tafsir ---")
print(get_tafsir(2, 1, 2, 'ar.muyassar')[:200])

print("\n--- WARSH 2:1 English ---")
print(get_tafsir(2, 1, 2, 'en.sahih'))

print("\n--- WARSH 5:1 (المائدة) French ---")
print(get_tafsir(5, 1, 2, 'fr.hamidullah'))

print("\n--- WARSH 5:2 (المائدة) French ---")
print(get_tafsir(5, 2, 2, 'fr.hamidullah'))

conn.close()
