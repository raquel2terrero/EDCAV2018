function [f0, coefs] = extract_feat(audioIn, fs, ncoef)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
    f0 = pitch(audioIn, fs, ...
        'WindowLength', round(fs*0.03), 'OverlapLength', round(fs*0.02), ...
        'MedianFilterLength', 50);
    coefs = mfcc(audioIn, fs, 'LogEnergy', 'Replace', 'NumCoeffs', ncoef);
end
