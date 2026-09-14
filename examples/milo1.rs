//! Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//! File: `acc1.rs`
//!
//! Purpose: To demonstrate how to solve a small mixed integer linear optimization problem using the MOSEK API.
//!
//! Problem:
//! ```
//! max x1 + 0.64 x2
//! s.t. 50 x1 + 31 x2 < 250
//!       3 x1 -  2 x2 > -4.0
//!       x1,x2 > 0
//! ```

use std::f64;

use mosek_stable_api as msk;

fn milo1() -> Result<(),msk::APIError> {
    let mskapi = msk::initialize_with_defaults()?;

    mskapi.task()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {
                let numvar = 2;

                let rowlen = &[2,2,2];
                let subj   = &[0,1,0,1,0,1];
                let  val   = &[1.0,0.64,50.0,31.0,3.0,-2.0];
                let  rhs   = &[250.0,-4.0];

                let dom_rminus = task.get_domain_rminus()?;
                let dom_rplus  = task.get_domain_rplus()?;

                // Append 'numvar' variables.
                // The variables will initially be fixed at zero (x=0).
                task.append_vars(numvar)?;

                task.append_rows(3)?;
                task.put_row_slice(0,rowlen,subj,val)?;

                task.put_obj_row(0)?;
                task.put_obj_sense(msk::ObjSense::MAXIMIZE);

                // Append 'numcon' constraints.
                task.append_cons(&[dom_rminus,dom_rplus],&[1,2],Some(rhs))?;

                // Set the bounds on variable j.
                //  blx[j] <= x_j <= bux[j]
                task.put_var_bound_slice_value(0,numvar,0.0,f64::INFINITY)?;

                /* Specify integer variables. */

                task.put_var_type_slice_value(0, numvar, msk::VariableType::INTEGER)?;

                /* Set max solution time */
                _ = task.put_double_param("dpar_mio_max_time", 60.0)?;

                // Run optimizer
                let trmcode = task.optimize()?;

                // Print a summary containing information
                // about the solution for debugging purposes
                task.solution_summary(msk::StreamType::MSG)?;
                task.write_task_to_file("dump.ptf")?;


                let solidx = 0;

                let (psolsta,_dsolsta) = task.get_sol_status(solidx)?;

                match psolsta {
                    msk::SolSta::INTEGER_OPTIMAL|msk::SolSta::OPTIMAL => {
                        let mut xx = vec![0.0; numvar as usize];
                        task.get_sol_xx_slice(solidx,0,&mut xx)?;
                        println!("Optimal solution: x = {:?}",xx);
                    },
                    msk::SolSta::FEASIBLE => {
                        // A feasible but not necessarily optimal solution was located.
                        let mut xx = vec![0.0; numvar as usize];
                        task.get_sol_xx_slice(solidx,0,&mut xx)?;
                        println!("Feasible solution: x = {:?}",xx);
                    },
                    msk::SolSta::UNDEFINED|msk::SolSta::UNKNOWN => {
                        let prosta = task.get_problem_status(solidx)?;
                        match prosta {
                            msk::ProSta::PRIMAL_INFEASIBLE_OR_UNBOUNDED => println!("Problem status Infeasible or unbounded"),
                            msk::ProSta::PRIMAL_INFEASIBLE => println!("Problem status Infeasible."),
                            msk::ProSta::UNKNOWN => println!("Problem status unknown. Termination code {}.", mskapi.get_trm_name(trmcode)),
                            _ => println!("Other problem status.")
                        }
                    },
                    _ => panic!("Other solution status.")
                }
                Ok(())
            })
}

fn main() {
    milo1().unwrap();
}

#[cfg(test)]
mod test {
    #[test]
    fn test() { super::main(); }
}
