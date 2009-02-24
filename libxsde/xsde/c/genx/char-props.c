/*
 * Copyright (c) 2007-2009 Code Synthesis Tools CC.
 * Copyright (c) 2004 by Tim Bray and Sun Microsystems.
 *
 * For copying permission, see the accompanying COPYING file.
 */

/*
 * Construct character-properties tables for genx.
 * Quite likely there's a better way.
 * This version is generated semi-automatically from the source code of the
 *  XML specification via emacs global replace and keyboard macros
 */
#include "genx.h"

static void charProp(char * p, int c, int prop)
{
  p[c] |= prop;
}

static void rangeProp(char * p, int start, int end, int prop)
{
  int i;
  for (i = start; i <= end; i++)
    p[i] |= prop;
}

void genxSetCharProps(char * p)
{
  int i;

  for (i = 0; i <= 0xffff; i++)
    p[i] = 0;

  /* per XML 1.0 */
  charProp(p, 0x9, GENX_XML_CHAR);
  charProp(p, 0xa, GENX_XML_CHAR);
  charProp(p, 0xd, GENX_XML_CHAR);
  rangeProp(p, 0x20, 0xd7ff, GENX_XML_CHAR);
  rangeProp(p, 0xe000, 0xfffd, GENX_XML_CHAR);

  /* Letter ::= BaseChar | Ideographic */
  rangeProp(p, 0x0041, 0x005A, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0061, 0x007A, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x00C0, 0x00D6, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x00D8, 0x00F6, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x00F8, 0x00FF, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0100, 0x0131, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0134, 0x013E, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0141, 0x0148, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x014A, 0x017E, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0180, 0x01C3, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x01CD, 0x01F0, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x01F4, 0x01F5, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x01FA, 0x0217, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0250, 0x02A8, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x02BB, 0x02C1, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0386, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0388, 0x038A, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x038C, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x038E, 0x03A1, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x03A3, 0x03CE, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x03D0, 0x03D6, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x03DA, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x03DC, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x03DE, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x03E0, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x03E2, 0x03F3, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0401, 0x040C, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x040E, 0x044F, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0451, 0x045C, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x045E, 0x0481, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0490, 0x04C4, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x04C7, 0x04C8, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x04CB, 0x04CC, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x04D0, 0x04EB, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x04EE, 0x04F5, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x04F8, 0x04F9, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0531, 0x0556, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0559, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0561, 0x0586, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x05D0, 0x05EA, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x05F0, 0x05F2, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0621, 0x063A, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0641, 0x064A, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0671, 0x06B7, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x06BA, 0x06BE, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x06C0, 0x06CE, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x06D0, 0x06D3, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x06D5, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x06E5, 0x06E6, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0905, 0x0939, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x093D, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0958, 0x0961, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0985, 0x098C, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x098F, 0x0990, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0993, 0x09A8, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x09AA, 0x09B0, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x09B2, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x09B6, 0x09B9, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x09DC, 0x09DD, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x09DF, 0x09E1, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x09F0, 0x09F1, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0A05, 0x0A0A, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0A0F, 0x0A10, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0A13, 0x0A28, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0A2A, 0x0A30, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0A32, 0x0A33, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0A35, 0x0A36, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0A38, 0x0A39, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0A59, 0x0A5C, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0A5E, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0A72, 0x0A74, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0A85, 0x0A8B, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0A8D, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0A8F, 0x0A91, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0A93, 0x0AA8, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0AAA, 0x0AB0, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0AB2, 0x0AB3, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0AB5, 0x0AB9, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0ABD, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0AE0, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0B05, 0x0B0C, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0B0F, 0x0B10, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0B13, 0x0B28, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0B2A, 0x0B30, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0B32, 0x0B33, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0B36, 0x0B39, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0B3D, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0B5C, 0x0B5D, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0B5F, 0x0B61, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0B85, 0x0B8A, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0B8E, 0x0B90, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0B92, 0x0B95, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0B99, 0x0B9A, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0B9C, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0B9E, 0x0B9F, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0BA3, 0x0BA4, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0BA8, 0x0BAA, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0BAE, 0x0BB5, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0BB7, 0x0BB9, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0C05, 0x0C0C, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0C0E, 0x0C10, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0C12, 0x0C28, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0C2A, 0x0C33, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0C35, 0x0C39, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0C60, 0x0C61, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0C85, 0x0C8C, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0C8E, 0x0C90, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0C92, 0x0CA8, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0CAA, 0x0CB3, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0CB5, 0x0CB9, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0CDE, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0CE0, 0x0CE1, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0D05, 0x0D0C, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0D0E, 0x0D10, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0D12, 0x0D28, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0D2A, 0x0D39, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0D60, 0x0D61, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0E01, 0x0E2E, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0E30, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0E32, 0x0E33, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0E40, 0x0E45, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0E81, 0x0E82, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0E84, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0E87, 0x0E88, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0E8A, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0E8D, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0E94, 0x0E97, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0E99, 0x0E9F, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0EA1, 0x0EA3, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0EA5, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0EA7, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0EAA, 0x0EAB, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0EAD, 0x0EAE, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0EB0, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0EB2, 0x0EB3, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0EBD, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0EC0, 0x0EC4, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0F40, 0x0F47, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0F49, 0x0F69, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x10A0, 0x10C5, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x10D0, 0x10F6, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x1100, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1102, 0x1103, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1105, 0x1107, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x1109, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x110B, 0x110C, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x110E, 0x1112, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x113C, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x113E, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x1140, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x114C, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x114E, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x1150, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1154, 0x1155, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x1159, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x115F, 0x1161, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x1163, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x1165, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x1167, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x1169, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x116D, 0x116E, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1172, 0x1173, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x1175, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x119E, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x11A8, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x11AB, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x11AE, 0x11AF, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x11B7, 0x11B8, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x11BA, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x11BC, 0x11C2, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x11EB, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x11F0, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x11F9, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1E00, 0x1E9B, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1EA0, 0x1EF9, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1F00, 0x1F15, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1F18, 0x1F1D, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1F20, 0x1F45, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1F48, 0x1F4D, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1F50, 0x1F57, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x1F59, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x1F5B, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x1F5D, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1F5F, 0x1F7D, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1F80, 0x1FB4, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1FB6, 0x1FBC, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x1FBE, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1FC2, 0x1FC4, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1FC6, 0x1FCC, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1FD0, 0x1FD3, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1FD6, 0x1FDB, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1FE0, 0x1FEC, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1FF2, 0x1FF4, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x1FF6, 0x1FFC, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x2126, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x212A, 0x212B, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x212E, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x2180, 0x2182, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x3041, 0x3094, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x30A1, 0x30FA, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x3105, 0x312C, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0xAC00, 0xD7A3, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x4E00, 0x9FA5, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x3007, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x3021, 0x3029, GENX_LETTER|GENX_NAMECHAR);

  /*
   * NameChar ::=
   *   Letter | Digit | '.' | '-' | '_' | ':' | CombiningChar | Extender
   */
  rangeProp(p, 0x0030, 0x0039, GENX_NAMECHAR);
  rangeProp(p, 0x0660, 0x0669, GENX_NAMECHAR);
  rangeProp(p, 0x06F0, 0x06F9, GENX_NAMECHAR);
  rangeProp(p, 0x0966, 0x096F, GENX_NAMECHAR);
  rangeProp(p, 0x09E6, 0x09EF, GENX_NAMECHAR);
  rangeProp(p, 0x0A66, 0x0A6F, GENX_NAMECHAR);
  rangeProp(p, 0x0AE6, 0x0AEF, GENX_NAMECHAR);
  rangeProp(p, 0x0B66, 0x0B6F, GENX_NAMECHAR);
  rangeProp(p, 0x0BE7, 0x0BEF, GENX_NAMECHAR);
  rangeProp(p, 0x0C66, 0x0C6F, GENX_NAMECHAR);
  rangeProp(p, 0x0CE6, 0x0CEF, GENX_NAMECHAR);
  rangeProp(p, 0x0D66, 0x0D6F, GENX_NAMECHAR);
  rangeProp(p, 0x0E50, 0x0E59, GENX_NAMECHAR);
  rangeProp(p, 0x0ED0, 0x0ED9, GENX_NAMECHAR);
  rangeProp(p, 0x0F20, 0x0F29, GENX_NAMECHAR);

  charProp(p, '.', GENX_NAMECHAR);
  charProp(p, '-', GENX_NAMECHAR);
  charProp(p, '_', GENX_NAMECHAR);

  rangeProp(p, 0x0300, 0x0345, GENX_NAMECHAR);
  rangeProp(p, 0x0360, 0x0361, GENX_NAMECHAR);
  rangeProp(p, 0x0483, 0x0486, GENX_NAMECHAR);
  rangeProp(p, 0x0591, 0x05A1, GENX_NAMECHAR);
  rangeProp(p, 0x05A3, 0x05B9, GENX_NAMECHAR);
  rangeProp(p, 0x05BB, 0x05BD, GENX_NAMECHAR);
  charProp(p, 0x05BF, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x05C1, 0x05C2, GENX_NAMECHAR);
  charProp(p, 0x05C4, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x064B, 0x0652, GENX_NAMECHAR);
  charProp(p, 0x0670, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x06D6, 0x06DC, GENX_NAMECHAR);
  rangeProp(p, 0x06DD, 0x06DF, GENX_NAMECHAR);
  rangeProp(p, 0x06E0, 0x06E4, GENX_NAMECHAR);
  rangeProp(p, 0x06E7, 0x06E8, GENX_NAMECHAR);
  rangeProp(p, 0x06EA, 0x06ED, GENX_NAMECHAR);
  rangeProp(p, 0x0901, 0x0903, GENX_NAMECHAR);
  charProp(p, 0x093C, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x093E, 0x094C, GENX_NAMECHAR);
  charProp(p, 0x094D, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0951, 0x0954, GENX_NAMECHAR);
  rangeProp(p, 0x0962, 0x0963, GENX_NAMECHAR);
  rangeProp(p, 0x0981, 0x0983, GENX_NAMECHAR);
  charProp(p, 0x09BC, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x09BE, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x09BF, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x09C0, 0x09C4, GENX_NAMECHAR);
  rangeProp(p, 0x09C7, 0x09C8, GENX_NAMECHAR);
  rangeProp(p, 0x09CB, 0x09CD, GENX_NAMECHAR);
  charProp(p, 0x09D7, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x09E2, 0x09E3, GENX_NAMECHAR);
  charProp(p, 0x0A02, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0A3C, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0A3E, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0A3F, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0A40, 0x0A42, GENX_NAMECHAR);
  rangeProp(p, 0x0A47, 0x0A48, GENX_NAMECHAR);
  rangeProp(p, 0x0A4B, 0x0A4D, GENX_NAMECHAR);
  rangeProp(p, 0x0A70, 0x0A71, GENX_NAMECHAR);
  rangeProp(p, 0x0A81, 0x0A83, GENX_NAMECHAR);
  charProp(p, 0x0ABC, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0ABE, 0x0AC5, GENX_NAMECHAR);
  rangeProp(p, 0x0AC7, 0x0AC9, GENX_NAMECHAR);
  rangeProp(p, 0x0ACB, 0x0ACD, GENX_NAMECHAR);
  rangeProp(p, 0x0B01, 0x0B03, GENX_NAMECHAR);
  charProp(p, 0x0B3C, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0B3E, 0x0B43, GENX_NAMECHAR);
  rangeProp(p, 0x0B47, 0x0B48, GENX_NAMECHAR);
  rangeProp(p, 0x0B4B, 0x0B4D, GENX_NAMECHAR);
  rangeProp(p, 0x0B56, 0x0B57, GENX_NAMECHAR);
  rangeProp(p, 0x0B82, 0x0B83, GENX_NAMECHAR);
  rangeProp(p, 0x0BBE, 0x0BC2, GENX_NAMECHAR);
  rangeProp(p, 0x0BC6, 0x0BC8, GENX_NAMECHAR);
  rangeProp(p, 0x0BCA, 0x0BCD, GENX_NAMECHAR);
  charProp(p, 0x0BD7, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0C01, 0x0C03, GENX_NAMECHAR);
  rangeProp(p, 0x0C3E, 0x0C44, GENX_NAMECHAR);
  rangeProp(p, 0x0C46, 0x0C48, GENX_NAMECHAR);
  rangeProp(p, 0x0C4A, 0x0C4D, GENX_NAMECHAR);
  rangeProp(p, 0x0C55, 0x0C56, GENX_NAMECHAR);
  rangeProp(p, 0x0C82, 0x0C83, GENX_NAMECHAR);
  rangeProp(p, 0x0CBE, 0x0CC4, GENX_NAMECHAR);
  rangeProp(p, 0x0CC6, 0x0CC8, GENX_NAMECHAR);
  rangeProp(p, 0x0CCA, 0x0CCD, GENX_NAMECHAR);
  rangeProp(p, 0x0CD5, 0x0CD6, GENX_NAMECHAR);
  rangeProp(p, 0x0D02, 0x0D03, GENX_NAMECHAR);
  rangeProp(p, 0x0D3E, 0x0D43, GENX_NAMECHAR);
  rangeProp(p, 0x0D46, 0x0D48, GENX_NAMECHAR);
  rangeProp(p, 0x0D4A, 0x0D4D, GENX_NAMECHAR);
  charProp(p, 0x0D57, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0E31, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0E34, 0x0E3A, GENX_NAMECHAR);
  rangeProp(p, 0x0E47, 0x0E4E, GENX_NAMECHAR);
  charProp(p, 0x0EB1, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0EB4, 0x0EB9, GENX_NAMECHAR);
  rangeProp(p, 0x0EBB, 0x0EBC, GENX_NAMECHAR);
  rangeProp(p, 0x0EC8, 0x0ECD, GENX_NAMECHAR);
  rangeProp(p, 0x0F18, 0x0F19, GENX_NAMECHAR);
  charProp(p, 0x0F35, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0F37, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0F39, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0F3E, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0F3F, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0F71, 0x0F84, GENX_NAMECHAR);
  rangeProp(p, 0x0F86, 0x0F8B, GENX_NAMECHAR);
  rangeProp(p, 0x0F90, 0x0F95, GENX_NAMECHAR);
  charProp(p, 0x0F97, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x0F99, 0x0FAD, GENX_NAMECHAR);
  rangeProp(p, 0x0FB1, 0x0FB7, GENX_NAMECHAR);
  charProp(p, 0x0FB9, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x20D0, 0x20DC, GENX_NAMECHAR);
  charProp(p, 0x20E1, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x302A, 0x302F, GENX_NAMECHAR);
  charProp(p, 0x3099, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x309A, GENX_LETTER|GENX_NAMECHAR);

  charProp(p, 0x00B7, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x02D0, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x02D1, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0387, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0640, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0E46, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x0EC6, GENX_LETTER|GENX_NAMECHAR);
  charProp(p, 0x3005, GENX_LETTER|GENX_NAMECHAR);
  rangeProp(p, 0x3031, 0x3035, GENX_NAMECHAR);
  rangeProp(p, 0x309D, 0x309E, GENX_NAMECHAR);
  rangeProp(p, 0x30FC, 0x30FE, GENX_NAMECHAR);
}
