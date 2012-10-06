#ifndef _SDD_HOM_FIXPOINT_HH_
#define _SDD_HOM_FIXPOINT_HH_

//#include <algorithm> // max
#include <iosfwd>

#include "sdd/dd/definition.hh"
#include "sdd/hom/context_fwd.hh"
#include "sdd/hom/definition_fwd.hh"
#include "sdd/hom/identity.hh"
#include "sdd/hom/local.hh"

namespace sdd { namespace hom {

/*-------------------------------------------------------------------------------------------*/

/// @cond INTERNAL_DOC

/// @brief Fixpoint homomorphism.
template <typename C>
class fixpoint
{
private:

  /// @brief The homomorphism to apply until a fixpoint is reached.
  const homomorphism<C> h_;

public:

  /// @brief Constructor.
  fixpoint(const homomorphism<C>& h)
    : h_(h)
  {
  }

  /// @brief Evaluation.
  SDD<C>
  operator()(context<C>& cxt, const SDD<C>& x)
  const
  {
    SDD<C> x1 = x;
    SDD<C> x2 = x1;
//    const std::size_t size = cxt.size() - (cxt.size() / 8);
//    context<C> fixpoint_context(std::max(size, 100), cxt.sdd_context());
    do
    {
      x2 = x1;
//      x1 = h_(fixpoint_context, x1);
      x1 = h_(cxt, x1);
    } while (x1 != x2);
    return x1;
  }

  /// @brief Skip variable predicate.
  bool
  skip(const typename C::Variable& v)
  const noexcept
  {
    return h_.skip(v);
  }

  homomorphism<C>
  hom()
  const noexcept
  {
    return h_;
  }
};

/*-------------------------------------------------------------------------------------------*/

/// @brief Equality of fixpoint.
/// @related fixpoint
template <typename C>
inline
bool
operator==(const fixpoint<C>& lhs, const fixpoint<C>& rhs)
noexcept
{
  return lhs.hom() == rhs.hom();
}

/// @related fixpoint
template <typename C>
std::ostream&
operator<<(std::ostream& os, const fixpoint<C>& f)
{
  return os << "(" << f.hom() << ")*";
}

/*-------------------------------------------------------------------------------------------*/

// Forward declaration.
template <typename C>
homomorphism<C>
Fixpoint(const homomorphism<C>&);

/// @brief Concrete creation of Fixpoint.
template <typename C>
struct fixpoint_builder_helper
{
  typedef homomorphism<C> result_type;

  homomorphism<C>
  operator()(const identity<C>&, const homomorphism<C>& h)
  const noexcept
  {
    return h;
  }

  homomorphism<C>
  operator()(const fixpoint<C>& f, const homomorphism<C>&)
  const noexcept
  {
    return f.hom();
  }

  homomorphism<C>
  operator()(const local<C>& l, const homomorphism<C>&)
  const noexcept
  {
    return Local<C>(l.variable(), Fixpoint<C>(l.hom()));
  }

  template <typename T>
  homomorphism<C>
  operator()(const T&, const homomorphism<C>& h)
  const noexcept
  {
    return homomorphism<C>::create(internal::mem::construct<fixpoint<C>>(), h);
  }
};

/// @endcond

/*-------------------------------------------------------------------------------------------*/

/// @brief Create the Fixpoint homomorphism.
/// @related homomorphism
template <typename C>
homomorphism<C>
Fixpoint(const homomorphism<C>& h)
{
  return apply_visitor(fixpoint_builder_helper<C>(), h->data(), h);
}

/*-------------------------------------------------------------------------------------------*/

}} // namespace sdd::hom

namespace std {

/*-------------------------------------------------------------------------------------------*/

/// @cond INTERNAL_DOC

/// @brief Hash specialization for sdd::hom::fixpoint.
template <typename C>
struct hash<sdd::hom::fixpoint<C>>
{
  std::size_t
  operator()(const sdd::hom::fixpoint<C>& f)
  const noexcept
  {
    std::size_t seed = 345789; // avoid to have the same hash as the contained homormorphism
    sdd::internal::util::hash_combine(seed, f.hom());
    return seed;
  }
};

/// @endcond

/*-------------------------------------------------------------------------------------------*/

} // namespace std

#endif // _SDD_HOM_FIXPOINT_HH_