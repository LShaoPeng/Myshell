/bin/myshell:src/main.c
	gcc -Iinclude src/main.c -o bin/myshell

.PHONY:
	clean distclean

clean:
	rm obj/*

distclean:
	rm bin/*
