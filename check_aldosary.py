import urllib.request

for rec in ['warsh_ibrahim_aldosary_128kbps']:
    for s, a in [(5, 120), (5, 121), (5, 122), (2, 285), (2, 286), (18, 105), (18, 110)]:
        url = f"https://everyayah.com/data/warsh/{rec}/{s:03d}{a:03d}.mp3"
        try:
            req = urllib.request.Request(url, method='HEAD')
            res = urllib.request.urlopen(req)
            print(f"{rec} {s:03d}{a:03d}.mp3: status {res.status}")
        except Exception as e:
            print(f"{rec} {s:03d}{a:03d}.mp3: {e}")
