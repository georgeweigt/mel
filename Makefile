.PHONEY: default clean

default:
	make a.out
	make nather.pdf

a.out: mel.c
	gcc mel.c

nather.pdf: nather.tex
	pdflatex nather.tex
	make clean

clean:
	rm -f *.aux *.log *.synctex.gz
