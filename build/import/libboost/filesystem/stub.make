# file      : build/import/libboost/filesystem/stub.make
# author    : Boris Kolpackov <boris@kolpackov.net>
# copyright : Copyright (c) 2005-2009 Boris Kolpackov
# license   : GNU GPL v2; see accompanying LICENSE file

$(call include-once,$(scf_root)/import/libboost/configuration-rules.make,$(dcf_root))

libboost_installed :=

$(call -include,$(dcf_root)/import/libboost/configuration-dynamic.make)

ifdef libboost_installed

ifeq ($(libboost_installed),y)

$(call export,l: -lboost_filesystem,cpp_options: )

else

$(call include-once,$(scf_root)/import/libboost/filesystem/rules.make,$(dcf_root))

$(call export,\
  l: $(dcf_root)/import/libboost/filesystem/filesystem.l,\
  cpp-options: $(dcf_root)/import/libboost/filesystem/filesystem.l.cpp-options)

endif

else

.NOTPARALLEL:

endif
