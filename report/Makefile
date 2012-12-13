#
# Makefile for latex pdf generation.
#

NAME = report

all: gen clean

gen: $(NAME).tex
	rubber --inplace --maxerr -1 --short --force --warn all --pdf $(NAME).tex

clean:
	rm -f *~ $(NAME).aux $(NAME).log $(NAME).toc $(NAME).bbl $(NAME).blg

distclean: clean
	rm -f $(NAME).pdf
