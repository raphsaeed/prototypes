function cordicSinWaveform(frequency, duration)
    % Define the frequency and duration of the sinusoidal waveform
    fs = 1000;  % Sampling frequency (in Hz)
    t = 0 : 1/fs : duration;  % Time vector

    % CORDIC parameters
    nIterations = 10;  % Number of CORDIC iterations

    % Calculate the CORDIC gain factor
    K = 1 / prod(sqrt(1 + 2 .^ (-2 * (0 : nIterations))));

    % Initialize variables
    x = 1.2075;  % Initial value of x (corresponds to atan(1))
    y = 0;       % Initial value of y
    z = frequency * (2 * pi / fs);  % Initial value of z (corresponds to desired frequency)

    % Perform CORDIC iterations
    for i = 1 : length(t)
        % Calculate the current output sample
        waveform(i) = K * y;

        % Update the variables using CORDIC rotation
        if z > 0
            xNew = x - y * 2^(-i);
            y = y + x * 2^(-i);
            z = z - atan(2^(-i));
        else
            xNew = x + y * 2^(-i);
            y = y - x * 2^(-i);
            z = z + atan(2^(-i));
        end
        x = xNew;
    end

    % Plot the generated sinusoidal waveform
    plot(t, waveform);
    xlabel('Time (s)');
    ylabel('Amplitude');
    title(['CORDIC Sinusoidal Waveform (Frequency = ', num2str(frequency), ' Hz)']);
    grid on;
end

% usage in Matlab window do following
% >> frequency = 5; 
% >> duration = 1;
% >> cordicSinWaveform(frequency, duration);
%
%