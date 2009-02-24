# file      : libxsde/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../build/bootstrap.make

default := $(out_base)/
install := $(out_base)/.install
clean   := $(out_base)/.clean

.PHONY: $(default) $(install) $(clean)

$(default): $(out_base)/xsde/
$(clean): $(out_base)/xsde/.clean

$(install):
	$(call install-dir,$(src_base)/xsde,$(install_inc_dir)/xsde)
	$(call install-data,$(src_base)/GPLv2,$(install_doc_dir)/libxsde/GPLv2)
	$(call install-data,$(src_base)/LICENSE,$(install_doc_dir)/libxsde/LICENSE)
	$(call install-data,$(src_base)/README,$(install_doc_dir)/libxsde/README)


$(call include,$(bld_root)/install.make)

$(call import,$(src_base)/xsde/makefile)
