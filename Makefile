.PHONY: default

default:
	make a.out
	make nather.pdf

a.out: mel.c
	gcc mel.c

nather.pdf: nather.tex
	pdflatex nather.tex
