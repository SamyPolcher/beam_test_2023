# Beam test 2023

## decoding fdf

```
cd decode
make
./decode filename.fdf
```
Outputs decoded file ftest.root, where each event has sample,channel,amplitude vectors of the same size. 

## FEU reconstruction

```
cd reco
make
./reco file.root FEUNb
```
Takes a decoded file and a FEU number.
Use FEUNb=5 to only make hits no need for any mapping info.
FEU 1 to 4 were connected to specific detectors in the beam test and the code knows which detector is connected to which FEU and how the connectors where inverted. It then pulls the maping for that detector and build hits&clusters.

## Decoding Alpide output

Takes the Alpide output root files for one ladder and a noise file to find the noisy pixels.
```
cd banco
make
./decode_banco ladder.root noise.root
```
Outputs fdec.root where each event has a hits and a clusters vector filled with all the reconstructed banco::hit and banco::cluster object reconstructed in that event.

## Tracks reconstruction

Takes the decoded files of all 4 ladders for that runs
```
./reco_banco ladder*.root
```
Outputs a root file with for each event a vector with all the banco::track objects.
Many options to input an alignment file with the offsets of each ladder, and a global rotation file. Also an option to compute the residues and add the plots to th output root file.