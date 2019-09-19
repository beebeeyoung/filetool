




all:
	gcc -o filetool filetool.c md5.c

clean:
	rm -rf *.o filetool
