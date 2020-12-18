NAME=presentation

all: do clean

do: $(NAME).tex
	pdflatex $(NAME).tex
	pdflatex $(NAME).tex

clean:
	-rm -f $(NAME).aux $(NAME).log $(NAME).toc $(NAME).out $(NAME).nav $(NAME).snm
