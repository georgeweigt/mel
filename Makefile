.PHONY: default clean

default:
	make a.out
	make nather.pdf

a.out: mel.c
	gcc mel.c

nather.pdf: nather.tex
	pdflatex nather.tex
	rm -f nather.aux nather.log

clean:
	rm -f a.out nather.pdf
