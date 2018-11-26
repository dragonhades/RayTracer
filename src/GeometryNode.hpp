// Fall 2018

#pragma once

#include "SolidNode.hpp"
#include "Material.hpp"

class Primitive;

class GeometryNode : public SolidNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

	void setMaterial( Material *material );

	Material *m_material;
	Primitive *m_primitive;
};
