#!/bin/bash

# Comprobamos que se ha proporcionado un directorio como argumento
if [ "$#" -ne 1 ]; then
    echo "Uso: $0 <directorio>"
    exit 1
fi

DIRECTORIO=$1

# Crear o acceder al directorio proporcionado
if [ ! -d "$DIRECTORIO" ]; then
    mkdir "$DIRECTORIO"
    echo "Directorio $DIRECTORIO creado."
else
    rm -rf "$DIRECTORIO"/*
    echo "Contenido del directorio $DIRECTORIO eliminado."
fi

# Cambiar al directorio proporcionado
cd "$DIRECTORIO" || exit 1

# Crear los archivos y directorios con sus características
mkdir subdir
touch fichero1
echo "1234567890" > fichero2
ln -s fichero2 enlaceS
ln fichero2 enlaceH

# Recorrer y mostrar los atributos de cada archivo y directorio
for file in subdir fichero1 fichero2 enlaceS enlaceH; do
    echo "Atributos de $file:"
    stat "$file"
    echo "---------------------------------"
done

echo "Proceso completado."
