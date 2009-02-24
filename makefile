# file      : makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))build/bootstrap.make

default := $(out_base)/
test    := $(out_base)/.test
install := $(out_base)/.install
clean   := $(out_base)/.clean

.PHONY: $(default) $(test) $(install) $(clean)

$(default): $(out_base)/xsde/      \
            $(out_base)/libxsde/   \
            $(out_base)/tests/      \
            $(out_base)/examples/

$(test): $(out_base)/tests/.test

$(install): $(out_base)/xsde/.install           \
            $(out_base)/libxsde/.install        \
            $(out_base)/documentation/.install
	$(call install-data,$(src_base)/FLOSSE,$(install_doc_dir)/xsde/FLOSSE)
	$(call install-data,$(src_base)/GPLv2,$(install_doc_dir)/xsde/GPLv2)
	$(call install-data,$(src_base)/LICENSE,$(install_doc_dir)/xsde/LICENSE)
	$(call install-data,$(src_base)/NEWS,$(install_doc_dir)/xsde/NEWS)
	$(call install-data,$(src_base)/README,$(install_doc_dir)/xsde/README)


$(clean): $(out_base)/xsde/.clean      \
          $(out_base)/libxsde/.clean   \
          $(out_base)/tests/.clean      \
          $(out_base)/examples/.clean

$(call include,$(bld_root)/install.make)

$(call import,$(src_base)/xsde/makefile)
$(call import,$(src_base)/libxsde/makefile)
$(call import,$(src_base)/tests/makefile)
$(call import,$(src_base)/examples/makefile)
$(call import,$(src_base)/documentation/makefile)
