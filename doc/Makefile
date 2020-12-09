NAME=dokumentace

all: do clean

do: $(NAME).tex
	pdflatex $(NAME).tex
	pdflatex $(NAME).tex

clean:
	-rm -f $(NAME).aux $(NAME).log $(NAME).toc
