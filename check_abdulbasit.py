import urllib.request

for fname in ["005001.mp3", "005002.mp3", "002001.mp3", "002002.mp3"]:
    url = f"https://everyayah.com/data/warsh/warsh_Abdul_Basit_128kbps/{fname}"
    try:
        req = urllib.request.Request(url, method='HEAD')
        res = urllib.request.urlopen(req)
        print(f"AbdulBasit {fname}: status {res.status}, size {res.headers.get('Content-Length')}")
    except Exception as e:
        print(f"AbdulBasit {fname}: {e}")
