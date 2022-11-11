#!/bin/bash
#Script para ejecutar el cliente

mkdir -p ./local
./a.out ./local sar sar | python3 cli_fich_inotify.py 