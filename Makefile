.PHONY: clean

a.out: mel.c
	$(CC) mel.c

clean:
	rm -f a.out *.aux *.log *.synctex.gz
