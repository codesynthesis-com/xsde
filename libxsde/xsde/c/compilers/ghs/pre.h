/* file      : xsde/c/compilers/ghs/pre.h
 * license   : GNU GPL v2 + exceptions; see accompanying LICENSE file
 */

/*
 * Warning pragmas are only available in the new generation compiler.
 */

/* 111 - statement is unreachable */
#ifdef __EDG__
#  pragma ghs nowarning 111
#endif
