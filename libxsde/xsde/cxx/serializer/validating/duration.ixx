// file      : xsde/cxx/serializer/validating/duration.ixx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsde
{
  namespace cxx
  {
    namespace serializer
    {
      namespace validating
      {
        inline duration_simpl::
        duration_simpl ()
            : value_ (false, 1, 0, 0, 0, 0, 0.0)
        {
        }
      }
    }
  }
}
