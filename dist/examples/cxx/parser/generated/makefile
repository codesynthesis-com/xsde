root := ../../../..

include $(root)/build/cxx/rules.make

# Build.
#
EXTRA_CPPFLAGS += -I$(root)/libxsde

EXTRA_XSDFLAGS := --generate-print-impl --generate-test-driver

# If you are basing your own code on this example and make changes
# to the sample implementation and/or test driver, make sure you
# remove this option or your changes will be lost forever.
#
EXTRA_XSDFLAGS += --force-overwrite

ifeq ($(XSDE_STL),n)
EXTRA_XSDFLAGS += --no-stl
endif

ifeq ($(XSDE_IOSTREAM),n)
EXTRA_XSDFLAGS += --no-iostream
endif

ifeq ($(XSDE_EXCEPTIONS),n)
EXTRA_XSDFLAGS += --no-exceptions
endif

ifeq ($(XSDE_LONGLONG),n)
EXTRA_XSDFLAGS += --no-long-long
endif

ifeq ($(XSDE_PARSER_VALIDATION),n)
EXTRA_XSDFLAGS += --suppress-validation
endif

ifeq ($(XSDE_REUSE_STYLE),mixin)
EXTRA_XSDFLAGS += --reuse-style-mixin
endif

ifeq ($(XSDE_POLYMORPHIC),y)
EXTRA_XSDFLAGS += --runtime-polymorphic
endif

library-pdriver: library-pdriver.o library-pskel.o library-pimpl.o $(root)/libxsde/xsde/libxsde.a

library-pdriver.o: library-pdriver.cxx library-pimpl.hxx library-pskel.hxx
library-pimpl.o: library-pimpl.cxx library-pimpl.hxx library-pskel.hxx
library-pskel.o: library-pskel.cxx library-pskel.hxx

.PRECIOUS: %-pskel.hxx %-pskel.cxx %-pimpl.hxx %-pimpl.cxx %-pdriver.cxx

%-pskel.hxx %-pskel.cxx %-pimpl.hxx %-pimpl.cxx %-pdriver.cxx: %.xsd
	$(root)/bin/xsde cxx-parser $(XSDFLAGS) $(EXTRA_XSDFLAGS) $<


# Test.
#
.PHONY: test
test: library-pdriver library.xml
	./library-pdriver library.xml


# Clean.
#
.PHONY: clean
clean:
	rm -f library-pdriver.?xx library-pimpl.?xx library-pskel.?xx
	rm -f library-pdriver.o library-pimpl.o library-pskel.o library-pdriver
