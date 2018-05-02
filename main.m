%% read train files
[fileNames, singers] = textread('trainListFile.txt', '%s %s', 'delimiter', '\t');
[singer, clase] = textread('singer_class.txt', '%s %s', 'delimiter', '\t');
N = length(singers);
clases = cell(N,1);
k = 1;
%sabiendo que estan ordenados
for i = 1:N
    if strcmp(singers{i},singer{k})==0
        k = k + 1;
    end
    clases{i} = clase{k};    
end

features = cell(N, 1); %{class, [pitch, coef]}
for i=1:N
    [audioIn, fs] = audioread(fileNames{i});
    [f0, coef] = extract_feat(audioIn, fs);
    features{i} = [f0, coef];
end
% normalizar
featureVectors = features{:};
m = mean(featureVectors);
s = std(featureVectors);
for i=1:N
    features{i,2} =  (features{i,2}-m)./s;
end
