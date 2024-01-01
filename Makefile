a.out: mel.c
	$(CC) mel.c

nather.pdf: nather.tex
	pdflatex nather.tex
