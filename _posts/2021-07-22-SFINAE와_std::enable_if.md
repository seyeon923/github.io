---
title:  "C++ SFINAE와 std::enable_if"
date:   2021-07-22 22:00:00 +0900
categories: 
    - C/C++ 
---

## Overview
앞선 포스트 [std::integral_constant를 활용한 컴파일 타임 분기]({% post_url 2021-07-21-integral_constant를_활용한_컴파일타임_분기 %})에서 `std::integral_constant`를 통해 템플릿 인자 `T` 타입에 따라 템플릿의 인스턴스화 여부를 선택할 수 있는 방법을 알아봤다. `std::enable_if`를 통해서도 템플릿의 인스턴스화 여부를 선택할 수 있는데, 그 방법과 원리에 대해서 알아보자.

먼저 앞선 포스트의 `Print` 함수 템플릿을 `std::enable_if`를 사용해서 `T`의 포인터 타입 여부에 따라 템플릿 인스턴스화를 선택하는 코드를 보면 다음과 같다.

``` c++
#include <type_traits>

// T가 포인터 타입이면 인스턴스화
template<typename T>
typename std::enable_if<std::is_pointer<T>::value>::type Print(const T& v) {
    std::cout << v << " -> " << *v << std::endl;
}

// T가 포인터 타입이 아닐 때 인스턴스화
template<typename T>
typename std::enable_if<!std::is_pointer<T>::value>::type Print(const T& v) {
    std::cout << v << std::endl;
}
```

## SFINAE
"**S**ubstitution **F**ailure **I**s **N**ot **A**n **E**rror"의 약자로 템플릿 인스턴스화 시 치환 실패는 에러가 아니라는 의미이다.

다음의 코드를 보자.

``` c++
struct Test {
    typedef int type;
};

template<typename T>
typename T::type Foo(const T&) {
    typename T::type ret{};
    std::cout << "Foo template" << std::endl;
    return ret;
}

void Foo(...) {
    std::cout << "Foo(...)" << std::endl;
}

int main() {
    Test t;
    Foo(t); // Foo template or Foo(...) ?

    return 0;
}
```

오버로딩된 함수가 있을 때 템플릿이 가변인자 함수보다 먼저 매칭되기 때문에 위 코드를 실행해 보면 함수 템플릿에 의해 생성된 함수가 호출되는 것을 확인할 수 있다.(가변인자 함수의 매칭 우선순위는 가장 낮다.)

만약 다음처럼 멤버 타입 `type`이 없는 타입을 `Foo`에 전달하게 되면 어떻게 될까? 

``` c++
int i;
Foo(i);
```

예를 들어 위 코드처럼 `int` 타입의 변수를 `Foo`에 전달한다고 하면 함수 템플릿이 있으므로 `T`를 `int`로 치환해서 다음처럼 함수를 생성하게 될 것이다.

``` c++
int::type Foo(const int&) {
    int::type ret{};
    std::cout << "Foo template" << std::endl;
    return ret;
}
```

그런데 `int::type`이라는 식은 유효하지 않은 식이 된다. 그러면 위 함수를 컴파일 실패가 될것 같아 보인다. 하지만 위 코드를 실행해보면 컴파일이 정상적으로 수행되고 가변인자 함수를 호출하는 것을 알 수 있다. 위처럼 `T`를 `int`로 치환했을 때 없는 타입이나 값을 의미하게 되면 치환 실패가 되고 치환된 함수가 생기지 않게 된다. 이를 "치환 실패는 에러가 아니다"라는 의미의 **SFINAE**라고 한다. 따라서 `Foo(...)` 함수가 호출되는 것이다.

<div class="notice--info" markdown="1">

위 코드에서 `Foo(...)` 함수를 제공하지 않게되면 컴파일 에러가 나게 되는데, 이는 함수 템플릿 치환 실패에 의한 에러가 아니고 함수를 찾을 수 없어 생기는 에러이다.
</div>

템플릿에서 SFINAE가 적용되는 위치는 반환 타입, 함수 인자, 템플릿 인자 3가지 경우가 있다. 위에서 사용한 `Foo` 함수 템플릿은 함수 반환 타입에서 치환 실패가 발생하여 함수가 생성되지 않은 경우이다.

다음은 반환타입 대신 함수 인자에서 SFINAE가 적용된 코드이다.

``` c++
template<typename T>
void Foo(const T&, typename T::type* p = nullptr){
    std::cout << "Foo template" << std::endl;
}

void Foo(...) {
    std::cout << "Foo(...)" << std::endl;
}

int main() {
    Test t;
    Foo(t); // Foo template

    int i;
    Foo(i); // Foo(...)

    return 0;
}
```

위 방법보다는 주로 반환타입에 사용하는 방법이 널리 쓰인다.

다음은 템플릿 인자에 SFINAE가 적용된 코드이다.

``` c++
template<typename T, typename T2 = typename T::type>
void Foo(const T&) {
    std::cout << "Foo template" << std::endl;
}

void Foo(...) {
    std::cout << "Foo(...)" << std::endl;
}

int main() {
    Test t;
    Foo(t); // Foo template

    int i;
    Foo(i); // Foo(...)

    return 0;
}
```

위처럼 템플릿 인자에 SFINAE가 적용되는 경우 함수 시그니쳐가 그대로 보인다는 장점이있다. 주로 생성자처럼 반환타입을 표기할 수 없는 경우 사용된다.

## std::enable_if
`std::enable_if`는 SFINAE 기법을 활용하여 템플릿의 인스턴스화 여부를 조절할 수 있는 도구라고 할 수 있다.

다음의 코드를 생각해보자.

``` c++
// Primary template
template<bool, typename T = void> struct enable_if {
    typedef T type;
};

// Partial specialization
template<typename T> struct enable_if<false, T> {
    // type 이 없다
};
```

`enable_if`의 기본 템플릿은 첫 번째 템플릿 인자는 `bool` 값을 받고 두 번쨰 인자로는 임의의 타입 `T`를 받아 이를 다시 내부에서 `type`이라는 이름으로 별칭을 만들고 있다. 이때 `T`를 제공하지 않으면 디폴트 타입은 `void`가 되게 된다.

부분 특수화가된 템플릿을 보면 첫 번째 템플릿 인자가 `false`인 경우에 대해서 부분 특수화 버전을 제공하는데 내부에 `type` 이라는 이름의 멤버타입이 없는 것을 알 수 있다.

따라서 `enable_if`의 첫 번째 템플릿 인자에 `true`를 전달하면 내부적으로 `type`이라는 이름의 `T` 타입의 멤버타입을 갖게 되지만 `false`를 전달하면 `type` 이라는 멤버타입이 없는 형태가 된다.

``` c++
int main() {
    enable_if<true, int>::type i1 = 10;      // int i1 = 10;
    enable_if<true, double>::type d1 = 3.14; // double d1 = 3.14;

    enable_if<false, int>::type i2 = 10;     // error! 멤버타입 type이 없다.
    enable_if<true, double>::type d2 = 3.14; // error! 멤버타입 type이 없다.
}
```

결국 `enable_if`의 첫 번째 인자에 템플릿 인스턴스화 조건이 되는 (컴파일 타임에 계산이 되는)식을 넣고 함수의 반환 타입을 `T`에 전달한뒤 함수 반환 타입을 `typename enable_if<조건, T>::type` 처럼 두게되면 해당 조건에 따라 템플릿 인스턴스화가 결정되는 일이 일어나게 된다.

조건이 `true`이면 Primary Template에 따라 `enable_if<조건, T>`는 `T` 타입의 멤버 타입 `type`을 가지게 됨으로 결국에 `typename enable_if<조건, T>::type` 은 `T`가 된다.

<div class="notice--info" markdown="1">

`typename enable_if<조건, T>::type` 앞에 `typename` 이 붙는 이유는 `type`이 값이 아니라 타입이기 때문에 붙이는 것이다. `typename`을 명기하지 않으면 컴파일러는 `type`이 값인지 타입인지 알 수 있는 방법이 없기 때문이다.(치환 되기 전에)

`typename`을 명기하지 않으면 컴파일러는 이를 값으로 해석하게 된다.
</div>

하지만 조건이 `false`이면 부분 특수화 템플릿을 사용하게 되고 내부에 `type` 멤버타입이 없게된다. 그러므로 만약 `typename enable_if<조건, T>::type` 을 함수 템플릿의 반환 값으로 사용하게되면 `type` 이 없으므로 치환 실패가 되고 함수 인스턴스화가 되지 않게 된다.

이러한 `enable_if`라는 클래스를 C++11 부터 `<type_traits>` 헤더에서 제공한다.

이제 다시 타입 `T`의 포인터 여부에 따라 인스턴스화 여부가 결정되는 함수 `Print`를 보자.

``` c++
#include <iostream>
#include <type_traits>

// T가 포인터 타입이면 인스턴스화
template<typename T>
typename std::enable_if<std::is_pointer<T>::value>::type Print(const T& v) {
    std::cout << v << " -> " << *v << std::endl;
}

// T가 포인터 타입이 아닐 때 인스턴스화
template<typename T>
typename std::enable_if<!std::is_pointer<T>::value>::type Print(const T& v) {
    std::cout << v << std::endl;
}

int main() {
    int i = 10;

    Print(&i); // 주소 -> 10
    Print(i);  // 10

    return 0;
}
```

첫 번째 함수 템플릿 `Print`를 보면 `enable_if` 의 조건식에 `std::is_pointer<T>::value` 가 들어 있다. 즉, `T`가 포인터 타입이면 해당 함수 템플릿이 인스턴스화 되게 된다. 이때 함수의 반환 타입은 `enable_if`의 두 번째 템플릿 인자가 되는데 전달하지 않았으므로 디폴트 타입인 `void`가 된다.

두 번째 함수 템플릿은 첫 번째 템플릿과 반대로 `T`가 포인터 타입이 아닌경우에 인스턴스화가 되는 템플릿이 된다.

<div class="notice--info" markdown="1">

C++14 부터 다음과 같은 별칭을 제공한다.

``` c++
template< bool B, class T = void >
using enable_if_t = typename enable_if<B,T>::type;
```

즉, `typename enable_if<B, T>::type` 대신 간단하게 `enable_if_t<B, T>` 처럼 사용할 수 있다.
</div>

<div class="notice--info" markdown="1">

C++20 부터 **concept** 라는 문법이 들어오면서 템플릿의 인스턴스화 여부를 다음처럼 간결하게 표현할 수 있다.

``` c++
// T가 포인터 타입이면 인스턴스화
template<typename T> requires std::is_pointer<T>::value
void Print(const T& v) {
    std::cout << v << " -> " << *v << std::endl;
}

// T가 포인터 타입이 아닐 때 인스턴스화
template<typename T> requires (!std::is_pointer<T>::value)
void Print(const T& v) {
    std::cout << v << std::endl;
}
```

위 코드는 C++20 의 문법을 사용하고 있으므로 컴파일이 정상적으로 수행되기 위해서는 c++20 옵션을 주어야한다.
필자의 경우는 g++ 11 버전 컴파일러를 사용하여 `-std=c++20` 옵션을 주어 컴파일 하였을 때 정상적으로 컴파일이 되었다.

컴파일러별 C++20 지원 버전은 [여기](https://en.cppreference.com/w/cpp/compiler_support/20)에서 확인할 수 있다.
</div>