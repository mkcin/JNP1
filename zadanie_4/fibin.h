#ifndef FIBIN_H
#define FIBIN_H

#include <type_traits>
#include <iostream>
#include <cassert>

/*************** Structs and functions described in problem statement. ***************/
template<unsigned N>
struct Fib {};

struct True {};

struct False {};

template<typename T>
struct Lit {};

/** Hashes string into unsigned and checks if @p name is valid variable name (as in problem statement).
 * @param name          - string to be hashed.
 * @return Hashed value.
 */
constexpr unsigned Var(const char *name) {
    // name should be string with length at least 1.
    assert(*name);

    unsigned result = 0;
    int length = 0;
    while (*name) {
        length++;
        assert(length <= 6);
        char c = *name;
        if ('0' <= c && c <= '9') {
            c = c - '0';
        } else if ('a' <= c && c <= 'z') {
            c = c - 'a' + 10;
        } else if ('A' <= c && c <= 'Z') {
            c = c - 'A' + 10;
        } else {
            assert(false);
        }
        result = result * ('z' - 'a' + 12) + 1 + c;
        name++;
    }
    return result;
}

template<typename T1, typename T2, typename ...Args>
struct Sum {};

template<typename Arg>
struct Inc1 {};

template<typename Arg>
struct Inc10 {};

template<typename Left, typename Right>
struct Eq {};

template<unsigned Var>
struct Ref {};

template<unsigned Var, typename Value, typename Expression>
struct Let {};

template<typename Condition, typename Then, typename Else>
struct If {};

template<unsigned Var, typename Body>
struct Lambda {};

template<typename Fun, typename Param>
struct Invoke {};

template<typename ValueType, typename Enable = void>
struct Fibin {};

/** Fibin specialization for non integral types
 * @tparam ValueType            - given type.
 */
template<typename ValueType>
struct Fibin<ValueType, std::enable_if_t<!std::is_integral_v<ValueType>>> {
    template<typename Expr>
    static void eval() {
        std::cout << "Fibin doesn't support: " << typeid(ValueType).name() << std::endl;
    }
};

/** Fibin specialization for integral types
 * @tparam ValueType            - given type.
 */
template<typename ValueType>
struct Fibin<ValueType, std::enable_if_t<std::is_integral_v<ValueType>>> {
    template<typename Expr>
    static constexpr ValueType eval() {
        return Eval<Expr, ListEnd>::result::value;
    }

private:
    /*************** Structs and functions used to help evaluate given expression. ***************/

    /** Calculates n-th Fibonacci number.
     * @param n             - n as described above.
     * @return n-th Fibonacci number.
     */
    static constexpr ValueType fib(unsigned n) {
        if (n == 0) {
            return 0;
        }

        ValueType a = 0, b = 1, c = 0;
        while (n-- > 1) {
            c = static_cast<ValueType>(a + b);
            a = b;
            b = c;
        }
        return b;
    }

    /*************** List utilities. ***************/
    template<unsigned Name, typename Value, typename Tail>
    struct List {};

    struct ListEnd {};

    /** Finds field T1 on list T2.
     */
    template<unsigned T1, typename T2>
    struct Find {};

    template<unsigned Name, typename Value, typename Tail>
    struct Find<Name, List<Name, Value, Tail>> {
        using result = Value;
    };

    template<unsigned Name1, unsigned Name2, typename Value, typename Tail>
    struct Find<Name1, List<Name2, Value, Tail>> {
        using result = typename Find<Name1, Tail>::result;
    };

    /*************** Structs to represent bool and integral values when evaluating expression. ***************/
    template<ValueType Value>
    struct Number {
        static constexpr ValueType value = Value;
    };

    template<bool Value>
    struct Bool {
        static constexpr bool logicalValue = Value;
    };

    /** Struct similar to Lambda but with additional information about variables that were valid during
     * lambda declaration.
     */
    template<unsigned Var, typename Body, typename Variables>
    struct LocalLambda {};

    /*************** Structs that evaluate expression in a way described in the problem statement. ***************/

    /** Evaluates expression @p T.
     * Field result contains evaluated expression. Evaluated expression can be one of three types:
     * Number, Bool or LocalLambda.
     * @tparam T            - expression;
     * @tparam Variables    - list of variables which given expression can use.
     */
    template<typename T, typename Variables>
    struct Eval {};

    template<unsigned N, typename Variables>
    struct Eval<Lit<Fib<N>>, Variables> {
        using result = Number<fib(N)>;
    };

    template<typename Variables>
    struct Eval<Lit<True>, Variables> {
        using result = Bool<true>;
    };

    template<typename Variables>
    struct Eval<Lit<False>, Variables> {
        using result = Bool<false>;
    };

    template<typename T1, typename T2, typename Variables>
    struct Eval<Sum<T1, T2>, Variables> {
        using result = Number<static_cast<ValueType>(Eval<T1, Variables>::result::value +
                                                     Eval<T2, Variables>::result::value)>;
    };

    template<typename T1, typename T2, typename ...Args, typename Variables>
    struct Eval<Sum<T1, T2, Args...>, Variables> {
        using result = Number<static_cast<ValueType>(Eval<T1, Variables>::result::value +
                                                     Eval<Sum<T2, Args...>, Variables>::result::value)>;
    };

    template<typename Arg, typename Variables>
    struct Eval<Inc1<Arg>, Variables> {
        using result = Number<Eval<Sum<Arg, Lit<Fib<1>>>, Variables>::result::value>;
    };

    template<typename Arg, typename Variables>
    struct Eval<Inc10<Arg>, Variables> {
        using result = Number<Eval<Sum<Arg, Lit<Fib<10>>>, Variables>::result::value>;
    };

    template<typename Left, typename Right, typename Variables>
    struct Eval<Eq<Left, Right>, Variables> {
        using result = Bool<Eval<Left, Variables>::result::value == Eval<Right, Variables>::result::value>;
    };

    template<unsigned Var, typename Variables>
    struct Eval<Ref<Var>, Variables> {
        using result = typename Find<Var, Variables>::result;
    };

    template<unsigned Var, typename Value, typename Expression, typename Variables>
    struct Eval<Let<Var, Value, Expression>, Variables> {
        using result = typename Eval<Expression, List<Var, typename Eval<Value, Variables>::result, Variables>>::result;
    };

    template<typename Condition, typename Then, typename Else, typename Variables>
    struct Eval<If<Condition, Then, Else>, Variables> {
        using result = typename Eval<
                std::conditional_t<Eval<Condition, Variables>::result::logicalValue, Then, Else>,
                Variables>::result;
    };

    template<unsigned Var, typename Body, typename Variables>
    struct Eval<Lambda<Var, Body>, Variables> {
        using result = LocalLambda<Var, Body, Variables>;
    };

    template<typename Fun, typename Param, typename Variables>
    struct Eval<Invoke<Fun, Param>, Variables> {
        using result = typename Eval<Invoke<typename Eval<Fun, Variables>::result, Param>, Variables>::result;
    };

    template<unsigned Var, typename Body, typename LambdaVariables, typename Param, typename Variables>
    struct Eval<Invoke<LocalLambda<Var, Body, LambdaVariables>, Param>, Variables> {
        using result = typename Eval<Body, List<Var, typename Eval<Param, Variables>::result, LambdaVariables>>::result;
    };
};

#endif //FIBIN_H