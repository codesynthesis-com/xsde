# file      : examples/cxx/hybrid/binary/custom/makefile
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../../../build/bootstrap.make

xsd := library.xsd
cxx := driver.cxx exceptions.cxx orawstream.cxx irawstream.cxx

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

driver   := $(out_base)/driver
dist     := $(out_base)/.dist
dist-win := $(out_base)/.dist-win
clean    := $(out_base)/.clean

# Build.
#
$(driver): $(obj) $(xsde.l)

$(obj) $(dep): $(xsde.l.cpp-options)

genf := $(xsd:.xsd=.hxx) $(xsd:.xsd=.ixx) $(xsd:.xsd=.cxx) \
        $(xsd:.xsd=-pskel.hxx) $(xsd:.xsd=-pskel.ixx) $(xsd:.xsd=-pskel.cxx) \
        $(xsd:.xsd=-pimpl.hxx) $(xsd:.xsd=-pimpl.cxx) \
        $(xsd:.xsd=-sskel.hxx) $(xsd:.xsd=-sskel.ixx) $(xsd:.xsd=-sskel.cxx) \
        $(xsd:.xsd=-simpl.hxx) $(xsd:.xsd=-simpl.cxx)

gen := $(addprefix $(out_base)/,$(genf))

$(gen): $(out_root)/xsde/xsde
$(gen): xsde := $(out_root)/xsde/xsde
$(gen): xsde_options += --generate-parser --generate-serializer \
--generate-aggregate --extern-xml-schema xml-schema.xsd \
--generate-insertion orawstream --hxx-prologue '\#include "orawstream.hxx"' \
--generate-extraction irawstream --hxx-prologue '\#include "irawstream.hxx"'

# Header files for XML Schema namespace.
#
$(out_base)/xml-schema%hxx       \
$(out_base)/xml-schema-pskel%hxx \
$(out_base)/xml-schema-sskel%hxx: $(out_root)/xsde/xsde | $(out_base)/.
	$(call message,xsde $(src_base)/xml-schema.xsd,\
$(out_root)/xsde/xsde cxx-hybrid --output-dir $(out_base) \
--generate-xml-schema --generate-parser --generate-serializer \
$(xsde_options) xml-schema.xsd)

genf += xml-schema.hxx xml-schema-pskel.hxx xml-schema-sskel.hxx

$(call include-dep,$(dep),$(obj),$(gen))

# Convenience alias for default target.
#
$(out_base)/: $(driver)


# Dist.
#
dist-common := $(out_base)/.dist-common
$(dist) $(dist-win) $(dist-common): path := $(subst $(src_root)/,,$(src_base))

$(dist-common):
	$(call install-data,$(src_base)/driver.cxx,$(dist_prefix)/$(path)/driver.cxx)
	$(call install-data,$(src_base)/exceptions.cxx,$(dist_prefix)/$(path)/exceptions.cxx)
	$(call install-data,$(src_base)/exceptions.hxx,$(dist_prefix)/$(path)/exceptions.hxx)
	$(call install-data,$(src_base)/orawstream.hxx,$(dist_prefix)/$(path)/orawstream.hxx)
	$(call install-data,$(src_base)/orawstream.ixx,$(dist_prefix)/$(path)/orawstream.ixx)
	$(call install-data,$(src_base)/orawstream.txx,$(dist_prefix)/$(path)/orawstream.txx)
	$(call install-data,$(src_base)/orawstream.cxx,$(dist_prefix)/$(path)/orawstream.cxx)
	$(call install-data,$(src_base)/irawstream.hxx,$(dist_prefix)/$(path)/irawstream.hxx)
	$(call install-data,$(src_base)/irawstream.ixx,$(dist_prefix)/$(path)/irawstream.ixx)
	$(call install-data,$(src_base)/irawstream.txx,$(dist_prefix)/$(path)/irawstream.txx)
	$(call install-data,$(src_base)/irawstream.cxx,$(dist_prefix)/$(path)/irawstream.cxx)
	$(call install-data,$(src_base)/library.xsd,$(dist_prefix)/$(path)/library.xsd)
	$(call install-data,$(src_base)/library.xml,$(dist_prefix)/$(path)/library.xml)

$(dist): $(dist-common)
	$(call install-data,$(src_base)/README,$(dist_prefix)/$(path)/README)

$(dist-win): $(dist-common)
	$(call install-data,$(src_base)/README,$(dist_prefix)/$(path)/README.txt)
	$(call message,,todos $(dist_prefix)/$(path)/README.txt)


# Clean.
#
$(clean): $(driver).o.clean                                \
  $(addsuffix .cxx.clean,$(obj))                           \
  $(addsuffix .cxx.clean,$(dep))                           \
  $(addprefix $(out_base)/,$(xsd:.xsd=.cxx.xsd.clean))
	$(call message,rm $$1,rm -f $$1,$(out_base)/xml-schema.hxx)
	$(call message,rm $$1,rm -f $$1,$(out_base)/xml-schema-pskel.hxx)
	$(call message,rm $$1,rm -f $$1,$(out_base)/xml-schema-sskel.hxx)


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
$(call include,$(bld_root)/install.make)
$(call include,$(scf_root)/xsde/hybrid/xsd-cxx.make)


# Dependencies.
#
$(call import,$(src_root)/xsde/makefile)
$(call import,$(src_root)/libxsde/xsde/makefile)
