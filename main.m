%% read train files - leer ficheros de entrenamiento
%[fichero de audio train, nombre del cantante]
[train_fileNames, train_singers] = textread('trainListFile.txt', ...
                                            '%s %s', 'delimiter', '\t');
N_train = length(train_singers);
singers = textread('singers.txt', '%s'); %lista con cantantes
%vector con clase (entero) de cada audio de entrenamiento
train_clases = zeros(N_train,1);
for i = 1:length(singers)
    train_clases(strcmp(singers{i},train_singers)) = i;
end
%% extract features for each train audio file - 
%  extraer caracteristicas de cada fichero de entrenamiento
ncoef = 13;
train_features = cell(N_train, 1);                 %{file1:{frame1:[pitch, log energy, MFCC2,...]
for i=1:N_train                                    %       ...
    [audioIn, fs] = audioread(train_fileNames{i}); %       frameN:[pitch, log energy, MFCC2,...]}
    [f0, coef] = extract_feat(audioIn, fs, ncoef); %...
    train_features{i} = [f0, coef];                %fileN_train:{frame1:[pitch, log energy, MFCC2,...]
end                                                %             ...}
                                                   %}                                             
clear audioIn f0 coef
%% normalize to avoid bias - normalizar para evitar sesgo
all_features = cell2mat(train_features(:));
m = mean(all_features);
s = std(all_features);
for i=1:N_train
    train_features{i} = (train_features{i}-m)./s;
end

clear all_features m s
%% train GMM for each class - entrenar GMM para cada classe
nclases = length(clases);
GMMs = cell(nclases,1);
for i=1:nclases
    GMMs{i} = train_model(cell2mat(train_features(train_clases==i)));
end

%% read test files, extract and normalize features - 
%  leer ficheros de test, extraer y normalizar caracteristicas
[test_fileNames, test_singers] = textread('testListFile.txt', ...
                                          '%s %s', 'delimiter', '\t');
N_test = length(test_singers);
%vector con clase (entero) de cada audio de test
test_clases = zeros(N_test,1);
for i = 1:length(singers)
    test_clases(strcmp(singers{i},test_singers)) = i;
end

ncoef = 13;
test_features = cell(N_test, 1);                 
for i=1:N_test                                    
    [audioIn, fs] = audioread(test_fileNames{i}); 
    [f0, coef] = extract_feat(audioIn, fs, ncoef); 
    test_features{i} = [f0, coef];                
end                                                

clear audioIn f0 coef

all_features = cell2mat(test_features(:));
m = mean(all_features);
s = std(all_features);
for i=1:N_test
    test_features{i} = (test_features{i}-m)./s;
end

clear all_features m s

%% predict class (by maximum likelihood)
pred_classes = zeros(N_test,1); %predicted class for each test file
for i=1:N_train                                    
    feats = test_features{i};
    N_frames = size(feats,1);
    pred_class = zeros(N_frames,1); %predicted class for each frame
    for k=1:N_frames
        pred_class(k) = classif(feats(k,:), GMMS, 'gmm');
    end
    %post prosesado
    %...
end                                                
num_err = sum(test_classes~=pred_classes)