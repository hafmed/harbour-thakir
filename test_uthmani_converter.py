import sqlite3, re

conn = sqlite3.connect('data/quran.db')
cursor = conn.cursor()

def warsh_uthmani_to_search_clean(tu):
    s = tu
    
    # Warsh specific decorative/recitation marks
    # U+06EC: Round filled centre (naql point / hamzat wasl)
    # U+06EA, U+06EB, U+06ED: empty center stops
    s = re.sub(r'[\u06EA-\u06ED]', '', s)
    
    # Quranic yeh barree \u06D2, \u06D3 -> \u064A (ي)
    s = s.replace('\u06D2', 'ي')
    s = s.replace('\u06D3', 'ي')
    
    # Tatweel
    s = s.replace('\u0640', '')
    
    # In words like الصَّلَوٰة, الزَّكَوٰة, الحَيَوٰة (waw with dagger alef),
    # in Arabic search it is الصلاة, الزكاة, الحياة.
    # So 'وٰ' -> 'ا'
    s = s.replace('و\u0670', 'ا')
    s = s.replace('ى\u0670', 'ا')
    s = s.replace('ي\u0670', 'ا')
    
    # In الرحمن, dagger alef is NOT written as Alef in standard Arabic spelling
    # Protect الرحمن:
    s = s.replace('رَّحْمَٰن', 'رحمن')
    s = s.replace('رَّحۡمَٰن', 'رحمن')
    s = s.replace('رحمَٰن', 'رحمن')
    s = s.replace('هَٰذَا', 'هذا')
    s = s.replace('هَٰذِهِ', 'هذه')
    s = s.replace('هَٰؤُلَآءِ', 'هؤلاء')
    s = s.replace('ذَٰلِكَ', 'ذلك')
    s = s.replace('لَٰكِن', 'لكن')
    s = s.replace('إِلَٰه', 'إله')
    s = s.replace('إلَٰه', 'إله')
    
    # All other dagger alefs \u0670 -> standard Alef \u0627
    s = s.replace('\u0670', 'ا')
    
    # Alefs with hamza / wasla / madda
    s = re.sub(r'[إأآٱ]', 'ا', s)
    s = s.replace('ءا', 'ا')
    s = s.replace('ء', 'ا')
    
    # Taa marbuta -> ha, Alif maqsura -> ya
    s = s.replace('ة', 'ه')
    s = s.replace('ى', 'ي')
    
    # Warsh hamza ibdal for standard searchability:
    # مومن -> مؤمن, يومن -> يؤمن, تومن -> تؤمن, نومن -> نؤمن
    # (or both forms will match via regex)
    
    # Remove remaining diacritics / harakat / stops
    s = re.sub(r'[\u064B-\u065F\u06D6-\u06DC\u06DF-\u06E8]', '', s)
    
    # Standardize whitespace
    s = re.sub(r'\s+', ' ', s)
    return s.strip()

# Test sample verses in Warsh
cursor.execute("SELECT surah_number, ayah_number, text_uthmani FROM ayahs WHERE riwayah_id = 2 AND surah_number IN (1, 36) AND ayah_number IN (1, 5, 57, 58)")
for r in cursor.fetchall():
    cleaned = warsh_uthmani_to_search_clean(r[2])
    print(f"{r[0]}:{r[1]} -> {cleaned.encode('ascii', 'backslashreplace').decode()}")
