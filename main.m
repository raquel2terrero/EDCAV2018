%% read train files
[fileNames, singers] = textread('trainListFile.txt', '%s %s', 'delimiter', '\t');
[singer, clase] = textread('singer_class.txt', '%s %s', 'delimiter', '\t');
N = length(singers);
for i=1:N
    
end