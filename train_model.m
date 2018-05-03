function model = train_model(feats)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
    model = fitgmdist(feats,8);
end

