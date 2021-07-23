---
title:  "C++ - 템플릿 인자에서 멤버타입/템플릿 꺼내기"
date:   2021-07-23 11:00:00 +0900
categories: 
    - C/C++ 
---

## Overview
클래스의 멤버 타입이나 템플릿 멤버를 꺼낼 때 해당 클래스가 템플릿의 인자인 경우 `typename` 혹은 `template` 키워드를 사용해야 되는 경우가 있는데, 이러한 경우를 알아본다.

다음의 클래스를 보자

``` c++
struct Test {
    using type = int;
    constexpr static int value = 0;

    template<typename T>
    static void Foo() { std::cout << "Test::Foo()" << std::endl; }

    template<typename T>
    struct Test2 {};
};
```

클래스 `Test`는 자신의 네임스페이스안에 `type` 이라는 이름의 멤버 타입, `value` 라는 static 상수, `Foo` 라는 static 함수, `Test2`라는 템플릿 클래스를 가지고 있다.

## 멤버 변수 꺼내기
`Test` 클래스의 static 멤버 상수 `value` 는 다음처럼 꺼내올 수 있다.

``` c++
int main() {
    Test t;
    Test* p = new Test;

    std::cout << Test::value << std::endl;  // 0
    std::cout << t.value << std::endl;      // 0
    std::cout << p->value << std::endl;     // 0

    delete p;

    return 0;
}
```

템플릿 인자에서 멤버 변수나 상수등을 꺼내는 것은 위와 동일하게 꺼낼 수 있다.

``` c++
template<typename T>
void Func() {
    T t;
    T* p = new T;

    std::cout << T::value << std::endl;
    std::cout << t.value << std::endl;
    std::cout << p->value << std::endl;

    delete p;
}

int main()
{
    Func<Test>();
}
```

`Func`의 템플릿 인자로 `Test`를 넘겼으므로 `Test`의 `value`를 출력하는 것을 알 수 있다.

## 멤버 타입 꺼내기
다음은 `Test`의 `type` 멤버 타입을 사용하는 코드이다.

``` c++
int main() {
    Test::type i = 10; // int i = 10;
    std::cout << i << std::endl;
}
```

`Test` 안의 멤버 타입 `type` 은 `int`로 정의되어 있으므로 `Test::type i = 10;` 은 결국 `int i = 10;` 과 동일한 코드가 된다.

이번에는 템플릿 인자에서 멤버타입 `type`을 꺼낸다고 해보자.

``` c++
template<typename T>
void Func() {
    T::type i = 10;
    std::cout << i << std::endl;
}

int main() {
    Func<Test>();
}
```

위 코드에서 `Func`의 템플릿 인자 `T`에 `Test`를 주고 있으므로 결국 위의 `Test`에서 `type` 멤버 타입을 사용하는 코드와 동일해 보인다. 
하지만 위 코드를 컴파일해보면 에러가 발생하는 것을 알 수 있다. 
컴파일 에러가 없으려면 다음처럼 `typename`을 적어 주어야 한다.

``` c++
template<typename T>
void Func() {
    typename T::type i = 10;
    std::cout << i << std::endl;
}
```

`Test::type` 처럼 사용하게 되면 컴파일러가 `Test`의 선언부를 보고 `type` 이 타입임을 알 수 있지만 템플릿에서 `T` 는 어느 타입인지 알 수 없으므로 `typename`이 없으면 컴파일러는 `T::type`을 타입을 꺼내는 것이 아니고 static 멤버 변수를 꺼내는 것으로 가정하게 되어 컴파일 에러가 발생하는 것이다. 

즉, 템플릿 인자로 주어진 임의의 타입에 대해서 멤버 타입을 꺼낼 때는 앞에 `typename`을 지정해 주어야 한다.

## 멤버 템플릿 꺼내기
다음은 `Test`의 템플릿으로 주어진 함수 `Foo`를 호출하는 코드이다.

``` c++
int main() {
    Test t;
    Test* p = new Test;

    Test::Foo<int>();   // Test::Foo()
    t.Foo<int>();       // Test::Foo()
    p->Foo<int>();      // Test::Foo()

    delete p;

    return 0;
}
```

위 코드에서 `Test` 대신 템플릿 인자 `T`를 사용한 함수 템플릿 `Func`를 만들고 함수 템플릿 인자에 `Test`를 가지고 호출해보자.

``` c++
template<typename T>
void Func() {
    T t;
    T* p = new T;

    T::Foo<int>();   // Test::Foo()
    t.Foo<int>();    // Test::Foo()
    p->Foo<int>();   // Test::Foo()

    delete p;
}

int main() {
    Func<Test>();

    return 0;
}
```

`Func` 함수 템플릿에서 `T` 가 `Test`로 치환되므로 결국 `Test`의 `Foo` 함수를 호출하는 처음의 코드와 동일한 모습이 될 것이고 동일한 결과가 나올 것 같지만 컴파일 해보면 에러가 발생하는 것을 알 수 있다. 
그 이유는 컴파일러가 `T::Foo`를 해석할 때 `T`의 타입을 모르기 때문에 이를 멤버변수나 상수로 해석하기 때문이다. 

따라서, 다음처럼 컴파일러에게 `template` 키워드를 추가하여 `Foo` 가 템플릿임을 알려줘야 한다. 

``` c++
template<typename T>
void Func() {
    T t;
    T* p = new T;

    T::template Foo<int>();   // Test::Foo()
    t.template Foo<int>();    // Test::Foo()
    p->template Foo<int>();   // Test::Foo()

    delete p;
}

int main() {
    Func<Test>();

    return 0;
}
```

## 멤버 템플릿 타입 꺼내기
만약 멤버가 타입인 동시에 템플릿이라면 `typename`과 `template`을 모두 표기해야 한다.

다음은 템플릿 인자가 아닐 때 `Test`에서 `Test2<int>` 타입을 꺼내는 코드이다.

``` c++
int main() {
    Test::Test2<int> t; // Test2<int> t;
    return 0;
}
```

다음은 템플릿 인자에서 `Test2<int>` 타입을 꺼내는 코드이다.

``` c++
template<typename T>
void Func() {
    typename T::template Test2<int> t;

    std::cout << typeid(t).name() << std::endl;
}

int main() {
    Func<Test>();

    return 0;
}
```

위 코드에서 `::template` 혹은 `typename` 둘 중 하나라도 빠지면 컴파일 에러가 생기는 것을 알 수 있다.

또한, 위 코드를 실행해보면 `t`의 타입이 `Test::Test2<int>` 임을 알 수 있다.

<div class="notice--info" markdown="1">

위 코드를 `g++` 로 컴파일 하여 실행시켜 보면 `N4Test5Test2IiEE` 처럼 컴파일러 내부에서 사용하는 이름을 보여준다. 
이때, `c++filt -t` 의 인자로 알고 싶은 타입 이름을 전달하면 우리가 읽을 수 있는 형태의 타입 이름을 알려준다.

필자의 경우 `c++filt -t N4Test5Test2IiEE` 를 실행하면 `Test::Test2<int>`가 나온다.
</div>