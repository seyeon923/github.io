#pragma once

#include <iostream>
#include <string>
#include <cmath>
#include <vector>

struct World {
    World() : msg("Hello World") {}
    World(const std::string& msg) : msg(msg) {}
    ~World() { std::cout << "~World()" << std::endl; }

    std::string Greet() { return msg; }

    const std::string msg;
    double val;
};

struct FloatNum {
    FloatNum() : val_(0) {}
    FloatNum(float val) : val_(val) {}
    float Get() const { return val_; }
    void Set(float val) { val_ = val; }
private:
    float val_;
};

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

void Bar(int i, double d = 3.14, char const* str = "Hello World") {
    std::cout << "Bar(" << i << ", " << d << ", '" << str << "')" << std::endl;
}

// Auto-overloadable Baz
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

struct DefaultArgFoo {
    void Foo(int i = 0, double d = 3.14, char const* str = "Hello World") {
        std::cout << "DefaultArgFoo::(" << i << ", " << d << ", '" << str << "')" << std::endl;
    }
};

struct OneShotConstructor {
    OneShotConstructor(int i = 0, double d = 3.14, const std::string& str = "Hello World") {
        std::cout << "Constructed with i = " << i << ", d = " << d << ", str = '" << str << "'" << std::endl;
    }
};


// Call Policies
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

X& F(Y& y, Z* z) {
    y.z = z;
    return y.x;
}


struct SomeScope {
    enum Color { red, green, blue };
};

std::vector<int> GetVector(int n) {
    std::vector<int> ret(n);
    for (int i = 0; i < n; ++i) {
        ret[i] = i;
    }
    return ret;
}