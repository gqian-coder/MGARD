#ifndef TENSORMESHHIERARCHY_HPP
#define TENSORMESHHIERARCHY_HPP
//!\file
//!\brief Increasing hierarchy of tensor meshes.

#include <cstddef>

#include <array>
#include <iterator>
#include <type_traits>
#include <vector>

#include "TensorMeshHierarchyIteration.hpp"
#include "utilities.hpp"

namespace mgard {

//! Hierarchy of meshes produced by subsampling an initial mesh.
template <std::size_t N, typename Real> class TensorMeshHierarchy {
public:
  //! Constructor.
  //!
  //!\param shape Shape of the initial, finest mesh to sit atop the hiearachy.
  explicit TensorMeshHierarchy(const std::array<std::size_t, N> &shape);

  //! Constructor.
  //!
  //!\param shape Shape of the initial, finest mesh to sit atop the hiearachy.
  //!\param coordinates Coordinates of the nodes in the finest mesh.
  TensorMeshHierarchy(const std::array<std::size_t, N> &shape,
                      const std::array<std::vector<Real>, N> &coordinates);

  //! Report the number of degrees of freedom in the finest mesh.
  std::size_t ndof() const;

  //! Report the number of degrees of freedom in a mesh.
  //!
  //!\param l Index of the mesh.
  std::size_t ndof(const std::size_t l) const;

  //! Generate the indices (in a particular dimension) of a mesh level.
  //!
  //!\param l Mesh index.
  //!\param dimension Dimension index.
  TensorIndexRange indices(const std::size_t l,
                           const std::size_t dimension) const;

  //! Find the index of the level which introduced a node.
  //!
  //!\param multiindex Multiindex of the node.
  std::size_t date_of_birth(const std::array<std::size_t, N> multiindex) const;

  //! Access the values at a level's nodes.
  //!
  //!\param v Dataset defined on the hierarchy.
  //!\param l Index of the level.
  PseudoArray<Real> on_nodes(Real *const v, const std::size_t l) const;

  //!\overload
  PseudoArray<const Real> on_nodes(Real const *const v,
                                   const std::size_t l) const;

  //! Access the values at a level's 'new' nodes.
  //!
  //!\param v Dataset defined on the hierarchy.
  //!\param l Index of the level.
  PseudoArray<Real> on_new_nodes(Real *const v, const std::size_t l) const;

  //!\overload
  PseudoArray<const Real> on_new_nodes(Real const *const v,
                                       const std::size_t l) const;

  //! Access the value associated to a particular node.
  //!
  //!\param v Dataset defined on the hierarchy.
  //!\param multiindex Multiindex of the node.
  Real &at(Real *const v, const std::array<std::size_t, N> multiindex) const;

  //!\overload
  const Real &at(Real const *const u,
                 const std::array<std::size_t, N> multiindex) const;

  //! Shapes of the meshes composing the hierarchy, in 'increasing' order.
  std::vector<std::array<std::size_t, N>> shapes;

  //! Coordinates of the nodes in the finest mesh.
  std::array<std::vector<Real>, N> coordinates;

  //! Whether the mesh on the finest level is the default one (nodes uniformly
  //! spaced on `[0, 1]` in each dimension).
  //!
  //!\note If the default mesh is manually passed to the constructor, this flag
  //! will be set to `false`. Manually passed meshes are not checked for
  //! equality with the default mesh.
  bool uniform;

  //! Index of finest mesh.
  std::size_t L;

  //! For each dimension, for each node in the finest level, the index of the
  //! level which introduced that node (its 'date of birth').
  std::array<std::vector<std::size_t>, N> dates_of_birth;

protected:
  //! Check that a mesh index is in bounds.
  //!
  //!\param l Mesh index.
  void check_mesh_index_bounds(const std::size_t l) const;

  //! Check that a pair of mesh indices are nondecreasing.
  //!
  //!\param l Smaller (nonlarger) mesh index.
  //!\param m Larger (nonsmaller) mesh index.
  void check_mesh_indices_nondecreasing(const std::size_t l,
                                        const std::size_t m) const;

  //! Check that a mesh index is nonzero.
  //!
  //!\param l Mesh index.
  void check_mesh_index_nonzero(const std::size_t l) const;

private:
  //! Compute the index of a node in the 'unshuffled' ordering.
  //!
  //!\param multiindex Multiindex of the node.
  std::size_t
  unshuffled_index(const std::array<std::size_t, N> multiindex) const;

  //! Compute the index of a node in the 'shuffled' ordering.
  //!
  //!\param multiindex Multiindex of the node.
  std::size_t index(const std::array<std::size_t, N> multiindex) const;

  //! Indices of the nodes in the 'shuffled' ordering.
  std::vector<std::size_t> _shuffled_indices;

  //! Indices (in each dimension) of the nodes in each mesh level.
  std::array<std::vector<std::vector<std::size_t>>, N> _indices;

  //! Implement `on_nodes`.
  template <typename T>
  PseudoArray<T> on_nodes(T *const v, const std::size_t l) const;

  //! Implement `on_new_nodes`.
  template <typename T>
  PseudoArray<T> on_new_nodes(T *const v, const std::size_t l) const;

  //! Implement `at`.
  template <typename T>
  T &at(T *const v, const std::array<std::size_t, N> multiindex) const;
};

//! Equality comparison.
template <std::size_t N, typename Real>
bool operator==(const TensorMeshHierarchy<N, Real> &a,
                const TensorMeshHierarchy<N, Real> &b);

//! Inequality comparison.
template <std::size_t N, typename Real>
bool operator!=(const TensorMeshHierarchy<N, Real> &a,
                const TensorMeshHierarchy<N, Real> &b);

} // namespace mgard

#include "TensorMeshHierarchy.tpp"
#endif
