/*
 *  OpenSCAD (www.openscad.org)
 *  Copyright (C) 2009-2011 Clifford Wolf <clifford@clifford.at> and
 *                          Marius Kintel <marius@kintel.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  As a special exception, you have permission to link this program
 *  with the CGAL library and distribute executables, as long as you
 *  follow the requirements of the GNU GPL in regard to all of the
 *  software in the executable aside from CGAL.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef CGAL_RENDERER_H
#define CGAL_RENDERER_H

#include <CGAL/Nef_3/OGL_helper.h>

#undef CGAL_NEF3_MARKED_VERTEX_COLOR
#undef CGAL_NEF3_MARKED_EDGE_COLOR
#undef CGAL_NEF3_MARKED_FACET_COLOR

#undef CGAL_NEF3_UNMARKED_VERTEX_COLOR
#undef CGAL_NEF3_UNMARKED_EDGE_COLOR
#undef CGAL_NEF3_UNMARKED_FACET_COLOR

using CGAL::OGL::SNC_BOUNDARY;
using CGAL::OGL::SNC_SKELETON;

// Originally from https://gist.github.com/1528856
// Modified to allow access two rather than one private member.
// This is needed because CGAL::OGL::Polyhedron has private members
// that we need access to.

// Generate a static data member of type Tag::type in which to store
// the address of a private member. It is crucial that Tag does not
// depend on the /value/ of the the stored address in any way so that
// we can access it from ordinary code without directly touching
// private data.
template <class Tag>
struct stowed
{
    static typename Tag::style_type style;
    static typename Tag::object_list_type object_list_;
};
template <class Tag>
typename Tag::style_type stowed<Tag>::style;
template <class Tag>
typename Tag::object_list_type stowed<Tag>::object_list_;

// Generate a static data member whose constructor initializes
// stowed<Tag>::value. This type will only be named in an explicit
// instantiation, where it is legal to pass the address of a private
// member.
template <class Tag, typename Tag::style_type Style, typename Tag::object_list_type Object_list_>
struct stow_private
{
 stow_private()
 {
   stowed<Tag>::style = Style;
   stowed<Tag>::object_list_ = Object_list_;

 }
 static stow_private instance;
};

struct Polyhedron_private
{
 typedef int(CGAL::OGL::Polyhedron::*style_type);
 typedef GLuint(CGAL::OGL::Polyhedron::*object_list_type);
};

class Polyhedron : public CGAL::OGL::Polyhedron
{
public:

	enum RenderColor {
		CGAL_NEF3_MARKED_VERTEX_COLOR,
		CGAL_NEF3_MARKED_EDGE_COLOR,
		CGAL_NEF3_MARKED_FACET_COLOR,
		CGAL_NEF3_UNMARKED_VERTEX_COLOR,
		CGAL_NEF3_UNMARKED_EDGE_COLOR,
		CGAL_NEF3_UNMARKED_FACET_COLOR,
		NUM_COLORS
	};

 Polyhedron() {
		setColor(CGAL_NEF3_MARKED_VERTEX_COLOR,0xb7,0xe8,0x5c);
		setColor(CGAL_NEF3_MARKED_EDGE_COLOR,0xab,0xd8,0x56);
		setColor(CGAL_NEF3_MARKED_FACET_COLOR,0x9d,0xcb,0x51);
		setColor(CGAL_NEF3_UNMARKED_VERTEX_COLOR,0xff,0xf6,0x7c);
		setColor(CGAL_NEF3_UNMARKED_EDGE_COLOR,0xff,0xec,0x5e);
		setColor(CGAL_NEF3_UNMARKED_FACET_COLOR,0xf9,0xd7,0x2c);
	}

	void draw(bool showedges) const {
         if(this->*stowed<Polyhedron_private>::style == SNC_BOUNDARY) {
			glCallList(this->*stowed<Polyhedron_private>::object_list_+2);
			if(showedges) {
				glDisable(GL_LIGHTING);
				glCallList(this->*stowed<Polyhedron_private>::object_list_+1);
				glCallList(this->*stowed<Polyhedron_private>::object_list_);
			}
		} else {
			glDisable(GL_LIGHTING);
			glCallList(this->*stowed<Polyhedron_private>::object_list_+1);
			glCallList(this->*stowed<Polyhedron_private>::object_list_);
		}
	}
	CGAL::Color getVertexColor(std::list<CGAL::OGL::DPoint>::const_iterator v) const {
		CGAL::Color c = v->mark() ? colors[CGAL_NEF3_UNMARKED_VERTEX_COLOR] : colors[CGAL_NEF3_MARKED_VERTEX_COLOR];
		return c;
	}

 CGAL::Color getEdgeColor(std::list<CGAL::OGL::DSegment>::const_iterator e) const {
		CGAL::Color c = e->mark() ? colors[CGAL_NEF3_UNMARKED_EDGE_COLOR] : colors[CGAL_NEF3_MARKED_EDGE_COLOR];
		return c;
	}

	CGAL::Color getFacetColor(std::list<CGAL::OGL::DFacet>::const_iterator f) const {
		CGAL::Color c = f->mark() ? colors[CGAL_NEF3_UNMARKED_FACET_COLOR] : colors[CGAL_NEF3_MARKED_FACET_COLOR];
		return c;
	}

	void setColor(Polyhedron::RenderColor color_index,
				  unsigned char r, unsigned char g, unsigned char b) {
		assert(color_index < Polyhedron::NUM_COLORS);
		this->colors[color_index] = CGAL::Color(r,g,b);
	}
private:
	CGAL::Color colors[NUM_COLORS];

}; // Polyhedron

#endif // CGAL_RENDERER_H
