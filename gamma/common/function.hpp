// Copyright (c) 2018 Aleksey Strelnikov
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef GAMMA_COMMON_FUNCTION_HPP_
#define GAMMA_COMMON_FUNCTION_HPP_

#include <cstddef>
#include <type_traits>
#include <utility>

namespace y {

// A non-allocating, move-only, type-erasing functor for storing and calling a
// small non-const "FunctionObject". The key reason for its existence is it can
// accept move-only types. The stored object can have a max size of
// 2 * sizeof(void*) and max alignment requirement of alignof(void*).
//
// As with the rest of the library, this type does *not* try to work with
// exceptions.
template <typename>
class Function;

template <typename R, typename... Args>
class Function<R(Args...)> {
 public:
  static constexpr size_t MaxTypeSize() { return 2 * sizeof(void*); }
  static constexpr size_t MaxTypeAlignment() { return alignof(void*); }

  Function() = default;
  Function(Function&& other) noexcept
      : move_or_destroy_(other.move_or_destroy_), call_(other.call_) {
    if (move_or_destroy_ != nullptr) move_or_destroy_(&other.bytes_, &bytes_);
  }

  Function& operator=(Function&& other) noexcept {
    clear();
    if (other.move_or_destroy_ != nullptr) {
      move_or_destroy_ = other.move_or_destroy_;
      call_ = other.call_;
      move_or_destroy_(&other.bytes_, &bytes_);
    }
    return *this;
  }

  ~Function() { clear(); }

  Function(const Function&) = delete;
  Function& operator=(const Function&) = delete;

  template <typename F>
  Function(F&& f) {
    assign(std::forward<F>(f));
  }

  template <typename F>
  Function& operator=(F&& f) {
    clear();
    assign(std::forward<F>(f));
    return *this;
  }

  // Overloads for function references to dispatch to function pointers.
  Function(R (&f)(Args...)) : Function(&f) {}
  Function& operator=(R (&f)(Args...)) { return operator=(&f); }

  Function(nullptr_t) : Function() {}
  Function& operator=(nullptr_t) {
    clear();
    return *this;
  }

  explicit operator bool() const noexcept {
    return move_or_destroy_ != nullptr;
  }

  R operator()(Args&&... args) {
    return call_(bytes_, std::forward<Args>(args)...);
  }

  void clear() {
    if (move_or_destroy_ != nullptr) {
      move_or_destroy_(nullptr, bytes_);
      move_or_destroy_ = nullptr;
      call_ = nullptr;
    }
  }

 private:
  template <typename F>
  void assign(F&& f) {
    static_assert(sizeof(F) <= MaxTypeSize(),
                  "exceeds size limit for type erased object");
    static_assert(alignof(F) <= MaxTypeAlignment(),
                  "exceeds alignment limit for type erased object");

    using Type = typename std::decay<F>::type;

    ::new (bytes_) Type(std::forward<F>(f));

    move_or_destroy_ = [](void* from, void* to) {
      if (from == nullptr) {
        // This signifies that `to` stores an object of `Type` to destroy.
        static_cast<Type*>(to)->~Type();
      } else {
        // This signifies that `to` is uninitialized memory and `from` holds an
        // object of `Type` to move construct into said memory.
        ::new (to) Type(std::move(*static_cast<Type*>(from)));
      }
    };

    call_ = [](void* bytes, Args&&... args) -> R {
      return (*static_cast<Type*>(bytes))(std::forward<Args>(args)...);
    };
  }

  using MoveOrDestroyFn = void (*)(void*, void*);
  using CallFn = R (*)(void*, Args&&...);

  alignas(MaxTypeAlignment()) unsigned char bytes_[MaxTypeSize()];
  MoveOrDestroyFn move_or_destroy_ = nullptr;
  CallFn call_ = nullptr;
};

}  // namespace y
#endif  // GAMMA_COMMON_OBJECT_HPP_
