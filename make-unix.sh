gcc -o2 -Wall -shared -ansi -pedantic -o ./bin/RSS.so -I ./include ./src/*.c
gcc -Wall -ansi -pedantic -D _TEST -o ./bin/test -I ./include ./src/*.c
