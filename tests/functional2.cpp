#include <algorithm>
#include <iostream>
#include <utility> // for std::forward
#include <vector>

struct struct_a {
  struct_a() {}
  struct_a(int Xi) : X(Xi) {}
  int X{};
  int age() const { return X; }
};

struct struct_b {
  struct_b() {}
  struct_b(int Yi) : Y(Yi) {}
  int Y{};
  int age() const { return Y; }
};

auto Increment(struct_a& StructA) -> void { StructA.X++; }

auto Increment(struct_b& StructB) -> void { StructB.Y++; }

template <typename Object, typename Function> decltype(auto) call_on_object(Object&& object, Function function) {
  return function(std::forward<Object>(object));
}

struct older_than {
  int m_limit;
  older_than(int limit) : m_limit(limit) {}
  template <typename T> bool operator()(T&& object) const { return std::forward<T>(object).age() > m_limit; }
};

int ask() { return 42; }

typedef decltype(ask)* function_ptr;
struct convertible_to_function_ptr {
  operator function_ptr() const { return ask; }
};

/**
 * Calculate next x from the current x without knowledge of the velocity.
 */
auto Verlet(double Position, double Accelleration, double dT) -> double {
  auto PrevPos = Position;
  auto Time    = 0.;

  while (Position > 0.) {
    Time += dT;
    double TempPos = Position;
    Position       = Position * 2. - PrevPos + Accelleration * dT * dT;
    PrevPos        = TempPos;
  }
  return Time;
}

auto main() -> int {
  struct_a A{};
  std::cout << "Initial struct_a.X: " << A.X << std::endl;
  call_on_object(A, [](struct_a& obj) { Increment(obj); });
  std::cout << "After Increment struct_a.X: " << A.X << std::endl;

  struct_b B{};
  std::cout << "Initial struct_b.Y: " << B.Y << std::endl;
  call_on_object(B, [](struct_b& obj) { Increment(obj); });
  std::cout << "After Increment struct_b.Y: " << B.Y << std::endl;

  auto                        ask_ptr = &ask;
  convertible_to_function_ptr ask_wrapper;
  std::cout << ask_ptr() << std::endl;
  std::cout << ask_wrapper() << std::endl;

  struct_a A1{42};
  struct_a A2(420);
  struct_b B1{43};
  struct_b B2(430);

  older_than const      OlderThan(42);
  std::vector<struct_a> vStructA{{45}, {43}};
  std::vector<struct_b> vStructB{{450}, {430}, {430}};
  std::cout << std::count_if(vStructA.begin(), vStructA.end(), OlderThan) << std::endl;
  std::cout << std::count_if(vStructB.begin(), vStructB.end(), OlderThan) << std::endl;

  return 0;
}
