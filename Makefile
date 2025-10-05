.PHONY: all clean

a.out: mel.c
	$(CC) mel.c

nather.pdf: nather.tex
	pdflatex nather.tex

all:
	make a.out
	make nather.pdf

clean:
	rm -f a.out *.aux *.log *.synctex.gz
