# file      : examples/cxx/hybrid/evolution/passthrough/makefile
# copyright : Copyright (c) 2005-2017 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../../../build/bootstrap.make

xsd := transform.xsd
cxx := driver.cxx xml.cxx unknown-type.cxx unknown-type-pimpl.cxx \
unknown-type-simpl.cxx

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
--generate-aggregate --root-element transformations --generate-polymorphic \
--custom-type unknown_type=//unknown_type_base/unknown-type.hxx \
--custom-parser unknown_type=unknown_type_base_pimpl/unknown-type-pimpl.hxx \
--custom-serializer unknown_type=unknown_type_base_simpl/unknown-type-simpl.hxx

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
	$(call install-data,$(src_base)/xml.hxx,$(dist_prefix)/$(path)/xml.hxx)
	$(call install-data,$(src_base)/xml.ixx,$(dist_prefix)/$(path)/xml.ixx)
	$(call install-data,$(src_base)/xml.cxx,$(dist_prefix)/$(path)/xml.cxx)
	$(call install-data,$(src_base)/unknown-type.hxx,$(dist_prefix)/$(path)/unknown-type.hxx)
	$(call install-data,$(src_base)/unknown-type.cxx,$(dist_prefix)/$(path)/unknown-type.cxx)
	$(call install-data,$(src_base)/unknown-type-pimpl.hxx,$(dist_prefix)/$(path)/unknown-type-pimpl.hxx)
	$(call install-data,$(src_base)/unknown-type-pimpl.cxx,$(dist_prefix)/$(path)/unknown-type-pimpl.cxx)
	$(call install-data,$(src_base)/unknown-type-simpl.hxx,$(dist_prefix)/$(path)/unknown-type-simpl.hxx)
	$(call install-data,$(src_base)/unknown-type-simpl.cxx,$(dist_prefix)/$(path)/unknown-type-simpl.cxx)
	$(call install-data,$(src_base)/transform.xsd,$(dist_prefix)/$(path)/transform.xsd)
	$(call install-data,$(src_base)/transform-v2.xsd,$(dist_prefix)/$(path)/transform-v2.xsd)
	$(call install-data,$(src_base)/transform.xml,$(dist_prefix)/$(path)/transform.xml)

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
