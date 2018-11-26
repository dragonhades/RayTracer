// Fall 2018

#include "ConstructiveNode.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

#include "DEBUG.hpp"

#include "Primitive.hpp"


//---------------------------------------------------------------------------------------
ConstructiveNode::ConstructiveNode(
	const std::string & name, const std::string & mode )
	: SolidNode( name )
{
	m_nodeType = NodeType::ConstructiveNode;

	if(mode == "union")
		m_mode = Mode::Union;
	else if(mode == "intersect")
		m_mode = Mode::Intersect;
	else if(mode == "difference")
		m_mode = Mode::Difference;
	else
		DASSERT(false, "Exception: Unknown CSG Type");
}


void ConstructiveNode::add_child(SceneNode* child) {
	if(children.size() == 2) {
		DASSERT(false, "Exception: CSG node cannot have more than two children");
	}
	SceneNode::add_child(child);
}

SceneNode * ConstructiveNode::get_SceneNode() {

	// TODO : Leave it for now. Extend this if needed...

	return children.front();
}

SceneNode * ConstructiveNode::leftNode() {
	return children.front();
}

SceneNode * ConstructiveNode::rightNode() {
	return children.back();
}