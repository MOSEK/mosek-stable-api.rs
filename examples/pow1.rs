//!
//! Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//! File:      pow1.rs
//!
//! Purpose: Demonstrates how to solve the problem
//! ```
//! max  x^0.2*y^0.8 + z^0.4 - x
//! s.t. x + y + 0.5z = 2
//!      x,y,z >= 0
//! ```
//!    as
//! ```
//! max  w0 + w1 - x
//! s.t. x + y + 0.5 z = 2
//!      w0 > x^0.2 * y^0.8
//!      w1 > z^0.4
//!      x,y,z >= 0
//! ```

use mosek_stable_api as msk;

fn pow1() -> Result<(),msk::APIError> {
    let mskapi = msk::initialize_with_defaults()?;

    let numvar : i32 = 5;
    let numrow : i64 = 8;

    let rowlen = &[ 3, 3, 1,1,1, 1,0,1 ];
    let subj   = &[ 3,4,0, 0,1,2, 0,1,3, 2,4 ];
    let  val   = &[ 1.0,1.0,-1.0, 1.0,1.0,0.5, 1.0,1.0,1.0, 1.0,1.0 ];

    let alpha_1 = &[0.2, 0.8];
    let alpha_2 = &[0.4, 0.6];

    let blx = &[ 0.0,0.0,0.0,f64::NEG_INFINITY,f64::NEG_INFINITY ];
    let bux = &[ f64::INFINITY,f64::INFINITY,f64::INFINITY,f64::INFINITY,f64::INFINITY ];

    mskapi.task()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {
                let domidx = &[
                    task.get_domain_rzero()?,
                    task.get_domain_primal_power_cone(3, alpha_1)?,
                    task.get_domain_primal_power_cone(3, alpha_2)?];

                /* Append 'numvar' variables. The variables will initially be fixed at zero (x=0). */
                task.append_vars(numvar)?;
                task.put_var_bound_slice(0, blx, bux)?;
                task.append_rows(numrow)?;
                task.put_row_slice(0, rowlen, subj, val)?;
                task.append_con(domidx[0], &[1], Some(&[2.0]))?;
                task.append_con(domidx[1], &[2,3,4], None)?;
                task.append_con(domidx[2], &[5,6,7], None)?;

                task.put_obj_sense(msk::ObjSense::MAXIMIZE);
                task.put_obj_row(0)?;

                /* Run optimizer */
                let trmcode = task.optimize()?;
                /* Print a summary containing information about the solution for debugging purposes*/
                task.solution_summary(msk::StreamType::MSG)?;

                let solidx = 0;

                let (psolsta,dsolsta) = task.get_sol_status(solidx)?;

                match psolsta {
                    msk::SolSta::OPTIMAL => {
                        let mut xx = vec![0.0; numvar as usize];
                        task.get_sol_xx_slice(solidx,0,&mut xx)?;
                        println!("Optimal primal solution : {:?}",xx);
                    },
                    msk::SolSta::INFEAS_CERT => println!("Dual infeasibility certificate found."),
                    msk::SolSta::ILLPOSED_CERT => println!("Dual illposedness certificate found."),
                    msk::SolSta::UNKNOWN|msk::SolSta::UNDEFINED =>
                        match dsolsta
                        {
                            msk::SolSta::INFEAS_CERT => println!("Primal infeasibility certificate found."),
                            msk::SolSta::ILLPOSED_CERT => println!("Primal illposedness certificate found."),
                            _ => println!("The status of the solution could not be determined. Termination code: {}.", mskapi.get_trm_name(trmcode))
                        }
                    _ => panic!("Other solution status.")
                }
                Ok(())
            })
}

fn main() {
    pow1().unwrap();
}

#[cfg(test)]
mod test {
    #[test]
    fn test() { super::main(); }
}
