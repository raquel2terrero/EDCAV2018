%% read train files - leer ficheros de entrenamiento
%[fichero de audio train, nombre del cantante]
[train_fileNames, train_singers] = textread('trainListFile.txt', ...
                                            '%s %s', 'delimiter', '\t');
N_train = length(train_singers);
singers = textread('singers.txt', '%s', 'delimiter', '\n'); %lista con cantantes
%vector con clase (entero) de cada audio de entrenamiento
train_clases = zeros(N_train,1);
for i = 1:length(singers)
    train_classes(strcmp(singers{i},train_singers)) = i;
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
nclases = length(singers);
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
pred_classes = zeros(N_test,1); %clase predicha para cada fichero de test
for i=1:N_test
    feats = test_features{i};
    N_frames = size(feats,1);
    %pred_class = zeros(N_frames,1);
    %for k=1:N_frames
        pred_class = classif(feats, GMMs, 'gmm'); %clase predicha para cada frame
    %end
    %decision final clase = clase mas representada despues de filtro mediana
    c = mode(movmedian(pred_class, 5));
    fprintf("El fichero "+test_fileNames{i}+" es de "+singers{c}+"\n")
    pred_classes(i) = c;
end

%% results - resultados
num_errors = sum(test_clases~=pred_classes)
confusion_matrix = confusionmat(test_clases, pred_classes)