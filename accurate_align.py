import sqlite3, re, io, sys

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
conn = sqlite3.connect('data/quran.db')

def norm(t):
    # strip all diacritics and normalize
    t = re.sub(r'[\u0610-\u061A\u064B-\u065F\u0670\u06D6-\u06ED\u08D4-\u08E1\u08E3-\u08FF]', '', t)
    t = re.sub(r'[\u06D6-\u06DC\u06DF-\u06E4\u06E7\u06E8\u06EA-\u06ED۞۩ۣۚۖۗۘۜ۟۠ۢۥۦ۪ۭۧۨ۫۬]', '', t)
    t = re.sub(r'[إأآاٱء]', 'ا', t)
    t = re.sub(r'[يىئ]', 'ي', t)
    t = re.sub(r'[ة]', 'ه', t)
    return re.sub(r'\s+', '', t)

# For every surah, map each Warsh ayah to the Hafs ayah(s) it contains
multi_mappings = []
single_mappings = []

for s in range(1, 115):
    h_rows = conn.execute("SELECT ayah_number, text_uthmani FROM ayahs WHERE riwayah_id=1 AND surah_number=? ORDER BY ayah_number", (s,)).fetchall()
    w_rows = conn.execute("SELECT ayah_number, text_uthmani FROM ayahs WHERE riwayah_id=2 AND surah_number=? ORDER BY ayah_number", (s,)).fetchall()
    
    if s == 1:
        # Fatiha
        multi_mappings.append((1, 1, [1, 2]))
        for w in range(2, 6):
            single_mappings.append((1, w, [w + 1]))
        single_mappings.append((1, 6, [7]))
        single_mappings.append((1, 7, [7]))
        continue
        
    # Build text stream of Hafs with character index to ayah mapping
    h_stream = ""
    h_char_map = [] # index in h_stream -> hafs_ayah
    for ha, ht in h_rows:
        n = norm(ht)
        for c in n:
            h_stream += c
            h_char_map.append(ha)
            
    # For each Warsh ayah, find where it appears in h_stream
    h_pos = 0
    for wa, wt in w_rows:
        wn = norm(wt)
        # Search for wn in h_stream starting near h_pos
        # Because of small word variants (e.g. malik vs maalik), search for chunks
        match_start = -1
        # Try finding prefix
        chunk_len = min(len(wn), 15)
        for offset in range(0, 50):
            if h_pos + offset + chunk_len <= len(h_stream):
                if h_stream[h_pos + offset : h_pos + offset + chunk_len] == wn[:chunk_len]:
                    match_start = h_pos + offset
                    break
        if match_start == -1:
            # fallback: search in whole remainder
            pos = h_stream.find(wn[:chunk_len], max(0, h_pos - 10))
            if pos != -1:
                match_start = pos
            else:
                match_start = h_pos

        match_end = min(len(h_stream), match_start + len(wn))
        h_pos = match_end
        
        # Covered hafs ayahs:
        covered = sorted(list(set(h_char_map[match_start:match_end]))) if match_end > match_start else []
        if len(covered) > 1:
            multi_mappings.append((s, wa, covered))
        elif len(covered) == 1:
            single_mappings.append((s, wa, covered))
        else:
            single_mappings.append((s, wa, [wa]))

print(f"Total multi-part Warsh ayahs found: {len(multi_mappings)}")
for s, w, h in multi_mappings:
    s_name = conn.execute("SELECT name_ar FROM surahs WHERE number=?", (s,)).fetchone()[0]
    print(f"Surah {s:3d} ({s_name}) Warsh {w:3d} -> Hafs {h}")
