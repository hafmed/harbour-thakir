import urllib.request, re, json

headers = {'User-Agent': 'Mozilla/5.0'}
req = urllib.request.Request('https://quranpedia.net/ar', headers=headers)
html = urllib.request.urlopen(req, timeout=10).read().decode('utf-8', errors='ignore')

# Search for any references to reciters, audio, mp3, everyayah, etc.
matches = re.findall(r'https?://[^\s"\'<>]+\.mp3', html)
print("MP3s:", matches[:5])

audio_routes = re.findall(r'href="([^"]*(?:audio|recit|warsh|qari|voice)[^"]*)"', html, re.I)
print("Routes:", audio_routes[:10])

# Also search for Yassin
yassin = [line.strip() for line in html.split('\n') if 'ياسين' in line or 'الجزائري' in line]
print("Yassin mentions:", len(yassin))
