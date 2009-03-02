root := ../../../..

include $(root)/build/cxx/rules.make

# Build.
#
EXTRA_CPPFLAGS := -I$(root)/libxsde

ifeq ($(XSDE_LONGLONG),n)
EXTRA_XSDFLAGS += --no-long-long
endif

ifeq ($(XSDE_PARSER_VALIDATION),n)
EXTRA_XSDFLAGS += --suppress-validation
endif

ifeq ($(XSDE_POLYMORPHIC),y)
EXTRA_XSDFLAGS += --runtime-polymorphic
endif

driver: driver.o email-pskel.o $(root)/libxsde/xsde/libxsde.a

driver.o: driver.cxx email-pskel.hxx
email-pskel.o: email-pskel.cxx email-pskel.hxx

.PRECIOUS: %-pskel.hxx %-pskel.cxx
%-pskel.hxx %-pskel.cxx: %.xsd
	$(root)/bin/xsde cxx-parser $(XSDFLAGS) $(EXTRA_XSDFLAGS) $<

# Test.
#
.PHONY: test
test: driver email.xml
	./driver email.xml


# Clean.
#
.PHONY: clean
clean:
	rm -f email-pskel.?xx email-pskel.o driver.o driver

