#pragma once

#include <memory> // shared_ptr

#include "sdd/dd/definition_fwd.hh"

namespace sdd { namespace dd {

/*------------------------------------------------------------------------------------------------*/

/// @internal
template <typename C>
struct sdd_stack
{
  SDD<C> sdd;
  std::shared_ptr<sdd_stack> next;

  sdd_stack(const SDD<C>& s, const std::shared_ptr<sdd_stack>& n)
    : sdd(s), next(n)
  {}
};

/*------------------------------------------------------------------------------------------------*/

}} // namespace sdd::dd