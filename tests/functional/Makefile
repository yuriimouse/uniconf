
yaml-scan.o: yaml-scan.c
	gcc -c -g -O0 -Wall yaml-scan.c

scan: yaml-scan.o
	gcc -o yaml-scan yaml-scan.o -lyaml

yaml-json.o: yaml-json.c
	gcc -c -g -O0 -Wall -I/usr/local/custom/include yaml-json.c

json: yaml-json.o
	gcc -o yaml-json yaml-json.o -lyaml -lcjson -llists -L/usr/local/custom/lib -L/usr/local/lib

clean:
	rm -f yaml-scan
	rm -f *.o core

default: scan
