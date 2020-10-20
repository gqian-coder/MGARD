#ifndef TENSORMESHHIERARCHYITERATION_HPP
#define TENSORMESHHIERARCHYITERATION_HPP
//!\file
//!\brief Iteration over `TensorMeshHierarchy`s.

#include <cstddef>

#include <array>
#include <vector>

#include "utilities.hpp"

namespace mgard {

//! Forward declaration.
template <std::size_t N, typename Real> class TensorMeshHierarchy;

//! Indices in a particular dimension of nodes of a particular level in a mesh
//! hierarchy.
class TensorIndexRange {
public:
  //! Constructor.
  //!
  //! We define this constructor for use in `singleton` and so that objects with
  //! data members of type `TensorIndexRange` (for example,
  //! `ConstituentRestriction`) may be default constructed.
  TensorIndexRange() = default;

  //! Constructor.
  //
  //!\param hierarchy Associated mesh hierarchy.
  //!\param l Mesh index.
  //!\param dimension Dimension index.
  template <std::size_t N, typename Real>
  TensorIndexRange(const TensorMeshHierarchy<N, Real> &hierarchy,
                   const std::size_t l, const std::size_t dimension);

  //! Factory member function.
  //!
  //! We define this function so that we can create ranges which yield the
  //! single value `0` when iterated over. This is convenient for
  //! `TensorLinearOperator`.
  static TensorIndexRange singleton();

  //! Return the size of the range.
  std::size_t size() const;

  // Forward declaration.
  class iterator;

  //! Return an iterator to the beginning of the indices.
  iterator begin() const;

  //! Return an iterator to the end of the indices.
  iterator end() const;

  // `size_finest` and `size_coarse` aren't `const` so that the defaulted copy
  // assignment operator won't be deleted.

  //! Size in the particular dimension of the finest mesh in the hierarchy.
  std::size_t size_finest;

  //! Size in the particular dimension of the mesh in question.
  std::size_t size_coarse;
};

//! Equality comparison.
bool operator==(const TensorIndexRange &a, const TensorIndexRange &b);

//! Inequality comparison.
bool operator!=(const TensorIndexRange &a, const TensorIndexRange &b);

//! Iterator over the indices in a particular dimension of nodes of a particular
//! level in a mesh hierarchy.
//!
//! This iterator does *not* satisfy all the requirements of a forward iterator.
//! See <https://en.cppreference.com/w/cpp/named_req/ForwardIterator>. Like a
//! forward iterator, though, a `TensorIndexRange::iterator` can be used to
//! iterate over a `TensorIndexRange` repeatedly, with the same values obtained
//! each time.
class TensorIndexRange::iterator {
public:
  // See note above.
  //! Category of the iterator.
  using iterator_category = std::input_iterator_tag;
  //! Type iterated over.
  using value_type = std::size_t;
  //! Type for distance between iterators.
  using difference_type = std::ptrdiff_t;
  //! Pointer to `value_type`.
  using pointer = value_type *;
  //! Type returned by the dereference operator.
  using reference = value_type;

  //! Constructor.
  //!
  //! This constructor is provided so that arrays of iterators may be formed.
  //! A default-constructed iterator must be assigned to before being used.
  iterator() = default;

  //! Constructor.
  //!
  //!\param iterable View of indices to be iterated over.
  //!\param inner Position in the index range.
  iterator(const TensorIndexRange &iterable, const std::size_t inner);

  //! Equality comparison.
  bool operator==(const iterator &other) const;

  //! Inequality comparison.
  bool operator!=(const iterator &other) const;

  //! Preincrement.
  iterator &operator++();

  //! Postincrement.
  iterator operator++(int);

  //! Predecrement.
  iterator &operator--();

  //! Postdecrement.
  iterator operator--(int);

  //! Dereference.
  reference operator*() const;

  //! View of indices being iterated over.
  TensorIndexRange const *iterable;

private:
  //! Position in the index range.
  std::size_t inner;
};

template <std::size_t N> class TensorNode {
public:
  //! Constructor.
  //!
  //!\param inner Underlying multiindex iterator.
  TensorNode(
      const typename CartesianProduct<TensorIndexRange, N>::iterator inner);

  //! Multiindex of the node.
  std::array<std::size_t, N> multiindex;

  //! Return the node to the left in a given dimension *in the mesh currently
  //! being iterated over* (determined by `inner`).
  //!
  //! If this node is at the lefthand boundary of the domain, this node will
  //! be returned.
  //!
  //!\param i Index of the dimension.
  TensorNode predecessor(const std::size_t i) const;

  //! Return the node to the right in a given dimension *in the mesh currently
  //! being iterated over* (determined by `inner`).
  //!
  //! If this node is at the righthand boundary of the domain, this node will
  //! be returned.
  //!
  //!\param i Index of the dimension.
  TensorNode successor(const std::size_t i) const;

private:
  //! Underlying multiindex iterator.
  const typename CartesianProduct<TensorIndexRange, N>::iterator inner;
};

//! Nodes of a particular level in a mesh hierarchy.
template <std::size_t N, typename Real> class TensorNodeRange {
public:
  //! Constructor.
  //!
  //!\param hierarchy Associated mesh hierarchy.
  //!\param l Index of the mesh level to be iterated over.
  TensorNodeRange(const TensorMeshHierarchy<N, Real> &hierarchy,
                  const std::size_t l);

  // Forward declaration.
  class iterator;

  //! Return an iterator to the beginning of the nodes.
  iterator begin() const;

  //! Return an iterator to the end of the nodes.
  iterator end() const;

  //! Equality comparison.
  bool operator==(const TensorNodeRange &other) const;

  //! Inequality comparison.
  bool operator!=(const TensorNodeRange &other) const;

  //! Associated mesh hierarchy.
  const TensorMeshHierarchy<N, Real> &hierarchy;

private:
  //! Index of the level being iterated over.
  //!
  //! This is only stored so we can avoid comparing `multiindices` in the
  //! (in)equality comparison operators.
  const std::size_t l;

  //! Multiindices of the nodes on the level being iterated over.
  const CartesianProduct<TensorIndexRange, N> multiindices;
};

//! Iterator over the nodes of a mesh in a mesh hierarchy.
template <std::size_t N, typename Real>
class TensorNodeRange<N, Real>::iterator {
public:
  //! Category of the iterator.
  using iterator_category = std::input_iterator_tag;
  //! Type iterated over.
  using value_type = TensorNode<N>;
  //! Type for distance between iterators.
  using difference_type = std::ptrdiff_t;
  //! Pointer to `value_type`.
  using pointer = value_type *;
  //! Type returned by the dereference operator.
  using reference = value_type;

  //! Constructor.
  //!
  //!\param iterable View of nodes to be iterated over.
  //!\param inner Underlying multiindex iterator.
  iterator(
      const TensorNodeRange &iterable,
      const typename CartesianProduct<TensorIndexRange, N>::iterator &inner);

  //! Equality comparison.
  bool operator==(const iterator &other) const;

  //! Inequality comparison.
  bool operator!=(const iterator &other) const;

  //! Preincrement.
  iterator &operator++();

  //! Postincrement.
  iterator operator++(int);

  //! Dereference.
  reference operator*() const;

  //! View of nodes being iterated over.
  const TensorNodeRange &iterable;

private:
  //! Underlying multiindex iterator.
  typename CartesianProduct<TensorIndexRange, N>::iterator inner;
};

//! 'Reserved' nodes of a particular level in a mesh hierarchy.
//!
//! The difference between `TensorNodeRange` and `TensorReservedNodeRange` is
//! that the nodes of the latter only think of nodes present at their
//! introduction when asked for their 'neighbors' (predecessors and successors).
//! They haven't made any new friends (are 'reserved'), so to speak. For
//! example, if we're iterating over the finest level but `node` was present in
//! the coarsest level, `node.predecessor(i)` and `node.successor(i)` will be
//! other nodes in the coarse level, even if there are closer neighbors found in
//! later levels.
template <std::size_t N, typename Real> class TensorReservedNodeRange {
public:
  //! Constructor.
  //!
  //!\param hierarchy Associated mesh hierarchy.
  //!\param l Index of the mesh level to be iterated over.
  TensorReservedNodeRange(const TensorMeshHierarchy<N, Real> &hierarchy,
                          const std::size_t l);

  // Forward declaration.
  class iterator;

  //! Return an iterator to the beginning of the nodes.
  iterator begin() const;

  //! Return an iterator to the end of the nodes.
  iterator end() const;

  //! Equality comparison.
  bool operator==(const TensorReservedNodeRange &other) const;

  //! Inequality comparison.
  bool operator!=(const TensorReservedNodeRange &other) const;

  //! Associated mesh hierarchy.
  const TensorMeshHierarchy<N, Real> &hierarchy;

private:
  //! Index of the level being iterated over.
  //!
  //! This is only stored so we can avoid comparing `ranges` in the (in)equality
  //! comparison operators.
  const std::size_t l;

  //! Node ranges from the coarsest level up to the level being iterated over.
  const std::vector<TensorNodeRange<N, Real>> ranges;
};

//! Iterator over the 'reserved' nodes of a mesh in a mesh hierarchy.
template <std::size_t N, typename Real>
class TensorReservedNodeRange<N, Real>::iterator {
public:
  //! Category of the iterator.
  using iterator_category = std::input_iterator_tag;
  //! Type iterated over.
  using value_type = TensorNode<N>;
  //! Type for distance between iterators.
  using difference_type = std::ptrdiff_t;
  //! Pointer to `value_type`.
  using pointer = value_type *;
  //! Type returned by the dereference operator.
  using reference = value_type;

  //! Constructor.
  //!
  //!\param iterable View of nodes to be iterated over.
  //!\param inners Underlying range iterators.
  iterator(
      const TensorReservedNodeRange &iterable,
      const std::vector<typename TensorNodeRange<N, Real>::iterator> inners);

  //! Equality comparison.
  bool operator==(const iterator &other) const;

  //! Inequality comparison.
  bool operator!=(const iterator &other) const;

  //! Preincrement.
  iterator &operator++();

  //! Postincrement.
  iterator operator++(int);

  //! Dereference.
  reference operator*() const;

  //! View of nodes being iterated over.
  const TensorReservedNodeRange &iterable;

private:
  //! Underlying range iterators on the coarsest level up to the level being
  //! iterated over.
  //!
  //! The last entry, `inner_finest`, doesn't need to be mutable.
  mutable std::vector<typename TensorNodeRange<N, Real>::iterator> inners;

  //! Underlying range iterator on the level being iterated over. This is just
  //! the last entry of `inners`.
  typename TensorNodeRange<N, Real>::iterator inner_finest;
};

} // namespace mgard

#include "TensorMeshHierarchyIteration.tpp"
#endif
