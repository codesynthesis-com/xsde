// file      : xsde/cxx/compilers/ghs/pre.hxx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

// Warning pragmas are only available in the new generation compiler.
//
// 997 - virtual function hidden
//
#ifdef __EDG__
#  pragma ghs nowarning 997
#endif
