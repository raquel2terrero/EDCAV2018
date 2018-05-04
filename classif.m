function est_class = classif(feat, model, type)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    if nargin==2
        type = 'gmm';
    end
    if strcmp(type,'gmm')==1
        c = 0;
        for i = 1:length(model)
            %p = posterior(model{i}, feat)
        end
    end
end

