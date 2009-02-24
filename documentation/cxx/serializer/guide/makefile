.PHONY: all
all: cxx-serializer-e-guide.ps cxx-serializer-e-guide.pdf

cxx-serializer-e-guide.pdf: cxx-serializer-e-guide.ps
	ps2pdf14 $<

cxx-serializer-e-guide.ps: index.xhtml guide.html2ps
	html2ps -f guide.html2ps -o $@ $<

.PHONY: clean
clean:
	rm -f cxx-serializer-e-guide.ps cxx-serializer-e-guide.pdf
