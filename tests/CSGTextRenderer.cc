#include "CSGTextRenderer.h"

#include <string>
#include <map>
#include <list>
#include "visitor.h"
#include "state.h"
#include "module.h" // FIXME: Temporarily for ModuleInstantiation

#include "csgnode.h"
#include "transformnode.h"

#include <sstream>
#include <iostream>
#include <assert.h>
#include <QRegExp>

bool CSGTextRenderer::isCached(const AbstractNode &node)
{
	return this->cache.contains(node);
}

/*!
	Modifies target by applying op to target and src:
	target = target [op] src
 */
void
CSGTextRenderer::process(string &target, const string &src, CSGTextRenderer::CsgOp op)
{
// 	if (target.dim != 2 && target.dim != 3) {
// 		assert(false && "Dimension of Nef polyhedron must be 2 or 3");
// 	}

	switch (op) {
	case UNION:
		target += "+" + src;
		break;
	case INTERSECTION:
		target += "*" + src;
		break;
	case DIFFERENCE:
		target += "-" + src;
		break;
	case MINKOWSKI:
		target += "M" + src;
		break;
	}
}

void CSGTextRenderer::applyToChildren(const AbstractNode &node, CSGTextRenderer::CsgOp op)
{
	std::stringstream stream;
	stream << node.name() << node.index();
	string N = stream.str();
	if (this->visitedchildren[node.index()].size() > 0) {
		// FIXME: assert that cache contains nodes in code below
		bool first = true;
		for (ChildList::const_iterator iter = this->visitedchildren[node.index()].begin();
				 iter != this->visitedchildren[node.index()].end();
				 iter++) {
			const AbstractNode *chnode = *iter;
			assert(this->cache.contains(*chnode));
			// FIXME: Don't use deep access to modinst members
			if (chnode->modinst->tag_background) continue;
			if (first) {
				N += "(" + this->cache[*chnode];
// 				if (N.dim != 0) first = false; // FIXME: when can this happen?
				first = false;
			} else {
				process(N, this->cache[*chnode], op);
			}
			chnode->progress_report();
		}
		N += ")";
	}
	this->cache.insert(node, N);
}

/*
	Typical visitor behavior:
	o In prefix: Check if we're cached -> prune
	o In postfix: Check if we're cached -> don't apply operator to children
	o In postfix: addToParent()
 */

Response CSGTextRenderer::visit(State &state, const AbstractNode &node)
{
	if (state.isPrefix() && isCached(node)) return PruneTraversal;
	if (state.isPostfix()) {
		if (!isCached(node)) applyToChildren(node, UNION);
		addToParent(state, node);
	}
	return ContinueTraversal;
}

Response CSGTextRenderer::visit(State &state, const AbstractIntersectionNode &node)
{
	if (state.isPrefix() && isCached(node)) return PruneTraversal;
	if (state.isPostfix()) {
		if (!isCached(node)) applyToChildren(node, INTERSECTION);
		addToParent(state, node);
	}
	return ContinueTraversal;
}

Response CSGTextRenderer::visit(State &state, const CsgNode &node)
{
	if (state.isPrefix() && isCached(node)) return PruneTraversal;
	if (state.isPostfix()) {
		if (!isCached(node)) {
			CsgOp op;
			switch (node.type) {
			case CSG_TYPE_UNION:
				op = UNION;
				break;
			case CSG_TYPE_DIFFERENCE:
				op = DIFFERENCE;
				break;
			case CSG_TYPE_INTERSECTION:
				op = INTERSECTION;
				break;
			}
			applyToChildren(node, op);
		}
		addToParent(state, node);
	}
	return ContinueTraversal;
}

Response CSGTextRenderer::visit(State &state, const TransformNode &node)
{
	if (state.isPrefix() && isCached(node)) return PruneTraversal;
	if (state.isPostfix()) {
		if (!isCached(node)) {
			// First union all children
			applyToChildren(node, UNION);
			// FIXME: Then apply transform
		}
		addToParent(state, node);
	}
	return ContinueTraversal;
}

// FIXME: RenderNode: Union over children + some magic
// FIXME: CgaladvNode: Iterate over children. Special operation

// FIXME: Subtypes of AbstractPolyNode:
// ProjectionNode
// DxfLinearExtrudeNode
// DxfRotateExtrudeNode
// (SurfaceNode)
// (PrimitiveNode)
Response CSGTextRenderer::visit(State &state, const AbstractPolyNode &node)
{
	if (state.isPrefix() && isCached(node)) return PruneTraversal;
	if (state.isPostfix()) {
		if (!isCached(node)) {

	// FIXME: Manage caching
	// FIXME: Will generate one single Nef polyhedron (no csg ops necessary)
 
			string N = node.name();
			this->cache.insert(node, N);
		
// 		std::cout << "Insert: " << N << "\n";
// 		std::cout << "Node: " << cacheid.toStdString() << "\n\n";
		}
		addToParent(state, node);
	}

	return ContinueTraversal;
}

/*!
	Adds ourself to out parent's list of traversed children.
	Call this for _every_ node which affects output during the postfix traversal.
*/
void CSGTextRenderer::addToParent(const State &state, const AbstractNode &node)
{
	assert(state.isPostfix());
	this->visitedchildren.erase(node.index());
	if (state.parent()) {
		this->visitedchildren[state.parent()->index()].push_back(&node);
	}
}
