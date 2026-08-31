//! Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//! File: `acc2.rs`
//!
//!   Purpose :   Tutorial example for affine conic constraints.
//!               Models the problem:
//!
//!               maximize c^T x
//!               subject to  sum(x) = 1
//!                           gamma >= |Gx+h|_2
//!
//!               This version inputs the linear constraint as an affine conic constraint.

use std::f64;

use mosek_stable_api as msk;

fn acc2() -> Result<(),msk::APIError> {
    let mskapi = msk::initialize()?;

    mskapi.task()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {

                let rowlen = &[3,3,0,2,2];
                let subj = &[
                    0,1,2, /* objective */
                    0,1,2,

                    0,1,
                    0,  2];
                let val = &[
                    2.0,3.0,-1.0, /* objective */
                    1.0,1.0,1.0,

                    1.5,0.1,
                    0.3,    2.1];
                let gamma = 0.03;
                let h     = &[0.0,0.0,gamma,0.0, 0.1];

                /* Input data dimensions */
                let n : i32 = 3;
                let k : i32 = 2;
                let nrow = 5;

                /* Create n free variables */
                task.append_vars(n)?;

                task.put_var_bound_slice_value(0, n, f64::NEG_INFINITY, f64::INFINITY)?;

                /* Append empty AFE rows for affine expression storage */
                task.append_rows(nrow)?;

                /* Fill in the affine expression storage with data */
                task.put_row_slice(0,rowlen,subj,val)?;
                task.put_row_slice_g(0,h)?;

                /* Set up the objective */
                task.put_obj_sense(msk::ObjSense::MAXIMIZE);
                task.put_obj_row(0)?;

                /* One linear constraint sum(x) == 1 */
                let dom_rzero = task.get_domain_rzero()?;
                task.append_con(dom_rzero, &[1], Some(&[1.0]))?;

                {
                    let quad_dom = task.get_domain_quadratic_cone(k as i64 + 1)?;
                    /* Define a conic quadratic domain */
                    /* Create the ACC */
                    let afeidx = &[2, 3, 4];
                    let rhs    = &[0.0, 0.0, 0.0];

                    task.append_con(quad_dom,
                                    afeidx,
                                    Some(rhs))?;
                }

                /* Begin optimization and fetching the solution */
                /* Run optimizer */
                let trmcode = task.optimize()?;

                // Print a summary containing information about the solution for debugging purposes
                task.solution_summary(msk::StreamType::MSG)?;

                let solidx = 0;
                let (psolsta,_dsolsta) = task.get_sol_status(solidx)?;

                match psolsta
                {
                    msk::SolSta::OPTIMAL => {
                        let mut xx = vec![0.0; n as usize];
                        task.get_sol_xx_slice(solidx, 0, &mut xx)?;

                        println!("Optimal primal solution: {:?}",xx);

                        /* Fetch the doty dual of the ACC */
                        let mut doty = vec![0.0; (k+1) as usize];

                        task.get_sol_y_slice(solidx,
                                             1,
                                             1,
                                             &mut doty)?;

                        println!("Dual doty of the ACC: {:?}",doty);
                    },
                    msk::SolSta::INFEAS_CERT => println!("Primal or dual infeasibility certificate found."),
                    msk::SolSta::UNKNOWN|msk::SolSta::UNDEFINED => println!("The status of the solution could not be determined. Termination code: {}.", trmcode),
                    _ => println!("Other solution status.")
                }
                Ok(())
            })
}

fn main() {
    acc2().unwrap();
}

#[cfg(test)]
mod test {
    #[test]
    fn test() { super::main(); }
}
