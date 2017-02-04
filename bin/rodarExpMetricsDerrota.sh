#!/bin/bash


for (( i = 1; i <= 25; i++ )); do
	echo "Iniciando teste"${i}"\n"
	./SparCraft ../sample_experiment/sample_exp_linux_derrota.txt >> estados_derrota/estado_${i}_der &
	sleep 1100	
done