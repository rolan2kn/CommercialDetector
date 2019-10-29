# CommercialDetector
Detecta comerciales en videos de programas de televisión

--> HERRAMIENTAS

Se implementó utilizando C++11, cmake 3.13 minimo y OpenCV 4.1.1
como IDE se utilizó CLion, se implementó en Ubuntu pero es compatible con Windows.

--> EJECUCIÓN

Para compilar se procede como sigue:

1. se abre un terminal y se posiciona en el directorio <TAREA_1_PATH>/CommercialDetector:
>> cd <TAREA_1_PATH>/CommercialDetector

2. una vez en el directorio se ejecuta cmake para la construcción (builds):
>> /usr/bin/cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=/usr/bin/g++ -G "CodeBlocks - Unix Makefiles" ./

3. se enlaza todo y se construye el ejecutable
>> make

4. se ejecuta el ejecutable CommercialDetector:
>> ./CommercialDetector "TV_VIDEO_PATH" "COMERCIALES_PATH"

TV_VIDEO_PATH: es el filename del television.mp4 o el directorio con los videos de tv.
COMERCIALES_PATH: es el directorio de comerciales.

las comillas son para incluir nombres con espacios.

--> Datos

DUrante la ejecución el programa genera una jerarquia de directorios:

APP_PATH
|__ data
    |__cache
        |__television
            |__video_name_1
                |__metadata
                |__0
                |__...
            |__video_name_...
                |__metadata
                |__0
                |__...
        |__comerciales
            |__comercial_name_1
                |__metadata
                |__0
                |__...
            |__comercial_name_...
                |__metadata
                |__0
                |__...
            |__closeness
            |__detecciones.txt

Donde se almacenan los descriptores de television y comerciales en sus respectivas carpetas.

-- SE crea una carpeta con el nombre de cada video de tv y comercial y dentro se almacena un archivo de metadatos
que contiene informaciones del video como: cantidad de frames, frecuencia de muestreo, cantidad de descriptores por segundo
y se almacenan todos los descriptores del video, se toma la convencion de nombrar cada fichero del descriptor con
el numero de frame que le corresponde.

-- En el mismo nivel de las carpetas de tv y comerciales se generan varios ficheros:
- closeness es el fichero que contiene la información de similitud entre cada frame de video de tv a
los comerciales (se usa el vecino mas cercano NN).
- detecciones.txt es el fichero de resultados de la detección
