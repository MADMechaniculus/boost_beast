#ifndef DEFINES_H
#define DEFINES_H

#include <string>

#define INTERFACE_SET_IMPL(classname, method, input_t) \
    std::string classname::method::name = ""#method""; \
    std::string classname::method::type = "interface_getter"; \
    void classname::method::__call(input_t & input)

#define INTERFACE_GET_IMPL(classname, method, output_t) \
    std::string classname::method::name = ""#method""; \
    std::string classname::method::type = "interface_getter"; \
    void classname::method::__call(output_t & output)

#define INTERFACE_IMPL(classname, method, input_t, output_t) \
    std::string classname::method::name = ""#method""; \
    std::string classname::method::type = "interface_getter"; \
    void classname::method::__call(input_t & input, output_t & output)

#define INTERFACE_BODY(method) \
    class method { \
    public: \
        static std::string name; \
        static std::string type;

#define INTERFACE_METHOD_DECL(method, out_construction, input_construction) \
    INTERFACE_BODY(method); \
    static void __call(input_construction & input, out_construction & output); \
};

#define INTERFACE_GET_DECL(method, out_construction) \
    INTERFACE_BODY(method) \
    static void __call(out_construction & output); \
};

#define INTERFACE_SET_DECL(method, input_construction) \
    INTERFACE_BODY(method) \
    static void __call(input_construction & input); \
};

#endif // DEFINES_H
