# **vector**

This is a test project to evaluate the performance of a basic SIMD vector
algebra implementation relative to a reference implementation. *It is *not*
currently implemented to be consumed by other code*.

## Setup

This project uses CMake and works with the following compilers:
- MSVC 14+ (i.e. Visual Studio 2015)
- GCC 4.8+
- Clang 3.8+ (requires libc++)

Part of the generation phase executes a program to query the current CPU for
supported SIMD instruction sets which is then used for conditional compilation
of several code paths in the library. Currently it checks for:

* MMX, SSE, SSE2, SSE3, SSSE3, SSE4.1, SSE4.2, FMA, AVX, AVX2

## Targets

| target      | description
|-------      |------------
| vector      | Static library for vector implementation
| vector_test | Executable for testing vector implementation
| trace       | Static library for ray tracing using the vector implementation
| trace_test  | Executable for testing ray tracing implementation
| light_app   | Executable for evaluating light models used for ray tracing

## Performance

Reported performance of the reference implementation is deceptive for basic
algebraic operations like addition, subtraction, and multiplication because
the compiler is able to vectorize the timing loop. However, in general the
compiler is not able to vectorize individual operations even when the operands
meet alignment requirements. For example:

```
// Will produce scalar code
c = a + b;

// Will produce vectorized code
for (size_t ii = 0; ii < size; ++ii) {
    z[ii] = x[ii] + y[ii];
}
```

This is important for general usage where hetergeneous sequential operations
are significantly faster using vectorized code than scalar code. For example,
general usage tests like hitSphere and traceScene are between one and a half to
two times faster using the intrinsics implementation even though the operations
they are composed of appear to have equivalent performance when tested in
isolation.

## Design

Both reference and SIMD implementations expose three fundamental types;
`Scalar`, `Vector`, and `Matrix`. The SIMD implementation also exposes a
`VectorScalar` type as a proxy for operations on a single vector component.
Algebraic operations are performed using operator overloads; specifically
the following operators are supported:

|              | `Scalar` | `Vector` | `Matrix` |
|------------- | -------- | -------- | -------- |
| **`Scalar`** | `+-*/<>` | `*/`     | `*/`     |
| **`Vector`** | `*/`     | `+-*%`   |          |
| **`Matrix`** | `*/`     | `*`      | `*`      |

Where `Vector * Vector` represents the dot product in **R**4 and `Vector %
Vector` represents the cross product in **R**3. The `%` operator was chosen
for the cross product to disambiguate it from the wedge product (typically
represented by `^`) which has different cardinality in **R**3 and **R**4
whereas the cross product is only defined in **R**3, and because it has the
same precedence as multiplication.
