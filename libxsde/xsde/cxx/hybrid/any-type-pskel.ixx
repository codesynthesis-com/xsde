// file      : xsde/cxx/hybrid/any-type-pskel.ixx
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsde
{
  namespace cxx
  {
    namespace hybrid
    {
      // any_type_pskel
      //
#ifdef XSDE_REUSE_STYLE_TIEIN
      inline any_type_pskel::
      any_type_pskel ()
          : any_type_impl_ (0)
      {
      }

      inline any_type_pskel::
      any_type_pskel (any_type_pskel* impl, void*)
          : complex_content (impl, 0), any_type_impl_ (impl)
      {
      }
#endif
    }
  }
}
