#!/bin/bash


for (( i =76; i <= 100; i++ )); do
	echo "Iniciando teste"${i}"\n"
	./SparCraft ../sample_experiment/sample_exp_linux_vitoria.txt >> estados_vitoria/estado_${i}_vic &
	sleep 1500	
done
