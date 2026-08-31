//! Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//! File: `ceo1.rs`
//!
//! Purpose:   To demonstrate how to solve a small conic exponential
//!            optimization problem using the MOSEK Core API.
//!
//! The problem:
//! ```
//! min  x1+x2
//! s.t. x1+x2+x3 = 1.0
//!      (x1,x2,x3) in C_exp
//! ```

use mosek_stable_api as msk;

fn ceo1() -> Result<(),msk::APIError> {
    let mskapi = msk::initialize()?;

    let numvar : i32 = 3;
    let numrow : i64 = 5;

    let subj : &[i32]   = &[0, 1,    /* objective */
                            0, 1, 2, /* linear constraint */
                            0,       /* \                   */
                               1,    /*  | conic constraint */
                                  2];/* /                   */
    let cof : &[f64]    = &[1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0];
    let rowlen : &[i32] = &[2, 3, 1, 1, 1];

    mskapi.task()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {
                task.append_vars(numvar)?;

                task.put_var_bound_slice_value(0, numvar, f64::NEG_INFINITY, f64::INFINITY)?;

                task.append_rows(numrow)?;


                /* Set up domains and bounds */
                let dom_rzero = task.get_domain_rzero()?;

                task.put_obj_row(0)?;
                task.put_obj_sense(msk::ObjSense::MINIMIZE);

                /* Set up the linear part */
                task.put_row_slice(0, rowlen, subj, cof)?;

                task.append_con(dom_rzero, &[1], Some(&[1.0]))?;

                task.put_row_g(0, 1.0)?;

                let dom_exp = task.get_domain_primal_exponential_cone()?;

                task.append_con(dom_exp, &[2,3,4], None)?;

                /* Run optimizer */
                let trmcode = task.optimize()?;

                // Print a summary containing information
                // about the solution for debugging purposes
                task.solution_summary(msk::StreamType::MSG)?;

                let solidx = 0;

                let (psolsta, dsolsta) = task.get_sol_status(solidx)?;

                match psolsta {
                    msk::SolSta::OPTIMAL|msk::SolSta::FEASIBLE => {
                        let mut xx = vec![0.0; numvar as usize];
                        task.get_sol_xx_slice(solidx, 0, &mut xx)?;
                        println!("Optimal primal solution : {:?}",xx);
                    },
                    msk::SolSta::INFEAS_CERT => println!("Dual infeasibility certificate found."),
                    msk::SolSta::ILLPOSED_CERT => println!("Dual illposedness certificate found."),
                    msk::SolSta::UNDEFINED|msk::SolSta::UNKNOWN =>
                        match dsolsta {
                            msk::SolSta::INFEAS_CERT => println!("Primal infeasibility certificate found."),
                            msk::SolSta::ILLPOSED_CERT => println!("Primal illposedness certificate found."),
                            _ => println!("The status of the solution could not be determined. Termination code: {}.", mskapi.get_trm_name(trmcode))
                        },
                    _ => println!("Other solution status.")
                }
                Ok(())
            })
} /* main */

fn main() {
    ceo1().unwrap();
}


#[cfg(test)]
mod test {
    #[test]
    fn test() { super::main(); }
}
