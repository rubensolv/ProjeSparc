#!/bin/bash


for (( i = 76; i <= 100; i++ )); do
	echo "Iniciando teste"${i}"\n"
	./SparCraft ../sample_experiment/sample_exp_linux_derrota.txt >> estados_derrota/estado_${i}_der &
	sleep 1500	
done
