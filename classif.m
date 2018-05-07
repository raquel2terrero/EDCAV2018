function pred_class = classif(feat, model, type)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    if nargin==2
        type = 'gmm';
    end
    if strcmp(type,'gmm')==1
        p = zeros(size(model));
        for i = 1:length(model)
            p(i) = posterior(model{i}, feat);
        end
        [~, pred_class] = max(p);
    end
end

