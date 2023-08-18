process_audio_files();

function process_audio_files()
    folder_path = 'E:\STEREO\DAQ';
    files = dir(fullfile(folder_path, '*.WAV'));

    for file_idx = 1:length(files)
        filename = files(file_idx).name;
        filepath = fullfile(folder_path, filename);

        % Read audio file
        [audio_data, sample_rate] = audioread(filepath);

        audio_data = audio_data(:, 1);

        N = length(audio_data);
        t = (0:N-1)/sample_rate;

        % Compute RMS
        rms_value = sqrt(mean(audio_data.^2));
        % disp(['RMS value of ', filename, ' is: ', num2str(rms_value)]);

        % Plot raw data
        plot_raw(audio_data, sample_rate, filename);

        % Plot FFT
        plot_fft(audio_data, sample_rate, filename);

        % Plot spectrogram
        figure;
        spectrogram(audio_data(:, 1),256,250,256,sample_rate,'yaxis');
        title("Spectrogram (" + filename + ")")
        

        w = hanning(N, 'periodic');

        figure;
        periodogram(audio_data, w, N, sample_rate, 'power')
        
        I = audio_data.^2;

        avg_I = mean(I);

        I0 = 10^-12;

        loudness = 10 * log10(avg_I/I0);

        disp(['The avergage loudness for file ', filename, ' is ', num2str(loudness), ' dB']);
        

        % Uncomment below if you want to pause before processing next file
        % pause;
    end
end

function plot_raw(audio, rate, filename)
    t = (0:length(audio)-1) / rate;
    figure;
    plot(t, audio);
    xlabel('Time [s]');
    ylabel('Amplitude');
    title(['Raw Data (', filename, ')']);
end

function plot_fft(audio, rate, filename)
    L = length(audio);
    Y = fft(audio);
    f = rate * (0:(L/2))/L;
    P = abs(Y/L);

    idx_below_1kHz = f < 1000;
    f = f(idx_below_1kHz);
    P = P(1:L/2 + 1);
    P = P(idx_below_1kHz);

    figure;
    plot(f, P);
    xlabel('Frequency (Hz)');
    ylabel('Amplitude');
    title(['Frequency Spectrum (', filename, ')']);

    [peak_val, peak_idx] = max(P);
    disp(['The most common frequency for file ', filename, ' is ', num2str(f(peak_idx)), ' Hz']);
end