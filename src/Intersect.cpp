// Fall 2018

#include <glm/ext.hpp>

#include <cmath>
#include <numeric>
#include <utility>

#include "Ray.hpp"
#include "DEBUG.hpp"
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "ConstructiveNode.hpp"
#include "SolidNode.hpp"
#include "Primitive.hpp"
#include "Mesh.hpp"
#include "Math.hpp"
#include "RayTracer.hpp"

#include "Intersect.hpp"

using namespace std;
using namespace glm;

int csgPenetrate(const SceneNode* ntarget, float t_end, Ray ray, const mat4 & trans, int acc=0, float t_target_acc=0){
	CastResult result_target = intersectNode(ntarget, ray, trans);

	if(!result_target.isHit()){
		return acc;
	}

	float t_target = result_target.t + t_target_acc;

	if(t_target>= t_end){
		return acc;
	}
	else {
		result_target.transform();
		ray.org = result_target.intersection;
		return csgPenetrate(ntarget, t_end, ray, trans, acc+1, t_target);
	}
}

CastResult intersectSceneRecurse(const SceneNode* node, 
						  const Ray & ray, 
						  const mat4 & trans) 
{
	double t_min = INT_MAX;
	CastResult result_min;
		
	CastResult result = intersectNode(node, ray, trans);

	if(result.isHit() && result.t < t_min) {
		t_min = result.t;
		result_min = result;
	}

	const ConstructiveNode* cnode = dynamic_cast<const ConstructiveNode*>(node);
	if(cnode){
		goto _SKIP_LOOP_;
	}

	for (SceneNode* child : node->children){
		const CastResult & result = 
			intersectSceneRecurse(child, ray, trans*node->get_transform());
		if(result.isHit() && result.t < t_min) {
			t_min = result.t;
			result_min = result;
		}
	}
_SKIP_LOOP_:
	if(t_min != INT_MAX) return result_min;
	else return CastResult();
}

CastResult intersectNodeRecurse(const SceneNode* target,
						  const SceneNode* node,
						  const Ray & ray, 
						  const mat4 & trans)
{
	CastResult result;
	if(target == node) {
		result = intersectSceneRecurse(node, ray, trans);
	} else {
		for(SceneNode* child : node->children){
			result = intersectNodeRecurse(target, child, ray, trans*node->get_transform());
			if(result.isHit()) return result;
		}
	}
	return result;
}

CastResult intersectNode(const SceneNode* node, 
						  const Ray & ray, 
						  const mat4 & trans) 
{
	DASSERT(node!=nullptr, "");
	if(node->m_nodeType == NodeType::ConstructiveNode)
	{
		ConstructiveNode* cnode = (ConstructiveNode*) node;
		DASSERT(cnode!=nullptr, "");
		DASSERT(cnode->leftNode()!=nullptr, "");
		DASSERT(cnode->rightNode()!=nullptr, "");

		if(cnode->is_Union()){

			const mat4 & new_trans = trans*cnode->get_transform();

			if(ray.inside_shape){

				// find max t, return

				float t_left_max;
				float t_right_max;

				// get left max t
				CastResult result_left_min = intersectSceneRecurse(cnode->leftNode(), ray, new_trans);
				CastResult result_left_max;
				if(result_left_min.isHit()){
					CastResult result_left_min_copy = result_left_min;
					result_left_min_copy.transform();
					Ray second_ray_left(result_left_min_copy.intersection, ray.dir, true);
					result_left_max = intersectSceneRecurse(cnode->leftNode(), second_ray_left, new_trans);
					if(!result_left_max.isHit()) result_left_max = result_left_min;
					t_left_max = result_left_max.t;
				} else {
					t_left_max = -1;
				}

				// get right max t
				CastResult result_right_min = intersectSceneRecurse(cnode->rightNode(), ray, new_trans);
				CastResult result_right_max;
				if(result_right_min.isHit()) {
					CastResult result_right_min_copy = result_right_min;
					result_right_min_copy.transform();
					Ray second_ray_right(result_right_min_copy.intersection, ray.dir, true);
					result_right_max = intersectSceneRecurse(cnode->rightNode(), second_ray_right, new_trans);
					if(!result_right_max.isHit()) result_right_max = result_right_min;
					t_right_max = result_right_max.t;
				} else {
					t_right_max = -1;
				}

				DASSERT(result_left_min.isHit() || result_right_min.isHit(), 
					"inside ray has no intersection with neither shapes");

				if(t_left_max > t_right_max){
					result_left_max.type = HitType::CSG;
					result_left_max.parentTrans = new_trans;
					return result_left_max;
				} else {
					result_right_max.type = HitType::CSG;
					result_right_max.parentTrans = new_trans;
					return result_right_max;
				}

			} else {
				CastResult result_left = intersectSceneRecurse(cnode->leftNode(), ray, new_trans);
				CastResult result_right = intersectSceneRecurse(cnode->rightNode(), ray, new_trans);

				if(!result_left.isHit() && !result_right.isHit()) return CastResult();
				if(result_left.isHit() && !result_right.isHit()) return result_left;
				if(!result_left.isHit() && result_right.isHit()) return result_right;
				else {
					if(result_left.t < result_right.t){
						DASSERT(result_left.gnode != nullptr, "Empty CastResult");
						result_left.type = HitType::CSG;
						result_left.parentTrans = new_trans;
						return result_left;
					} else {
						DASSERT(result_right.gnode != nullptr, "Empty CastResult");
						result_right.type = HitType::CSG;
						result_right.parentTrans = new_trans;
						return result_right;
					}
				}
			}
		}
		else if(cnode->is_Intersect()){
			// TODO
		}
		else if(cnode->is_Difference()){

			const mat4 & new_trans = trans*cnode->get_transform();
			CastResult result_right_min = intersectSceneRecurse(cnode->rightNode(), ray, new_trans);
			if(result_right_min.isHit()){
				
				CastResult result_left_min = intersectSceneRecurse(cnode->leftNode(), ray, new_trans);

				if(!result_left_min.isHit()) return CastResult();

				CastResult result_left_min_copy = result_left_min;
				result_left_min_copy.transform();
				Ray ray_inside_left(result_left_min_copy.intersection, ray.dir, true);
				CastResult result_left_max = intersectSceneRecurse(cnode->leftNode(), ray_inside_left, new_trans);

				if(!result_left_max.isHit()){
					float t_left = result_left_min.t;
					float t_right = result_right_min.t;
					if(t_left <= t_right){
						return CastResult();
					} else {
						CastResult rtv = result_right_min;
						rtv.gnode = result_left_min.gnode;
						rtv.surface_normal = -rtv.surface_normal;
						rtv.type = HitType::CSG;
						rtv.parentTrans = new_trans;
						return rtv;
					}
				}

				CastResult result_right_min_copy = result_right_min;
				result_right_min_copy.transform();

				Ray ray_inside_right(result_right_min_copy.intersection, ray.dir, true); 
				CastResult result_right_max = intersectSceneRecurse(cnode->rightNode(), ray_inside_right, new_trans);

				float t_left_min = result_left_min.t;
				float t_right_min = result_right_min.t;
				float t_left_max = result_left_max.t + result_left_min.t;
				float t_right_max = result_right_max.t + result_right_min.t;

				if(!result_right_max.isHit()){
					t_right_min = INT_MIN;
				}

				CastResult rtv;
				if(t_left_min < t_right_min || t_left_min > t_right_max){
					rtv = result_left_min;
				} else if(t_right_max < t_left_max){
					rtv = result_right_max;
					rtv.t = t_right_max;
					rtv.gnode = result_left_min.gnode;
					rtv.surface_normal = -rtv.surface_normal;
				} else {
					return CastResult();
				}

				rtv.type = HitType::CSG;
				rtv.parentTrans = new_trans;
				return rtv;
			} else {
				// test the supposely exposed left shape surface
				return intersectSceneRecurse(cnode->leftNode(), ray, new_trans);
			}
		}
	}
//----------------------------------------------------------------------------------------//
	else if(node->m_nodeType == NodeType::GeometryNode)
	{
		GeometryNode* gnode = (GeometryNode*) node;

		Ray ray_copy = ray;
		ray_copy.setTransform(inverse(trans*gnode->get_transform()));
		CastResult result = gnode->m_primitive->intersect(ray_copy);

		if(result.isHit()){

#ifndef DRAW_BBOX
			if(result.type == HitType::BBox){
				Mesh* mesh = (Mesh*) gnode->m_primitive;
				result = mesh->intersectTriangles(ray_copy);
			}
#endif
			result.gnode = gnode;
			result.parentTrans = trans;
			return result;
		}
	}
	return CastResult();
}


CastResult intersectScene(const Ray & ray, 
						const SceneNode* target){
	CastResult result;
	if(target){
		intersectNodeRecurse(target, root, ray, mat4());
	} else {
		result = intersectSceneRecurse(root, ray, mat4());
	}
	if(result.isHit()){
		 result.transform();
		 if(ray.inside_shape) result.surface_normal = -result.surface_normal;
		 return result;
	} else {
		return CastResult();
	}
}

const SceneNode* findNode(const SceneNode* node,
						  const std::string & name, 
						  glm::mat4 & trans)
{
	CastResult result;
	if(node->m_name == name) {
		trans = node->get_transform();
		return node;
	} else {
		for(SceneNode* child : node->children){
			mat4 T = trans*node->get_transform();
			const SceneNode* result = findNode(child, name, T);
			if(result) {
				trans = T*node->get_transform(); 
				return result;
			}
		}
	}
	return nullptr;
}