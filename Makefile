.PHONEY: default clean

default:
	gcc mel.c
	pdflatex nather.tex
	make clean

clean:
	rm -f *.aux *.log *.synctex.gz
