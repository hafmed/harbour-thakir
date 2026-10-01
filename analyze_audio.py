import soundfile as sf
import numpy as np

data, samplerate = sf.read('005001.mp3')
print(f"Sample rate: {samplerate}, Channels: {data.ndim}, Total duration: {len(data)/samplerate:.2f}s")

# Let's find pauses (silence) around 5s - 12s where "أوفوا بالعقود" finishes
window = int(samplerate * 0.1) # 100ms
energy = [np.mean(np.abs(data[i:i+window])) for i in range(0, len(data)-window, window)]
times = [i * 0.1 for i in range(len(energy))]

for t, e in zip(times, energy):
    if 4.0 <= t <= 15.0:
        bar = '#' * int(e * 200)
        print(f"{t:5.1f}s: {e:7.4f} | {bar}")
