import sqlite3, re, io, sys

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
conn = sqlite3.connect('data/quran.db')

def norm(t):
    t = re.sub(r'[\u0610-\u061A\u064B-\u065F\u0670\u06D6-\u06ED\u08D4-\u08E1\u08E3-\u08FF]', '', t)
    t = re.sub(r'[\u06D6-\u06DC\u06DF-\u06E4\u06E7\u06E8\u06EA-\u06ED۞۩ۣۚۖۗۘۜ۟۠ۢۥۦ۪ۭۧۨ۫۬]', '', t)
    t = re.sub(r'[إأآاٱء]', 'ا', t)
    t = re.sub(r'[يىئ]', 'ي', t)
    t = re.sub(r'[ة]', 'ه', t)
    return re.sub(r'\s+', '', t)

# Recreate warsh_to_hafs_map with hafs_ayah_start and hafs_ayah_end
conn.execute("DROP TABLE IF EXISTS warsh_to_hafs_map")
conn.execute("""
CREATE TABLE warsh_to_hafs_map (
    surah_number INTEGER NOT NULL,
    warsh_ayah INTEGER NOT NULL,
    hafs_ayah_start INTEGER NOT NULL,
    hafs_ayah_end INTEGER NOT NULL,
    PRIMARY KEY (surah_number, warsh_ayah)
)
""")

# The 19 opening muqatta'at / phrase chapters where Warsh 1 = Hafs 1 + 2
OPENING_MULTI_SURAHS = {
    1,  # Fatiha (Basmalah + Alhamd)
    2,  # Baqarah (الم + ذلك الكتاب...)
    3,  # Ali 'Imran (الم + الله لا إله...)
    20, # Ta-Ha (طه + ما أنزلنا...)
    26, # Ash-Shu'ara (طسم + تلك آيات...)
    28, # Al-Qasas (طسم + تلك آيات...)
    31, # Luqman (الم + تلك آيات...)
    32, # As-Sajdah (الم + تنزيل...)
    40, # Ghafir (حم + تنزيل...)
    41, # Fussilat (حم + تنزيل...)
    42, # Ash-Shura (حم + عسق)
    43, # Az-Zukhruf (حم + والكتاب...)
    44, # Ad-Dukhan (حم + والكتاب...)
    45, # Al-Jathiyah (حم + تنزيل...)
    46, # Al-Ahqaf (حم + تنزيل...)
    69, # Al-Haqqah (الحاقة + ما الحاقة)
    73, # Al-Muzzammil (المزمل + قم الليل...)
    101,# Al-Qari'ah (القارعة + ما القارعة)
    103 # Al-'Asr (والعصر + إن الإنسان...)
}

rows_to_insert = []

for s in range(1, 115):
    h_rows = conn.execute("SELECT ayah_number, text_uthmani FROM ayahs WHERE riwayah_id=1 AND surah_number=? ORDER BY ayah_number", (s,)).fetchall()
    w_rows = conn.execute("SELECT ayah_number, text_uthmani FROM ayahs WHERE riwayah_id=2 AND surah_number=? ORDER BY ayah_number", (s,)).fetchall()
    
    total_w = len(w_rows)
    total_h = len(h_rows)
    
    # Surah 1: Al-Fatiha
    if s == 1:
        rows_to_insert.append((1, 1, 1, 2)) # Basmalah + Al-Hamd
        rows_to_insert.append((1, 2, 3, 3)) # Ar-Rahman
        rows_to_insert.append((1, 3, 4, 4)) # Malik
        rows_to_insert.append((1, 4, 5, 5)) # Iyyaka
        rows_to_insert.append((1, 5, 6, 6)) # Ihdina
        rows_to_insert.append((1, 6, 7, 7)) # Sirata
        rows_to_insert.append((1, 7, 7, 7)) # Ghayr
        continue

    # Build character-to-ayah map for Hafs text stream
    h_stream = ""
    h_char_map = []
    for ha, ht in h_rows:
        n = norm(ht)
        for c in n:
            h_stream += c
            h_char_map.append(ha)
            
    # For each Warsh ayah, find matching range in Hafs text stream
    h_pos = 0
    for wa, wt in w_rows:
        # Check special case: opening multi-ayah
        if wa == 1 and s in OPENING_MULTI_SURAHS:
            h_start = 1
            h_end = 2
            wn = norm(wt)
            h_pos = len(norm(h_rows[0][1])) + len(norm(h_rows[1][1]))
            rows_to_insert.append((s, wa, h_start, h_end))
            continue
            
        wn = norm(wt)
        chunk_len = min(len(wn), 15)
        match_start = -1
        for offset in range(0, 50):
            if h_pos + offset + chunk_len <= len(h_stream):
                if h_stream[h_pos + offset : h_pos + offset + chunk_len] == wn[:chunk_len]:
                    match_start = h_pos + offset
                    break
        if match_start == -1:
            pos = h_stream.find(wn[:chunk_len], max(0, h_pos - 10))
            match_start = pos if pos != -1 else h_pos

        match_end = min(len(h_stream), match_start + len(wn))
        h_pos = match_end
        
        covered = sorted(list(set(h_char_map[match_start:match_end]))) if match_end > match_start else [min(wa, total_h)]
        if not covered:
            covered = [min(wa, total_h)]
            
        h_start = covered[0]
        h_end = covered[-1]
        rows_to_insert.append((s, wa, h_start, h_end))

conn.executemany("INSERT INTO warsh_to_hafs_map VALUES (?, ?, ?, ?)", rows_to_insert)
conn.commit()

count = conn.execute("SELECT COUNT(*) FROM warsh_to_hafs_map").fetchone()[0]
print(f"Successfully inserted {count} rows into warsh_to_hafs_map.")

# Verification
print("\nSample verifications:")
print("Surah 1 (Fatiha):", conn.execute("SELECT * FROM warsh_to_hafs_map WHERE surah_number=1").fetchall())
print("Surah 2 (Baqarah 1-5):", conn.execute("SELECT * FROM warsh_to_hafs_map WHERE surah_number=2 AND warsh_ayah<=5").fetchall())
print("Surah 3 (Ali Imran 1-5):", conn.execute("SELECT * FROM warsh_to_hafs_map WHERE surah_number=3 AND warsh_ayah<=5").fetchall())
print("Surah 5 (Maidah 1-5):", conn.execute("SELECT * FROM warsh_to_hafs_map WHERE surah_number=5 AND warsh_ayah<=5").fetchall())
print("Surah 20 (Ta-Ha 1-3):", conn.execute("SELECT * FROM warsh_to_hafs_map WHERE surah_number=20 AND warsh_ayah<=3").fetchall())
print("Surah 103 (Asr):", conn.execute("SELECT * FROM warsh_to_hafs_map WHERE surah_number=103").fetchall())

conn.close()
