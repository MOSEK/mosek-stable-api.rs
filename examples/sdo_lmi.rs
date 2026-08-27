//!
//!  Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//!  File :      sdo_lmi.rs
//!
//!  Purpose :   To solve a problem with an LMI and an affine conic constrained problem with a PSD term
//!  ```
//!               minimize       ⎡ 1  0 ⎤
//!                           Tr ⎣ 0  1 ⎦ * X + x₁ + x₂ + 1
//!
//!               subject to     ⎡ 0  1 ⎤
//!                           Tr ⎣ 0  1 ⎦ * X - x₁ - x₂ >= 0
//!
//!                              ⎡ 0  1 ⎤      ⎡ 3  1 ⎤   ⎡ 1  0 ⎤
//!                           x₁ ⎣ 1  3 ⎦ + x₂ ⎣ 1  0 ⎦ + ⎣ 0  1 ⎦  ≽ 0
//!
//!                           X ≽ 0
//!                           x ∈ R²
//! ```


const NUMVAR : i32 = 2;    /* Number of scalar variables */

use mosek_stable_api as msk;

#[allow(non_snake_case)]
fn sdo_lmi() -> Result<(),msk::APIError> {
    let mskapi = msk::initialize()?;

    let DIMBARVAR = &[2];         /* Dimension of semidefinite cone */
    let LENBARVAR = &[2 * (2 + 1) / 2]; /* Number of scalar SD variables  */


    let rowlen = &[
        // objective
        2,
        // linear constraint
        2,
        // PSD constraint
        1,
        2,
        1,
    ];
    let subj = &[
        0,1,
        0,1,

        1,
        0,1,
        0,
    ];
    let val : &[f64] = &[
        1.0,1.0,
        -1.0,-1.0,

        3.0*(2.0f64).sqrt(),
        1.0*(2.0f64).sqrt(),1.0*(2.0f64).sqrt(),
        3.0*(2.0f64).sqrt(),
    ];
    let g : &[f64] = &[
        1.0,
        0.0,

        1.0*(2.0f64).sqrt(),
        0.0,
        1.0*(2.0f64).sqrt(),
    ];

    let symmat_nnz  = &[2,2];
    let symmat_dim  = &[2,2];
    let symmat_subi = &[0,1, 1,1];
    let symmat_subj = &[0,1, 0,1];
    let symmat_val  = &[1.0,1.0, 1.0,1.0];

    mskapi.task()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {

                let barx0 = 0;

                let dom_rplus = task.get_domain_rplus()?;
                let dom_svecpsd2 = task.get_domain_svecpsd_cone(3)?;

                /* Append 'NUMAFE' empty affine expressions. */

                task.append_rows(5)?;

                // Append 'NUMVAR' scalar variables.
                // The variables will initially be fixed at zero (x=0).
                task.append_vars(NUMVAR)?;

                task.put_var_bound_slice_value(0,NUMVAR,f64::NEG_INFINITY,f64::INFINITY)?;

                /* Append 'NUMBARVAR' semidefinite variables. */

                task.append_barvars(DIMBARVAR)?;

                /* Input row data and symmetric matrixes */

                task.put_row_slice(0, rowlen, subj, val)?;
                task.put_row_slice_g(0, g)?;

                task.append_symmats(symmat_dim, symmat_nnz, symmat_subi, symmat_subj, symmat_val)?;
                task.put_bar_entry(0, barx0, &[0], &[1.0])?;
                task.put_bar_entry(1, barx0, &[1], &[1.0])?;

                /* Set the objective */
                task.put_obj_row(0)?;
                task.put_obj_sense(msk::ObjSense::MINIMIZE);

                /* Add linear constraint */
                task.append_con(dom_rplus, &[1], None)?;

                /* Add PSD constraint */
                task.append_con(dom_svecpsd2, &[2,3,4], None)?;

                let solidx = 0;

                /* Run optimizer */
                let trmcode = task.optimize()?;
                // Print a summary containing information
                // about the solution for debugging purposes
                task.solution_summary(msk::StreamType::MSG)?;

                let (psolsta,dsolsta) = task.get_sol_status(solidx)?;

                match psolsta {
                    msk::SolSta::OPTIMAL => {
                        let mut xx = vec![0.0; NUMVAR as usize];
                        let mut barx = vec![0.0; LENBARVAR[0] as usize];

                        task.get_sol_xx_slice(solidx,0,&mut xx)?;
                        task.get_sol_barxj(solidx,barx0,&mut barx)?;

                        println!("Optimal primal solution:\n\txx = {:?}\n\tbarx = {:?}",xx,barx);
                    },
                    msk::SolSta::INFEAS_CERT => println!("Dual infeasibility certificate found."),
                    msk::SolSta::UNDEFINED =>
                        match dsolsta {
                            msk::SolSta::INFEAS_CERT => println!("Primal infeasibility certificate found."),
                            msk::SolSta::ILLPOSED_CERT => println!("Primal certificate of illposedness found."),
                            _ => {
                                println!("The solution status is unknown.");
                                println!("The optimizer terminitated with code: {}\n", mskapi.get_trm_name(trmcode));
                            }
                        },
                    msk::SolSta::UNKNOWN => {
                        println!("The solution status is unknown.");
                        println!("The optimizer terminitated with code: {}", mskapi.get_trm_name(trmcode));
                    },
                    _ => panic!("Other solution status.")
                }

                Ok(())
        })
}


fn main() {
    sdo_lmi().unwrap();
}
