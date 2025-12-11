//
// Created by usatiynyan.
//

#include "sl/meta/monad.hpp"
#include "sl/meta/traits/unique.hpp"

#include <gtest/gtest.h>

namespace sl::meta {

struct MoveOnly : meta::unique {
    MoveOnly(int v) : val(v) {}
    constexpr bool operator==(const MoveOnly& other) const { return val == other.val; }

public:
    int val;
};

TEST(monad, ConstructionAndState) {
    result<int, std::string> r1 = ok(42);
    EXPECT_TRUE(r1.is_ok());
    EXPECT_FALSE(r1.is_err());
    EXPECT_EQ(r1.ok(), 42);

    result<int, std::string> r2 = err(std::string("fail"));
    EXPECT_FALSE(r2.is_ok());
    EXPECT_TRUE(r2.is_err());
    EXPECT_EQ(r2.err(), "fail");

    result<std::string, int> r3(ok_tag, "hello");
    EXPECT_EQ(r3.ok(), "hello");

    result<std::string, int> r4(err_tag, 404);
    EXPECT_EQ(r4.err(), 404);
}

TEST(monad, Comparisons) {
    result<int, int> r1 = ok(10);
    result<int, int> r2 = ok(10);
    result<int, int> r3 = ok(20);
    result<int, int> e1 = err(10);

    EXPECT_EQ(r1, r2);
    EXPECT_NE(r1, r3);
    EXPECT_NE(r1, e1);

    EXPECT_TRUE(r1 < r3);

    // Implementation dependent: usually variant ordering depends on index
    // ok_t is index 0, err_t is index 1. So Ok should be < Err ?
    // std::variant comparison: if index is different, lower index < higher index.
    // So ok < err.
    EXPECT_TRUE(r1 < e1);
}

TEST(monad, AccessorsRefQualifiers) {
    result<int, int> r = ok(10);

    static_assert(std::is_same_v<decltype(r.ok()), int&>);
    EXPECT_EQ(r.ok(), 10);

    const result<int, int> cr = ok(20);
    static_assert(std::is_same_v<decltype(cr.ok()), const int&>);
    EXPECT_EQ(cr.ok(), 20);

    result<std::unique_ptr<int>, int> move_r = ok(std::make_unique<int>(99));
    static_assert(std::is_same_v<decltype(std::move(move_r).ok()), std::unique_ptr<int>&&>);

    auto ptr = std::move(move_r).ok();
    EXPECT_EQ(*ptr, 99);
}

TEST(monad, BothAccessor) {
    result<int, int> r_ok = ok(1);
    EXPECT_EQ(r_ok.both(), 1);

    result<int, int> r_err = err(2);
    EXPECT_EQ(r_err.both(), 2);
}

TEST(monad, OkOrAndLazy) {
    result<int, std::string> r_ok = ok(10);
    result<int, std::string> r_err = err("fail");

    EXPECT_EQ(r_ok.ok_or(20), 10);
    EXPECT_EQ(r_err.ok_or(20), 20);

    EXPECT_EQ(r_ok.ok_or_lazy([] { return 50; }), 10);
    EXPECT_EQ(r_err.ok_or_lazy([] { return 50; }), 50);
}

TEST(monad, Monad_Map) {
    result<int, std::string> r = ok(10);
    auto r2 = r.map([](int x) { return std::to_string(x) + "!"; });

    EXPECT_TRUE(r2.is_ok());
    EXPECT_EQ(r2.ok(), "10!");

    result<int, std::string> e = err("error");
    auto e2 = e.map([](int x) { return x * 2; });

    EXPECT_TRUE(e2.is_err());
    EXPECT_EQ(e2.err(), "error");
}

TEST(monad, Monad_MapErr) {
    result<int, int> r = ok(10);
    auto r2 = r.map_err([](int x) { return x * 2; });
    EXPECT_EQ(r2.ok(), 10);

    result<int, int> e = err(10);
    auto e2 = e.map_err([](int x) { return x * 2; });
    EXPECT_TRUE(e2.is_err());
    EXPECT_EQ(e2.err(), 20);
}

TEST(monad, Monad_AndThen) {
    using Res = result<std::string, int>;

    Res r_ok = ok(std::string("5"));
    Res r_err = err(404);

    auto f_success = [](std::string s) -> Res { return ok(s + "0"); };
    auto f_fail = [](std::string) -> Res { return err(500); };

    EXPECT_EQ(r_ok.and_then(f_success).ok(), "50");
    EXPECT_EQ(r_ok.and_then(f_fail).err(), 500);
    EXPECT_EQ(r_err.and_then(f_success).err(), 404);
}

TEST(monad, Monad_OrElse) {
    using Res = result<int, int>;

    Res r_ok = ok(10);
    Res r_err1 = err(20);

    auto f_recover = [](int e) -> Res { return ok(e + 1); };
    auto f_pass = [](int e) -> Res { return err(e + 1); };

    EXPECT_EQ(r_err1.or_else(f_recover).ok(), 21);
    EXPECT_EQ(r_err1.or_else(f_pass).err(), 21);
    EXPECT_EQ(r_ok.or_else(f_recover).ok(), 10);
}

TEST(monad, Match) {
    result<int, std::string> r = ok(10);
    result<int, std::string> e = err("fail");

    auto ok_f = [](int i) { return i * 2; };
    auto err_f = [](const std::string& s) { return static_cast<int>(s.length()); };

    EXPECT_EQ(r.match(ok_f, err_f), 20);
    EXPECT_EQ(e.match(ok_f, err_f), 4);
}

TEST(monad, Flatten) {
    // Test flatten() &&
    // result<result<T, E>, E> -> result<T, E>

    result<result<int, int>, int> nested_ok = ok(result<int, int>(ok(1)));
    auto flat_ok = std::move(nested_ok).flatten();
    EXPECT_TRUE(flat_ok.is_ok());
    EXPECT_EQ(flat_ok.ok(), 1);

    result<result<int, int>, int> nested_inner_err = ok(result<int, int>(err(2)));
    auto flat_inner_err = std::move(nested_inner_err).flatten();
    EXPECT_TRUE(flat_inner_err.is_err());
    EXPECT_EQ(flat_inner_err.err(), 2);

    result<result<int, int>, int> nested_outer_err = err(3);
    auto flat_outer_err = std::move(nested_outer_err).flatten();
    EXPECT_TRUE(flat_outer_err.is_err());
    EXPECT_EQ(flat_outer_err.err(), 3);
}

TEST(monad, MoveOnlyTypes) {
    // Validate the Monad works with types that cannot be copied
    result<MoveOnly, int> r = ok(MoveOnly(10));

    auto r2 = std::move(r).map([](MoveOnly m) { return MoveOnly(m.val + 1); });

    ASSERT_TRUE(r2.is_ok());
    EXPECT_EQ(r2.ok().val, 11);
}

} // namespace sl::meta
