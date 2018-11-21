// Fall 2018

#pragma once

#include "SceneNode.hpp"

class SolidNode : public SceneNode {
public:
	SolidNode( const std::string & name ): SceneNode( name ){}

    virtual ~SolidNode(){};
};