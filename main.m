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
    features{i,1} = [f0, coef];
end

%% normalizar
featureVectors = features{:,1};
m = mean(featureVectors);
s = std(featureVectors);
features_norm = features;
for i=1:N
    features_norm{i,2} = (features{i,2}-m)./s;
end
