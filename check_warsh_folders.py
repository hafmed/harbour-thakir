import urllib.request, re

req = urllib.request.Request('https://everyayah.com/data/warsh/', headers={'User-Agent': 'Mozilla/5.0'})
try:
    html = urllib.request.urlopen(req, timeout=10).read().decode('utf-8', errors='ignore')
    folders = re.findall(r'href="([^"]*/)"', html)
    print("Warsh folders on EveryAyah:", folders)
except Exception as e:
    print("Error:", e)
