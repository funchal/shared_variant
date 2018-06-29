#include <memory>

struct enable_shared_variant : public std::enable_shared_from_this<enable_shared_variant>
{};

class X : public enable_shared_variant { X() = delete; };
class Y : public enable_shared_variant {};
class Z : public enable_shared_variant {};
class W {};

template<class T>
struct in_place_type_t {
    explicit in_place_type_t() = default;
};

template<typename... Args>
class shared_variant_helper;

template<typename T, typename... Args>
struct shared_variant_helper<T, Args...> {
};

template<typename... Args>
struct is_in_variant : std::false_type {};

template<typename U, typename T, typename... Args>
struct is_in_variant<U, T, Args...>
    : std::conditional_t<bool(std::is_same<U, T>::value), std::true_type, is_in_variant<U, Args...>> {};

template<typename ...Args>
class shared_variant {
public:
    shared_variant(const shared_variant&) = default;
    shared_variant(shared_variant&&) = default;
    shared_variant& operator=(const shared_variant& rhs) = default;
    shared_variant& operator=(shared_variant&& rhs) = default;

    // todo: require T::element_type in Args...
    template<typename T>
    explicit shared_variant(const std::shared_ptr<T>& p) : b(p) {
        static_assert(is_in_variant<T, Args...>::value, "type T is not in variant");
    }

    template<typename T>
    explicit shared_variant(std::shared_ptr<T>&& p) : b(std::move(p)) {
        static_assert(is_in_variant<T, Args...>::value, "type T is not in variant");
    }

    // visit, holds alternative get
private:
    std::shared_ptr<enable_shared_variant> b;
};

template<typename T, typename ...Args>
bool holds_alternative(const shared_variant<T>& v) {};

int main() {
    std::shared_ptr<X> a;
    shared_variant<X, Y> v(a);
    return 0;
}
