# file      : libxsde/makefile
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../build/bootstrap.make

default  := $(out_base)/
dist     := $(out_base)/.dist
dist-win := $(out_base)/.dist-win
clean    := $(out_base)/.clean

$(default): $(out_base)/xsde/
$(dist): $(out_base)/xsde/.dist
$(dist-win): $(out_base)/xsde/.dist-win
$(clean): $(out_base)/xsde/.clean

$(call import,$(src_base)/xsde/makefile)
