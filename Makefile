default:
	gcc mel.c
	pdflatex nather.tex
	rm -f nather.aux nather.log

clean:
	rm -f nather.aux nather.log nather.pdf nather.synctex.gz
