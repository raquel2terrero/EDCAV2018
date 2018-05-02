%% read train files
[fileNames, singers] = textread('trainListFile.txt', '%s %s', 'delimiter', '\t');
[singer, clase] = textread('singer_class.txt', '%s %s', 'delimiter', '\t');
N = length(singers);

features = cell(N, 2); %{class, [pitch, coef]}
for i=1:N
    [audioIn, fs] = audioread(fileNames{i});
    [f0, coef] = extract_feat(audioIn, fs);
    features{i,1} = clases{i};
    features{i,2} = [f0, coef];
end
% normalizar
featureVectors = features{:,2};
m = mean(featureVectors);
s = std(featureVectors);
for i=1:N
    features{i,2} =  (features{i,2}-m)./s;
end
