root := ../../../..

include $(root)/build/cxx/rules.make

# Build.
#
EXTRA_CPPFLAGS += -I$(root)/libxsde

ifeq ($(XSDE_LONGLONG),n)
EXTRA_XSDFLAGS += --no-long-long
endif

ifeq ($(XSDE_SERIALIZER_VALIDATION),n)
EXTRA_XSDFLAGS += --suppress-validation
endif

ifeq ($(XSDE_POLYMORPHIC),y)
EXTRA_XSDFLAGS += --runtime-polymorphic
endif

driver: driver.o email-sskel.o $(root)/libxsde/xsde/libxsde.a

driver.o: driver.cxx email-sskel.hxx email.hxx
email-sskel.o: email-sskel.cxx email-sskel.hxx email.hxx

.PRECIOUS: %-sskel.hxx %-sskel.cxx

%-sskel.hxx %-sskel.cxx: %.xsd %.map
	$(root)/bin/xsde cxx-serializer $(XSDFLAGS) $(EXTRA_XSDFLAGS) \
--type-map $*.map $<


# Test.
#
.PHONY: test
test: driver
	./driver


# Clean.
#
.PHONY: clean
clean:
	rm -f email-sskel.?xx email-sskel.o driver.o driver

