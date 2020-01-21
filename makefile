CC=gcc
DEPS=myheader.h
OBJ=utils.o myshell.o run_commands.o parsecmds.o
%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

myshell: $(OBJ)
		gcc -o $@ $^ $(CFLAGS)
clean:
	rm myshell.o
