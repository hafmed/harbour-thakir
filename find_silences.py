import soundfile as sf
import numpy as np

data, samplerate = sf.read('005001.mp3')
window = int(samplerate * 0.05) # 50ms
energy = [np.mean(np.abs(data[i:i+window])) for i in range(0, len(data)-window, window)]

# find intervals where energy < 0.015 for at least 0.4 seconds
threshold = 0.015
min_silence_len = int(0.4 / 0.05)

in_silence = False
silence_start = 0
silences = []

for idx, e in enumerate(energy):
    t = idx * 0.05
    if e < threshold:
        if not in_silence:
            in_silence = True
            silence_start = t
    else:
        if in_silence:
            in_silence = False
            duration = t - silence_start
            if duration >= 0.3:
                silences.append((silence_start, t, duration))

print("Silences in 005001.mp3:")
for s, e, d in silences:
    print(f"From {s:5.2f}s to {e:5.2f}s (duration {d:.2f}s)")
