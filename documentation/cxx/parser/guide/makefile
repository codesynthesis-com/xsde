.PHONY: all
all: cxx-parser-e-guide.ps cxx-parser-e-guide.pdf

cxx-parser-e-guide.pdf: cxx-parser-e-guide.ps
	ps2pdf14 $<

cxx-parser-e-guide.ps: index.xhtml guide.html2ps
	html2ps -f guide.html2ps -o $@ $<

.PHONY: clean
clean:
	rm -f cxx-parser-e-guide.ps cxx-parser-e-guide.pdf
