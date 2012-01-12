#include "CGAL_renderer.h"

// Explicit instantiation; the only place where it is legal to pass
// the address of a private member. Generates the static ::instance
// that in turn initializes stowed<Tag>::value.
template class stow_private<Polyhedron_private,&CGAL::OGL::Polyhedron::style,
                           &CGAL::OGL::Polyhedron::object_list_>;

template <class Tag, typename Tag::style_type Style, typename Tag::object_list_type Object_list_>
stow_private<Tag,Style,Object_list_>
stow_private<Tag,Style,Object_list_>::instance;
