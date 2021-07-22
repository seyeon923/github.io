---
layout: single
title:  "std::integral_constant를 활용한 컴파일 타임 분기"
date:   2021-07-21 18:00:00 +0900
categories: 
    - C/C++ 
---

## Overview
다음처럼 포인터를 전달하게 되면 포인터의 주소와 해당 주소가 가리키는 값을 출력하고,
포인터가 아닌 값을 전달하게 대면 해당 값을 출력하는 함수 템플릿이 있다고 해보자.

``` c++
template<typename T>
void Print(const T& v) {
    if (std::is_pointer<T>::value) {
        std::cout << v << " -> " << *v << std::endl;
    }
    else {
        std::cout << v << std::endl;
    }
}
```

다음처럼 `int*`를 전달하게 되면 문제없이 컴파일 되어 실행이 되는 것을 확인할 수 있다.

``` c++
int main() {
    int i = 10;
    Print(&i);
}
```

<br>

실행결과 :

```
0x7fffffffddfc -> 10
```

<br>

하지만 다음처럼 그냥 `int`를 전달하게 되면 컴파일이 되지 않고 에러가 발생한다.

``` c++
int main() {
    int i = 10;
    Print(i);
}
```

그 이유는 위처럼 `Print(i)`를 호출하게 되면 템플릿에 의해 `T`가 `int`인 함수 `Print`가 다음과 같이 만들어 지게 된다.

``` c++
template<typename T>
void Print(const int& v) {
    if (std::is_pointer<int>::value) {
        std::cout << v << " -> " << *v << std::endl;
    }
    else {
        std::cout << v << std::endl;
    }
}
```

이때, `int` 타입에 대해서 `*` 참조 연산이 불가능 함으로 컴파일 에러가 발생하게 된다.<br>

`std::is_pointer<int>::value` 값이 컴파일 시간 `false`임에도 불구하고 컴파일러는 `if`가 실행시간 조사문임으로 `if` 안에 블록도 컴파일하여 기계어 코드를 생성하게 된다.<br>
따라서, `*v`에서 컴파일 에러가 발생하게 되는 것이다.<br>

C++17 부터 static if라고 불리는  `if constexpr`이 추가되어 위와 같은 경우 `if` 대신 `if constexpr`을 사용하면 위 문제가 깔끔하게 해결된다.<br>
> `if constexpr`의 조건식에는 컴파일 상수만 올 수 있으며, 해당 조건이 `false`이면 해당 부분을 컴파일 하지 않고 기계어 코드를 생성하지 않게 된다.

다음은 `if constexpr`을 사용하여 `Print` 함수 템플릿을 작성한 코드이다.

``` c++
template<typename T>
void Print(const T& v) {
    if constexpr (std::is_pointer<T>::value) {
        std::cout << v << " -> " << *v << std::endl;
    }
    else {
        std::cout << v << std::endl;
    }
}
```

위처럼 템플릿을 작성하면 포인터인 값과 포인터가 아닌 값 모두에 대해 정상적으로 실행되는 것을 알 수 있다.

``` c++
int main() {
    int i = 10;

    Print(&i);
    Print(i);
}
```

<br>

실행결과 : 

```
0x7fffffffddfc -> 10
10
```

<br>

하지만 `if constexpr`은 C++17 에서 생긴 문법이고 그 이전에도 위와 같은 문제를 해결하는 방법이 있었는데 그 중 하나가 `std::integral_constant`를 이용한 방법이다.


## std::integral_constant
`std::integral_constant`를 통해 컴파일 타임 정수형 상수를 타입으로 만들 수 있다.

`std::integral_constant`는 다음과 같이 생겼다.

``` c++
template<class T, T v>
struct integral_constant;
```

`std::integral_constant`는 `T` 타입의 값이 `v`인 컴파일 타임 정수형 상수를 `static` 멤버로 가지고 있는 하나의 타입(클래스)가 된다.<br>
`std::integral_constant`가 가지고 있는 컴파일 타임 상수는 `value` 라는 이름의 `static` 멤버로 가지고 올 수 있다.

``` c++
#include <iostream>
#include <type_traits> // std::integral_constant 선언됨

int main() {
    std::cout << std::integral_constant<int, 0>::value << std::endl; // 0
    std::cout << std::integral_constant<int, 1>::value << std::endl; // 1

    std::integral_constant<int, 0> i1;
    std::integral_constant<int, 1> i2;
    std::cout << std::boolalpha << (typeid(i1) == typeid(i2)) << std::endl; // false
    return 0;
}
```

`bool` 타입으로 `std::integral_constant`를 만들면 `true`, `false` 에 대해서 각각의 타입을 만들 수 있게 되는데 각각에 대해서 STL에서 다음처럼 `true_type`, `false_type`을 정의하고 있다.

``` c++
typedef std::integral_constant<bool, true> true_type;
typedef std::integral_constant<bool, false> false_type;
```

C++ `<type_traits>`의 클래스들은 모두 위 `true_type`, `false_type`을 기반클래스로 파생하여 만들어져 있다.<br>
즉, 특정 타입이 포인터인지 조사하는 `std::is_pointer<T>`의 경우 `T`가 포인터 타입인 경우 `true_type`의 파생클래스가 되고, `T`가 포인터 타입이 아닌 경우 `false_type`의 파생클래스가 된다.<br>

<div class="notice--info" markdown="1">

**std::is_pointer**<br>
`true_type`, `false_type`을 파생하여 `std::is_pointer`는 다음과 같이 만들 수 있다.

``` c++
// Primary template
template<typename T> struct is_pointer : false_type {};

// 포인터 타입에 대해서 부분 특수화(partial specialization)
template<typename T> struct is_pointer<T*> : true_type{};
```
</div>

## Static Dispatch
다시 본론으로 돌아와서 `std::integral_constant`를 활용해서 어떻게 `if constexpr`처럼 컴파일 타임 분기를 할 수 있는지 궁금할 것이다.

바로 아래 코드를 보자.

``` c++
#include <iostream>
#include <type_traits>

namespace details {
    // T가 포인터인 경우
    template<typename T>
    void Print(const T& v, std::true_type) {
        std::cout << v << " -> " << *v << std::endl;
    }

    // T가 포인터가 아닌 경우
    template<typename T>
    void Print(const T& v, std::false_type){
        std::cout << v << std::endl;
    }
}

template<typename T>
void Print(const T& v){
    details::Print(v, std::is_pointer<T>());
}


int main() {
    int i = 10;

    Print(&i);
    Print(i);

    return 0;
}
```

위 코드를 컴파일하여 실행해보면 `T`가 포인터 타입인 경우와 아닌 경우 모두 에러 없이 컴파일 되고 실행되는 것을 확인할 수 있다.

위 코드에서 `Print`에 전달되는 인자에 따라 `T`가 결정되게 되고 인스턴스화가 되게 된다.
이때, 인자로 포인터타입을 전달하게 되면 `T`는 포인터 타입이 되게 되고 `details::Print`에 전달되는 두 번쨰 인자는 `std::true_type`이 되게 된다.
따라서, 두 번째 인자가 `std::true_type`인 함수 템플릿만 인스턴스화가 되고 두 번째 인자가 `false_type`인 함수 템플릿에 의해서는 인스턴스화가 되지 않고, `std::true_type`을 인자로 갖는 함수만 컴파일이 된다.
반대로 `Print`에 포인터 타입이 아닌 인자를 전달하게 되면 `T`는 포인터 타입이 아닌 타입이 되고 `details::Print` 중 두 번째 인자가 `std::false` 타입인 함수만 인스턴스화가 되고 컴파일이 되게 된다. 따라서 포인터가 아닌 타입의 경우 `*` 참조 연산자를 사용하는 코드가 생기지 않고 컴파일 에러도 생기지 않게 된다.

<div class="notice--warning" markdown="1">

다음의 경우에는 인스턴스화가 되지 않더라도 컴파일 에러이다.

``` c++

template<typename T>
void Foo(T v){
    int i;
    *i = 10;
}
```

인스턴스화가 되지 않는다고 컴파일이 안되는 것은 아니고 인스턴스화를 하면서 한번 인스턴스화를 한뒤 한번 더 컴파일을 한다고 생각할 수 있다.

위의 경우 명확한 타입 `int`를 가지고 `*` 참조 연산을 하고 있으므로 컴파일 에러가 발생하게 된다.<br>
`Print` 함수 템플릿의 경우 인스턴스화 되기 전에는 어떤 타입인지 모르는 `T` 타입에 `*` 참조 연산을 하고 있으므로 인스턴스화 되기 전에는 에러여부를 컴파일러가 알 수 없다.
</div>

<div class="notice--info" markdown="1">

`std::integral_constant` 와 `type_traits`는 C++11 부터 들어온 개념이지만 새로운 문법으로 생긴 방법이 아니라 라이브러리에 추가된 것으로 C++11 이전에서도 위와 같은 클래스를 직접 만들어 적용할 수 있는 방법이다.

``` c++
#include <iostream>

template<size_t N> struct integral_constant {
    enum { value = N };
};

typedef integral_constant<1> true_type;
typedef integral_constant<2> false_type;

template<typename T>
struct is_pointer : false_type {};

template<typename T>
struct is_pointer<T*> : true_type {};

namespace detail {
    template<typename T>
    void Print(const T& v, true_type) {
        std::cout << v << " -> " << *v << std::endl;
    }

    template<typename T>
    void Print(const T& v, false_type) {
        std::cout << v << std::endl;
    }
}

template<typename T>
void Print(const T& v) {
    detail::Print(v, is_pointer<T>());
}

int main() {
    int i = 10;

    Print(&i);
    Print(i);

    return 0;
}
```

위 코드를 `g++`로 `-std=c++98` 옵션을 주고 컴파일 시 문제 없이 컴파일 되고 실행까지 되는 것을 확인 할 수 있다.

</div>