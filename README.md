# Proyecto EDCAV Audio
## Singing Voice Separation

## Singer Classification
Singer classification using pitch and mfcc.
Implemented on MATLAB 2018a. Using Audio System Toolbox. 
### List Files:
feature extraction list file (list of files to extract features from): featureExtractionListFile.txt

training list file (list of files to train from): trainListFile.txt

test list file (list of files to test from): testListFile.txt

classification output file (classification result): outputListFile.txt

### Functions:
singers.txt: list of (used) dataset singers

extract_feat: extract pitch and mffc from audio file

train_model: trains classification model from features

classif: classify from features using trained model

main: script to call functions
