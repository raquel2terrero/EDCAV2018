%% read train files
%[fichero de audio train, nombre del cantante]
[train_fileNames, train_singers] = textread('trainListFile.txt', '%s %s', 'delimiter', '\t');
N_train = length(train_singers);
%[nombre del cantante, clase asociada]
[singers, clases] = textread('singer_class.txt', '%s %d', 'delimiter', '\t');
train_clases = zeros(N_train,1);
%vector con clase de cada audio (sabiendo que la lista esta ordenada)
for i = 1:N_train
    if strcmp(train_singers{i},singers{k})==0
        k = k + 1;
    end
    train_clases(i) = clases(k);    
end

%% extract features for each audio file
ncoef = 13;
train_features = cell(N_train, 1); %{[pitch, coef]}
for i=1:N_train
    [audioIn, fs] = audioread(train_fileNames{i});
    [f0, coef] = extract_feat(audioIn, fs, ncoef);
    train_features{i} = [f0, coef];
end

clear audioIn f0 coef
%% normalize to avoid bias
all_features = cell2mat(train_features(:));
m = mean(all_features);
s = std(all_features);
for i=1:N_train
    train_features{i} = (train_features{i}-m)./s;
end

clear all_features
%% train
nclases = length(clases);
GMModels = cell(nclases,1);
for i=1:nclases
    GMModels{i} = train_model(cell2mat(train_features(train_clases==i)));
end

%% read test files
[test_fileNames, test_singers] = textread('testListFile.txt', '%s %s', 'delimiter', '\t');
N_test = length(test_singers);
test_clases = zeros(N_test,1);
k = 1;
%vector con clase de cada audio (sabiendo que la lista esta ordenada)
for i = 1:N_test
    if strcmp(test_singers{i},singers{k})==0
        k = k + 1;
    end
    test_clases(i) = clases(k);    
end
