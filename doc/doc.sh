#! /usr/bin/env bash

version=3.3.0.a10

trap 'exit 1' ERR
set -o errtrace # Trap in functions.

function info () { echo "$*" 1>&2; }
function error () { info "$*"; exit 1; }

date="$(date +"%B %Y")"
copyright="$(sed -n -re 's%^Copyright \(c\) (.+)\.$%\1%p' ../LICENSE)"

while [ $# -gt 0 ]; do
  case $1 in
    --clean)
      rm -f xsde.xhtml xsde.1
      rm -f cxx/hybrid/guide/cxx-hybrid-e-guide.{ps,pdf}
      rm -f cxx/parser/guide/cxx-parser-e-guide.{ps,pdf}
      rm -f cxx/serializer/guide/cxx-serializer-e-guide.{ps,pdf}
      exit 0
      ;;
    *)
      error "unexpected $1"
      ;;
  esac
done

ops=(\
-v project="xsde" \
-v version="$version" \
-v date="$date" \
-v copyright="$copyright" \
-I ../xsde \
--stdout \
--suppress-undocumented \
--exclude-base)

# XHTML
#
# Common options.
#
cli "${ops[@]}" --generate-html --class CXX::options --class options \
--html-prologue-file xsde-prologue.xhtml \
../xsde/cxx/options.cli >xsde.xhtml

# C++/Hybrid options.
#
cli "${ops[@]}" --generate-html \
--html-prologue-file xsde-hybrid-header.xhtml \
../xsde/cxx/hybrid/options.cli >>xsde.xhtml

# C++/Parser options.
#
cli "${ops[@]}" --generate-html \
--html-prologue-file xsde-parser-header.xhtml \
../xsde/cxx/parser/options.cli >>xsde.xhtml

# C++/Serializer options.
#
cli "${ops[@]}" --generate-html \
--html-prologue-file xsde-serializer-header.xhtml \
--html-epilogue-file xsde-epilogue.xhtml \
../xsde/cxx/serializer/options.cli >>xsde.xhtml

# MAN
#
# Common options.
#
cli "${ops[@]}" --generate-man --class CXX::options --class options \
--man-prologue-file xsde-prologue.1 \
../xsde/cxx/options.cli >xsde.1

# C++/Hybrid options.
#
cli "${ops[@]}" --generate-man \
--man-prologue-file xsde-hybrid-header.1 \
../xsde/cxx/hybrid/options.cli >>xsde.1

# C++/Parser options.
#
cli "${ops[@]}" --generate-man \
--man-prologue-file xsde-parser-header.1 \
../xsde/cxx/parser/options.cli >>xsde.1

# C++/Serializer options.
#
cli "${ops[@]}" --generate-man \
--man-prologue-file xsde-serializer-header.1 \
--man-epilogue-file xsde-epilogue.1 \
../xsde/cxx/serializer/options.cli >>xsde.1

# PS, PDF
#
# C++/Hybrid guide.
#
html2ps -f cxx/hybrid/guide/guide.html2ps \
-o cxx/hybrid/guide/cxx-hybrid-e-guide.ps cxx/hybrid/guide/index.xhtml

ps2pdf14 -dOptimize=true -dEmbedAllFonts=true \
cxx/hybrid/guide/cxx-hybrid-e-guide.ps cxx/hybrid/guide/cxx-hybrid-e-guide.pdf

# C++/Parser guide.
#
html2ps -f cxx/parser/guide/guide.html2ps \
-o cxx/parser/guide/cxx-parser-e-guide.ps cxx/parser/guide/index.xhtml

ps2pdf14 -dOptimize=true -dEmbedAllFonts=true \
cxx/parser/guide/cxx-parser-e-guide.ps cxx/parser/guide/cxx-parser-e-guide.pdf

# C++/Serializer guide.
#
html2ps -f cxx/serializer/guide/guide.html2ps \
-o cxx/serializer/guide/cxx-serializer-e-guide.ps cxx/serializer/guide/index.xhtml

ps2pdf14 -dOptimize=true -dEmbedAllFonts=true \
cxx/serializer/guide/cxx-serializer-e-guide.ps cxx/serializer/guide/cxx-serializer-e-guide.pdf
