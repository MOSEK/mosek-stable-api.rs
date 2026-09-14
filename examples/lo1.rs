//!
//!  Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//!  File: `lo1.rs`
//!

extern crate mosek_stable_api;
use mosek_stable_api as moco;

fn main() {
    lo1().unwrap();
}

fn lo1() -> Result<(),moco::APIError> {
    let msk = moco::initialize_with_defaults()?;
    // All the normal lo1 data:
    const NUMVAR : i32 = 4;
    const _NUMCON : i64 = 3;
    let cj : &[i32] = &[0,   1,   2,   3];
    let c  : &[f64] = &[3.0, 1.0, 5.0, 1.0];
    let rownum : &[i32]  = &[3, 4, 2 ];
    let subj : &[i32]    = &[0, 1, 2,
                             0, 1, 2, 3,
                                1,    3 ];
    let valj : &[f64] = &[3.0, 1.0, 2.0,
                          2.0, 1.0, 3.0, 1.0,
                               2.0,      3.0 ];
    let blc : &[f64]  = &[30.0, 15.0,          f64::NEG_INFINITY ];
    let buc : &[f64]  = &[30.0, f64::INFINITY, 25.0 ];
    let blx : &[f64]  = &[ 0.0,           0.0,            0.0,           0.0 ];
    let bux : &[f64]  = &[ f64::INFINITY, 10.0, f64::INFINITY, f64::INFINITY ];
    // implementation
    msk.task()?
        .with_stream_callback(
            moco::StreamType::MSG,
            |msg| print!("{0}",msg),
            |task : &mut moco::Task| {
                task.append_vars(NUMVAR)?;
                task.append_rows(4)?;
                task.put_var_bound_slice(0,blx,bux)?;
                task.put_row_slice(1,rownum, subj, valj)?;
                task.append_vars(NUMVAR)?;
                task.append_rows(4)?;
                task.put_var_bound_slice(0,blx,bux)?;
                task.put_row_slice(1,rownum,subj,valj)?;
                //int64_t domain_idx;
                for (i,(&bl,&bu)) in blc.iter().zip(buc.iter()).enumerate() {
                    if bl.is_finite() {
                        let domidx = task.get_domain_rplus()?;
                        task.append_con(domidx,&[(i+1) as i64],Some(&[bl]))?;
                    }
                    if bu.is_finite() {
                        let domidx = task.get_domain_rminus()?;
                        task.append_con(domidx,&[(i+1) as i64],Some(&[bu]))?;
                    }
                }
                task.put_obj_sense(moco::ObjSense::MAXIMIZE);
                task.put_row(0, cj, c)?;
                task.put_obj_row(0)?;

                let _trmcode = task.optimize()?;

                task.solution_summary(moco::StreamType::MSG)?;

                const SOLIDX : i32 = 0;
                match task.get_sol_status(SOLIDX)? {
                    (moco::SolSta::OPTIMAL,_) => {
                        let mut xx = vec![0.0; NUMVAR as usize]; task.get_sol_xx_slice(SOLIDX, 0,&mut xx)?;
                        println!("xx: {:?}\n", xx);
                    },
                    (moco::SolSta::INFEAS_CERT,_)|(_,moco::SolSta::INFEAS_CERT) => {
                        println!("Primal or dual infeasibility certificate found.");
                    },
                    (moco::SolSta::ILLPOSED_CERT,_)|(_,moco::SolSta::ILLPOSED_CERT) => {
                        println!("Primal or dual illposed certificate found.");
                    },
                    _ => {
                        panic!("Other solution status");
                    }
                }

                Ok(())
            })
}

#[cfg(test)]
mod test {
    #[test]
    fn test() { super::main(); }
}
