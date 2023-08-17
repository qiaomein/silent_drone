import numpy as np
from scipy.io.wavfile import read
from scipy.fft import fft
import matplotlib.pyplot as plt
import librosa
import os
import pandas as pd

path = "E"
a,b = [50,2000]

def plot_raw(rate, data, filename):
    plt.figure()
    audio = data[:, 1] if len(data.shape) == 2 else data
    times = np.arange(len(audio)) / rate  # Create a time axis in seconds

    plt.plot(times, audio)  # Use the time axis for the x-values
    plt.ylabel("Amplitude")
    plt.xlabel("Time [s]")
    plt.title(f"Raw Data ({filename})")


def plot_fft(rate, data, filename):
    global a,b


    plt.figure()
    if len(data.shape) == 2:
        data = data.mean(axis=1)
    

    # Apply Fast Fourier Transform (FFT)
    y_fft = fft(data)

    # Get the absolute value of the real part
    y_fft = np.abs(y_fft[:len(y_fft) // 2])

    # Create a frequency axis
    freqsx = np.fft.fftfreq(len(y_fft) * 2, 1 / rate)[:len(y_fft)]

    freqs = freqsx[ freqsx > a]
    y_fft = y_fft[freqsx > a]
    y_fft = y_fft[freqs < b]
    freqs = freqs[ freqs < b]
    



    dominant_frequency = round(freqs[np.argmax(y_fft)], 3)

    print(f"The most common frequency for file {filename} is {dominant_frequency} Hz")

    # Plot the results
    plt.plot(freqs, y_fft)
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Amplitude')
    plt.title(f'Frequency Spectrum ({filename})')
    plt.text(1, 1, f"Dominant Frequency [Hz]: {dominant_frequency}", ha="right", va="top",
             transform=plt.gca().transAxes)
    return dominant_frequency


def plot_spectrogram(y, sr, filename):
    global a,b
    plt.figure(figsize=(10, 4))
    D = librosa.stft(y)  # Short-time Fourier transform
    librosa.display.specshow(librosa.amplitude_to_db(D, ref=np.max),
                             sr=sr, x_axis='time', y_axis='log')
    plt.colorbar(format='%+2.0f dB')
    plt.title(f'Spectrogram ({filename})')


def plot_loudness(y, sr, filename):
    plt.figure()
    # Calculate the RMS energy
    rms_energy = librosa.feature.rms(y=y)

    # Convert RMS energy to dB
    loudness_db = librosa.amplitude_to_db(rms_energy, ref=np.max)
    average_loudness = np.mean(loudness_db)

    # Create a histogram with 100 bins
    # hist, bin_edges = np.histogram(loudness_db, bins=100)

    # # Find the bin with the highest frequency
    # most_common_bin = np.argmax(hist)
    #
    # # Get the range of loudness levels for that bin
    # most_common_range = (bin_edges[most_common_bin], bin_edges[most_common_bin + 1])
    #
    # # Print the result
    # print(f"The most common loudness level is between {most_common_range[0]:.2f} dB and {most_common_range[1]:.2f} dB.")

    # Optional: Plot the histogram
    plt.hist(loudness_db.flatten(), bins=100)
    plt.xlabel('Loudness (dB)')
    plt.ylabel('Frequency')
    plt.title(f"Loudness vs. Frequency ({filename})")
    print(average_loudness)
    return average_loudness


def process_files(folder_path):
    l = os.listdir(folder_path)[-1:]
    for i in range(len(l)):
        filename = l[i]
        if filename.endswith(".WAV"):
            filepath = os.path.join(folder_path, filename)
            print(f"Processing file: {filepath}")

            sample_rate, audio_data = read(filepath)
            df = pd.DataFrame({"Sample Rate": sample_rate, "Audio Data": audio_data[:, 1]})
            df.to_csv("audio_data.csv", index=False)

            y, sr = librosa.load(filepath)

            plot_raw(sample_rate, audio_data, filename)
            plot_fft(sample_rate, audio_data, filename)
            plot_loudness(y, sr, filename)
            plot_spectrogram(y, sr, filename)
            plt.show()  # Show plots after processing each file


def main():
    plt.rcParams["figure.figsize"] = [7.50, 3.50]
    plt.rcParams["figure.autolayout"] = True
    pathname_ref = f"{path}:\STEREO\FOLDER01"
    process_files(pathname_ref)


if __name__ == "__main__":
    main()
