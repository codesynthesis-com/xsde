# file      : build/configuration.make
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

$(call include-once,$(scf_root)/configuration-rules.make,$(dcf_root))


# Dynamic configuration.
#
xsde_arch_width                     :=
xsde_byteorder                      :=
xsde_encoding                       :=
xsde_stl                            :=
xsde_stl_iterator                   :=
xsde_iostream                       :=
xsde_exceptions                     :=
xsde_longlong                       :=
xsde_snprintf                       :=
xsde_parser_validation              :=
xsde_serializer_validation          :=
xsde_regexp                         :=
xsde_reuse_style                    :=
xsde_custom_allocator               :=
xsde_default_allocator              :=
xsde_xdr                            :=
xsde_cdr                            :=
xsde_polymorphic                    :=
xsde_parser_smap_buckets            :=
xsde_parser_imap_buckets            :=
xsde_serializer_smap_buckets        :=
xsde_serializer_smap_bucket_buckets :=
xsde_serializer_imap_buckets        :=


$(call -include,$(dcf_root)/configuration-dynamic.make)

ifdef xsde_stl

$(out_root)/%: xsde_arch_width                     := $(xsde_arch_width)
$(out_root)/%: xsde_byteorder                      := $(xsde_byteorder)
$(out_root)/%: xsde_encoding                       := $(xsde_encoding)
$(out_root)/%: xsde_stl                            := $(xsde_stl)
$(out_root)/%: xsde_stl_iterator                   := $(xsde_stl_iterator)
$(out_root)/%: xsde_iostream                       := $(xsde_iostream)
$(out_root)/%: xsde_exceptions                     := $(xsde_exceptions)
$(out_root)/%: xsde_longlong                       := $(xsde_longlong)
$(out_root)/%: xsde_snprintf                       := $(xsde_snprintf)
$(out_root)/%: xsde_parser_validation              := $(xsde_parser_validation)
$(out_root)/%: xsde_serializer_validation          := $(xsde_serializer_validation)
$(out_root)/%: xsde_regexp                         := $(xsde_regexp)
$(out_root)/%: xsde_reuse_style                    := $(xsde_reuse_style)
$(out_root)/%: xsde_custom_allocator               := $(xsde_custom_allocator)
$(out_root)/%: xsde_default_allocator              := $(xsde_default_allocator)
$(out_root)/%: xsde_xdr                            := $(xsde_xdr)
$(out_root)/%: xsde_cdr                            := $(xsde_cdr)
$(out_root)/%: xsde_polymorphic                    := $(xsde_polymorphic)
$(out_root)/%: xsde_parser_smap_buckets            := $(xsde_parser_smap_buckets)
$(out_root)/%: xsde_parser_imap_buckets            := $(xsde_parser_imap_buckets)
$(out_root)/%: xsde_serializer_smap_buckets        := $(xsde_serializer_smap_buckets)
$(out_root)/%: xsde_serializer_smap_bucket_buckets := $(xsde_serializer_smap_bucket_buckets)
$(out_root)/%: xsde_serializer_imap_buckets        := $(xsde_serializer_imap_buckets)

else

.NOTPARALLEL:

endif
