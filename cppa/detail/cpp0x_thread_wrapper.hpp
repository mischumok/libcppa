#ifndef CPPA_DETAIL_CPP0X_THREAD_WRAPPER_HPP
#define CPPA_DETAIL_CPP0X_THREAD_WRAPPER_HPP

#include <boost/thread.hpp>

namespace std {

using boost::mutex;
using boost::recursive_mutex;
using boost::timed_mutex;
using boost::recursive_timed_mutex;
using boost::adopt_lock;
using boost::defer_lock;
using boost::try_to_lock;

using boost::lock_guard;
using boost::unique_lock;

} // namespace std

#endif // CPPA_DETAIL_CPP0X_THREAD_WRAPPER_HPP