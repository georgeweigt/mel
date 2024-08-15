.PHONY: default clean

default:
	make a.out
	make nather.pdf

a.out: mel.c
	$(CC) mel.c

nather.pdf: nather.tex
	pdflatex nather.tex

clean:
	rm -f *.aux *.log *.synctex.gz
