#ifndef _SDD_ORDER_ORDER_HH_
#define _SDD_ORDER_ORDER_HH_

#include <initializer_list>
#include <memory>
#include <vector>

#include <boost/optional.hpp>

#include "sdd/conf/variable_traits.hh"

namespace sdd { namespace order {

/// @cond INTERNAL_DOC

/*-------------------------------------------------------------------------------------------*/

// Forward declaration of an order's node.
template <typename C>
struct node;

/*-------------------------------------------------------------------------------------------*/

/// @brief Represent an order of identifiers.
template <typename C>
using order_ptr_type = std::shared_ptr<node<C>>;

/*-------------------------------------------------------------------------------------------*/

/// @brief An element of a linked list of nodes, associating a (library) variable to an (user)
/// identifier.
///
/// We create our own type rather than using std::list. This is beacause we don't want the user
/// to add identifier to an order which has already been added as a nested order.
template <typename C>
struct node
{
  /// @brief A variable type.
  typedef typename C::Variable variable_type;

  /// @brief An identifier type.
  typedef typename C::Identifier identifier_type;

  /// @brief The library variable.
  ///
  /// A variable is automatically assigned to an identifier by the library.
  const variable_type variable;

  /// @brief The user identifier.
  ///
  /// When nullptr, it's an artificial node. That is, a node generated by the library.
  const std::unique_ptr<identifier_type> identifier;

  /// @brief The nested order.
  ///
  /// If empty, the node is a flat node.
  const order_ptr_type<C> nested;

  /// @brief The node's next variable.
  const order_ptr_type<C> next;

  /// @brief Constructor.
  node( const variable_type& var, std::unique_ptr<identifier_type>&& id
      , order_ptr_type<C> nst, order_ptr_type<C> nxt)
    : variable(var)
    , identifier(std::move(id))
    , nested(nst)
    , next(nxt)
  {
  }
};

/// @endcond

/*-------------------------------------------------------------------------------------------*/

/// @brief Represent an order of identifiers.
template <typename C>
class order
{
public:

  /// @brief The type of a variable.
  typedef typename C::Variable variable_type;

  /// @brief The type of an identifier.
  typedef typename C::Identifier identifier_type;

/// @cond INTERNAL_DOC
private:

  /// @brief The concrete order.
  order_ptr_type<C> order_ptr_;

public:

  /// @brief Constructor.
  order(const order_ptr_type<C>& ptr)
    : order_ptr_(ptr)
  {
  }

  /// @brief Get the concrete order.
  const order_ptr_type<C>&
  ptr()
  const noexcept
  {
    return order_ptr_;
  }

/// @endcond

  /// @brief Default constructor.
  order()
    : order_ptr_(nullptr)
  {
  }

  /// @brief Constructor from a list of identifiers.
  template <typename InputIterator>
  order(InputIterator begin, InputIterator end)
    : order()
  {
    std::vector<identifier_type> tmp(begin, end);
    for (auto rcit = tmp.crbegin(); rcit != tmp.crend(); ++rcit)
    {
      add(*rcit);
    }
  }

  /// @brief Constructor from a list of identifiers.
  order(std::initializer_list<identifier_type> list)
    : order(list.begin(), list.end())
  {
  }


  /// @brief Copy operator.
  order&
  operator=(const order& other)
  {
    order_ptr_ = other.order_ptr_;
    return *this;
  }

  /// @brief Tell if this order is empty.
  ///
  /// It's unsafe to call any other method, except add(), if this order is empty.
  bool
  empty()
  const noexcept
  {
    return not order_ptr_;
  }

  /// @brief Get the variable of this order's head.
  const variable_type&
  variable()
  const noexcept
  {
    return order_ptr_->variable;
  }

  /// @brief Get the idetifier of this order's head.
  const identifier_type&
  identifier()
  const noexcept
  {
    return *order_ptr_->identifier;
  }

  /// @brief Get this order's head's next order.
  order
  next()
  const noexcept
  {
    return order(order_ptr_->next);
  }

  /// @brief Get this order's head's nested order.
  order
  nested()
  const noexcept
  {
    return order(order_ptr_->nested);
  }

  /// @brief Get the variable of an identifier
  const variable_type&
  identifier_variable(const identifier_type& id)
  const
  {
    std::function<boost::optional<variable_type>(const order_ptr_type<C>&)> helper;
    helper = [&helper, &id](const order_ptr_type<C>& ptr)
    -> boost::optional<variable_type>
    {
      if (not ptr)
      {
        return boost::optional<variable_type>();
      }
      if (ptr->identifier and id == *ptr->identifier)
      {
        return ptr->variable;
      }
      if (ptr->nested)
      {
        const auto res = helper(ptr->nested);
        if (res)
        {
          return res;
        }
      }
      return helper(ptr->next);
    };

    const auto res = helper(order_ptr_);
    if (res)
    {
      return *res;
    }
    else
    {
      throw std::runtime_error("Identifier not found.");
    }
  }

  order&
  add(const identifier_type& id)
  {
    return add(id, nullptr);
  }

  order&
  add(const identifier_type& id, const order& nested)
  {
    return add(id, nested.order_ptr_);
  }

private:

  order&
  add(const identifier_type& id, order_ptr_type<C> nested_ptr)
  {
    const variable_type var = empty()
                            ? conf::variable_traits<variable_type>::first()
                            : conf::variable_traits<variable_type>::next(variable());

    typedef std::unique_ptr<identifier_type> optional_id_type;

    order_ptr_ = std::make_shared<node<C>>( var
                                          , optional_id_type(new identifier_type(id))
                                          , nested_ptr
                                          , order_ptr_);
    return *this;
  }
};

/*-------------------------------------------------------------------------------------------*/

/// @related order
template <typename C>
std::ostream&
operator<<(std::ostream& os, const order<C>& o)
{
  if (not o.empty())
  {
    os << o.identifier();
    if (not o.nested().empty())
    {
      os << " | (" << o.nested() << ")";
    }
    if (not o.next().empty())
    {
      os << " >> " << o.next();
    }
  }
  return os;
}

/*-------------------------------------------------------------------------------------------*/

}} // namespace sdd::order

#endif // _SDD_ORDER_ORDER_HH_
