// file      : xsde/cxx/hybrid/base.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSDE_CXX_HYBRID_BASE_HXX
#define XSDE_CXX_HYBRID_BASE_HXX

#include <xsde/cxx/config.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
      // boolean
      //
      struct boolean_base
      {
        bool base_value () const {return x_;}
        bool& base_value () {return x_;}
        void base_value (bool x) {x_ = x;}

        operator const bool& () const {return x_;}
        operator bool& () {return x_;}

        boolean_base& operator= (bool x) {x_ = x; return *this;}

      protected:
        bool x_;
      };

      // byte
      //
      struct byte_base
      {
        signed char base_value () const {return x_;}
        signed char& base_value () {return x_;}
        void base_value (signed char x) {x_ = x;}

        operator const signed char& () const {return x_;}
        operator signed char& () {return x_;}

        byte_base& operator= (signed char x) {x_ = x; return *this;}

      protected:
        signed char x_;
      };

      // unsigned_byte
      //
      struct unsigned_byte_base
      {
        unsigned char base_value () const {return x_;}
        unsigned char& base_value () {return x_;}
        void base_value (unsigned char x) {x_ = x;}

        operator const unsigned char& () const {return x_;}
        operator unsigned char& () {return x_;}

        unsigned_byte_base&
        operator= (unsigned char x) {x_ = x; return *this;}

      protected:
        unsigned char x_;
      };

      // short
      //
      struct short_base
      {
        short base_value () const {return x_;}
        short& base_value () {return x_;}
        void base_value (short x) {x_ = x;}

        operator const short& () const {return x_;}
        operator short& () {return x_;}

        short_base& operator= (short x) {x_ = x; return *this;}

      protected:
        short x_;
      };

      // unsigned_short
      //
      struct unsigned_short_base
      {
        unsigned short base_value () const {return x_;}
        unsigned short& base_value () {return x_;}
        void base_value (unsigned short x) {x_ = x;}

        operator const unsigned short& () const {return x_;}
        operator unsigned short& () {return x_;}

        unsigned_short_base&
        operator= (unsigned short x) {x_ = x; return *this;}

      protected:
        unsigned short x_;
      };

      // int
      //
      struct int_base
      {
        int base_value () const {return x_;}
        int& base_value () {return x_;}
        void base_value (int x) {x_ = x;}

        operator const int& () const {return x_;}
        operator int& () {return x_;}

        int_base& operator= (int x) {x_ = x; return *this;}

      protected:
        int x_;
      };

      // unsigned_int
      //
      struct unsigned_int_base
      {
        unsigned int base_value () const {return x_;}
        unsigned int& base_value () {return x_;}
        void base_value (unsigned int x) {x_ = x;}

        operator const unsigned int& () const {return x_;}
        operator unsigned int& () {return x_;}

        unsigned_int_base& operator= (unsigned int x) {x_ = x; return *this;}

      protected:
        unsigned int x_;
      };

      // long
      //
#ifdef XSDE_LONGLONG
      struct long_base
      {
        long long base_value () const {return x_;}
        long long& base_value () {return x_;}
        void base_value (long long x) {x_ = x;}

        operator const long long& () const {return x_;}
        operator long long& () {return x_;}

        long_base& operator= (long long x) {x_ = x; return *this;}

      protected:
        long long x_;
      };
#else
      struct long_base
      {
        long base_value () const {return x_;}
        long& base_value () {return x_;}
        void base_value (long x) {x_ = x;}

        operator const long& () const {return x_;}
        operator long& () {return x_;}

        long_base& operator= (long x) {x_ = x; return *this;}

      protected:
        long x_;
      };
#endif


      // unsigned_long
      //
#ifdef XSDE_LONGLONG
      struct unsigned_long_base
      {
        unsigned long long base_value () const {return x_;}
        unsigned long long& base_value () {return x_;}
        void base_value (unsigned long long x) {x_ = x;}

        operator const unsigned long long& () const {return x_;}
        operator unsigned long long& () {return x_;}

        unsigned_long_base&
        operator= (unsigned long long x) {x_ = x; return *this;}

      protected:
        unsigned long long x_;
      };
#else
      struct unsigned_long_base
      {
        unsigned long base_value () const {return x_;}
        unsigned long& base_value () {return x_;}
        void base_value (unsigned long x) {x_ = x;}

        operator const unsigned long& () const {return x_;}
        operator unsigned long& () {return x_;}

        unsigned_long_base&
        operator= (unsigned long x) {x_ = x; return *this;}

      protected:
        unsigned long x_;
      };
#endif

      // integer
      //
      struct integer_base
      {
        long base_value () const {return x_;}
        long& base_value () {return x_;}
        void base_value (long x) {x_ = x;}

        operator const long& () const {return x_;}
        operator long& () {return x_;}

        integer_base& operator= (long x) {x_ = x; return *this;}

      protected:
        long x_;
      };

      // negative_integer
      //
      struct negative_integer_base
      {
        long base_value () const {return x_;}
        long& base_value () {return x_;}
        void base_value (long x) {x_ = x;}

        operator const long& () const {return x_;}
        operator long& () {return x_;}

        negative_integer_base& operator= (long x) {x_ = x; return *this;}

      protected:
        long x_;
      };

      // non_positive_integer
      //
      struct non_positive_integer_base
      {
        long base_value () const {return x_;}
        long& base_value () {return x_;}
        void base_value (long x) {x_ = x;}

        operator const long& () const {return x_;}
        operator long& () {return x_;}

        non_positive_integer_base& operator= (long x) {x_ = x; return *this;}

      protected:
        long x_;
      };

      // positive_integer
      //
      struct positive_integer_base
      {
        unsigned long base_value () const {return x_;}
        unsigned long& base_value () {return x_;}
        void base_value (unsigned long x) {x_ = x;}

        operator const unsigned long& () const {return x_;}
        operator unsigned long& () {return x_;}

        positive_integer_base&
        operator= (unsigned long x) {x_ = x; return *this;}

      protected:
        unsigned long x_;
      };

      // non_negative_integer
      //
      struct non_negative_integer_base
      {
        unsigned long base_value () const {return x_;}
        unsigned long& base_value () {return x_;}
        void base_value (unsigned long x) {x_ = x;}

        operator const unsigned long& () const {return x_;}
        operator unsigned long& () {return x_;}

        non_negative_integer_base&
        operator= (unsigned long x) {x_ = x; return *this;}

      protected:
        unsigned long x_;
      };

      // float
      //
      struct float_base
      {
        float base_value () const {return x_;}
        float& base_value () {return x_;}
        void base_value (float x) {x_ = x;}

        operator const float& () const {return x_;}
        operator float& () {return x_;}

        float_base& operator= (float x) {x_ = x; return *this;}

      protected:
        float x_;
      };

      // double
      //
      struct double_base
      {
        double base_value () const {return x_;}
        double& base_value () {return x_;}
        void base_value (double x) {x_ = x;}

        operator const double& () const {return x_;}
        operator double& () {return x_;}

        double_base& operator= (double x) {x_ = x; return *this;}

      protected:
        double x_;
      };

      // decimal
      //
      struct decimal_base
      {
        double base_value () const {return x_;}
        double& base_value () {return x_;}
        void base_value (double x) {x_ = x;}

        operator const double& () const {return x_;}
        operator double& () {return x_;}

        decimal_base& operator= (double x) {x_ = x; return *this;}

      protected:
        double x_;
      };

      // string
      //
      struct string_base
      {
        string_base () : x_ (0) {}
        ~string_base () {delete[] x_;}

        const char* base_value () const {return x_;}
        char* base_value () {return x_;}
        void base_value (char* x) {delete[] x_; x_ = x;}

        operator const char* () const {return x_;}
        operator char* () {return x_;}

        string_base& operator= (char* x) {delete[] x_; x_ = x; return *this;}

      protected:
        char* x_;
      };
    }
  }
}

#endif  // XSDE_CXX_HYBRID_BASE_HXX
