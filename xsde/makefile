# file      : xsde/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../build/bootstrap.make

cxx_tun := xsde.cxx


# C++
#
cxx_tun += cxx/elements.cxx

# C++/Parser
#
cxx_tun += cxx/parser/elements.cxx                        \
           cxx/parser/validator.cxx                       \
           cxx/parser/name-processor.cxx                  \
           cxx/parser/state-processor.cxx                 \
           cxx/parser/type-processor.cxx                  \
           cxx/parser/generator.cxx                       \
           cxx/parser/parser-forward.cxx                  \
           cxx/parser/parser-header.cxx                   \
           cxx/parser/parser-inline.cxx                   \
           cxx/parser/parser-source.cxx                   \
           cxx/parser/impl-header.cxx                     \
           cxx/parser/impl-source.cxx                     \
	   cxx/parser/driver-source.cxx                   \
           cxx/parser/element-validation-source.cxx       \
           cxx/parser/attribute-validation-source.cxx     \
           cxx/parser/characters-validation-source.cxx

# C++/Serializer
#
cxx_tun += cxx/serializer/elements.cxx                    \
           cxx/serializer/validator.cxx                   \
           cxx/serializer/name-processor.cxx              \
           cxx/serializer/type-processor.cxx              \
           cxx/serializer/generator.cxx                   \
           cxx/serializer/serializer-forward.cxx          \
           cxx/serializer/serializer-header.cxx           \
           cxx/serializer/serializer-inline.cxx           \
           cxx/serializer/serializer-source.cxx           \
           cxx/serializer/impl-header.cxx                 \
           cxx/serializer/impl-source.cxx                 \
	   cxx/serializer/driver-source.cxx               \
           cxx/serializer/element-validation-source.cxx   \
           cxx/serializer/attribute-validation-source.cxx

# C++/Hybrid
#
cxx_tun += cxx/hybrid/elements.cxx                        \
           cxx/hybrid/validator.cxx                       \
           cxx/hybrid/generator.cxx                       \
	   cxx/hybrid/default-value.cxx                   \
           cxx/hybrid/tree-size-processor.cxx             \
           cxx/hybrid/tree-name-processor.cxx             \
           cxx/hybrid/tree-forward.cxx                    \
           cxx/hybrid/tree-header.cxx                     \
           cxx/hybrid/tree-inline.cxx                     \
           cxx/hybrid/tree-source.cxx                     \
           cxx/hybrid/tree-type-map.cxx                   \
	   cxx/hybrid/insertion-header.cxx                \
	   cxx/hybrid/insertion-source.cxx                \
	   cxx/hybrid/extraction-header.cxx               \
	   cxx/hybrid/extraction-source.cxx               \
           cxx/hybrid/parser-name-processor.cxx           \
           cxx/hybrid/parser-header.cxx                   \
           cxx/hybrid/parser-source.cxx                   \
           cxx/hybrid/parser-aggregate-header.cxx         \
           cxx/hybrid/parser-aggregate-source.cxx         \
           cxx/hybrid/serializer-name-processor.cxx       \
           cxx/hybrid/serializer-header.cxx               \
           cxx/hybrid/serializer-source.cxx               \
           cxx/hybrid/serializer-aggregate-header.cxx     \
           cxx/hybrid/serializer-aggregate-source.cxx

# Type map
#
cxx_tun += type-map/lexer.cxx  \
           type-map/parser.cxx

# Processing
#
cxx_tun += processing/inheritance/processor.cxx


#
#
cxx_obj  := $(addprefix $(out_base)/,$(cxx_tun:.cxx=.o))
cxx_od   := $(cxx_obj:.o=.o.d)

xsde     := $(out_base)/xsde
dist     := $(out_base)/.dist
dist-win := $(out_base)/.dist-win
clean    := $(out_base)/.clean
install  := $(out_base)/.install

# Import.
#
$(call import,\
  $(scf_root)/import/libcult/stub.make,\
  l: cult.l,cpp-options: cult.l.cpp-options)

$(call import,\
  $(scf_root)/import/libbackend-elements/stub.make,\
  l: be.l,cpp-options: be.l.cpp-options)

$(call import,\
  $(scf_root)/import/libboost/regex/stub.make,\
  l: re.l,cpp-options: re.l.cpp-options)

$(call import,\
  $(scf_root)/import/libboost/filesystem/stub.make,\
  l: fs.l,cpp-options: fs.l.cpp-options)

$(call import,\
  $(scf_root)/import/libxsd-frontend/stub.make,\
  l: xsd_fe.l,cpp-options: xsd_fe.l.cpp-options)


# Build.
#
$(xsde): $(cxx_obj) $(xsd_fe.l) $(be.l) $(cult.l) $(fs.l) $(re.l)

$(cxx_obj) $(cxx_od): cpp_options := -I$(src_base)
$(cxx_obj) $(cxx_od):     \
  $(xsd_fe.l.cpp-options) \
  $(be.l.cpp-options)     \
  $(cult.l.cpp-options)   \
  $(fs.l.cpp-options)     \
  $(re.l.cpp-options)


$(call include-dep,$(cxx_od))

# Alias for default target.
#
$(out_base)/: $(xsde)

# Dist.
#
$(dist): $(xsde)
	$(call install-exec,$<,$(dist_prefix)/bin/xsde)

$(dist-win): $(xsde)
	$(call install-exec,$<,$(dist_prefix)/bin/xsde.exe)

# Install.
#
$(install): $(xsde)
	$(call install-exec,$<,$(install_bin_dir)/xsde)

# Clean.
#
$(clean):                            \
  $(xsde).o.clean                    \
  $(addsuffix .cxx.clean,$(cxx_obj)) \
  $(addsuffix .cxx.clean,$(cxx_od))


# Generated .gitignore.
#
ifeq ($(out_base),$(src_base))
$(xsde): | $(out_base)/.gitignore

$(out_base)/.gitignore: files := xsde
$(clean): $(out_base)/.gitignore.clean

$(call include,$(bld_root)/git/gitignore.make)
endif


# how to
#
$(call include,$(bld_root)/cxx/o-e.make)
$(call include,$(bld_root)/cxx/cxx-o.make)
$(call include,$(bld_root)/cxx/cxx-d.make)
$(call include,$(bld_root)/install.make)
