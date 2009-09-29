# file      : build/import/libboost/filesystem/rules.make
# author    : Boris Kolpackov <boris@kolpackov.net>
# copyright : Copyright (c) 2005-2008 Boris Kolpackov
# license   : GNU GPL v2; see accompanying LICENSE file

#@@ Should use message everywhere.
#

$(dcf_root)/import/libboost/%: root := $(libboost_root)

$(dcf_root)/import/libboost/filesystem/filesystem.l: \
  | $(dcf_root)/import/libboost/filesystem/.

ifeq ($(libboost_type),archive)

$(dcf_root)/import/libboost/filesystem/filesystem.l: \
  $(libboost_root)/stage/lib/libboost_filesystem$(libboost_suffix).a
	@echo $< >$@

else

$(dcf_root)/import/libboost/filesystem/filesystem.l: \
  $(libboost_root)/stage/lib/libboost_filesystem$(libboost_suffix).so
	@echo $< >$@
	@echo rpath:$(root)/stage/lib >>$@

endif


$(dcf_root)/import/libboost/filesystem/filesystem.l.cpp-options: \
  | $(dcf_root)/import/libboost/filesystem/.
	@echo include: -I$(root) >$@

ifndef %foreign%

disfigure::
	$(call message,rm $(dcf_root)/import/libboost/filesystem/filesystem.l,\
rm -f $(dcf_root)/import/libboost/filesystem/filesystem.l)
	$(call message,,rm -f $(dcf_root)/import/libboost/filesystem/filesystem.l.cpp-options)

endif
