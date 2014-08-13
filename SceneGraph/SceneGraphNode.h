#pragma once

#include <Eigen/Geometry>
#include <memory>
#include <unordered_set>

#include "EigenTypes.h"

// This class contains base functionality common to all primitives:
// - translation
// - linear transformation
// - Parent/child transform hierarchy (in progress)
// - Local/global coordinate system conversion (in progress)

// see http://en.wikipedia.org/wiki/Scene_graph
// TODO: write a design for a generalized property propagation scheme in which
//       each property would be defined partially by an "application" operation
//       which defines how the properties propagate from parent to child.
template <typename Scalar, unsigned int DIM>
class SceneGraphNode : public std::enable_shared_from_this<SceneGraphNode<Scalar,DIM>> {
public:

  // A Transform is a translation and a linear transformation, in the block matrix form
  //   [ L_00 L_01 L_02 T_0 ]
  //   [ L_10 L_11 L_12 T_1 ]
  //   [ L_20 L_21 L_22 T_2 ]
  //   [ 0    0    0    1   ]
  // where the 3x3 L matrix is the linear transformation and the 3x1 T column matrix
  // is the translation vector.  The semantic is that this transformation acts on a vector X
  // of the form
  //   [ X_0 ]
  //   [ X_1 ]
  //   [ X_2 ]
  //   [ 1   ]
  // and is equivalent to the expression
  //   L*X + T.
  // The 3x4 matrix A consisting of only the L and T parts is called the affine transformation,
  // and if X' is the 4x1 column matrix written above (the one with the 1 at the bottom), then
  // the transformation acts simply as
  //   A*X'
  // and produces the same 3x1 column matrix value as the expression L*X + T.
  typedef Eigen::Transform<Scalar,DIM,Eigen::AffineCompact> Transform;
  SceneGraphNode() { m_transform.setIdentity(); }
  typedef std::unordered_set<std::shared_ptr<SceneGraphNode>,
    std::hash<std::shared_ptr<SceneGraphNode>>,
    std::equal_to<std::shared_ptr<SceneGraphNode>>,
    Eigen::aligned_allocator<std::shared_ptr<SceneGraphNode>>
  > ChildSet;
  virtual ~SceneGraphNode() { }

  using std::enable_shared_from_this<SceneGraphNode<Scalar,DIM>>::shared_from_this;

  // Used to read the translation, linear transform
  typename Transform::ConstTranslationPart Translation ()          const { return m_transform.translation(); }
  typename Transform::ConstLinearPart      LinearTransformation () const { return m_transform.linear(); }
  typename Transform::ConstAffinePart      AffineTransformation () const { return m_transform.affine(); }
  const Transform &                        FullTransform ()        const { return m_transform; }

  typename Transform::TranslationPart      Translation ()                { return m_transform.translation(); }
  typename Transform::LinearPart           LinearTransformation ()       { return m_transform.linear(); }
  typename Transform::AffinePart           AffineTransformation ()       { return m_transform.affine(); }
  Transform &                              FullTransform ()              { return m_transform; }

  // TODO: make special translation-modifying methods (e.g. rotation, scaling),
  // by encapsulating them inside a different class, or using Transform directly.

  const std::shared_ptr<SceneGraphNode>& Parent () const { return m_parent.lock(); }
  const ChildSet& Children() const { return m_children; }
  ChildSet& Children() { return m_children; }

  // these are virtual so that particular behavior can be added while adding/removing nodes.
  // any overrides should make sure to call the base class' version of the method, of course.
  virtual void AddChild(std::shared_ptr<SceneGraphNode>& child) {
    m_children.emplace(child);
    child->m_parent = shared_from_this();
  }
  virtual void RemoveFromParent() {
    std::shared_ptr<SceneGraphNode> parent = m_parent.lock();
    if (parent) {
      parent->m_children.erase(shared_from_this());
      m_parent.reset();
    }
  }

  enum class DFTCallOrder { CALL_ON_PARENT_BEFORE_CHILDREN, CALL_ON_PARENT_AFTER_CHILDREN };

  // Traverse this tree, depth-first, calling call_this_function_first (if valid) 
  // on this node, then calling this function recursively on the child nodes, then 
  // calling call_this_function_last (if valid) on this node.
  void DepthFirstTraverse (std::function<void(const SceneGraphNode&)> &call_this_function_first,
                           std::function<void(const SceneGraphNode&)> &call_this_function_last) const {
    if (call_this_function_first) {
      call_this_function_first(*this);
    }
    for (auto it = m_children.begin(); it != m_children.end(); ++it) {
      const std::shared_ptr<SceneGraphNode> &child = *it;
      assert(bool(child));
      child->DepthFirstTraverse(call_this_function_first, call_this_function_last);
    }
    if (call_this_function_last) {
      call_this_function_last(*this);
    }
  }
  // Traverse this tree, depth-first, calling the specified function on each node.
  // The call_order parameter specifies if the function should be called on each
  // parent node before its children or after.
  void DepthFirstTraverse (std::function<void(const SceneGraphNode&)> &call_this_function_on_each_node,
                           DFTCallOrder call_order) const {
    std::function<void(const SceneGraphNode&)> call_this_function_first; // invalid/empty
    std::function<void(const SceneGraphNode&)> call_this_function_last;  // invalid/empty
    if (call_order == DFTCallOrder::CALL_ON_PARENT_BEFORE_CHILDREN) {
      call_this_function_first = call_this_function_on_each_node;
    } else { // call_order == DFTCallOrder::CALL_ON_PARENT_AFTER_CHILDREN
      call_this_function_last = call_this_function_on_each_node;
    }
    DepthFirstTraverse(call_this_function_first, call_this_function_last);
  }

  // This computes the transformation taking points in this node's coordinate
  // system and produces those points expressed in the coordinate system of
  // the other node.
  //
  // The transform for a node gives its transform taking its parent's coordinate
  // system to its coordinate system.
  Transform ComputeTransformToCoordinatesOf (const SceneGraphNode &other) const {
    auto closest_common_ancestor = ClosestCommonAncestor(other);

    // Compute the transformation from this node's coordinate system to the 
    // common ancestor's.  For convenience in later comments, call this A.
    Transform this_transform_stack;
    this_transform_stack.setIdentity();
    auto traversal_node = shared_from_this();
    while (traversal_node != closest_common_ancestor) {
      // A node's transform gives the node-to-parent coordinate transformation.
      this_transform_stack *= traversal_node->FullTransform(); 
    }

    // Compute the transformation from the other node's coordinate system
    // to the common ancestor's.  For convenience in later comments, call this B.
    Transform other_transform_stack;
    other_transform_stack.setIdentity();
    traversal_node = other.shared_from_this();
    while (traversal_node != closest_common_ancestor) {
      other_transform_stack *= traversal_node->FullTransform();
    }

    // TODO: somehow check that other_transform_stack is actually invertible (it's not
    // clear that this functionality is directly provided via Eigen::Transform).

    // The total transformation is first applying A and then applying B inverse.
    // Because transforms act on the left of vectors, this ordering of the operands
    // (as B^{-1} * A) is necessary.
    return other_transform_stack.inverse(Eigen::Affine) * this_transform_stack;
  }

  // This will return an empty shared_ptr if there was no common ancestor, which
  // should happen if and only if the two nodes come from different scene graph trees.
  std::shared_ptr<SceneGraphNode> ClosestCommonAncestor (const SceneGraphNode &other) const {
    // TODO: develop ancestry lists for each, then find the last common one, root-down.
    std::vector<SceneGraphNode> this_ancestors;
    std::vector<SceneGraphNode> other_ancestors;
    this->AppendAncestors(this_ancestors);
    other.AppendAncestors(other_ancestors);

    std::shared_ptr<SceneGraphNode> retval;
    // "zip" together the lists, starting at the end (which is the root), and determine
    // the last matching ancestor.
    auto this_it = this_ancestors.rbegin();
    auto other_it = other_ancestors.rbegin();
    while (this_it != this_ancestors.rend() && other_it != other_ancestors.rend() && *this_it == *other_it) {
      ++this_it;
      ++other_it;
    }
    // we went one too far, so back up one.
    --this_it;
    // if this iterator is valid, then return what it points to.
    if (this_it >= this_ancestors.rbegin()) {
      return *this_it;
    } else { // otherwise return empty.
      return std::shared_ptr<SceneGraphNode>();
    }
  }

  // TODO: special local to global and global to local transforms.

  // Necessary because a member variable is a statically-sized Eigen type.
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:

  // This populates a vector with the ancestors of this node, starting with this node,
  // then its parent, then its parent's parent, etc (i.e. this node, going toward the root).
  void AppendAncestors (std::vector<std::shared_ptr<SceneGraphNode>> &ancestors) const {
    ancestors.emplace_back(shared_from_this());
    std::shared_ptr<SceneGraphNode> parent(m_parent.lock());
    if (parent) {
      AppendAncestors(ancestors);
    }
  }

  // The transform member gives the coordinate transformation (an affine transformation)
  // from this node's coordinate system to its parent's.  For the root node, the "parent
  // coordinate system" is the standard coordinate system (which can be thought of as
  // some kind of global coordinates).
  Transform m_transform;

  // This uses a weak_ptr to avoid a cycle of shared_ptrs which would then be indestructible.
  std::weak_ptr<SceneGraphNode> m_parent;
  // This is the set of all child nodes.
  ChildSet m_children;
  
};
