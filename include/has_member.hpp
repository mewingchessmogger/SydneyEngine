// Source - https://stackoverflow.com/a/62292282
// Posted by Dmytro Ovdiienko, modified by community. See post 'Timeline' for change history
// Retrieved 2026-06-14, License - CC BY-SA 4.0

/// @file has_something.h
/// The `has_member` macro implementation

#pragma once

#include <type_traits>

template<typename T, typename F>
constexpr auto has_member_impl(F&& f) -> decltype(f(std::declval<T>()), true)
{
  return true;
}

template<typename>
constexpr bool has_member_impl(...) { return false; }

#define has_member(V, EXPR) \
 has_member_impl<decltype(V)>( [](auto&& obj)->decltype(obj.EXPR){} )
