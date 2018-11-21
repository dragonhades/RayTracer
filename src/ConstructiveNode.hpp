// Fall 2018

#pragma once

#include "SceneNode.hpp"
#include "SolidNode.hpp"

class ConstructiveNode : public SolidNode {
public:
	ConstructiveNode( const std::string & name, const std::string & mode );

	bool is_Union() const { return m_mode == Mode::Union; }
	bool is_Intersect() const { return m_mode == Mode::Intersect; }
	bool is_Difference() const { return m_mode == Mode::Difference; }

    void add_child(SceneNode* child) override;

	SceneNode * get_SceneNode();
	SceneNode * leftNode();
	SceneNode * rightNode();

private:
	enum class Mode {
		Union,
		Intersect,
		Difference
	};

	Mode m_mode;
};
