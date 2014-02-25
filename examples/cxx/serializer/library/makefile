# file      : examples/cxx/serializer/library/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../../build/bootstrap.make

xsd := library.xsd
cxx := driver.cxx

ifeq ($(xsde_reuse_style),mixin)
cxx += library-simpl-mixin.cxx
else
cxx += library-simpl-tiein.cxx
endif

obj := $(addprefix $(out_base)/,$(cxx:.cxx=.o) $(xsd:.xsd=-sskel.o))
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

$(obj) $(dep): cpp_options := -I$(out_base) -I$(src_base)
$(obj) $(dep): $(xsde.l.cpp-options)

skelf := $(xsd:.xsd=-sskel.hxx) $(xsd:.xsd=-sskel.ixx) $(xsd:.xsd=-sskel.cxx)
skel  := $(addprefix $(out_base)/,$(skelf))

$(skel): xsde := $(out_root)/xsde/xsde
$(skel): xsde_options += --generate-inline --type-map $(src_base)/library.map

$(skel): $(out_root)/xsde/xsde $(src_base)/library.map

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
	$(call install-data,$(src_base)/library.hxx,$(dist_prefix)/$(path)/library.hxx)
	$(call install-data,$(src_base)/library-simpl-mixin.hxx,$(dist_prefix)/$(path)/library-simpl-mixin.hxx)
	$(call install-data,$(src_base)/library-simpl-mixin.cxx,$(dist_prefix)/$(path)/library-simpl-mixin.cxx)
	$(call install-data,$(src_base)/library-simpl-tiein.hxx,$(dist_prefix)/$(path)/library-simpl-tiein.hxx)
	$(call install-data,$(src_base)/library-simpl-tiein.cxx,$(dist_prefix)/$(path)/library-simpl-tiein.cxx)
	$(call install-data,$(src_base)/library.map,$(dist_prefix)/$(path)/library.map)
	$(call install-data,$(src_base)/library.xsd,$(dist_prefix)/$(path)/library.xsd)

$(dist): $(dist-common)
	$(call install-data,$(src_base)/README,$(dist_prefix)/$(path)/README)

$(dist-win): $(dist-common)
	$(call install-data,$(src_base)/README,$(dist_prefix)/$(path)/README.txt)
	$(call message,,unix2dos $(dist_prefix)/$(path)/README.txt)


# Clean.
#
$(clean): $(driver).o.clean                                \
  $(addsuffix .cxx.clean,$(obj))                           \
  $(addsuffix .cxx.clean,$(dep))                           \
  $(addprefix $(out_base)/,$(xsd:.xsd=-sskel.cxx.xsd.clean))


# Generated .gitignore.
#
ifeq ($(out_base),$(src_base))
$(skel): | $(out_base)/.gitignore
$(driver): | $(out_base)/.gitignore

$(out_base)/.gitignore: files := driver $(skelf)
$(clean): $(out_base)/.gitignore.clean

$(call include,$(bld_root)/git/gitignore.make)
endif


# How to.
#
$(call include,$(bld_root)/cxx/o-e.make)
$(call include,$(bld_root)/cxx/cxx-o.make)
$(call include,$(bld_root)/cxx/cxx-d.make)
$(call include,$(bld_root)/install.make)
$(call include,$(scf_root)/xsde/serializer/xsd-cxx.make)


# Dependencies.
#
$(call import,$(src_root)/xsde/makefile)
$(call import,$(src_root)/libxsde/xsde/makefile)
