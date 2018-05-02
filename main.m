%% read train files
[fileNames, singers] = textread('trainListFile.txt', '%s %s', 'delimiter', '\t');
[singer, clase] = textread('singer_class.txt', '%s %s', 'delimiter', '\t');
N = length(singers);
clases = cell(N,1);
k = 1;
%sabiendo que estan ordenados
for i = 1:N
    if singers{i} ~= singer{k}
        k = k + 1;
    end
    clases{i} = clase{k};    
end

features = cell(N, 2);
for i=1:N
    [audioIn, fs] = audioread(fileNames{i});
    [f0, coef] = extract_feat(audioIn, fs);
    features{i,1} = [f0, coef];
    features{i,2} = clases{i};
end