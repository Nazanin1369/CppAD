/* $Id$ */
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-16 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */
/* my_example.i */
%module my_example
%{
# include "my_example.h"
%}

%include "cpointer.i"
%pointer_class(int, int_ptr);

%include "carrays.i"
%array_functions(int, int_vector);

int         my_fact(int n);
int         my_mod(int x, int y);
const char* my_message();
void        my_add(int x, int y, int* result);
int         my_max(int n, int* x);