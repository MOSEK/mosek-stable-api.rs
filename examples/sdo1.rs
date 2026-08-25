//!
//!   Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//!   File:      sdo1.rs
//!
//!   Purpose:   Solves the following small semidefinite optimization problem
//!              using the MOSEK API.
//!
//!     minimize       ⎡ 2 1 0 ⎤
//!                 Tr ⎢ 1 2 1 ⎥ * X + x0
//!                    ⎣ 0 1 2 ⎦
//!
//!     subject to     ⎡ 1 0 0 ⎤
//!                 Tr ⎢ 0 1 0 ⎥ * X + x0 = 1
//!                    ⎣ 0 0 1 ⎦
//!
//!                    ⎡ 1 1 1 ⎤
//!                 Tr ⎢ 1 1 1 ⎥ * X + x1 + x2 = 0.5
//!                    ⎣ 1 1 1 ⎦
//!                 (x0,x1,x2) ∈ Q,  X ≽ 0
//!

use mosek_stable_api as msk;

fn sdo1() -> Result<(),msk::APIError> {
    let mskapi = msk::initialize()?;

    const LENBARVAR : &[i32] = &[3 * (3 + 1) / 2]; /* Number of scalar SD variables  */
    const NUMVAR : i32 = 3;
    const NUMROW : i64 = 6;

    let subj = &[0, 0, 1, 2, 0, 1, 2];
    let cof = &[1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0];
    let rowlen = &[1, 1, 2, 1, 1, 1];

    let rhsc = &[1.0, 0.5];

    let barc_i = &[0, 1, 1, 2, 2];
    let barc_j = &[0, 0, 1, 1, 2];
    let barc_v = &[2.0, 1.0, 2.0, 1.0, 2.0];

    let aptrb = &[0, 1];
    let aptre = &[1, 3];
    let asub = &[0, 1, 2];
    /* column subscripts of A */
    let aval = &[1.0, 1.0, 1.0];

    let bara_i = &[0, 1, 2, 0, 1, 2, 1, 2, 2];
    let bara_j = &[0, 1, 2, 0, 0, 0, 1, 1, 2];
    let bara_v = &[1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0];

    let conesub = &[0, 1, 2];
    let afeidx = &[0, 1, 2];
    let varidx = &[0, 1, 2];
    let f_val = &[1, 1, 1];

    mskapi.task()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {
                /* Append empty rows. */
                task.append_rows(NUMROW)?;

                task.put_obj_row(0)?;
                task.put_obj_sense(msk::ObjSense::MINIMIZE);

                // Append 'NUMVAR' variables.
                // The variables will initially be fixed at zero (x=0).
                task.append_vars(NUMVAR)?;

                task.put_var_bound_slice_value(0, NUMVAR, f64::NEG_INFINITY, f64::INFINITY)?;

                task.put_row_slice(0, rowlen, subj, cof)?;

                /* Append 'NUMBARVAR' semidefinite variables. */
                const DIMBARVAR : &[i32] = &[3]; /* Dimension of semidefinite cone */
                task.append_barvars(DIMBARVAR)?;

                /* Set the linear term barc_j in the objective.*/
                let midx = task.get_num_symmat();
                task.append_symmat(DIMBARVAR[0], barc_i, barc_j, barc_v)?;

                task.put_bar_entry(0, 0, &[midx], &[1.0])?;

                let dom_rzero = task.get_domain_rzero()?;

                // Set the bounds on constraints.
                // for i=1, ...,NUMCON : blc[i] <= constraint i <= buc[i]


                task.append_cons(
                    &[dom_rzero,dom_rzero],
                    &[1,1],
                    &[1,2],
                    Some(rhsc))?;

                /* Append the affine conic constraint with quadratic cone */
                let dom_quad3 = task.get_domain_quadratic_cone(3)?;
                task.append_con(dom_quad3, &[3,4,5], None)?;

                /* Add the first row of barA */
                let midx = task.get_num_symmat();
                task.append_symmat(DIMBARVAR[0], &bara_i[0..3], &bara_j[0..3], &bara_v[0..3])?;

                task.put_bar_entry(1, 0, &[midx], &[1.0])?;

                /* Add the second row of barA */
                task.append_symmat(DIMBARVAR[0], &bara_i[3..9], &bara_j[3..9], &bara_v[3..9])?;
                task.put_bar_entry(2, 0, &[midx+1], &[1.0])?;

                task.write_task_to_file("sdo1.ptf")?;
                /* Run optimizer */
                let trmcode = task.optimize()?;

                /* Print a summary containing information about the solution for debugging purposes*/
                task.solution_summary(msk::StreamType::MSG)?;

                let solidx = 0;

                let (psolsta,dsolsta) = task.get_sol_status(solidx)?;

                match psolsta {
                    msk::SolSta::OPTIMAL => {
                        let mut xx = vec![0.0; NUMVAR as usize];
                        let mut barx = vec![0.0; LENBARVAR[0] as usize];

                        task.get_sol_xx_slice(solidx, 0, &mut xx)?;
                        task.get_sol_barxj(solidx, 0, &mut barx)?;

                        println!("Optimal primal solution:\n\txx = {:?}\n\tbarx = {:?}",xx,barx);
                    }
                    msk::SolSta::INFEAS_CERT => println!("Dual infeasibility certificate found."),
                    msk::SolSta::UNDEFINED|msk::SolSta::UNKNOWN =>
                        match dsolsta {
                            msk::SolSta::INFEAS_CERT => println!("Primal infeasibility certificate found."),
                            msk::SolSta::ILLPOSED_CERT => println!("Primal certificate of illposedness found."),
                            _ => {
                                println!("The solution status is unknown.");
                                println!("The optimizer terminitated with code: {}\n", mskapi.get_trm_name(trmcode));
                            }
                        },
                    _ => println!("Other solution status.")
                }

                Ok(())
            })
}

fn main() {
    sdo1().unwrap();
}
