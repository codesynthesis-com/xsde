// file      : xsde/cxx/serializer/validating/hex-binary.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xsde/cxx/serializer/validating/hex-binary.hxx>

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        hex_binary_simpl::
        ~hex_binary_simpl ()
        {
          if (free_)
            delete const_cast<buffer*> (value_);
        }

        void hex_binary_simpl::
        pre (const buffer* value)
        {
          value_ = value;
        }

        static const char hex_tab[] = "0123456789ABCDEF";

        void hex_binary_simpl::
        _serialize_content ()
        {
          char buf[512];

          size_t size = value_->size ();
          const char* data = value_->data ();

          // Covert the data in 256-byte chunks.
          //
          for (size_t j = 0; j < size;)
          {
            size_t i = 0;

            for (; i < 512 && j < size; ++j)
            {
              unsigned char byte = static_cast<unsigned char> (data[j]);

              buf[i++] = hex_tab[byte >> 4];
              buf[i++] = hex_tab[byte & 0x0F];
            }

#ifdef XSDE_EXCEPTIONS
            _characters (buf, i);
#else
            if (!_characters (buf, i))
              break;
#endif
          }

          if (free_)
          {
            delete const_cast<buffer*> (value_);
            value_ = 0;
          }
        }
      }
    }
  }
}
