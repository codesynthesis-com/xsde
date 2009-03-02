# Sample configuration file for iPhone OS 2.x using XCode 3.1.x iPhone
# SDK.
#
XCODE           := /Developer
IPHONE_PLATFORM := $(XCODE)/Platforms/iPhoneOS.platform/Developer
IPHONE_SDK      := $(IPHONE_PLATFORM)/SDKs/iPhoneOS2.0.sdk

# Toolchain.
#
CC       := $(IPHONE_PLATFORM)/usr/bin/arm-apple-darwin9-gcc-4.0.1
CFLAGS   := -W -Wall -arch armv6 -O3 -isysroot $(IPHONE_SDK)
CPPFLAGS :=

CXX      := $(IPHONE_PLATFORM)/usr/bin/arm-apple-darwin9-g++-4.0.1
CXXFLAGS := -W -Wall -arch armv6 -O3 -isysroot $(IPHONE_SDK)

LD       := $(CXX)
LDFLAGS  := $(CXXFLAGS)
LIBS     :=

# Set RANLIB to empty if your system does not need ranlib.
#
AR       := $(IPHONE_PLATFORM)/usr/bin/ar
ARFLAGS  := rc
RANLIB   := $(IPHONE_PLATFORM)/usr/bin/ranlib


# Common XSD/e flags.
#
XSDFLAGS := --generate-inline


# Platform. Valid values are:
#
# 'wince'  - Windows CE
# 'win32'  - Windows 2000, XP, etc.
# 'posix'  - POSIX OS, including UNIX/Linux, VxWorks, etc.
#
XSDE_PLATFORM   := posix


# Platform architecture width in bits.
#
XSDE_ARCH_WIDTH := 32


# Platform byte order. Valid values are 'b' for big-endian
# and 'l' for little-endian.
#
XSDE_BYTEORDER  := l


# Set to 'n' if you don't have STL (std::string, etc.). Also don't
# forget to use the --no-stl option when compiling your schemas.
#
XSDE_STL        := y


# Set to 'n' if you don't have iostream.
#
XSDE_IOSTREAM   := y


# Set to 'n' if you don't have C++ exceptions. Also don't forget to
# use the --no-exceptions option when compiling your schemas.
#
XSDE_EXCEPTIONS := y


# Set to 'n' if your platform doesn't have the "long long int" type or
# the strtoull function. Also don't forget to use the --no-long-long
# option when compiling your schemas.
#
XSDE_LONGLONG   := y


# Set to 'n' if your platform doesn't have the snprintf function.
#
XSDE_SNPRINTF   := y


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
