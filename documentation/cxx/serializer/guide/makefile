# file      : documentation/cxx/serializer/guide/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../../build/bootstrap.make

default  := $(out_base)/
dist     := $(out_base)/.dist
dist-win := $(out_base)/.dist-win
cleandoc := $(out_base)/.cleandoc

# Build.
#
$(default): $(out_base)/cxx-serializer-e-guide.ps \
            $(out_base)/cxx-serializer-e-guide.pdf


$(out_base)/cxx-serializer-e-guide.ps: $(src_base)/index.xhtml   \
                                   $(src_base)/figure-1.png      \
                                   $(src_base)/guide.html2ps     \
                                   | $(out_base)/.
	$(call message,html2ps $<,html2ps -f $(src_base)/guide.html2ps -o $@ $<)

$(out_base)/cxx-serializer-e-guide.pdf: $(out_base)/cxx-serializer-e-guide.ps | $(out_base)/.
	$(call message,ps2pdf $<,ps2pdf14 $< $@)

# Dist.
#
$(dist): path := $(subst $(src_root)/,,$(src_base))
$(dist): $(out_base)/cxx-serializer-e-guide.ps $(out_base)/cxx-serializer-e-guide.pdf
	$(call install-data,$(src_base)/figure-1.png,$(dist_prefix)/$(path)/figure-1.png)
	$(call install-data,$(src_base)/index.xhtml,$(dist_prefix)/$(path)/index.xhtml)
	$(call install-data,$(out_base)/cxx-serializer-e-guide.ps,$(dist_prefix)/$(path)/cxx-serializer-e-guide.ps)
	$(call install-data,$(out_base)/cxx-serializer-e-guide.pdf,$(dist_prefix)/$(path)/cxx-serializer-e-guide.pdf)

$(dist-win): $(dist)


# Clean.
#
$(cleandoc):
	$(call message,rm $$1,rm -f $$1,$(out_base)/cxx-serializer-e-guide.ps)
	$(call message,rm $$1,rm -f $$1,$(out_base)/cxx-serializer-e-guide.pdf)

# How to.
#
$(call include,$(bld_root)/install.make)
