function [f0, coef] = extract_feat(audioIn, fs)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
    f0 = pitch(audioIn, fs, ...
        'WindowLength', round(fs*0.03), 'OverlapLength', round(fs*0.02), ...
        'MedianFilterLength', 50);
    coef = mfcc(audioIn, fs, 'LogEnergy', 'Replace');
end

