import urllib.request

for fname in ["001001.mp3", "001002.mp3", "002001.mp3", "002002.mp3", "002003.mp3"]:
    url = f"https://everyayah.com/data/warsh/warsh_yassin_al_jazaery_64kbps/{fname}"
    urllib.request.urlretrieve(url, fname)
    print(f"Downloaded {fname}")
