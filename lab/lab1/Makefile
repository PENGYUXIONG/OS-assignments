shell379: pcb_control.o exec.o parse.o header.h
	gcc -o shell379 pcb_control.o exec.o parse.o 
pcb_control: pcb_control.c header.h
	gcc -Wall -c pcb_control.c -o pcb_control
parse: parse.c header.h
	gcc -Wall -c parse.c -o parse
exec: exec.c header.h
	gcc -Wall -c exec.c -o exec
clean:
	rm shell379 exec.o pcb_control.o parse.o
