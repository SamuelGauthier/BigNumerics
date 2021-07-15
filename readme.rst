===========
BigNumerics
===========

.. image:: https://img.shields.io/badge/-C%2B%2B-blue
    :target: https://img.shields.io/badge/-C%2B%2B-blue

.. image:: https://img.shields.io/badge/license-wtfpl-brightgreen 
    :target: https://img.shields.io/badge/license-wtfpl-brightgreen 

.. contents:: Table of Contents
    :depth: 2
    :backlinks: top

This repository contains header files for the :code:`BigInteger` and
:code:`BigDecimal` implementation in C++. The algorithms are based on Knut's
`"The Art of Computer Programming Volume 2"
<https://cs.stanford.edu/~knuth/taocp.html>`_ Chapter 4.3 "Multiple-Precision
Arithmetic".

Implemented Operators
=====================

- Addition :code:`+`
- Substraction :code:`-`
- Multiplication :code:`*`
- Division :code:`/`

How to Use the Library
======================

Download the repo and include either the :code:`BigInteger.h` or the
:code:`BigDecimal.h` files in your code. Both :code:`BigInteger` and
:code:`BigDecimal` live in the namespace :code:`BigNumerics`.

BigInteger Usage Example
------------------------

.. code:: c++

    #include <iostream>
    #include "BigInteger.h"

    #define endl "\n"

    using namespace std;

    int main(int argc, char *argv[])
    {
        BigNumerics::BigInteger A("123456789012345678901234567890");
        BigNumerics::BigInteger B("123456789012345678901234567890");

        cout << A + B << endl;
        cout << A * B << endl;
        cout << A - B << endl;
        cout << A / B << endl;

        return 0;
    }

BigDecimal Usage Example
------------------------

.. code:: c++

    #include <iostream>
    #include "BigDecimal.h"

    #define endl "\n"

    using namespace std;

    int main(int argc, char *argv[])
    {
        BigNumerics::BigDecimal A("1234567890.12345678901234567890");
        BigNumerics::BigDecimal B("1234567890.12345678901234567890");

        cout << A + B << endl;
        cout << A * B << endl;
        cout << A - B << endl;
        cout << A / B << endl;

        return 0;
    } 


TODO
====

- [ ] :code:`BigDecimal` division implementation
- [ ] Benchmark tests
- [ ] Unit tests

.. Benchmarking
.. ============

.. This implementation is slower than java's :code:`BigInteger`.
