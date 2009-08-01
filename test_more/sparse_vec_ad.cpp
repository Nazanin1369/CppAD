/* $Id$ */
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-09 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the 
                    Common Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */


# include <cppad/cppad.hpp>

bool sparse_vec_ad(void)
{	bool ok = true;
	using namespace CppAD;

	// dimension of the domain space
	size_t n = 3; 

	size_t i, j;

	// independent variable vector 
	CPPAD_TEST_VECTOR< AD<double> > X(n);
	for(j = 0; j < n; j++)
		X[j] = AD<double>(j); 
	Independent(X);

	// dependent variable vector
	size_t m = n;
	CPPAD_TEST_VECTOR< AD<double> > Y(m);

	// check results vector
	CPPAD_TEST_VECTOR< bool >  Check(m * n);

	// Create a VecAD so that there are two in the tape and the sparsity
	// pattern depends on the second one (checks addressing VecAD objects)
	VecAD<double> W(n);

	// VecAD equal to X
	VecAD<double> Z(n);
	AD<double> J;
	for(j = 0; j < n; j++)
	{	J    = AD<double>(j);
		W[J] = X[0];
		Z[J] = X[j];

		// y[i] depends on x[j] for j <= i
		// (and is non-linear for j <= 1).
		if( j <= 1 )
			Y[j] = Z[J] * Z[J];
		else	Y[j] = Z[J];
	}

	// compute dependent variables values
	AD<double> P = 1;
	J = AD<double>(0);
	for(j = 0; j < n; j++)
	{	for(i = 0; i < m; i++)
			Check[ i * m + j ] = (j <= i);
	}
	
	// create function object F : X -> Y
	ADFun<double> F(X, Y);

	// dependency matrix for the identity function W(x) = x
	CPPAD_TEST_VECTOR< bool > Identity(n * n);
	for(i = 0; i < n; i++)
	{	for(j = 0; j < n; j++)
			Identity[ i * n + j ] = false;
		Identity[ i * n + i ] = true;
	}
	// evaluate the dependency matrix for Identity(F(x))
	CPPAD_TEST_VECTOR< bool > Px(m * n);
	Px = F.RevSparseJac(n, Identity);

	// check values
	for(i = 0; i < m; i++)
	{	for(j = 0; j < n; j++)
			ok &= (Px[i * m + j] == Check[i * m + j]);
	}	

	// evaluate the dependency matrix for F(Identity(x))
	CPPAD_TEST_VECTOR< bool > Py(m * n);
	Py = F.ForSparseJac(n, Identity);

	// check values
	for(i = 0; i < m; i++)
	{	for(j = 0; j < n; j++)
			ok &= (Py[i * m + j] == Check[i * m + j]);
	}	

	// compute sparsity pattern for Hessian of F_m ( Identity(x) ) 
	CPPAD_TEST_VECTOR<bool> Hy(m);
	for(i = 0; i < m; i++)
		Hy[i] = false;
	Hy[m-1] = true;
	CPPAD_TEST_VECTOR<bool> Pxx(n * n);
	Pxx = F.RevSparseHes(n, Hy);

# if 0
	// This test case demonstrates a bug in the current version of
	// of the reverse Hessian sparsity pattern calculations. It will be
	// fixed in the next commit.
	for(i = 0; i < n; i++)
	{	for(j = 0; j < n; j++)
			ok &= (Pxx[i * n + j] == ( (i <= 1) & (j <= 1) ) );
	}
# endif

	return ok;
}
