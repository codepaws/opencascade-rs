#pragma once
#include "rust/cxx.h"
#include <NCollection_List.hxx>
#include <memory>

// Generic template constructor
template <typename T, typename... Args> std::unique_ptr<T> construct_unique(Args... args) {
  return std::unique_ptr<T>(new T(args...));
}

// Type casting
template <typename T, typename U> inline U upcast(T src) { return src; }
template <typename T, typename U> inline const U &upcast_ref(const T &src) { return src; }

// Generic List
template <typename T> std::unique_ptr<std::vector<T>> list_to_vector(const NCollection_List<T> &list) {
  return std::unique_ptr<std::vector<T>>(new std::vector<T>(list.begin(), list.end()));
}

template <typename T> const T &handle_try_deref(const opencascade::handle<T> &handle) {
  if (handle.IsNull()) {
    throw std::runtime_error("null handle dereference");
  }
  return *handle;
}

// daedalus-fixes-v0.3.0 P8: MSVC-compat explicit handle_try_deref overloads.
//
// OCCT's DEFINE_STANDARD_HANDLE(Handle_X, X) emits Handle_X as:
//   - TYPEDEF for opencascade::handle<X> on POSIX (template above works directly).
//   - CLASS deriving from opencascade::handle<X> on MSVC (template can't be
//     selected when cxx-bridge takes the address with const Handle_X & arg).
//
// Solution: provide a NON-TEMPLATE overload per Handle_X type used by the
// bridge, gated on _MSC_VER (on POSIX the macro is a no-op so we don't
// collide with the template above, which already works there).
//
// To use: in your per-feature .hxx, include the relevant Handle type's
// header and call DAEDALUS_HANDLE_TRY_DEREF_OVERLOAD(YourType).
#ifdef _MSC_VER
#define DAEDALUS_HANDLE_TRY_DEREF_OVERLOAD(T)                              \
  inline const T &handle_try_deref(const Handle_##T &h) {                  \
    if (h.IsNull()) {                                                      \
      throw std::runtime_error("null handle dereference");                 \
    }                                                                      \
    return *h;                                                             \
  }
#else
#define DAEDALUS_HANDLE_TRY_DEREF_OVERLOAD(T)
#endif
