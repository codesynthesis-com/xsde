# file      : tests/cxx/hybrid/recursive/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../../build/bootstrap.make

xsd := test.xsd
cxx := driver.cxx

obj := $(addprefix $(out_base)/,\
$(cxx:.cxx=.o)       \
$(xsd:.xsd=.o)       \
$(xsd:.xsd=-pskel.o) \
$(xsd:.xsd=-pimpl.o) \
$(xsd:.xsd=-sskel.o) \
$(xsd:.xsd=-simpl.o))

dep := $(obj:.o=.o.d)

xsde.l             := $(out_root)/libxsde/xsde/xsde.l
xsde.l.cpp-options := $(out_root)/libxsde/xsde/xsde.l.cpp-options

driver := $(out_base)/driver
test   := $(out_base)/.test
clean  := $(out_base)/.clean


# Build.
#
$(driver): $(obj) $(xsde.l)

$(obj) $(dep): $(xsde.l.cpp-options)

genf := $(xsd:.xsd=.hxx) $(xsd:.xsd=.cxx) \
        $(xsd:.xsd=-pskel.hxx) $(xsd:.xsd=-pskel.cxx) \
        $(xsd:.xsd=-pimpl.hxx) $(xsd:.xsd=-pimpl.cxx) \
        $(xsd:.xsd=-sskel.hxx) $(xsd:.xsd=-sskel.cxx) \
        $(xsd:.xsd=-simpl.hxx) $(xsd:.xsd=-simpl.cxx)

gen := $(addprefix $(out_base)/,$(genf))

$(gen): $(out_root)/xsde/xsde
$(gen): xsde := $(out_root)/xsde/xsde
$(gen): xsde_options += --generate-parser --generate-serializer \
--generate-aggregate

$(call include-dep,$(dep))

# Convenience alias for default target.
#
$(out_base)/: $(driver)


# Test.
#
$(test): driver := $(driver)
$(test): $(driver) $(src_base)/test-000.xml $(src_base)/test-000.std
	$(call message,test $$1,$$1 $(src_base)/test-000.xml | diff -u $(src_base)/test-000.std -,$(driver))

# Clean.
#
$(clean): $(driver).o.clean                                \
  $(addsuffix .cxx.clean,$(obj))                           \
  $(addsuffix .cxx.clean,$(dep))                           \
  $(addprefix $(out_base)/,$(xsd:.xsd=.cxx.xsd.clean))


# Generated .gitignore.
#
ifeq ($(out_base),$(src_base))
$(gen): | $(out_base)/.gitignore
$(driver): | $(out_base)/.gitignore

$(out_base)/.gitignore: files := driver $(genf)
$(clean): $(out_base)/.gitignore.clean

$(call include,$(bld_root)/git/gitignore.make)
endif


# How to.
#
$(call include,$(bld_root)/cxx/o-e.make)
$(call include,$(bld_root)/cxx/cxx-o.make)
$(call include,$(bld_root)/cxx/cxx-d.make)
$(call include,$(scf_root)/xsde/hybrid/xsd-cxx.make)


# Dependencies.
#
$(call import,$(src_root)/xsde/makefile)
$(call import,$(src_root)/libxsde/xsde/makefile)
