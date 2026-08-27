//! Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//! File: `acc1.rs`
//!
//! Purpose :   Tutorial example for affine conic constraints.
//!             Models the problem:
//!
//! ```
//! maximize c^T x
//! subject to  sum(x) = 1
//!             gamma >= |Gx+h|_2
//! ```


use mosek_stable_api as msk;

fn acc1() -> Result<(),msk::APIError> {
    let mskapi = msk::initialize()?;

    mskapi.task()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {
                /* Input data dimensions */
                let n = 3;
                let k = 2;

                /* Create n free variables */
                task.append_vars(n)?;
                task.put_var_bound_slice_value(0, n, f64::NEG_INFINITY, f64::INFINITY)?;

                /* Append empty AFE rows for affine expression storage */
                task.append_rows(k + 2)?;

                /* Set up the objective */
                {
                    let cj = &[0, 1, 2];
                    let c  = &[2.0, 3.0, -1.0];

                    task.put_row(0, cj, c)?;
                    task.put_obj_sense(msk::ObjSense::MAXIMIZE);
                    task.put_obj_row(0)?;
                }

                /* One linear constraint sum(x) == 1 */
                let zero_dom = task.get_domain_rzero()?;

                // define sum constraint
                for i in 0..n { task.put_ijc(1, i, 1.0)?; }
                task.append_cons(&[zero_dom], &[1], &[1], Some(&[1.0]))?;

                {
                    /* Fill in the affine expression storage with data */
                    /* F matrix in sparse form */
                    let h     = &[0.0, 0.1];
                    let gamma = 0.03;

                    /* Fill in F storage */
                    task.put_ijc_list(&[2,2,3,3], &[0,1,0,2], &[1.5, 0.1, 0.3, 2.1])?;

                    /* Fill in g storage */
                    task.put_row_g(0, gamma)?;
                    task.put_row_slice_g(2, h)?;
                }

                /* Define a conic quadratic domain */
                let quad_dom = task.get_domain_quadratic_cone(k + 1)?;

                {
                    /* Create the ACC */
                    let afeidx = &[1, 2, 3];
                    let rhs    = &[0.0, 0.0, 0.0];

                    task.append_con(quad_dom, /* Domain index */
                                    afeidx,   /* Indices of AFE rows [0,...,k] */
                                    Some(rhs))?;
                }

                /* Begin optimization and fetching the solution */
                /* Run optimizer */
                let trmcode = task.optimize()?;

                /* Print a summary containing information about the solution for debugging purposes*/
                task.solution_summary(msk::StreamType::MSG)?;

                let solidx = 0;
                let(psolsta, dsolsta) = task.get_sol_status(solidx)?;

                match psolsta
                {
                    msk::SolSta::OPTIMAL => {
                        let mut xx = vec![0.0; n as usize];
                        let mut doty = vec![0.0; k as usize+1];

                        task.get_sol_xx_slice(solidx, 0, &mut xx)?;
                        println!("Optimal primal solution: {:?}",xx);

                        /* Fetch the doty dual of the ACC */
                        task.get_sol_y_slice(solidx, 1, /* first constraint */
                                            1,         /* number of constraints */
                                            &mut doty)?;

                        println!("Dual doty of the ACC: {:?}",doty);
                    }
                    msk::SolSta::INFEAS_CERT => println!("Dual infeasibility certificate found."),
                    msk::SolSta::ILLPOSED_CERT => println!("Dual illposedness certificate found."),
                    msk::SolSta::UNKNOWN|msk::SolSta::UNDEFINED =>
                        match dsolsta
                        {
                            msk::SolSta::INFEAS_CERT   => println!("Primal infeasibility certificate found."),
                            msk::SolSta::ILLPOSED_CERT => println!("Primal illposedness certificate found."),
                            _ => println!("The status of the solution could not be determined. Termination code: {}.\n", mskapi.get_trm_name(trmcode))
                        }
                    _ => {}
                }
                Ok(())
            })
}

fn main() {
    acc1().unwrap();
}
