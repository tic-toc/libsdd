#ifndef _SDD_ORDER_STRATEGIES_FORCE_HYPEREDGE_HH_
#define _SDD_ORDER_STRATEGIES_FORCE_HYPEREDGE_HH_

#include <algorithm>  // minmax_element
#include <numeric>    // accumulate
#include <vector>

#include <boost/container/flat_set.hpp>

#include "sdd/order/strategies/force_hyperedge_fwd.hh"
#include "sdd/order/strategies/force_vertex.hh"

namespace sdd { namespace force {

/*------------------------------------------------------------------------------------------------*/

struct hyperedge
{
  /// @internal
  /// @brief The center of gravity.
  double cog;

  /// @internal
  /// @brief Vertices connected to this hyperedge.
  boost::container::flat_set<vertex*> vertices;

  /// @brief Constructor with an already existing container of vertices.
  hyperedge(const boost::container::flat_set<vertex*>& v)
    : cog(0), vertices(v)
  {}

  /// @internal
  /// @brief Compute the center of gravity.
  void
  center_of_gravity()
  noexcept
  {
    assert(not vertices.empty());
    cog = std::accumulate( vertices.cbegin(), vertices.cend(), 0
                         , [](double acc, const vertex* v){return acc + v->location;}
                         ) / vertices.size();
  }

  /// @internal
  /// @brief Compute the span of all vertices.
  double
  span()
  const noexcept
  {
    assert(not vertices.empty());
    const auto minmax = std::minmax_element( vertices.cbegin(), vertices.cend()
                                           , [](const vertex* lhs, const vertex* rhs)
                                               {return lhs->location < rhs->location;});
    return *minmax.second - *minmax.first;
  }
};

/*------------------------------------------------------------------------------------------------*/

}} // namespace sdd::force

#endif // _SDD_ORDER_STRATEGIES_FORCE_HYPEREDGE_HH_
