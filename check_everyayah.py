import urllib.request

for surah in [1, 2, 3, 5]:
    print(f"--- Surah {surah} ---")
    for ayah in range(1, 6):
        url = f"https://everyayah.com/data/warsh/warsh_yassin_al_jazaery_64kbps/{surah:03d}{ayah:03d}.mp3"
        try:
            req = urllib.request.Request(url, method='HEAD')
            res = urllib.request.urlopen(req, timeout=5)
            print(f"{surah:03d}{ayah:03d}.mp3: status {res.status}, size {res.headers.get('Content-Length')}")
        except Exception as e:
            print(f"{surah:03d}{ayah:03d}.mp3: {e}")
