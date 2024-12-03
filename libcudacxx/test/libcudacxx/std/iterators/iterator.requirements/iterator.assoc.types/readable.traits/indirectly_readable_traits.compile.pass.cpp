//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES.
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// template<class T>
// struct indirectly_readable_traits;

#include <cuda/std/concepts>
#include <cuda/std/iterator>

#if TEST_STD_VER > 2017
template <class T>
concept has_no_value_type = !requires { typename cuda::std::indirectly_readable_traits<T>::value_type; };

template <class T, class Expected>
concept value_type_matches =
  cuda::std::same_as<typename cuda::std::indirectly_readable_traits<T>::value_type, Expected>;

#else
template <class T>
_CCCL_CONCEPT_FRAGMENT(has_no_value_type_,
                       requires()(typename(typename cuda::std::indirectly_readable_traits<T>::value_type)));

template <class T>
_CCCL_CONCEPT has_no_value_type = !_CCCL_FRAGMENT(has_no_value_type_, T);

template <class T, class Expected>
_CCCL_CONCEPT_FRAGMENT(
  value_type_matches_,
  requires()(typename(typename cuda::std::indirectly_readable_traits<T>::value_type),
             requires(cuda::std::same_as<typename cuda::std::indirectly_readable_traits<T>::value_type, Expected>)));

template <class T, class Expected>
_CCCL_CONCEPT value_type_matches = _CCCL_FRAGMENT(value_type_matches_, T, Expected);
#endif

template <class T>
__host__ __device__ constexpr bool check_pointer()
{
  constexpr bool result = value_type_matches<T*, T>;
  static_assert(value_type_matches<T const*, T> == result);
  static_assert(value_type_matches<T volatile*, T> == result);
  static_assert(value_type_matches<T const volatile*, T> == result);

  static_assert(value_type_matches<T* const, T> == result);
  static_assert(value_type_matches<T const* const, T> == result);
  static_assert(value_type_matches<T volatile* const, T> == result);
  static_assert(value_type_matches<T const volatile* const, T> == result);

  return result;
}

template <class T>
__host__ __device__ constexpr bool check_array()
{
  static_assert(value_type_matches<T[], T>);
  static_assert(value_type_matches<T const[], T>);
  static_assert(value_type_matches<T volatile[], T>);
  static_assert(value_type_matches<T const volatile[], T>);
  static_assert(value_type_matches<T[10], T>);
  static_assert(value_type_matches<T const[10], T>);
  static_assert(value_type_matches<T volatile[10], T>);
  static_assert(value_type_matches<T const volatile[10], T>);
  return true;
}

template <class T, class Expected>
__host__ __device__ constexpr bool check_member()
{
  static_assert(value_type_matches<T, Expected>);
  static_assert(value_type_matches<T const, Expected>);
  static_assert(value_type_matches<T volatile, Expected>);
  return true;
}

static_assert(check_pointer<int>());
static_assert(check_pointer<int*>());
static_assert(check_pointer<int[10]>());
static_assert(!check_pointer<void>());
static_assert(!check_pointer<int()>());

static_assert(check_array<int>());
static_assert(check_array<int*>());
static_assert(check_array<int[10]>());

template <class T>
struct ValueOf
{
  using value_type = T;
};

template <class U>
struct ElementOf
{
  using element_type = U;
};

template <class T, class U>
struct TwoTypes
{
  using value_type   = T;
  using element_type = U;
};

static_assert(check_member<ValueOf<int>, int>());
static_assert(check_member<ValueOf<int[10]>, int[10]>());
static_assert(check_member<ValueOf<int[]>, int[]>());
static_assert(has_no_value_type<ValueOf<void>>);
static_assert(has_no_value_type<ValueOf<int()>>);
static_assert(has_no_value_type<ValueOf<int&>>);
static_assert(has_no_value_type<ValueOf<int&&>>);

static_assert(check_member<ElementOf<int>, int>());
static_assert(check_member<ElementOf<int[10]>, int[10]>());
static_assert(check_member<ElementOf<int[]>, int[]>());
static_assert(has_no_value_type<ElementOf<void>>);
static_assert(has_no_value_type<ElementOf<int()>>);
static_assert(has_no_value_type<ElementOf<int&>>);
static_assert(has_no_value_type<ElementOf<int&&>>);

static_assert(check_member<TwoTypes<int, int>, int>());
static_assert(check_member<TwoTypes<int, int const>, int>());
static_assert(check_member<TwoTypes<int, int volatile>, int>());
static_assert(check_member<TwoTypes<int, int const volatile>, int>());
static_assert(check_member<TwoTypes<int const, int>, int>());
static_assert(check_member<TwoTypes<int const, int const>, int>());
static_assert(check_member<TwoTypes<int const, int volatile>, int>());
static_assert(check_member<TwoTypes<int const, int const volatile>, int>());
static_assert(check_member<TwoTypes<int volatile, int>, int>());
static_assert(check_member<TwoTypes<int volatile, int const>, int>());
static_assert(check_member<TwoTypes<int volatile, int volatile>, int>());
static_assert(check_member<TwoTypes<int volatile, int const volatile>, int>());
static_assert(check_member<TwoTypes<int const volatile, int>, int>());
static_assert(check_member<TwoTypes<int const volatile, int const>, int>());
static_assert(check_member<TwoTypes<int const volatile, int volatile>, int>());
static_assert(check_member<TwoTypes<int const volatile, int const volatile>, int>());
static_assert(has_no_value_type<TwoTypes<int, long>>);
static_assert(has_no_value_type<TwoTypes<int, int&>>);
static_assert(has_no_value_type<TwoTypes<int&, int>>);

struct S2
{};
template <>
struct cuda::std::indirectly_readable_traits<S2>
{
  using value_type = int;
};
static_assert(value_type_matches<S2, int>);
static_assert(value_type_matches<const S2, int>);
static_assert(has_no_value_type<volatile S2>);
static_assert(has_no_value_type<const volatile S2>);
static_assert(has_no_value_type<S2&>);
static_assert(has_no_value_type<const S2&>);

static_assert(has_no_value_type<void>);
static_assert(has_no_value_type<int>);
static_assert(has_no_value_type<cuda::std::nullptr_t>);

int main(int, char**)
{
  return 0;
}
