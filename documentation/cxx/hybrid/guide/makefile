.PHONY: all
all: cxx-hybrid-e-guide.ps cxx-hybrid-e-guide.pdf

cxx-hybrid-e-guide.pdf: cxx-hybrid-e-guide.ps
	ps2pdf14 $<

cxx-hybrid-e-guide.ps: index.xhtml guide.html2ps
	html2ps -f guide.html2ps -o $@ $<

.PHONY: clean
clean:
	rm -f cxx-hybrid-e-guide.ps cxx-hybrid-e-guide.pdf
