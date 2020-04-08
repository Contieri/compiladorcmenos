bison -d cminus.y
flex cminus.l
gcc *.c -o cll -ly -lfl
