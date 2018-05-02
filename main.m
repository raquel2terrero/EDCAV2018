%% read train files
[fileNames, singers] = textread('trainListFile.txt', '%s %s', 'delimiter', '\t');
N = length(singers);

features = cell(N, 2);
for i=1:N
    [audioIn, fs] = audioread(fileNames{i});
    [f0, coef] = extract_feat(audioIn, fs);
    features{i,1} = [f0, coef];
    features{i,2} = clase{i};
end