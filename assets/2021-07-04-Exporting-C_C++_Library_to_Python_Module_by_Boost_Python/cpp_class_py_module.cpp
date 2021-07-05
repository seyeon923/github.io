#include <boost/python.hpp> // include order important

#include "cpp_class.h" 

using namespace boost::python;

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

// Thin wrapper for Bar
void Bar1(int i) { Bar(i); }
void Bar2(int i, double d) { Bar(i, d); }

// Create thin wrappers by boost macro
BOOST_PYTHON_FUNCTION_OVERLOADS(bar_overloads, Bar, 1, 3) // (Generator Name, Function Name, Minimum Arguments, Maximum Arguments)

// Create thin wrappers by boost macro
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(m_foo_overloads, Foo, 0, 3)

BOOST_PYTHON_FUNCTION_OVERLOADS(baz_overaloads, Baz, 0, 3)

str ToUpper(str python_str) {
    str uppered = python_str.upper();
    return uppered;
}

BOOST_PYTHON_MODULE(CppClass) {

    class_<World>("World")
        .def(init<const std::string&>())
        .def("Greet", &World::Greet)
        .def_readonly("msg", &World::msg)
        .def_readwrite("val", &World::val)
        ;

    class_<FloatNum>("FloatNum")
        .def(init<float>())
        .add_property("Val", &FloatNum::Get, &FloatNum::Set)
        .add_property("RoVal", &FloatNum::Get);

    class_<Base>("Base")
        .def("Func", &Base::Func)
        .def("VirtFunc", &Base::VirtFunc);

    class_<Derived, bases<Base>>("Derived");

    def("RefCallBase", RefCall);
    def("PtrCallBase", PtrCall);
    //def("NewDerived", NewDerived);
    def("NewDerived", NewDerived,
        return_value_policy<manage_new_object>()); // Tell Python to take ownership of NewDerived's result

    class_<Base2Wrap, boost::noncopyable>("Base2")
        //.def("PureVirt", pure_virtual(&Base2::PureVirt))
        .def("PureVirt", &Base2::PureVirt)
        .def("VirtFunc1", &Base2::VirtFunc1, &Base2Wrap::DefaultVirtFunc1)
        .def("VirtFunc2", &Base2::VirtFunc2, &Base2Wrap::DefaultVirtFunc2)
        .def("Func", &Base2::Func);

    def("CallBase2Funcs", &CallBase2Funcs);

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

    // Function Overloadings
    bool (OverloadedFoos:: * foo_ptr1)(int) = &OverloadedFoos::Foo;
    bool (OverloadedFoos:: * foo_ptr2)(int, double) = &OverloadedFoos::Foo;
    class_<OverloadedFoos>("OverloadedFoos")
        .def("Foo", foo_ptr1)
        .def("Foo", foo_ptr2)
        .def("Foo", static_cast<int (OverloadedFoos::*)(int, int, int)>(&OverloadedFoos::Foo));

    // Default Arguments
    void (*bar_ptr)(int, double, char const*) = &Bar; // Lost default!!!

    def("Bar", &Bar1); // one argument
    def("Bar", &Bar2); // two arguments
    def("Bar", bar_ptr); // all arguments

    // Use BOOST_PYTHON_FUNCTION_OVERLOADS
    def("Bar2", Bar, bar_overloads());

    // Member function with default arguments
    class_<DefaultArgFoo>("DefaultArgFoo")
        .def("Foo", &DefaultArgFoo::Foo, m_foo_overloads());

    class_<OneShotConstructor>("OneShotConstructor")
        .def(init<int, optional<double, const std::string&>>());

    def("Baz", (void(*)(int, double, const std::string&))0, baz_overaloads());
    def("Baz", static_cast<void(*)(int, const std::string&)>(&Baz));


    // Call Policies
    class_<X>("X", init<int>())
        .def("Get", &X::Get);
    class_<Y>("Y", init<int>())
        .def("GetZ", &Y::GetZ);
    class_<Z>("Z", init<int>())
        .add_property("Val", &Z::Get, &Z::Set);

    // def("F", &F, return_value_policy<reference_existing_object>()); <- May crash
    // def("F", &F, return_internal_reference<1>()); <- May crash too
    def("F", &F,
        return_internal_reference <1, with_custodian_and_ward<1, 2>>());

    {
        // change current scope
        scope in_some_scope = class_<SomeScope>("SomeScope");

        // Expose enum SomeScope::Color as SomeScope.Color
        enum_<SomeScope::Color>("Color")
            .value("red", SomeScope::red)
            .value("blue", SomeScope::blue);

        in_some_scope.attr("current_color") = SomeScope::red;
    } // ~in_some_scope() : revert to orignal scope

    scope().attr("yes") = 1;
    scope().attr("no") = 0;

    def("ToUpper", &ToUpper);

    def("GetVector", &GetVector);
}