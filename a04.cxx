// Copyright (c) 2017 Paul Preney. All Rights Reserved.
//
// Compile with GCC options:
//   -std=c++14 -O3 -Werror -Wall -Wextra -Wold-style-cast -Woverloaded-virtual

#include <map>
#include <stdexcept>
#include <memory>
#include <tuple>
#include <limits>
#include <iostream>

using uint_type = unsigned long long;

class ack
{
public:
  using tuple_type = std::tuple<uint_type, uint_type>;
  
  uint_type operator ()(uint_type const& m, uint_type const& n) const
  {
    return calculate({m,n});
  }

private:
  static std::map<tuple_type, uint_type> cache_;

  uint_type calculate(tuple_type const& args) const
  {
    // Search if the result was previously computed...
    auto result_iter = cache_.find(args);
    if (result_iter != cache_.end())
      return result_iter->second;
  
    // Variable to hold the calculated value...
    uint_type retval;

    // Make it easier to get the current m and n values...
    uint_type const m = std::get<0>(args);
    uint_type const n = std::get<1>(args);

    // Compute the Ackermann function...
    if (m == 0)
    {
      if (n < std::numeric_limits<uint_type>::max())
        retval = n+1;
      else
        throw std::range_error{"ack::uint_type overflow!"};
    }
    else if (m > 0 && n == 0)
      retval = calculate({m-1,1});
    else if (m > 0 && n > 0)
      retval = calculate({m-1, calculate({m, n-1})});

    // Memoize its value...
    cache_.insert({args, retval});

    // Return the result...
    return retval;
  }
};

std::map<ack::tuple_type, uint_type> ack::cache_;


int main()
{
  using std::cout;

  ack f;

  for (uint_type m=0; m != 5; ++m)
  {
    for (uint_type n=0; n != 10; ++n)
    {
      cout << "f(" << m << ',' << n << ") = ";
      try
      {
        cout << f(m,n) << '\n';
      }
      catch (std::range_error &e)
      {
        cout << e.what() << '\n';
      }
      catch (std::bad_alloc &e)
      {
        cout << e.what() << '\n';
      }
      catch (...)
      {
        cout << "<UNKNOWN ERROR>\n";
      }
    }
    cout << '\n';
  }
}

