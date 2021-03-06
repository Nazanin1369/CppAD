/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-17 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

// system include files used for I/O
# include <iostream>

// C style asserts
# include <cassert>

// CppAD include file
# include <cppad/cppad.hpp>

// test runner
# include <cppad/utility/test_boolofvoid.hpp>

// external complied tests
extern bool k_gt_one(void);
extern bool multiple_solution(void);
extern bool retape_k1_l1(void);
extern bool retape_k1_l2(void);

// main program that runs all the tests
int main(void)
{	std::string group = "cppad_ipopt/test";
	size_t      width = 20;
	CppAD::test_boolofvoid Run(group, width);

	// external compiled tests
	Run( k_gt_one,            "k_gt_one"           );
	Run( multiple_solution,   "multiple_solution"  );
	Run( retape_k1_l1,        "retape_k1_l1"       );
	Run( retape_k1_l2,        "retape_k1_l2"       );
	//
	// check for memory leak
	bool memory_ok = CppAD::thread_alloc::free_all();
	// print summary at end
	bool ok = Run.summary(memory_ok);
	//
	return static_cast<int>( ! ok );
}
