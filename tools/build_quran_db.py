#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Build authentic SQLite Quran database with multiple Riwayat (Hafs & Warsh)
for harbour-thakir (Sailfish OS).
"""

import os
import sys
import json
import sqlite3
import urllib.request
import xml.etree.ElementTree as ET
import bisect
import re

if sys.stdout.encoding != 'utf-8':
    try:
        sys.stdout.reconfigure(encoding='utf-8')
    except Exception:
        pass

DATA_DIR = os.path.join(os.path.dirname(__file__), "..", "data")
FILES_DIR = os.path.join(os.path.dirname(__file__), "..", "files")
DB_PATH = os.path.join(DATA_DIR, "quran.db")

HEADERS = {'User-Agent': 'Mozilla/5.0'}

def fetch_url(url, cache_path=None):
    if cache_path and os.path.exists(cache_path):
        print(f"Loading cached: {cache_path}")
        with open(cache_path, 'rb') as f:
            return f.read()
    print(f"Fetching: {url}")
    req = urllib.request.Request(url, headers=HEADERS)
    with urllib.request.urlopen(req) as resp:
        content = resp.read()
    if cache_path:
        with open(cache_path, 'wb') as f:
            f.write(content)
    return content

def remove_tashkeel(text):
    """Normalize Arabic text for search: remove harakat, tatweel, normalize alefs, etc."""
    # Tashkeel regex
    tashkeel = re.compile(r'[\u0617-\u061A\u064B-\u065F\u0670\u06D6-\u06ED\u08D4-\u08ED\u06DF\u06E0\u06E2\u06E3\u06E5\u06E6\u06EA-\u06EC]')
    text = tashkeel.sub('', text)
    # Tatweel
    text = text.replace('\u0640', '')
    # Normalize Alef forms
    text = re.sub(r'[إأآٱ]', 'ا', text)
    # Normalize Teh Marbuta
    text = text.replace('ة', 'ه')
    # Normalize Yeh
    text = text.replace('ى', 'ي')
    return text.strip()

def main():
    os.makedirs(DATA_DIR, exist_ok=True)
    os.makedirs(FILES_DIR, exist_ok=True)

    # 1. Fetch metadata (Tanzil quran-data.xml)
    metadata_cache = os.path.join(FILES_DIR, "tanzil-quran-data.xml")
    metadata_xml = fetch_url("https://tanzil.net/res/text/metadata/quran-data.xml", metadata_cache)
    tree = ET.fromstring(metadata_xml)

    suras_elem = tree.find('suras')
    pages_elem = tree.find('pages')
    juzs_elem = tree.find('juzs')
    hizbs_elem = tree.find('hizbs') # 240 quarters (60 hizbs * 4)

    # 2. Fetch Hafs text
    hafs_cache = os.path.join(FILES_DIR, "ara-quranuthmanihaf.min.json")
    hafs_json = fetch_url("https://cdn.jsdelivr.net/gh/fawazahmed0/quran-api@1/editions/ara-quranuthmanihaf.min.json", hafs_cache)
    hafs_data = json.loads(hafs_json.decode('utf-8'))['quran']

    # 3. Fetch Warsh text
    warsh_cache = os.path.join(FILES_DIR, "ara-quranwarsh.min.json")
    warsh_json = fetch_url("https://cdn.jsdelivr.net/gh/fawazahmed0/quran-api@1/editions/ara-quranwarsh.min.json", warsh_cache)
    warsh_data = json.loads(warsh_json.decode('utf-8'))['quran']

    # 4. Read clean search text from local files/quran-simple-clean.xml if available
    clean_text_map = {}
    clean_xml_path = os.path.join(FILES_DIR, "quran-simple-clean.xml")
    if os.path.exists(clean_xml_path):
        print(f"Reading clean text from: {clean_xml_path}")
        clean_tree = ET.parse(clean_xml_path)
        for s in clean_tree.findall('sura'):
            s_idx = int(s.attrib['index'])
            for a in s.findall('aya'):
                a_idx = int(a.attrib['index'])
                clean_text_map[(s_idx, a_idx)] = a.attrib['text']

    # 5. Build page / juz / hizb locator lists
    # Each entry is (sura, aya)
    page_points = []
    for p in pages_elem.findall('page'):
        page_points.append((int(p.attrib['sura']), int(p.attrib['aya'])))

    juz_points = []
    for j in juzs_elem.findall('juz'):
        juz_points.append((int(j.attrib['sura']), int(j.attrib['aya'])))

    hizb_quarter_points = []
    for h in hizbs_elem.findall('quarter'):
        hizb_quarter_points.append((int(h.attrib['sura']), int(h.attrib['aya'])))

    def get_page(sura, aya):
        idx = bisect.bisect_right(page_points, (sura, aya))
        return max(1, min(604, idx))

    def get_juz(sura, aya):
        idx = bisect.bisect_right(juz_points, (sura, aya))
        return max(1, min(30, idx))

    def get_hizb_and_rub(sura, aya):
        quarter_idx = bisect.bisect_right(hizb_quarter_points, (sura, aya)) # 1..240
        quarter_idx = max(1, min(240, quarter_idx))
        hizb = ((quarter_idx - 1) // 4) + 1 # 1..60
        rub = ((quarter_idx - 1) % 4) + 1   # 1..4 (1st quarter, half, 3rd quarter, full)
        return hizb, rub

    # Sajdas
    sajda_points = set()
    sajdas_elem = tree.find('sajdas')
    if sajdas_elem is not None:
        for s in sajdas_elem.findall('sajda'):
            sajda_points.add((int(s.attrib['sura']), int(s.attrib['aya'])))

    # 6. Initialize SQLite DB
    if os.path.exists(DB_PATH):
        os.remove(DB_PATH)

    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()

    cur.execute("PRAGMA journal_mode = OFF;")
    cur.execute("PRAGMA synchronous = 0;")

    # Tables
    cur.execute("""
    CREATE TABLE riwayat (
        id INTEGER PRIMARY KEY,
        code TEXT UNIQUE NOT NULL,
        name_ar TEXT NOT NULL,
        name_en TEXT NOT NULL,
        description_ar TEXT,
        total_verses INTEGER NOT NULL,
        page_url_template TEXT NOT NULL,
        reciters_json TEXT NOT NULL
    );
    """)

    cur.execute("""
    CREATE TABLE surahs (
        number INTEGER PRIMARY KEY,
        name_ar TEXT NOT NULL,
        name_en TEXT NOT NULL,
        name_translation TEXT NOT NULL,
        revelation_type TEXT NOT NULL,
        total_verses INTEGER NOT NULL,
        start_page INTEGER NOT NULL
    );
    """)

    cur.execute("""
    CREATE TABLE juzs (
        number INTEGER PRIMARY KEY,
        name_ar TEXT NOT NULL,
        surah_number INTEGER NOT NULL,
        ayah_number INTEGER NOT NULL,
        start_page INTEGER NOT NULL
    );
    """)

    cur.execute("""
    CREATE TABLE hizbs (
        number INTEGER PRIMARY KEY,
        juz_number INTEGER NOT NULL,
        surah_number INTEGER NOT NULL,
        ayah_number INTEGER NOT NULL,
        start_page INTEGER NOT NULL
    );
    """)

    cur.execute("""
    CREATE TABLE ayahs (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        riwayah_id INTEGER NOT NULL,
        surah_number INTEGER NOT NULL,
        ayah_number INTEGER NOT NULL,
        text_uthmani TEXT NOT NULL,
        text_search TEXT NOT NULL,
        page_number INTEGER NOT NULL,
        juz_number INTEGER NOT NULL,
        hizb_number INTEGER NOT NULL,
        rub_number INTEGER NOT NULL,
        sajda INTEGER DEFAULT 0,
        FOREIGN KEY(riwayah_id) REFERENCES riwayat(id),
        FOREIGN KEY(surah_number) REFERENCES surahs(number)
    );
    """)

    # Populate Riwayat
    riwayat_rows = [
        (
            1,
            'hafs',
            'حفص عن عاصم',
            "Hafs 'an 'Asim",
            'الرواية الأكثر انتشاراً في العالم الإسلامي، بخط مصحف المدينة النبوية.',
            6236,
            'https://quranpedia.net/api/page/hafs/{page}',
            json.dumps([
                {
                    "id": "alafasy",
                    "name_ar": "مشاري راشد العفاسي",
                    "name_en": "Mishary Alafasy",
                    "url_template": "https://everyayah.com/data/Alafasy_128kbps/{surah:03d}{ayah:03d}.mp3"
                },
                {
                    "id": "abdulbasit",
                    "name_ar": "عبد الباسط عبد الصمد (مرتل)",
                    "name_en": "Abdul Basit (Murattal)",
                    "url_template": "https://everyayah.com/data/Abdul_Basit_Murattal_192kbps/{surah:03d}{ayah:03d}.mp3"
                },
                {
                    "id": "husary",
                    "name_ar": "محمود خليل الحصري",
                    "name_en": "Mahmoud Khalil Al-Husary",
                    "url_template": "https://everyayah.com/data/Husary_128kbps/{surah:03d}{ayah:03d}.mp3"
                }
            ], ensure_ascii=False)
        ),
        (
            2,
            'warsh',
            'ورش عن نافع',
            "Warsh 'an Nafi'",
            'رواية الإمام ورش عن نافع المدني من طريق الأزرق، الرواية الرسمية في الجزائر والمغرب العربي.',
            6214,
            'https://quranpedia.net/api/page/warsh/{page}',
            json.dumps([
                {
                    "id": "abdulbasit_warsh",
                    "name_ar": "عبد الباسط عبد الصمد (ورش)",
                    "name_en": "Abdul Basit (Warsh)",
                    "url_template": "https://everyayah.com/data/warsh/warsh_Abdul_Basit_128kbps/{surah:03d}{ayah:03d}.mp3"
                },
                {
                    "id": "yassin_al_jazaery",
                    "name_ar": "ياسين الجزائري (ورش)",
                    "name_en": "Yassin Al-Jazairi (Warsh)",
                    "url_template": "https://everyayah.com/data/warsh/warsh_yassin_al_jazaery_64kbps/{surah:03d}{ayah:03d}.mp3"
                }
            ], ensure_ascii=False)
        )
    ]
    cur.executemany("INSERT INTO riwayat VALUES (?, ?, ?, ?, ?, ?, ?, ?)", riwayat_rows)

    # Populate Surahs
    surah_rows = []
    for s in suras_elem.findall('sura'):
        s_num = int(s.attrib['index'])
        s_ar = s.attrib['name']
        s_trans = s.attrib.get('tname', s.attrib.get('ename', ''))
        s_en = s_trans
        s_type = 'Meccan' if s.attrib.get('type') == 'Meccan' else 'Medinan'
        s_verses = int(s.attrib['ayas'])
        start_p = get_page(s_num, 1)
        surah_rows.append((s_num, s_ar, s_en, s_trans, s_type, s_verses, start_p))

    cur.executemany("INSERT INTO surahs VALUES (?, ?, ?, ?, ?, ?, ?)", surah_rows)

    # Populate Juzs (30)
    juz_rows = []
    for i, j in enumerate(juzs_elem.findall('juz'), 1):
        s_num = int(j.attrib['sura'])
        a_num = int(j.attrib['aya'])
        p_num = get_page(s_num, a_num)
        name_ar = f"الجزء {i}"
        juz_rows.append((i, name_ar, s_num, a_num, p_num))
    cur.executemany("INSERT INTO juzs VALUES (?, ?, ?, ?, ?)", juz_rows)

    # Populate Hizbs (60) - take every 4th quarter from the 240 quarters
    hizb_rows = []
    for i in range(60):
        q = hizb_quarter_points[i * 4]
        s_num, a_num = q
        p_num = get_page(s_num, a_num)
        hizb_num = i + 1
        juz_num = (i // 2) + 1
        hizb_rows.append((hizb_num, juz_num, s_num, a_num, p_num))
    cur.executemany("INSERT INTO hizbs VALUES (?, ?, ?, ?, ?)", hizb_rows)

    # Helper function to insert ayahs for a riwayah
    def insert_riwayah_ayahs(riwayah_id, dataset):
        print(f"Inserting ayahs for riwayah_id={riwayah_id} ({len(dataset)} ayahs)...")
        rows = []
        for item in dataset:
            s_num = int(item['chapter'])
            a_num = int(item['verse'])
            text_u = item['text'].strip()

            # Search text: use Tanzil clean text if available, otherwise normalize
            if (s_num, a_num) in clean_text_map:
                text_s = remove_tashkeel(clean_text_map[(s_num, a_num)])
            else:
                text_s = remove_tashkeel(text_u)

            page = get_page(s_num, a_num)
            juz = get_juz(s_num, a_num)
            hizb, rub = get_hizb_and_rub(s_num, a_num)
            sajda = 1 if (s_num, a_num) in sajda_points else 0

            rows.append((
                riwayah_id,
                s_num,
                a_num,
                text_u,
                text_s,
                page,
                juz,
                hizb,
                rub,
                sajda
            ))

        cur.executemany("""
        INSERT INTO ayahs (
            riwayah_id, surah_number, ayah_number,
            text_uthmani, text_search,
            page_number, juz_number, hizb_number, rub_number,
            sajda
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        """, rows)

    # Insert Hafs (riwayah_id = 1)
    insert_riwayah_ayahs(1, hafs_data)

    # Insert Warsh (riwayah_id = 2)
    insert_riwayah_ayahs(2, warsh_data)

    # 7. Create Indexes
    print("Creating database indexes...")
    cur.execute("CREATE INDEX idx_ayahs_lookup ON ayahs(riwayah_id, surah_number, ayah_number);")
    cur.execute("CREATE INDEX idx_ayahs_page ON ayahs(riwayah_id, page_number);")
    cur.execute("CREATE INDEX idx_ayahs_search ON ayahs(riwayah_id, text_search);")
    cur.execute("CREATE INDEX idx_ayahs_juz ON ayahs(riwayah_id, juz_number);")
    cur.execute("CREATE INDEX idx_ayahs_hizb ON ayahs(riwayah_id, hizb_number);")

    conn.commit()

    # 8. Verification & Stats
    cur.execute("SELECT COUNT(*) FROM ayahs WHERE riwayah_id = 1")
    count_hafs = cur.fetchone()[0]
    cur.execute("SELECT COUNT(*) FROM ayahs WHERE riwayah_id = 2")
    count_warsh = cur.fetchone()[0]

    cur.execute("SELECT text_uthmani FROM ayahs WHERE riwayah_id = 1 AND surah_number = 1 AND ayah_number = 1")
    sample_hafs = cur.fetchone()[0]
    cur.execute("SELECT text_uthmani FROM ayahs WHERE riwayah_id = 2 AND surah_number = 1 AND ayah_number = 1")
    sample_warsh = cur.fetchone()[0]

    db_size = os.path.getsize(DB_PATH)
    conn.close()

    print("\n" + "=" * 50)
    print("SUCCESS: Quran database created successfully!")
    print(f"Path: {DB_PATH}")
    print(f"Size: {db_size / (1024 * 1024):.2f} MB")
    print(f"Hafs Ayahs count:  {count_hafs}")
    print(f"Warsh Ayahs count: {count_warsh}")
    print(f"Hafs 1:1:  {sample_hafs}")
    print(f"Warsh 1:1: {sample_warsh}")
    print("=" * 50)

if __name__ == '__main__':
    main()
