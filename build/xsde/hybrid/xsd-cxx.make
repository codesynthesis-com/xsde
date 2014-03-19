# file      : build/xsde/hybrid/xsd-cxx.make
# copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#@@ Need to use extensions from cxx config.
#

$(call include,$(scf_root)/configuration.make)

ifeq ($(xsd_pskel_suffix),)
xsd_pskel_suffix := -pskel
endif

ifeq ($(xsd_pimpl_suffix),)
xsd_pimpl_suffix := -pimpl
endif

ifeq ($(xsd_sskel_suffix),)
xsd_sskel_suffix := -sskel
endif

ifeq ($(xsd_simpl_suffix),)
xsd_simpl_suffix := -simpl
endif

xsd_hybrid_pattern := \
$(out_base)/%.cxx     \
$(out_base)/%.hxx     \
$(out_base)/%.ixx

xsd_hybrid_pattern +=                \
$(out_base)/%$(xsd_pskel_suffix).cxx \
$(out_base)/%$(xsd_pskel_suffix).hxx \
$(out_base)/%$(xsd_pskel_suffix).ixx

xsd_hybrid_pattern +=                \
$(out_base)/%$(xsd_pimpl_suffix).cxx \
$(out_base)/%$(xsd_pimpl_suffix).hxx

xsd_hybrid_pattern +=                \
$(out_base)/%$(xsd_sskel_suffix).cxx \
$(out_base)/%$(xsd_sskel_suffix).hxx \
$(out_base)/%$(xsd_sskel_suffix).ixx

xsd_hybrid_pattern +=                \
$(out_base)/%$(xsd_simpl_suffix).cxx \
$(out_base)/%$(xsd_simpl_suffix).hxx

$(xsd_hybrid_pattern): xsde := xsde
$(xsd_hybrid_pattern): xsde_command := cxx-hybrid

ops :=                                  \
--generate-inline                       \
--pskel-file-suffix $(xsd_pskel_suffix) \
--pimpl-file-suffix $(xsd_pimpl_suffix) \
--sskel-file-suffix $(xsd_sskel_suffix) \
--simpl-file-suffix $(xsd_simpl_suffix)

ops += --char-encoding $(xsde_encoding)

ifeq ($(xsde_stl),n)
ops += --no-stl
endif

ifeq ($(xsde_iostream),n)
ops += --no-iostream
endif

ifeq ($(xsde_exceptions),n)
ops += --no-exceptions
endif

ifeq ($(xsde_longlong),n)
ops += --no-long-long
endif

ifeq ($(xsde_parser_validation),n)
ops += --suppress-parser-val
endif

ifeq ($(xsde_serializer_validation),n)
ops += --suppress-serializer-val
endif

ifeq ($(xsde_polymorphic),y)
ops += --runtime-polymorphic
endif

ifeq ($(xsde_reuse_style),mixin)
ops += --reuse-style-mixin
endif

ifeq ($(xsde_reuse_style),none)
$(error Hybrid mapping requires support for base parser/serializer reuse)
endif

ifeq ($(xsde_custom_allocator),y)
ops += $(xsde_options) --custom-allocator
endif

$(xsd_hybrid_pattern): xsde_options := $(ops)

.PRECIOUS: $(xsd_hybrid_pattern)

ifeq ($(out_base),$(src_base))

$(xsd_hybrid_pattern): $(src_base)/%.xsd
	$(call message,xsde $<,$(xsde) $(xsde_command) $(xsde_options) --output-dir $(dir $@) $<)

else

$(xsd_hybrid_pattern): $(src_base)/%.xsd | $$(dir $$@).
	$(call message,xsde $<,$(xsde) $(xsde_command) $(xsde_options) --output-dir $(dir $@) $<)

$(xsd_hybrid_pattern): $(out_base)/%.xsd | $$(dir $$@).
	$(call message,xsde $<,$(xsde) $(xsde_command) $(xsde_options) --output-dir $(dir $@) $<)

endif


.PHONY: $(out_base)/%.cxx.xsd.clean

$(out_base)/%.cxx.xsd.clean: pskel := $(xsd_pskel_suffix)
$(out_base)/%.cxx.xsd.clean: pimpl := $(xsd_pimpl_suffix)
$(out_base)/%.cxx.xsd.clean: sskel := $(xsd_sskel_suffix)
$(out_base)/%.cxx.xsd.clean: simpl := $(xsd_simpl_suffix)

$(out_base)/%.cxx.xsd.clean:
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=.cxx))
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=.hxx))
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=.ixx))
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=$(pskel).cxx))
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=$(pskel).hxx))
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=$(pskel).ixx))
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=$(pimpl).cxx))
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=$(pimpl).hxx))
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=$(sskel).cxx))
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=$(sskel).hxx))
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=$(sskel).ixx))
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=$(simpl).cxx))
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=$(simpl).hxx))


# Reset the config variables so they won't take effect in other places.
#
xsd_pskel_suffix :=
xsd_pimpl_suffix :=
xsd_sskel_suffix :=
xsd_simpl_suffix :=
