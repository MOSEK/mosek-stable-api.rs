//! Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//! File: `cqo1.rs`
//!
//!
//!   Purpose:   To demonstrate how to solve a small conic quadratic
//!              optimization problem using the MOSEK API.
//!
//!   Problems:  min y1 + y2 + y3
//!              s.t.
//!

use mosek_stable_api as msk;


fn cqo1() -> Result<(),msk::APIError> {
    let mskapi = msk::initialize_with_defaults()?;

    let numvar : i32 = 6;
    let numrow : i64 = 8;

    let blx    = &[0.0, 0.0, 0.0, f64::NEG_INFINITY, f64::NEG_INFINITY, f64::NEG_INFINITY];
    let bux    = &[f64::INFINITY, f64::INFINITY, f64::INFINITY, f64::INFINITY, f64::INFINITY, f64::INFINITY];
    let subj   = &[3, 4, 5, /* objective */
                   0, 1, 2, /* linear constraint */
                   3, 0, 1, 4, 5, 2];
    let cof    = &[1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0];
    let rowlen = &[3, 3, 1, 1, 1, 1, 1, 1];

    mskapi.task()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {

                /* Append 'numvar' variables.
                    The variables will initially be fixed at zero (x=0). */
                task.append_vars(numvar)?;

                /* Append 'numcon' empty constraints.
                The constraints will initially have no bounds. */
                task.append_rows(numrow)?;
                task.put_row_slice(0, rowlen, subj, cof)?;

                let dom_rzero = task.get_domain_rzero()?;
                task.append_con(dom_rzero, &[1], Some(&[1.0]))?;

                task.put_obj_row(0)?;
                task.put_obj_sense(msk::ObjSense::MINIMIZE);

                // Set the bounds on variable j.
                // blx[j] <= x_j <= bux[j] */
                task.put_var_bound_slice(0,blx,bux)?;

                /* Set the non-zero entries of the F matrix */
                let domidx = &[
                    /* Append quadratic cone domain */
                    task.get_domain_quadratic_cone(3)?,
                    /* Append rotated quadratic cone domain */
                    task.get_domain_rotated_quadratic_cone(3)? ];

                /* Append two ACCs made up of the AFEs and the domains defined above. */
                task.append_con(domidx[0],&[2,3,4],None)?;
                task.append_con(domidx[1], &[5,6,7],None)?;

                /* Run optimizer */
                let trmcode = task.optimize()?;

                /* Print a summary containing information about the solution for debugging purposes*/
                task.solution_summary(msk::StreamType::MSG)?;
                let solidx = 0;

                let (psolsta, dsolsta) = task.get_sol_status(solidx)?;

                match psolsta
                {
                    msk::SolSta::OPTIMAL => {
                        let mut xx = vec![0.0;numvar as usize];
                        task.get_sol_xx_slice(solidx, 0, &mut xx)?;
                        println!("Optimal primal solution: {:?}",xx);
                    },
                    msk::SolSta::INFEAS_CERT => println!("Primal or dual infeasibility certificate found."),
                    msk::SolSta::ILLPOSED_CERT => println!("Primal or dual illposedness certificate found."),
                    msk::SolSta::UNKNOWN|msk::SolSta::UNDEFINED =>
                        match dsolsta
                        {
                            msk::SolSta::INFEAS_CERT => println!("Primal or dual infeasibility certificate found."),
                            msk::SolSta::ILLPOSED_CERT => println!("Primal or dual illposedness certificate found."),
                            _ => println!("The status of the solution could not be determined. Termination code: {}.\n", mskapi.get_trm_name(trmcode))
                        },
                    _ => println!("Other solution status.")
                }
                Ok(())
            })
}



fn main() {
    cqo1().unwrap();
}

#[cfg(test)]
mod test {
    #[test]
    fn test() { super::main(); }
}
