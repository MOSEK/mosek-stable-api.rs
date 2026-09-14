//!
//!  Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//!  File :      sdo2.rs
//!
//!  Purpose :   Solves the semidefinite problem with two symmetric variables:
//!  ```
//!   min   <C1,X1> + <C2,X2>
//!   st.   <A1,X1> + <A2,X2> = b
//!               (X2)_{1,2} <= k
//!  ```
//!  where X1, X2 are symmetric positive semidefinite,
//!  C1, C2, A1, A2 are assumed to be constant symmetric matrices,
//!  and b, k are constants.
//!

use mosek_stable_api as msk;

fn sdo2() -> Result<(),msk::APIError> {
    let mskapi = msk::initialize_with_defaults()?;

    mskapi.task()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {
                /* Input data */
                /* Dimension of semidefinite variables */
                let dimbarvar = &[3, 4];

                /* Matrixes */
                let mx_nnz = &[2, 4, 3, 3, 1];
                let mx_dim = &[3, 4, 3, 4, 4];

                let mx_subk = &[
                    0, 2,
                    0, 1, 1, 2,
                    0, 2, 2,
                    1, 1, 3,
                    1]; /* Which entry (k,l)->v */
                let mx_subl = &[
                    0, 2,
                    0, 0, 1, 2,
                    0, 0, 2,
                    0, 1, 3,
                    0];
                let mx_val = &[
                    1.0, 6.0,
                    1.0, -3.0, 2.0, 1.0,
                    1.0, 1.0, 2.0,
                    1.0, -1.0, -3.0,
                    0.5];

                /* Constraint bounds and values */
                let bc = &[23.0, -3.0];

                let dom_rzero = task.get_domain_rzero()?;
                let dom_rminus = task.get_domain_rminus()?;

                // Append semidefinite variables.
                task.append_barvars(dimbarvar)?;
                task.append_rows(3)?;

                // Append empty constraints.
                // The constraints will initially have no bounds.
                task.append_con(dom_rzero, &[1], Some(&bc[0..1]))?;
                task.append_con(dom_rminus, &[2], Some(&bc[1..2]))?;
                task.put_obj_row(0)?;
                task.put_obj_sense(msk::ObjSense::MINIMIZE);

                /* Append all symmetric matrixes */
                task.append_symmats(mx_dim, mx_nnz, mx_subk, mx_subl, mx_val)?;

                // Add all semidefinite entries
                task.put_bar_entry_list(
                    &[0, 0, 1, 1, 2],            // rowidx
                    &[0, 1, 0, 1, 1],            // barj
                    &[1, 1, 1, 1, 1],            // numw
                    &[0, 1, 2, 3, 4],            // midx
                    &[1.0, 1.0, 1.0, 1.0, 1.0])?; // mwgt


                // Run optimizer
                let trmcode = task.optimize()?;
                let solidx = 0;

                task.solution_summary(msk::StreamType::MSG)?;

                let (psolsta, dsolsta) = task.get_sol_status(solidx)?;

                match psolsta {
                    msk::SolSta::OPTIMAL => {
                        /* Retrieve the soution for all symmetric variables */
                        println!("Solution (lower triangular part vectorized):");

                        for (i,&d) in dimbarvar.iter().enumerate() {
                            let mut barx = vec![0.0;(d*(d+1) / 2) as usize];
                            task.get_sol_barxj(solidx, i as i32, &mut barx)?;
                            println!("X[{}]:",i);
                            for ((i,j),x) in (0..d).flat_map(|j| (j..d).zip(std::iter::repeat(j))).zip(barx.iter()) {
                                println!("\t[{},{}] = {}",i,j,x);
                            }
                        }
                    }
                    msk::SolSta::INFEAS_CERT => println!("Dual infeasibility certificate found."),
                    msk::SolSta::ILLPOSED_CERT => println!("Dual illposedness certificate found."),
                    msk::SolSta::UNKNOWN|msk::SolSta::UNDEFINED =>
                        match dsolsta {
                            msk::SolSta::INFEAS_CERT => println!("Primal infeasibility certificate found."),
                            msk::SolSta::ILLPOSED_CERT => println!("Primal illposedness certificate found."),
                            _ => println!("The status of the solution could not be determined. Termination code: {}.",
                                         mskapi.get_trm_name(trmcode)),
                        },
                    _ => panic!("Other solution status.")
                }
                Ok(())
            })

}


fn main() {
    sdo2().unwrap();
}

#[cfg(test)]
mod test {
    #[test]
    fn test() { super::main(); }
}
