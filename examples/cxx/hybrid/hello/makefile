# file      : examples/cxx/hybrid/hello/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../../build/bootstrap.make

xsd := hello.xsd
cxx := driver.cxx

obj := $(addprefix $(out_base)/,\
$(cxx:.cxx=.o)       \
$(xsd:.xsd=.o)       \
$(xsd:.xsd=-pskel.o) \
$(xsd:.xsd=-pimpl.o))

dep := $(obj:.o=.o.d)

xsde.l             := $(out_root)/libxsde/xsde/xsde.l
xsde.l.cpp-options := $(out_root)/libxsde/xsde/xsde.l.cpp-options

driver := $(out_base)/driver
clean  := $(out_base)/.clean

# Build.
#
$(driver): $(obj) $(xsde.l)

$(obj) $(dep): $(xsde.l.cpp-options)

gen := $(out_base)/$(xsd:.xsd=.hxx)       \
       $(out_base)/$(xsd:.xsd=.cxx)       \
       $(out_base)/$(xsd:.xsd=-pskel.hxx) \
       $(out_base)/$(xsd:.xsd=-pskel.cxx) \
       $(out_base)/$(xsd:.xsd=-pimpl.hxx) \
       $(out_base)/$(xsd:.xsd=-pimpl.cxx)

$(gen): $(out_root)/xsde/xsde
$(gen): xsde := $(out_root)/xsde/xsde
$(gen): xsde_options += --generate-parser --generate-aggregate

$(call include-dep,$(dep))

# Convenience alias for default target.
#
.PHONY: $(out_base)/
$(out_base)/: $(driver)


# Clean.
#
.PHONY: $(clean)

$(clean): $(driver).o.clean                                \
  $(addsuffix .cxx.clean,$(obj))                           \
  $(addsuffix .cxx.clean,$(dep))                           \
  $(addprefix $(out_base)/,$(xsd:.xsd=.cxx.xsd.clean))


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
