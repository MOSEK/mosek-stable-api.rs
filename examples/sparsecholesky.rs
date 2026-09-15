//!   Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//!   File: sparsecholesky.rs
//!
//!   Purpose: Demonstrate the sparse Cholesky factorization.
//!

use mosek_stable_api as msk;
use mosek_stable_api::APIError;



/* Prints out a Cholesky factor presented in sparse form */
fn print_sparse(
                        perm : &[i32],
                        diag : &[f64] ,
                        lnzc : &[i32],
                        lsubc : &[i32],
                        lvalc : &[f64]  )
{
    println!("P       = {:?}",perm);
    println!("diag(D) = {:?}",diag);
    println!("L       = ");
    for (i,(p,l)) in lnzc.iter().scan(0usize,|p,&l| { let r = (*p,l as usize); *p += r.1; Some(r) }).enumerate() {
        println!("\tcol{}: {:?}",i,lsubc[p..p+l].iter().cloned().zip(lvalc[p..p+l].iter().cloned()).collect::<Vec<(i32,f64)>>());
    }
}

fn sparse_cholelsky() -> Result<(),APIError>
{
    msk::initialize_with_defaults()?;

    //Observe that anzc, aptrc, asubc and avalc only specify the lower triangular part.
    {
        let a_col_num_nonzero = &[ 4,1,1,1 ];
        let a_subi = &[ 0, 1, 2, 3, 1, 2, 3 ];
        let a_val  = &[4.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0];
        let b      = &[13.0, 3.0, 4.0, 5.0];
        let n = a_col_num_nonzero.len();

        let mut perm = vec![0i32; n];
        let mut diag = vec![0.0; n];
        let mut l_col_num_nonzero = vec![0i32; n];

        println!("\nExample with positive definite A.");

        let (l_subi, l_val) = msk::compute_sparse_cholesky(
            0,       /* Mosek chooses number of threads */
            true,    /* Apply a reordering heuristic */
            1.0e-14, /* Singularity tolerance */
            a_col_num_nonzero,
            a_subi,
            a_val,
            &mut perm,
            &mut diag,
            &mut l_col_num_nonzero)?;

        print_sparse(&perm, &diag, &l_col_num_nonzero, &l_subi, &l_val);

        /* Permuted b is stored as x. */
        let mut x : Vec<f64> = perm.iter().map(|&i| b[i as usize]).collect();

        /* Compute inv(L)*x. */
        msk::sparse_triangular_solve_dense(false, &l_col_num_nonzero, &l_subi, &l_val, &mut x)?;

        /* Compute inv(L^T)*x. */
        msk::sparse_triangular_solve_dense(true, &l_col_num_nonzero, &l_subi, &l_val, &mut x)?;

        let mut res = vec![0.0;n]; for (&j,&xj) in perm.iter().zip(x.iter()) { res[j as usize] = xj; }
        println!("\nSolution A x = b, x = {:?} ",res);
    }


    {
        let a_col_num_nonzero = &[3, 2, 1];
        let a_subi = &[0, 1, 2, 1, 2, 2];
        let a_val  = &[1.0, 1.0, 1.0, 1.0, 1.0, 1.0];
        /* Let A be

            [1.0 1.0 1.0]
            [1.0 1.0 1.0]
            [1.0 1.0 1.0]

        then compute a sparse Cholesky factorization A. Observe A is NOT
        positive definite.
        */

        println!("\nExample with a semidefinite A.");
        let n = a_col_num_nonzero.len();
        let mut perm = vec![0i32; n];
        let mut diag = vec![0.0; n];
        let mut l_col_num_nonzero = vec![0i32; n];
        let (l_subi,l_val) = msk::compute_sparse_cholesky(
            0,      /* Mosek chooses number of threads */
            true,   /* Use reordering heuristic */
            1.0e-14,/* Singularity tolerance */
            a_col_num_nonzero,
            a_subi,
            a_val,
            &mut perm,
            &mut diag,
            &mut l_col_num_nonzero)?;
        print_sparse(&perm, &diag, &l_col_num_nonzero, &l_subi, &l_val);
    }
    Ok(())
}

fn main() {
    sparse_cholelsky().unwrap();
}


#[cfg(test)]
mod test {
    #[test]
    fn test() { super::main(); }
}
