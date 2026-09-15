//!
//! Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//! File: `lo2.rs`
//!
//! # Description
//!
//! To demonstrate how to solve a small linear optimization problem using the MOSEK C API, and handle the solver result
//! and the problem solution.
//!
//! Data is entered by row.
//!
//! ```
//! min    3 x1 +   x2 + 5 x3 +   x4
//! s.t. + 3 x1 +   x2 + 2 x3        = 30.0
//!      + 2 x1 +   x2 + 3 x3 +   x4 > 15.0
//!             + 2 x2        + 3 x4 < 25.0
//!      0.0 < x1,x3,x4
//!      0.0 < x2 < 10.0
//! ```

use mosek_stable_api as msk;

fn lo2() -> Result<(),msk::APIError> {
    msk::initialize_with_defaults()?;

    let numvar : usize = 4;
    let numrow : usize = 4;
    let numcon : usize = 3;

    // Below is the sparse representation of the objective and A
    // matrix stored by row. First row is the objcetive. */
    let subj : &[&[i32]] = &[&[0, 1, 2, 3],
                             &[0, 1, 2   ],
                             &[0, 1, 2, 3],
                             &[    1, 3  ]];
    let aval : &[&[f64]] = &[&[3.0,1.0,5.0,1.0],
                             &[3.0,1.0,2.0    ],
                             &[2.0,1.0,3.0,1.0],
                             &[    2.0,    3.0]];
    let bc : &[f64]      = &[ 30.0, 15.0, 25.0 ];

    /* Bounds on variables. */
    let blx : &[f64]     = &[ 0.0, 0.0, 0.0, 0.0 ];
    let bux : &[f64]     = &[ f64::INFINITY, 10.0, f64::INFINITY, f64::INFINITY ];

    msk::Task::new()?
        /* Directs the log task stream to the printer function. */
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{0}",msg),
            |task| {
                /* Bounds on constraints. */
                let con_dom_idx = [
                    task.get_domain_rzero()?,
                    task.get_domain_rplus()?,
                    task.get_domain_rminus()? ];

                /* Append 'numcon' empty constraints.
                 * The constraints will initially have no bounds. */
                task.append_rows(numrow as i64)?;

                task.append_empty_cons(numcon as i64)?;

                /* Append 'numvar' variables.
                The variables will initially be fixed at zero (x=0). */
                task.append_vars(numvar as i32)?;

                for j in 0..numvar {
                    // Set the bounds on variable j.
                    // blx[j] <= x_j <= bux[j]
                    task.put_var_bound(j as i32, /* Index of variable.*/
                                       blx[j],  /* Numerical value of lower bound.*/
                                       bux[j])?; /* Numerical value of upper bound.*/
                }

                /* Set the bounds on constraints.
                 * for i=1, ...,numcon : blc[i] <= constraint i <= buc[i] */
                for i in 0..numcon {
                    task.append_con(con_dom_idx[i], &[(i+1) as i64], Some(&bc[i..i+1]))?;
                }

                for i in 0..numrow {
                    /* Input row i of A */
                    task.put_row(i as i64,    /* Row index.*/
                                 subj[i],     /* Pointer to column indexes of row i.*/
                                 aval[i])?;   /* Pointer to values of row i.*/
                }

            /* Maximize objective function. */
            task.put_obj_sense(msk::ObjSense::MAXIMIZE);
            task.put_obj_row(0)?;

            /* Run optimizer */
            let trmcode = task.optimize()?;
            /* Print a summary containing information
                * about the solution for debugging purposes. */
            task.solution_summary(msk::StreamType::LOG)?;

            let solidx : i32 = 0;
            match task.get_sol_status(solidx)? {
                (msk::SolSta::OPTIMAL,_) => {
                    let mut xx = vec![0.0; numvar]; task.get_sol_xx_slice(solidx,0,&mut xx)?;
                    println!("Optimal primal solution = {:?}",xx);
                },
                (msk::SolSta::INFEAS_CERT,_) => println!("Certificate of dual infeasibility found."),
                (_,msk::SolSta::INFEAS_CERT) => println!("Certificate of primal infeasibility found."),
                (msk::SolSta::ILLPOSED_CERT,_) => println!("Certificate of dual illposedness found."),
                (_,msk::SolSta::ILLPOSED_CERT) => println!("Certificate of primal illposedness found."),
                (msk::SolSta::UNKNOWN,msk::SolSta::UNKNOWN) => {
                    println!("The solution status is unknown.");
                    println!("The optimizer terminitated with code: {}", msk::get_trm_name(trmcode));
                },
                _ => println!("Other solution status.")
            }
            Ok(())
        })
}
fn main() {
    lo2().unwrap();
}

#[cfg(test)]
mod test {
    #[test]
    fn test() { super::main(); }
}
