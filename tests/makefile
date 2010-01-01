# file      : tests/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../build/bootstrap.make

# NOTE: remember to update dist/tests/{makefile,nmakefile} if
# you change anything here.
#

default  := $(out_base)/
test     := $(out_base)/.test
dist     := $(out_base)/.dist
dist-win := $(out_base)/.dist-win
clean    := $(out_base)/.clean

$(default): $(out_base)/cxx/
$(test): $(out_base)/cxx/.test
$(dist): $(out_base)/cxx/.dist
$(dist-win): $(out_base)/cxx/.dist-win
$(clean): $(out_base)/cxx/.clean

$(call import,$(src_base)/cxx/makefile)
