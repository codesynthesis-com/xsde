# file      : doc/cxx/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2011 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../build/bootstrap.make

mappings := hybrid/guide parser/guide serializer/guide

default  := $(out_base)/
dist     := $(out_base)/.dist
dist-win := $(out_base)/.dist-win
clean    := $(out_base)/.clean

$(default): $(addprefix $(out_base)/,$(addsuffix /,$(mappings)))
$(dist): $(addprefix $(out_base)/,$(addsuffix /.dist,$(mappings)))
$(dist-win): $(addprefix $(out_base)/,$(addsuffix /.dist-win,$(mappings)))
$(clean): $(addprefix $(out_base)/,$(addsuffix /.clean,$(mappings)))

$(foreach m,$(mappings),$(call import,$(src_base)/$m/makefile))
