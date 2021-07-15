---
layout: single
title:  "Boost Python 라이브러리를 활용하여 C/C++ 라이브러리를 Python Module로 Export하기"
date:   2021-07-04 21:08:00 +0900
categories: 
    - C/C++ 
    - Boost 
    - Python
classes: narrow
toc: true
---

## Overview
Microsoft Visual Studio 에서 Boost Python 라이브러리를 활용하여 C/C++ 라이브러리를 Python 모듈로 Export 하는 방법을 알아본다.

## Boost Python 특징

- 기존 C/C++ 인터페이스의 변경없이 그대로 사용할 수 있다.
- C/C++ <-> Python 양뱡향 인터페이싱이 가능하다.
- Boost Library 만으로 별도의 툴 설치없이 파이썬 모듈을 만들 수 있다.
- Declarative Interface Definition Language(IDL)와 유사한 형태로 익히고 사용하기 쉽다.

## Boost C++ Library 설치

### 1. Boost C++ Library 다운로드
현재 포스트 작성 중 가장 최신 버전인 1.76.0 버전을 다운로드한다.<br>
Boost 1.76.0 버전은 [여기][boost_1_76_0_link] 에서 다운 받을 수 있다.<br>
위 링크에서 [boost_1_76_0.7z][boost_1_76_0.7z_link] 혹은 [boost_1_76_0.zip][boost_1_76_0.zip_link] 을 다운로드 받은 뒤 압축을 풀어 준다.

### 2. Boost Python Library 빌드
Boost 라이브러리는 기본적으로 Header-only library로 빌드가 따로 필요없지만 Boost Python 라이브러리의 경우는 빌드가 필요하다.<br>
<br>
압축을 푼 `boost_1_76_0` 폴더로 이동한 뒤 `bootstrap.bat` 을 실행한다.(Windows 기준)<br>
실행이 완료되면 `b2.exe` 파일이 생성된 것을 확인할 수 있다.<br>
<br>
빌드에 사용할 Python 환경을 설정하기 위해 `project-config.jam` 파일을 연다.<br>
`project-config.jam` 파일에 다음과 같은 구문을 추가한다.

``` 
using python : [version] : [command-or-prefix] : [includes] : [libraries] : [conditions] : [extension-suffix] ;
```

위 구문에 대한 설명은 [여기](https://www.boost.org/doc/libs/1_76_0/tools/build/doc/html/index.html#bbv2.reference.tools.libraries.python)에서 확인할 수 있다.<br>
<br>
필자는 anaconda의 64 bit 3.8 버전의 파이썬을 사용하였으며 다음의 내용을 `project-config.jam`에 추가하였다.

``` 
using python
: 3.8 # version
: C:/Users/tpdus/anaconda3/python.exe # Interperter/path to dir
: C:/Users/tpdus/anaconda3/include # includes
: C:/Users/tpdus/anaconda3/libs # libs
: <address-model>64 # conditions
;
```

커맨드 창에서 `boost_1_76_0` 폴더로 이동한 뒤 다음 명령어를 실행한다.
```
.\b2.exe toolset=msvc --build-type=complete --with-python stage
```

빌드가 성공적으로 완료되면 `boost_1_76_0/stage/lib` 에 `boost_python`으로 시작되는 `.dll`, `.lib` 파일들이 생성된 것을 확인할 수 있다.<br>

참고로 여러 버전의 MSVC 가 설치되어 있는 경우 옵션 `toolset=msvc-<version>` 으로 버전을 지정할 수 있다.


```
# Visual Studio 2019
./b2.exe toolset=msvc-142 --build-type=complete --with-python stage

# Visual Studio 2017
./b2.exe toolset=msvc-141 --build-type=complete --with-python stage
```

B2의 자세한 사용방법은 [여기](https://www.boost.org/doc/libs/1_76_0/tools/build/doc/html/index.html)에서 확인할 수 있다.

### 3. Boost Python Library Linking
#### 3.1. Visual Studio C++ DLL Project를 생성
![DLL Project Creation1](/assets/2021-07-04-Exporting-C_C++_Library_to_Python_Module_by_Boost_Python/DLL_Project_Creation1.png)<br>
![DLL Project Creation2](/assets/2021-07-04-Exporting-C_C++_Library_to_Python_Module_by_Boost_Python/DLL_Project_Creation2.png)

#### 3.2. Project 설정

* **Target Extension 설정**<br>
다음과 같이 프로젝트 설정의 `Advanced - Target File Extension` 에서 Target File Extension을 Python Module Extension인 .pyd로 변경한다.<br>
![Set Project Config Target File Extension](/assets/2021-07-04-Exporting-C_C++_Library_to_Python_Module_by_Boost_Python/Set_Project_Config_Target_Extension.png)

* **Include Path 설정**<br>
다음과 같이 Boost C++ Libarary의 Include 경로와 Python의 Include 경로를 추가한다.<br>
![Set Project Config Include Paths](/assets/2021-07-04-Exporting-C_C++_Library_to_Python_Module_by_Boost_Python/Set_Project_Config_Include_Paths.png) <br>
위 이미지에서 `%BOOST_ROOT%`는 위에서 다운받은 `boost_1_76_0` 폴더의 경로이며,  <br>
`%PYTHON_HOME%`은 Boost Python 라이브러리를 빌드할 때 설정했던 python.exe가 있는 폴더로 Boost Python 라이브러리 빌드 시 설정하였던 Include 경로와 동일하게 설정하면 된다.

* **Library Path 설정**<br>
다음과 같이 위에서 빌드했던 Boost Python 라이브러리와 Python 라이브러리 경로를 설정해준다.<br>
![Set Project Config Include Paths](/assets/2021-07-04-Exporting-C_C++_Library_to_Python_Module_by_Boost_Python/Set_Project_Config_Library_Paths.png) <br>
각각 Boost Python 빌드를 통해 생성된 라이브러리 파일이 있는 경로와 Boost Python 빌드 시 설정했던 Python 라이브러리 경로이다.

#### 3.3. Python Module 생성
다음 소스코드를 추가한다.

``` c++
// helloworld.cpp
#include "pch.h"
#include <boost/python.hpp>

const char* Greet()
{
    return "Hello World";
}

BOOST_PYTHON_MODULE(HelloWorld) {
    boost::python::def("Greet", Greet);
}
```

위 코드는 C++ `Greet()` 함수를 Python Module `HelloWorld` 에 `Greet` 라는 이름을 export하는 코드이다.<br>

`BOOST_PYTHON_MODULE(HelloWorld)`에서 `HelloWorld` 부분은 export 하려는 모듈 이름으로 Project Name과 동일하게 `HelloWorld` 라는 이름의 모듈을 export할 것을 의미하며, <br>
해당 모듈에 노출시키려는 함수, 변수, 클래스 등을 해당 블럭안에 선언하여 해당 모듈에 노출 시키게 된다.<br>
`def` 함수의 첫 번째 인자는 파이썬에서 노출되는 이름(**"Greet"**)을 나타내고, 두 번째 인자는 해당 이름으로 노출 시키려는 함수 포인터 혹은 함수(`Greet` or `&Greet`)를 전달한다.<br>
<br>
위와 같은 소스코드를 추가한 뒤 빌드하게 되면 출력디렉터리에 `HelloWorld.pyd` 라는 이름으로 파이썬 모듈이 생성되는 것을 확인할 수 있다.<br>
해당 디렉터리에 Boost Python 빌드로 생성된 Boost Python DLL 파일을 복사해 준다.<br>(boost_pythonxx 로 시작하는 dll 파일로 Debug 모드로 빌드 한 경우 `gd` 가 파일명이 있는 것을 사용하고, Release 모드로 빌드 한 경우 `gd` 부분이 파일명에서 빠진 dll을 사용한다.)<br>
<br>
빌드된 파이썬 모듈이 있는 디렉터리로 이동해서 파이썬 인터프리터를 실행한 뒤 다음 처럼 생성된 모듈을 임포트한 뒤 `Greet` 함수를 호출 하면 `'Hello Wolrd'` 문자열을 반환하는 것을 확인할 수 있다.

``` python
import HelloWorld

HelloWorld.Greet() # 'Hello World'
```

## C++ Class 내보내기

``` c++
struct World {
    World() : msg("Hello World") {}
    World(const std::string& msg) : msg(msg) {}
    ~World() { std::cout << "~World()" << std::endl; }

    std::string Greet() { return msg; }

    const std::string msg;
};
```

위와 같이 C++ 에서 선언된 클래스 `World`가 있을 때, `boost::python::class_`를 통해 해당 클래스를 파이썬 모듈로 내보낼 수 있다.<br>
<br>

``` c++
#include <boost/python.hpp>
using namespace boost::python;

BOOST_PYTHON_MODULE(CppClass) {
    class_<World>("World");
}
```

위 코드와 같이 `class_<ClassName>("ExposedName")` 과 같은 형태로 선언하게 되면 ClassName 클래스를 파이썬에 ExposedName으로 노출 시킨다.<br>
<br>
위 코드를 빌드하여 파이썬에서 다음과 같이 임포트하여 사용한다.<br>

``` python
import CppClass

obj = CppClass.World()
del obj # ~World()
```

`del obj`를 통해 객체를 파괴할 때 소멸자가 잘 불리는 것을 확인할 수 있다.<br>
<br>
또한, 파이썬에서 `World`의 객체를 인자가 없는 생성자를 통해 객체 생성이 가능한 것을 확인할 수 있는데, `class_<World>("World");` 와 같이 사용하게 되면 디폴트 생성자도 함께 노출되게 된다.<br>
하지만 멤버함수 `Greet()`나 `const std::string&`을 인자로 받는 생성자는 따로 노출시키지 않았으므로 다음과 같이 사용하면 에러가 발생하는 것을 알 수 있다.<br>

``` python
obj.Greet() # Error
obj = CppClass.World('Hi, World') # Error
```

### 생성자 및 멤버 함수
디폴트 생성자외의 생성자나 멤버함수를 노출시키려면 `class_::def` 함수를 통해 노출 시킬 수 있다.<br>

``` c++
class_<World>("World")
        .def(init<const std::string&>())
        .def("Greet", &World::Greet)
```

위 처럼 선언하게 되면 인자가 `const std::string&`인 생성자와 `Greet()` 멤버 함수를 내보내게 된다.<br>
디폴트 생성자 외에 추가 생성자를 노출 할 때는 `def`의 인자에 `init<생성자 인자>()`를 전달하고, <br>
멤버 함수를 노출 시에는 `def`의 첫 번째 인자에는 노출 시킬 이름, 두 번쨰 인자에는 노출시킬 멤버함수 포인터를 전달한다.<br>
<br>

노출된 멤버를 사용하는 파이썬 코드는 다음과 같다.

``` python
import CppClass

obj1 = CppClass.obj()
obj1.Greet() # 'Hello World'

obj2 = CppClass.obj('Hi, Wolrd')
obj2.Greet() # 'Hi, World'
```

### 멤버 변수
생성자와 멤버함수 외에 멤버변수도 노출 시킬 수 있다.<br>
다음과 같이 `World` 클래스에 멤버변수 `val`을 추가하자.

``` c++
struct World {
    World() : msg("Hello World") {}
    World(const std::string& msg) : msg(msg) {}
    ~World() { std::cout << "~World()" << std::endl; }

    std::string Greet() { return msg; }

    const std::string msg;
    double val;
};
```

`class_`의 멤버함수 `def_readonly`를 통해 읽기 전용으로 멤버변수를 노출시키거나, <br>
`def_readwrite` 멤버함수로 읽기 쓰기가 가능하도록 멤버변수를 노출 시킬 수 있다. <br>
두 함수 모두 첫 번째 인자는 노출시키는 이름, 두 번째 인자는 노출시키는 멤버 변수의 멤버 포인터이다.

``` c++
class_<World>("World")
    .def(init<const std::string&>())
    .def("Greet", &World::Greet)
    .def_readonly("msg", &World::msg)
    .def_readwrite("val", &World::val)
    ;
```

다음은 `World`의 멤버변수 `msg`, `val`을 접근하는 코드이다.

``` python
obj = CppClass.World()
print(obj.msg) # Hello World
obj.msg = 'another message' # Error! - readonly

obj.val = -10 # ok
print(obj.val) # -10.0
```

### Class Property
객체지향 프로그래밍에서 클래스 멤버를 바로 노출 시키기 보다는 해당 멤버 변수는 private 으로 숨기고 getter/setter 함수를 제공하는 것이 일반적이다.<br>
Boost Python 라이브러리에서는 getter/setter 함수를 통해 프로퍼티를 선언할 수 있다.

``` c++
struct FloatNum {
    FloatNum() : val_(0) {}
    FloatNum(float val) : val_(val) {}
    float Get() const { return val_; }
    void Set(float val) { val_ = val; }
private:
    float val_;
};
```

위 클래스를 파이썬 모듈로 노출 시킬 때, `Get()`, `Set()` 함수를 다음처럼 `add_property` 함수를 통해 프로퍼티로 노출 시킬 수 있다.

``` c++
class_<FloatNum>("FloatNum")
    .def(init<float>())
    .add_property("Val", &FloatNum::Get, &FloatNum::Set)
    .add_property("RoVal", &FloatNum::Get);
```

`add_property`의 첫 번째 인자는 노출시킬 이름, 두 번째 인자는 getter 함수의 멤버포인터, 세 번째 인자는 setter 함수의 멤버포인터이다.<br>
위 코드는 `Val` 로는 값을 읽는 것과 대입하는 것 모두 가능하고,
`RoVal` 로는 값을 읽는 것만 가능하다.

``` python
f = CppClass.FloatNum(3.14)
print(f.Val) # 3.14

f.Val = 2.718 # ok
print(f.Val) # 2.718

print(f.RoVal) # 2.718
f.RoVal = 0 # Error
```

### 상속
다음과 같이 상속관계의 클래스 2개와 이를 인자로 받는 함수가 다음과 같이 있다고 해보자.

``` c++
struct Base {
    virtual ~Base() {
        std::cout << "Base::~Base()" << std::endl;
    }
    void Func() const {
        std::cout << "Base::Func()" << std::endl;
    }
    virtual void VirtFunc() const {
        std::cout << "Base::VirtFunc()" << std::endl;
    }
};

struct Derived : Base {
    virtual void VirtFunc() const override {
        std::cout << "Derived::VirtFunc()" << std::endl;
    }
};

inline void RefCall(const Base& b) {
    b.Func();
    b.VirtFunc();
}
inline void PtrCall(const Base* pb) {
    pb->Func();
    pb->VirtFunc();
}

inline Base* NewDerived() {
    return new Derived;
}
```

Boost Python 에서 두 객체를 노출 시킬 때 다형성 유지되도록 할 수 있다.<br>
상위 클래스의 경우 위에서 설명한 것처럼 동일한 방식으로 노출 시킨다.

``` c++
class_<Base>("Base")
    .def("Func", &Base::Func)
    .def("VirtFunc", &Base::VirtFunc);
```
<br>

파생 클래스의 경우 노출 시킬 때 상위 클래스를 `class_`의 두 번째 템플릿 인자에 다음처럼 명시해 준다.

``` c++
class_<Derived, bases<Base>>("Derived");
```

위처럼 노출 시키게 되면 따로 함수 `Func()`나 `VirtFunc()`를 노출 시키지 않아도 `Base` 클래스를 노출 시킬 때 이미 노출 시켰으므로 파이썬에서 호출할 수 있다.<br>
<br>
위 두 클래스의 다형성을 확인해보기 위한 함수 `RefCall`, `PtrCall`, `NewDerived` 함수도 다음처럼 노출 시킨다.

``` c++
def("RefCallBase", RefCall);
def("PtrCallBase", PtrCall);
def("NewDerived", NewDerived,
    return_value_policy<manage_new_object>()); // Tell Python to take ownership of NewDerived's result
```

위 코드에서 `return_value_policy<manage_new_object>()`는 함수에서 리턴된 객체의 관리를 python에서 관리하겠다는 의미로 뒤에서 자세히 설명하겠다.<br>
<br>

다음 코드에서 다형성이 제대로 동작하는 것을 확인할 수 있다.
또한 함수 인자가 각각 참조와 포인터인 경우 모두 잘 동작하는 것을 알 수 있다.

``` python
import CppClass

b = CppClass.Base()
d = CppClass.Derived()

CppClass.RefCallBase(b)
# Output:
#   Base::Func()
#   Base::VirtFunc()
CppClass.PtrCallBase(b)
# Output:
#   Base::Func()
#   Base::VirtFunc()

CppClass.RefCallBase(d)
# Output:
#   Base::Func()
#   Derived::VirtFunc()
CppClass.PtrCallBase(d)
# Output:
#   Base::Func()
#   Derived::VirtFunc()

d2 = CppClass.NewDerived()
print(d2) # <CppClass.Derived object at 0x0000016F4D6547B0>
CppClass.RefCallBase(d2)
# Output:
#   Base::Func()
#   Derived::VirtFunc()
```

추가로, `CppClass.NewDerived()` 함수를 계속 호출 해보면 따로 반횐된 객체를 delete 해주지 않아도 중간 중간 소멸자가 호출 되는 것을 알 수 있다.<br>
`return_value_policy`를 `manage_new_object`로 지정하여 파이썬에서 해당 객체의 소멸 시점을 관리하기 때문인데, `manage_new_object` 대신 `reference_existing_object`로 지정하게 되면 반환된 객체가 소멸되지 않고 메모리 사용량이 계속 증가하는 것을 확인할 수 있다.<br>
<br>
하지만 위의 경우 하나의 문제가 있는데 `CppClass.Base`의 파생 클래스를 파이썬 내에서 선언하게 되면 다형성이 제대로 동작하지 않는 다는 것이다.

``` python
class DerivedInPy(CppClass.Base):
    def VirtFunc(self):
        print('DerivedInPy::VirtFunc()')

derived_in_py = DerivedInPy()
CppClass.RefCallBase(derived_in_py)
# Output:
#   Base::Func()
#   Base::VirtFunc()
```

<br>

다음과 같이 순수가상함수를 포함하는 다른 Base 클래스를 파이썬에서도 파생클래스를 만들 수 있도록 해보자.

``` c++
struct Base2 {
    virtual ~Base2() {}
    virtual int PureVirt() = 0;
    virtual int VirtFunc1() {
        int ret = 0;
        std::cout << "Base2::VirtFunc1() returnning " << ret << std::endl;
        return ret;
    }
    virtual void VirtFunc2() {
        std::cout << "Base2::VirtFunc2()" << std::endl;
    }
    void Func() {
        std::cout << "Base2::Func()" << std::endl;
    }
};
inline void CallBase2Funcs(Base2& obj) {
    std::cout << "obj.PureVirt(): " << obj.PureVirt() << std::endl;
    std::cout << "obj.VirtFunc1(): " << obj.VirtFunc1() << std::endl;
    obj.VirtFunc2();
    obj.Func();
}
```

위 클래스 `Base2`의 파생클래스를 파이썬에서 선언하기 위해서는 다음처럼 `Base2`와 `boost::python::wrapper<Base2>`를 상속한 래퍼클래스가 필요하다.

``` c++
struct Base2Wrap :
    Base2, wrapper<Base2> {
    int PureVirt() {
        return this->get_override("PureVirt")();
    }

    int VirtFunc1() {
        if (override virt_func1 = this->get_override("VirtFunc1")) {
            return virt_func1();
        }
        return this->Base2::VirtFunc1();
    }
    int DefaultVirtFunc1() {
        return this->Base2::VirtFunc1();
    }

    void VirtFunc2() {
        if (override virt_func2 = this->get_override("VirtFunc2")) {
            virt_func2();
        }
        else {
            this->Base2::VirtFunc2();
        }
    }
    void DefaultVirtFunc2() {
        this->Base2::VirtFunc2();
    }
};
```

순수 가상함수와 디폴트 구현이 있는 가상함수, 그리고 반환 값이 있는 경우와 없는경우 약간씩 래핑하는 방법이 달라지는 것을 알 수 있다.<br>
<br>
`Base2`를 모듈에 선언은 다음과 같이 한다.

``` c++
class_<Base2Wrap, boost::noncopyable>("Base2")
    .def("PureVirt", &Base2::PureVirt)
    .def("VirtFunc1", &Base2::VirtFunc1, &Base2Wrap::DefaultVirtFunc1)
    .def("VirtFunc2", &Base2::VirtFunc2, &Base2Wrap::DefaultVirtFunc2)
    .def("Func", &Base2::Func);

    def("CallBase2Funcs", &CallBase2Funcs);
```

<br>
다음은 위 코드를 테스트하는 파이썬 코드이다

``` python
import CppClass

class Derived2Py(CppClass.Base2):
    def PureVirt(self):
        ret = 42
        print('Derived2Py.PureVirt() retunrning {}'.format(ret))
        return ret
    
    def VirtFunc1(self):
        ret = 52
        print('Derived2Py.VirtFunc1() returnning {}'.format(ret))
        return ret

    def VirtFunc2(self):
        print('Derived2Py.VirtFunc2()')

der2py = Derived2Py()

ret = der2py.PureVirt() # Derived2Py.PureVirt() retunrning 42
print(ret) # 42
ret = der2py.VirtFunc1() # Derived2Py.VirtFunc2() returnning 52
print(ret) # 52
der2py.VirtFunc2() # Derived2Py.VirtFunc2()

print()
print('CppClass.CallBase2Funcs(der2py):')
CppClass.CallBase2Funcs(der2py)
# out:
    # CppClass.CallBase2Funcs(der2py):
    # Derived2Py.PureVirt() retunrning 42
    # obj.PureVirt(): 42
    # Derived2Py.VirtFunc1() returnning 52
    # obj.VirtFunc1(): 52
    # Derived2Py.VirtFunc2()
    # Base2::Func()
```

파이썬에서 파생된 클래스 에서도 다형성이 정상적으로 동작하는 것을 알 수 있다.

### Class Operators and Special Methods
다음은 파이썬에서 지원하는 일부 클래스 연산자와 특수 메소드를 파이썬 모듈에 노출시키는 예제이다.

``` c++
#include <boost/python.hpp>

#include <iostream>

struct Double {
    Double() : val_(0) {}
    Double(double val) : val_(val) {}

    double GetVal() const { return val_; }
    void SetVal(double val) { val_ = val; }

    Double& operator+=(double d) {
        this->val_ += d;
        return *this;
    }
    Double& operator-=(const Double& d) {
        this->val_ -= d.val_;
        return *this;
    }

    operator double() const { return val_; }

    friend Double operator+(const Double&, const Double&);
    friend Double operator+(const Double&, double);
    friend Double operator-(const Double&, const Double&);
    friend Double operator-(double, const Double&);
    friend bool operator<(const Double&, const Double&);

    friend Double pow(const Double&, const Double&);
    friend Double abs(const Double&);
    friend std::ostream& operator<<(std::ostream&, const Double&);

private:
    double val_;
};

inline Double operator+(const Double& d1, const Double& d2) {
    return Double(d1.val_ + d2.val_);
}
inline Double operator+(const Double& d_obj, double d) {
    return Double(d_obj.val_ + d);
}
inline Double operator-(const Double& d1, const Double& d2) {
    return Double(d1.val_ - d2.val_);
}
inline Double operator-(double d, const Double& d_obj) {
    return Double(d - d_obj.val_);
}
inline bool operator<(const Double& d1, const Double& d2) {
    return d1.val_ < d2.val_;
}

inline Double pow(const Double& d1, const Double& d2) {
    return Double(std::pow(d1.val_, d2.val_));
}
inline Double abs(const Double& d) {
    return Double(std::abs(d.val_));
}
inline std::ostream& operator<<(std::ostream& os, const Double& d) {
    os << "Double is holding " << d.val_ << std::endl;
    return os;
}

using namespace boost::python;
BOOST_PYTHON_MODULE(CppClass){
class_<Double>("Double")
    .def(init<double>())
    .add_property("Val", &Double::GetVal, &Double::SetVal)

    // Python Operators
    .def(self + self)       // __add__ : Double operator+(const Double&, const Double&)
    .def(self + double())   // __add__ : Double operator+(const Double&, double)
    .def(self - self)       // __sub__ : Double operator-(const Double&, const Double&)
    .def(double() - self)   // __rsub__ : Double operator-(double, const Double&)
    .def(self < self)       // __lt__ : bool operator<(const Double&, const Double&)
    .def(self += double())  // __ladd__ : Double& Double::operator+=(double)
    //.def(self += other<double>()) // <- ok
    .def(self -= self)      // Double& Double::operator-=(const Double&) 
    //.def(self -= other<Double>()) // <- ok

    // Special Methods
    .def(float_(self))          // __float__ : operator double()
    .def(pow(self, self))       // __pow__ : Double pow(const Double& d1, const Double& d2)
    .def(abs(self))             // __abs__ : Double abs(const Double& d)
    .def(self_ns::str(self))    // __str__ : ostream& operator<<(ostream&, const Double&)
    ;
}
```
<br>

다음은 이를 테스트한 파이썬 코드이다.
``` python
import CppClass

# Test Class Operators
d1 = CppClass.Double(3.14)
d2 = CppClass.Double(1)

print('d1 = {}, d2 = {}'.format(d1.Val, d2.Val)) # d1 = 3.14, d2 = 1.0

# self + self
d3 = d1 + d2
print('d1 + d2 = {:.2f}'.format(d3.Val)) # d1 + d2 = 4.14

# self + double()
d3 = d1 + 10
print('d1 + 10 = {}'.format(d3.Val)) # d1 + 10 = 13.14

# self - self
d3 = d1 - d2
print('d1 - d2 = {}'.format(d3.Val)) # d1 - d2 = 2.14

# double() - self
d3 = 10 - d2
print('10 - d2 = {}'.format(d3.Val)) # 10 - d2 = 9.0

# self < self
lt = d1 < d2
print('d1 < d2 ? = {}'.format(lt)) # d1 < d2 ? = False

# self += double()
d1 += 10
print('d1 += 10: d1 = {}'.format(d1.Val)) # d1 += 10: d1 = 13.14

# self -= self
d1 -= d2
print('d1 -= d2: d1 = {}'.format(d1.Val)) # d1 -= d2: d1 = 12.14

# Test Special Methods
d1 = CppClass.Double(-2)
d2 = CppClass.Double(2)

print('d1 = {}, d2 = {}'.format(d1.Val, d2.Val)) # d1 = -2.0, d2 = 2.0

f = float(d1)
print('f = float(d1), f = {}'.format(f)) # f = float(d1), f = -2.0

d3 = d1 ** d2
print('d1 ** d2 = {}'.format(d3.Val)) # d1 ** d2 = 4.0

d3 = abs(d1)
print('abs(d1) = {}'.format(d3.Val)) # abs(d1) = 2.0

print(d3) # Double is holding 2
```

## Functions

### Overloading
다음처럼 오버로딩 된 함수의 경우 각 오버로드된 (멤버)함수 포인터 타입의 변수를 전달하거나 해당 (멤버)함수 포인터 타입으로 형변환 하여 하나씩 노출시킬 수 있다.<br>

``` c++
struct OverloadedFoos {
    bool Foo(int i) {
        std::cout << "OverloadedFoos::Foo(int)" << std::endl;
        return true;
    }
    bool Foo(int i, double d) {
        std::cout << "OverloadedFoos::Foo(int, double)" << std::endl;
        return true;
    }
    int Foo(int i1, int i2, int i3) {
        std::cout << "OverloadedFoos::Foo(int, int, int)" << std::endl;
        return i1 + i2 + i3;
    }
};
```

다음은 각 오버로딩된 멤버함수를 각각 하나씩 노출시킨 코드이다.<br>

``` c++
bool (OverloadedFoos:: * foo_ptr1)(int) = &OverloadedFoos::Foo;
bool (OverloadedFoos:: * foo_ptr2)(int, double) = &OverloadedFoos::Foo;
class_<OverloadedFoos>("OverloadedFoos")
    .def("Foo", foo_ptr1) // by passing member function pointer varaible
    .def("Foo", foo_ptr2) // by passing member function pointer varaible
    .def("Foo", static_cast<int (OverloadedFoos::*)(int, int, int)>
        (&OverloadedFoos::Foo)) // by type casting
    ;
```

#### Default Argument
디폴트 인자가 있는 함수의 경우 다음처럼 간단한 래핑 함수를 이용하여 하나씩 노출 시킴으로 디폴트 인자의 효과를 낼 수 있다.

``` c++
void Bar(int i, double d = 3.14, char const* str = "Hello World") {
    std::cout << "Bar(" << i << ", " << d << ", '" << str << "')" << std::endl;
}

// Thin wrapper for Bar
void Bar1(int i) { Bar(i); }
void Bar2(int i, double d) { Bar(i, d); }

BOOST_PYTHON_MODULE(PyModuleName){
    void (*bar_all_args_fptr)(int, double, char const*) = &Bar; // Lost default!!!

    def("Bar", &Bar1); // one argument
    def("Bar", &Bar2); // two arguments
    def("Bar", bar_all_args_fptr); // all arguments
}
```

#### Auto Overloading
위의 함수 `Bar` 의 경우 `BOOST_PYTHON_FUNCTION_OVERLOADS` 매크로를 통해 쉽게 래핑할 수 있다.

``` c++
BOOST_PYTHON_FUNCTION_OVERLOADS(bar_overloads, Bar, 1, 3) 
BOOST_PYTHON_MODULE(PyModuleName){
    def("Bar", Bar, bar_overloads());
}
```

`BOOST_PYTHON_FUNCTION_OVERLOADS`의 첫 번째 인자는 `def` 함수에 전달되는 generator name,<br>
두 번째 인자는 오버로딩된 함수의 최소 인자 수,<br>
세 번째 인자는 오버로딩된 함수의 최대 인자 수이다.<br>
<br>

`BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS` 매크로를 통해 클래스 멤버함수의 오버로딩도 쉽게 할 수 있다.

``` c++
struct DefaultArgFoo {
    void Foo(int i = 0, double d = 3.14, char const* str = "Hello World") {
        std::cout << "DefaultArgFoo::(" << i << ", " << d << ", '" << str << "')" << std::endl;
    }
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(m_foo_overloads, Foo, 0, 3)
BOOST_PYTHON_MODULE(PyModuleName){
    class_<DefaultArgFoo>("DefaultArgFoo")
        .def("Foo", &DefaultArgFoo::Foo, m_foo_overloads());
}
```
<br>

위의 두 매크로를 통한 오버로딩을 위해선 디폴트인자가 있는 함수처럼 인자가 하나씩 추가되는 형태여야 한다.
예를들어 다음과 같은 경우는 위의 매크로 사용이 불가하고 직접 래퍼 함수를 통해 노출 시켜야 한다.

``` c++
void Foo(int){
    std::cout << "Foo" << std::endl;
}
void Foo(double, double){
    std::cout << "Foo(double, double)" << std::endl;
}
```
<br>

다음처럼 직접 래핑하는 것과 매크로로 자동화 하는 방식을 섞을 수도 있다.

``` c++
void Baz() {
    std::cout << "Baz()" << std::endl;
}
void Baz(int i) {
    std::cout << "Baz(int)" << std::endl;
}
void Baz(int i, double d) {
    std::cout << "Baz(int, double)" << std::endl;
}
void Baz(int i, double, const std::string& str) {
    std::cout << "Baz(int, double, const string&)" << std::endl;
}
void Baz(int, const std::string&) {
    std::cout << "Baz(int, const string&)" << std::endl;
}

BOOST_PYTHON_FUNCTION_OVERLOADS(baz_overaloads, Baz, 0, 3)
BOOST_PYTHON_MODULE(PyModuleName) {
    def("Baz", (void(*)(int, double, const std::string&))0, baz_overaloads());
    def("Baz", static_cast<void(*)(int, const std::string&)>(&Baz));
}
```

### Call Policies
다음의 함수를 파이썬으로 내보낸다고 해보자.

``` c++
struct Y {
    X x;
    Z* ptr_z = nullptr;
};
X& F(Y& y, Z* z){
    y.z = z;
    return y.x;
}
```

그리고 노출된 함수를 다음처럼 사용한다고 해보자.

``` python
x = F(y, z)
del y
x.SomeFunction() # CRASH!
```

문제가 무엇인지 바로 알 수 있을 것이다.<br>
<br>

함수 `F`에서 반환된 `x`는 `y` 객체가 가지고 있는 멤버의 참조인데 `y`가 소멸되면 `x`도 같이 소멸될 것이다.<br>
소멸된 객체의 멤버함수를 호출하고 있으니 심각한 문제가 있음을 알 수 있다.<br>
<br>

실제로 `X`에 다음처럼 내부 멤버변수 `x_`를 반환하는 멤버함수 `Get`을 추가한 뒤 위와 같은 경우를 테스트해보면 동작이 이상하게 되는 것을 알 수 있다.

``` c++
struct X {
    X(int x) :x_(x) {
        std::cout << "X(" << x << ")" << std::endl;
    }
    ~X() {
        std::cout << "~X(): " << x_ << std::endl;
    }
    int Get() const { return x_; }

private:
    int x_;
};

struct Z {
    Z(int z) : z_(z) {
        std::cout << "Z(" << z << ")" << std::endl;
    }
    ~Z() {
        std::cout << "~Z(): " << z_ << std::endl;
    }
    int Get() const { return z_; }
    void Set(int z) { z_ = z; }

private:
    int z_;
};

struct Y {
    X x;
    Z* z;

    Y(int y) : x(y) {
        std::cout << "Y(" << y << ")" << std::endl;
        z = nullptr;
    }
    ~Y() {
        std::cout << "~Y(): " << x.Get() << std::endl;
    }
    int GetZ() const { return z->Get(); }
};
```

<br>
위의 `X`, `Y`, `Z`와 함수 `F`를 다음처럼 내보낸다.

``` c++
class_<X>("X", init<int>())
    .def("Get", &X::Get);
class_<Y>("Y", init<int>())
    .def("GetZ", &Y::GetZ);
class_<Z>("Z", init<int>())
    .add_property("Val", &Z::Get, &Z::Set);

def("F", &F, return_value_policy<reference_existing_object>()); //<- May crash
```

`F`를 노출 시키는 `def` 함수의 `return_value_policy<reference_existing_object>()` 인자는 `F`의 반환 값이 C++의 객체 참조를 그대로 반환함을 나타낸다.<br>
<br>

다음은 이를 테스트한 파이썬 코드이다.

``` python
import PyModuleName

y = PyModuleName.Y(10)
# Out:
#   X(10)
#   Y(10)
z = PyModuleName.Z(20)
# Out:
#   Z(20)

x = PyModuleName.F(y, z)
del y
# Out:
#   ~Y(): 10
#   ~X(): 10

print(x.Get()) # ???
```

`y` 가 소멸되면서 `x`도 소멸되어 `x.Get()` 의 값이 임의의 값이 나오는 것을 확인할 수 있다.<br>
<br>

`F`를 노출 시킬 때 다음처럼 `return_internal_reference`를 사용하면 위와 같은 경우가 발생하는 것을 막을 수 있다.

``` c++
def("F", &F, return_internal_reference<1>())
```

`return_internal_reference<1>` 은 함수의 반환되는 참조의 소유를 첫번째(템플릿 인자 1에 의해) 인자가 가지고 있음을 나타낸다.<br>
(즉, `return_internal_reference<n>` 은 반환되는 참조가 함수의 n번째 인자에 의해 소유됨을 의미한다.)<br>
따라서, 위처럼 반환되는 참조의 소유를 나타내게 되면 `del y`로 `y`를 지워도 `x`에 대한 참조가 살아있어 둘다 소멸되지 않고 `x.Get()`이 10을 반환하는 것을 확인할 수 있다.

<div class="notice--primary" markdown="1">
**<i class="fa fa-exclamation-triangle" aria-hidden="true"></i> NOTE**

함수 `F`의 반환되는 참조를 복사해서 반환하도록 노출시킬 수도 있겠지만 그럴 경우 C++ 함수의 의도와 맞지 않을 수 있다.

``` python
F(y, z).Set(42)
print(y.x.Get()) # 42?
```

위와 같은 경우 `F`가 `y.x`의 복사본을 반환하게 되면 `.Set(42)`가 `y.x`에 적용되지 않고 복사된 임시객체에 적용되게 될 것이다.
</div>

함수 `F`에 아직 하나의 문제가 더 남아있는데 다음의 코드를 보자.

``` python
x = F(y, z) # y.z 가 z를 가르킴
del z # z 소멸
y.GetZ() # Crash
```

함수 `F`에 의해 `y.z`가 `z`를 가르키는 상태에서 `del z`에 의해 `z`가 소멸되었는데 `y.z`가 이 소멸된 `z`를 가르키고 있는 상태에서 `y.GetZ()`를 통해 소멸된 `z`를 접근하면서 오동작을 일으키게 된다.<br>
따라서, `return_internal_reference<1>`에 추가로 `with_custodian_and_ward` 정책을 전달하여 `z`가 `y`에 의해 참조됨을 나타내주면 위와 같은 현상을 막을 수 있다.

``` c++
def("F", &F,
    return_internal_reference <1, 
        with_custodian_and_ward<1, 2>>());
```

`with_custodian_and_ward<1, 2>`는 첫(1) 번째 인자(custodian)가 두(2) 번째 인자(ward) 를 참조하고 있음을 의미한다.<br>
> <small><i class="fa fa-info-circle" aria-hidden="true"></i> custodian은 **관리인**, ward는 **피보호자**를 뜻한다.</small>

2개 이상의 정책을 전달할 때는 위처럼 마지막 템플릿 인자에 nested loop 처럼 추가하면 된다.<br>
다음은 2개이상의 정책을 전달할 때의 일반적인 문법을 나타낸 것이다.

```
policy1<args...,
    policy2<args...,
        policy3<args...>>>
```

다음은 미리 정의된 call polices 목록이다.

- **with_custodian_and_ward**: 함수 인자간의 수명(lifetime) 관계를 표시
- **with_custodian_and_ward_postcall**: 함수 인자들과 반환 결과간의 수명 관계를 표시
- **return_internal_reference**: 함수 인자 하나와 반환 결과간의 수명 관계를 표시
- **return_value_policy<T> T는 다음 중 하나:**
  - **reference_existing_object**
  - **copy_const_reference**
  - **copy_non_const_reference**
  - **manage_new_object**

위 리스트들의 자세한 정보는 [여기](https://www.boost.org/doc/libs/1_76_0/libs/python/doc/html/reference/function_invocation_and_creation/models_of_callpolicies.html)에서 볼 수 있다.

## Object Interface
기본적으로 Boost Python의 클래스 `object`를 통해 모든 파이썬 객체를 파이썬에서처럼 다룰 수 있다.<br>

> <small><i class="fa fa-info-circle" aria-hidden="true"></i> Boost Python 문서에서는 `object` 가 `PyObject*`를 래핑한 클래스라고 설명하고 있다.</small>

``` python
def mul(x, y):
    return x * y

def f(x, y):
    if(y == 'foo'):
        x[3:7] = 'bar'
    else:
        x.val += y(3, 2)
    return x

def getfunc():
    return mul
```

위의 파이썬 코드는 다음의 Boost.Python 을 통해 다음처럼 C++로 동일하게 표현할 수 있다.

``` c++
object mul(object x, object y) {
    return x * y;
}

object f(object x, object y) {
    if (y == "foo") {
        x.slice(3, 7) = "bar";
    }
    else {
        x.attr("val") += y(3, 2);
    }
    return x;
}

object getfunc() {
    return object(mul);
}
```

위 코드를 다음처럼 파이썬 모듈로 내보낸 뒤 테스트 해보면 동일한 결과가 나오는 것을 확인할 수 있다.

``` c++
BOOST_PYTHON_MODULE(PyModuleName) {
    def("mul", mul);
    def("f", f);
    def("getfunc", getfunc);
}
```

``` python
import PyModuleName

class X:
    def __init__(self, val):
        self.val = val

x = [0,1,2,3,4,5,6,7,8,9]
x = f(x, 'foo') # 파이썬에서 선언된 함수 f 호출
print(x) # [0, 1, 2, 'b', 'a', 'r', 7, 8, 9]

x = [0,1,2,3,4,5,6,7,8,9]
x = PyModuleName.f(x, 'foo') # C++ 에서 내보낸 함수 f 호출
print(x) # [0, 1, 2, 'b', 'a', 'r', 7, 8, 9]

x = X(10)
x = f(x, getfunc()) # 파이썬에서 선언된 함수 f, getfunc 호출
print(x.val) # 16

x = X(10)
x = PyModuleName.f(x, PyModuleName.getfunc()) # C++ 에서 내보낸 함수 사용
print(x.val) # 16
```

### Derived Object Types
Boost.Python 에는 파이썬과 대응되는 다음의 파생 클래스를 제공한다.

- list
- dict
- tuple
- str
- long_
- enum_

위 파생 클래스 타입들은 파이썬 타입들과 동일하게 동작한다.<br>
예:
```
str(1) ==> '1'
```

또한 파이썬 객체의 멤버 메소드를 동일하게 가지고 있는 경우도 있다.<br>
예를 들면 `dict`의 `keys()` 멤버 함수는 Boost.Python의 `dict` 클래스도 동일하게 가지고 있다.<br>
<br>

`make_tuple` 함수를 통해 `tuple` 객체를 만들 수 도 있다.

``` c++
make_tuple(123, 'D', "Hello, World", 0.0); // (123, 'D', 'Hello, World', 0.0)
```

Boost.Python의 `object`를 인자로 받는 함수를 만들어 이를 파이썬 모듈로 내보내는 경우 호출 시 해당 타입 혹은 서브타입을 전달해야 한다.<br>
예를 들어 다음처럼 `str`을 인자로 받는 함수를 만들어 파이썬 모듈로 내보내면 파이썬에서 호출 시 파이썬의 `str` 혹은 그 서브타입만 전달 할 수 있다.

``` c++
object ToUpper(str python_str) {
    str uppered = python_str.upper(); // better
    //object uppered = python_str.attr("upper")(); // also available
    object msg = "%s is uppered to %s" % make_tuple(python_str, uppered);
    return msg;
}
```

위 코드를 보면 파이썬 문자열 객체의 멤버 메소드인 `.upper()`를 C++ 멤버 함수로 제공하는 것을 알 수 있다.<br>
또한, 파이썬의 `"format" % (x, y, z)`처럼 사용하는 문자열 포멧 기능을 C++에서도 제공하는 것을 알 수 있다.

### class_\<T\> as objects
Boost.Python의 `object`의 동적인 성질에 따라 어떤 `class_<T>` 타입이든 `object` 타입이 될 수 있다.<br>
다음은 `class_<T>` 타입을 `object` 타입으로 래핑한 것을 보여준다.

``` c++
object vec345 = (
    class_<Vec2>("Vec2", init<double, double>())
        .def_readonly("length", &Vec2::length)
        .def_readonly("angle", &Vec2::angle)
    )(3.0, 4.0); // Vec2(3, 4) to object

assert(vec345.attr("length") == 5.0);
```

### C++ 객체 꺼내기
`object` 객체에서 C++ 타입의 값을 꺼내기 위해서는 `extract<T>` 함수를 사용해야 한다.<br>

``` c++
double x = o.attr("length"); // compile error
```

위 코드를 빌드해보면 컴파일 에러가 발생하는 것을 확인할 수 있는데, `object` 타입은 `double` 타입으로 암시적 형변환 될 수 없기 때문이다.<br>
대신 다음처럼 `extract<T>` 함수를 사용하면 C++ 타입으로 변환이 가능하게 된다.

``` c++
double l = extract<double>(o.attr("length"));
Vec2& v = extract<Vec2&>(o);
```

첫 번째 라인은 `o` 객체의 `length` 속성을 double 타입으로 추출을 **시도**하는 것이고, <br>
두 번째 라인은 `o` 객체를 `Vec2` 타입으로 가져오는 것을 **시도**하는 것이다.<br>
<br>

여기서 **시도**한다고 말하는 이유는 `extract<T>` 함수에서 `T`타입으로 변환이 항상 성공하는 것이 아니기 때문이다.<br>
예를 들어 위에서 `o`객체의 타입이 `Vec2`인데 `extract<double>(o)` 처럼 double 값을 꺼내려고 한다면 예외가 발생하게 될 것이다.<br>
따라서 안전하게 특정 타입으로 꺼내기 위해서 다음처럼 해당 타입으로 꺼낼 수 있는지 테스트하는 것이 필요하다.

``` c++
extract<Vec2&> x(o);
if (x.check()) { // 변환이 되면 true, 아니면 false
    Vec2& v = x(); ...
```

### Enums
``` c++
enum Color { red, bule };
```

위의 enum 타입은 다음처럼 내보낼 수 있다.

``` c++
enum_<Color>("Color")
    .value("red", red)
    .value("blue", blue)
    ;
```

<div class="notice--primary" markdown="1">

**<i class="fa fa-info-circle" aria-hidden="true"></i> scope**<br>

지금까지 내보낸 모든 클래스, 함수, enum 들은 현재 `scope()`에 노출 되게 되는데 기본은 현재 모듈에 해당하는 네임스페이스가 된다.<br>
현재 모듈에 새로운 네임 스페이스에 무언가를 노출시키고 싶으면 해당 네임스페이스에 해당하는 클래스를 노출 시키면서 이를 `scope` 객체에 대입하면 된다.<br>
이후에 선언되는 것들은 해당 네임스페이스에 노출되게 되며 다시 이전의 네임스페이스에 노출 시키고 싶으면 해당 `scope` 객체를 파괴시켜주면 된다.<br>

``` c++
struct SomeScope {
    enum Color { red, green, blue };
};

BOOST_PYTHON_MODULE(PyModuleName){
    // scope()는 현재 모듈
    // ...
    {
        // change current scope
        // 현재 scope()는 현재모듈.SomeScope
        scope in_some_scope = class_<SomeScope>("SomeScope");

        // Expose enum SomeScope::Color as SomeScope.Color
        enum_<SomeScope::Color>("Color")
            .value("red", SomeScope::red)
            .value("blue", SomeScope::blue);

        in_some_scope.attr("current_color") = SomeScope::red;
    } // ~in_some_scope() : revert to orignal scope
    
    // 다시 scope()는 현재 모듈
    // ...
    
}
```

위 코드는 모듈아래 `SomeScope`라는 이름의 네임스페이스에 `Color` enum을 노출시키는 코드이다.<br>
블록안에 선언된 지역변수는 해당 블록을 벗어나면 자동으로 소멸 되는 성질을 이용하여 `in_some_scope` 변수가 자동으로 소멸되도록 하였다.(`in_some_scope` 변수가 소멸되면서 `scope()`가 `PyModuleName.SomeScope` 에서 `PyModuleName`으로 돌아옴)<br>
</div>


[boost_1_76_0_link]: https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/
[boost_1_76_0.7z_link]: https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.7z
[boost_1_76_0.zip_link]: https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.zip