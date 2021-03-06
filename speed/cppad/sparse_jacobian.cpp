/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-17 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */
/*
$begin cppad_sparse_jacobian.cpp$$
$spell
	const
	ifdef
	ifndef
	colpack
	boolsparsity
	namespace
	onetape
	work work
	jac
	CppAD
	cppad
	hpp
	bool
	typedef
	endif
	tmp
	std
	var
	cout
	endl
	Jacobian
	Fp
$$

$section CppAD Speed: Sparse Jacobian$$
$mindex link_sparse_jacobian speed$$


$head Specifications$$
See $cref link_sparse_jacobian$$.

$head Implementation$$

$srccode%cpp% */
# include <cppad/cppad.hpp>
# include <cppad/speed/uniform_01.hpp>
# include <cppad/speed/sparse_jac_fun.hpp>

// Note that CppAD uses global_option["memory"] at the main program level
# include <map>
extern std::map<std::string, bool> global_option;

namespace {
	using CppAD::vector;
	typedef vector<size_t>  s_vector;
	typedef vector<bool>    b_vector;

	void calc_sparsity(
		CppAD::sparse_rc<s_vector>& sparsity ,
		CppAD::ADFun<double>&       f        )
	{	bool reverse       = global_option["revsparsity"];
		bool transpose     = false;
		bool internal_bool = global_option["boolsparsity"];
		bool dependency    = false;
		size_t q;
		if( reverse )
			q = f.Range();
		else
			q = f.Domain();
		//
		CppAD::sparse_rc<s_vector> identity;
		identity.resize(q, q, q);
		for(size_t k = 0; k < q; k++)
			identity.set(k, k, k);
		//
		if( reverse )
		{	f.rev_jac_sparsity(
				identity, transpose, dependency, internal_bool, sparsity
			);
		}
		else
		{	f.for_jac_sparsity(
				identity, transpose, dependency, internal_bool, sparsity
			);
		}
	}
}

bool link_sparse_jacobian(
	size_t                           size     ,
	size_t                           repeat   ,
	size_t                           m        ,
	const CppAD::vector<size_t>&     row      ,
	const CppAD::vector<size_t>&     col      ,
	      CppAD::vector<double>&     x        ,
	      CppAD::vector<double>&     jacobian ,
	      size_t&                    n_sweep  )
{
	if( global_option["atomic"] )
		return false;
# if ! CPPAD_HAS_COLPACK
	if( global_option["colpack"] )
		return false;
# endif
	// optimization options: no conditional skips or compare operators
	std::string options="no_compare_op";
	// -----------------------------------------------------
	// setup
	typedef CppAD::AD<double>    a_double;
	typedef vector<double>       d_vector;
	typedef vector<a_double>     ad_vector;
	//
	size_t order = 0;         // derivative order corresponding to function
	size_t n     = size;      // number of independent variables
	ad_vector  a_x(n);        // AD domain space vector
	ad_vector  a_y(m);        // AD range space vector y = f(x)
	CppAD::ADFun<double> f;   // AD function object
	//
	// declare sparsity pattern
	CppAD::sparse_rc<s_vector>  sparsity;
	//
	// declare subset where Jacobian is evaluated
	CppAD::sparse_rc<s_vector> subset_pattern;
	size_t nr  = m;
	size_t nc  = n;
	size_t nnz = row.size();
	subset_pattern.resize(nr, nc, nnz);
	for(size_t k = 0; k < nnz; k++)
		subset_pattern.set(k, row[k], col[k]);
	CppAD::sparse_rcv<s_vector, d_vector> subset( subset_pattern );
	const d_vector& subset_val( subset.val() );
	//
	// coloring method
	std::string coloring = "cppad";
# if CPPAD_HAS_COLPACK
	if( global_option["colpack"] )
		coloring = "colpack";
# endif
	//
	// maximum number of colors at once
	size_t group_max = 25;
	// ------------------------------------------------------
	if( ! global_option["onetape"] ) while(repeat--)
	{	// choose a value for x
		CppAD::uniform_01(n, x);
		for(size_t j = 0; j < n; j++)
			a_x[j] = x[j];
		//
		// declare independent variables
		Independent(a_x);
		//
		// AD computation of f(x)
		CppAD::sparse_jac_fun<a_double>(m, n, a_x, row, col, order, a_y);
		//
		// create function object f : X -> Y
		f.Dependent(a_x, a_y);
		//
		if( global_option["optimize"] )
			f.optimize(options);
		//
		// skip comparison operators
		f.compare_change_count(0);
		//
		// calculate the Jacobian sparsity pattern for this function
		calc_sparsity(sparsity, f);
		//
		// structure that holds some of the work done by sparse_jac_for
		CppAD::sparse_jac_work work;
		//
		// calculate the Jacobian at this x
		// (use forward mode because m > n ?)
		n_sweep = f.sparse_jac_for(
			group_max, x, subset, sparsity, coloring, work
		);
		for(size_t k = 0; k < nnz; k++)
			jacobian[k] = subset_val[k];
	}
	else
	{	// choose a value for x
		CppAD::uniform_01(n, x);
		for(size_t j = 0; j < n; j++)
			a_x[j] = x[j];
		//
		// declare independent variables
		Independent(a_x);
		//
		// AD computation of f(x)
		CppAD::sparse_jac_fun<a_double>(m, n, a_x, row, col, order, a_y);
		//
		// create function object f : X -> Y
		f.Dependent(a_x, a_y);
		//
		if( global_option["optimize"] )
			f.optimize(options);
		//
		// skip comparison operators
		f.compare_change_count(0);
		//
		// calculate the Jacobian sparsity pattern for this function
		calc_sparsity(sparsity, f);
		//
		// structure that holds some of the work done by sparse_jac_for
		CppAD::sparse_jac_work work;
		//
		while(repeat--)
		{	// choose a value for x
			CppAD::uniform_01(n, x);
			//
			// calculate the Jacobian at this x
			// (use forward mode because m > n ?)
			n_sweep = f.sparse_jac_for(
				group_max, x, subset, sparsity, coloring, work
			);
			for(size_t k = 0; k < nnz; k++)
				jacobian[k] = subset_val[k];
		}
	}
	return true;
}
/* %$$
$end
*/
