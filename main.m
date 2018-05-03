%% read train files
[fileNames, singers] = textread('trainListFile.txt', '%s %s', 'delimiter', '\t');
[singer, clase] = textread('singer_class.txt', '%s %d', 'delimiter', '\t');
N = length(singers);
clases = zeros(N,1);
k = 1;
%vector con clase de cada audio (sabiendo que la lista esta ordenada)
for i = 1:N
    if strcmp(singers{i},singer{k})==0
        k = k + 1;
    end
    clases(i) = clase(k);    
end

%% extract features for each audio file
ncoef = 13;
features = cell(N, 1); %{[pitch, coef]}
for i=1:N
    [audioIn, fs] = audioread(fileNames{i});
    [f0, coef] = extract_feat(audioIn, fs, ncoef);
    features{i} = [f0, coef];
end

clear audioIn
%% normalize
all_features = cell2mat(features(:));
m = mean(all_features);
s = std(all_features);
features_norm = cell(N,1);
for i=1:N
    features_norm{i} = (features{i}-m)./s;
end

%% train