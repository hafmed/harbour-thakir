import urllib.request, re

headers = {'User-Agent': 'Mozilla/5.0'}
req = urllib.request.Request('https://quranpedia.net/reciters', headers=headers)
html = urllib.request.urlopen(req, timeout=10).read().decode('utf-8', errors='ignore')

reciters = re.findall(r'href="([^"]*/reciters/[^"]*)"', html)
print("Reciters count:", len(reciters))
print("Sample reciter links:", reciters[:10])

for r in reciters:
    if 'yass' in r.lower() or 'jaza' in r.lower() or 'warsh' in r.lower() or 'basit' in r.lower():
        print("Matching link:", r)
