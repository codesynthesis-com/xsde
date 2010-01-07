# Sample configuration file for VxWorks 5.5.1 on PowerPC 603 using
# Tornado 2.2 GNU toolchain. While this configuration disables STL
# and C++ exceptions, those are supported on VxWorks and can be
# enabled.
#

# Toolchain.
#
CC       := ccppc
CFLAGS   := -W -Wall -O3 -mlongcall -mcpu=603 -mstrict-align -ansi -DCPU=PPC603 -DTOOL_FAMILY=gnu -DTOOL=gnu -fsigned-char
CPPFLAGS :=

CXX      := ccppc
CXXFLAGS := -W -Wall -O3 -mlongcall -mcpu=603 -mstrict-align -ansi -DCPU=PPC603 -DTOOL_FAMILY=gnu -DTOOL=gnu -fsigned-char

LD       := $(CXX)
LDFLAGS  := $(CXXFLAGS) -r -nostdlib -Wl,-X
LIBS     :=

# Set RANLIB to empty if your system does not need ranlib.
#
AR       := arppc
ARFLAGS  := rc
RANLIB   := ranlibppc


# Common XSD/e flags.
#
XSDFLAGS := --generate-inline


# Platform. Valid values are:
#
# 'wince'  - Windows CE
# 'win32'  - Windows 2000, XP, etc.
# 'posix'  - POSIX OS, including UNIX/Linux, VxWorks, etc.
#
XSDE_PLATFORM     := posix


# Platform architecture width in bits.
#
XSDE_ARCH_WIDTH   := 32


# Platform byte order. Valid values are 'b' for big-endian
# and 'l' for little-endian.
#
XSDE_BYTEORDER    := b


# Application character encoding. Valid values are 'utf8' for UTF-8
# and 'iso8859-1' for ISO-8859-1. Note that this encoding is not
# the same as the XML document encoding that is being parsed or
# serialized. Rather, it is the encoding that is used inside the
# application. When an XML document is parsed, the character data
# is automatically converted to the application encoding. Similarly,
# when an XML document is serialized, the data in the application
# encoding is automatically converted to the resulting document
# encoding. Also don't forget to use the --char-encoding option
# when compiling your schemas if using an encoding other than UTF-8.
#
XSDE_ENCODING     := utf8


# Set to 'n' if you don't have STL (std::string, etc.). Also don't
# forget to use the --no-stl option when compiling your schemas.
#
XSDE_STL          := n


# Set to 'n' if you don't want iterators to conform to the STL
# requirements. This feature requires working <iterator> header
# and allows you to use the standard algorithms such as find_if,
# etc.
#
XSDE_STL_ITERATOR := n


# Set to 'n' if you don't have iostream.
#
XSDE_IOSTREAM     := n


# Set to 'n' if you don't have C++ exceptions. Also don't forget to
# use the --no-exceptions option when compiling your schemas.
#
XSDE_EXCEPTIONS   := n


# Set to 'n' if your platform doesn't have the "long long int" type or
# the strtoull function. Also don't forget to use the --no-long-long
# option when compiling your schemas.
#
XSDE_LONGLONG     := n


# Set to 'n' if your platform doesn't have the snprintf function.
#
XSDE_SNPRINTF     := n


# Set to 'n' if you don't want support for XML Schema validation in
# C++/Parser. Also don't forget to use the --suppress-validation
# option when compiling your schemas.
#
XSDE_PARSER_VALIDATION := y


# Set to 'n' if you don't want support for XML Schema validation in
# C++/Serializer. Also don't forget to use the --suppress-validation
# option when compiling your schemas.
#
XSDE_SERIALIZER_VALIDATION := y


# Base parser/serializer implementation reuse style. Valid values are:
#
# 'mixin'  - virtual inheritance-based reuse (specify --reuse-style-mixin)
# 'tiein'  - delegation-based reuse (recommended)
# 'none'   - no reuse support (specify --reuse-style-none)
#
XSDE_REUSE_STYLE := tiein


# Set to 'y' if you want support for serialization of the C++/Hybrid
# object model to the CDR (Common Data Representation) binary format.
# This functionality requires the ACE library.
#
XSDE_CDR := n


# Set to 'y' if you want support for serialization of the C++/Hybrid
# object model to the XDR (eXternal Data Representation) binary format.
# This functionality requires the XDR API which is available out of the
# box on most POSIX systems as part of Sun RPC. On some systems (e.g.,
# (Linux, VxWorks, iPhone OS) this API is part of libc in which case
# you don't need to link anything extra. On other platforms, the XDR
# API may require linking to another library (which you can add to the
# LIBS variable above), such as -lrpc (QNX, LynxOS) or -lnsl. On non-
# POSIX platforms you may need to install a third-party library which
# provides the XDR API. Also note that some older versions of the API
# (e.g., those found on LynxOS) may not support serialization of the
# long long type. In this case you will get a compilation error saying
# that xdr_longlong_t and xdr_u_longlong_t are not declared. One way to
# resolve this is to disable the use of the long long type in XSD/e (see
# XSDE_LONGLONG above).
#
XSDE_XDR := n


# Set to 'y' if you need to handle XML vocabularies that use XML Schema
# polymorphism (xsi:type or substitution groups). Also don't forget to
# use either --generate-polymorphic (generates polymorphism-aware code)
# or --runtime-polymorphic (generates non-polymorphic code that uses the
# runtime library configured with polymorphism support). Note that support
# for XML Schema polymorphism requires runtime static initialization
# support in the C++ compiler (that is, support for automatic calling
# of constructors for static objects). Furthermore, if the mixin reuse
# style is used (XSDE_REUSE_STYLE) then the generated code requires
# support for dynamic_cast.
#
XSDE_POLYMORPHIC := n


# When polymorphism support is enabled (XSDE_POLYMORPHIC), the following
# parameters control the substitution and inheritance hashmaps bucket
# allocation. Because the number of elements in these hashmaps depends
# on the schemas being compiled and thus is fairly static, these hashmaps
# do not perform automatic table resizing. To obtain good performance the
# elements to buckets ratio should be between 0.7 and 0.9. The recommended
# way to ensure this range is to add diagnostics code to your application
# as shown in the documentation and examples. It is also a good idea to
# use prime numbers for bucket counts: 53 97 193 389 769 1543 3079 6151
# 12289 24593 49157 98317 196613 393241. Inheritance hashmaps are only
# used when validation is enabled.
#
XSDE_PARSER_SMAP_BUCKETS := 53
XSDE_PARSER_IMAP_BUCKETS := 97
XSDE_SERIALIZER_SMAP_BUCKETS := 53
XSDE_SERIALIZER_SMAP_BUCKET_BUCKETS := 53
XSDE_SERIALIZER_IMAP_BUCKETS := 97


# Options tuning depending on the features selected.
#
ifeq ($(XSDE_EXCEPTIONS),y)
CFLAGS += -fexceptions
endif
