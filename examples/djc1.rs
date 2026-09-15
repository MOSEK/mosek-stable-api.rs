//!
//! Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//! File: djc1.rs
//!
//! Purpose: Demonstrates how to solve the problem with two disjunctions:
//! ```
//!      minimize    2 x0 + x1 + 3x2 + x3
//!      subject to   x0 + x1 + x2 + x3 >= -10
//!                  (x0-2 x1<=-1 and x2=x3=0) or (x2-3x3<=-2 and x1=x2=0)
//!                  x0=2.5 or x1=2.5 or x2=2.5 or x3=2.5
//! ```

use mosek_stable_api as msk;

fn djc1() -> Result<(),msk::APIError> {
    msk::initialize_with_defaults()?;

    let numvar : i32 = 4;
    let rowlen : &[i32] = &[ 4,4,2,1,1,2,1,1,1,1,1,1 ];

    let subj : &[i32] = &[
        0,1,2,3,
        0,1,2,3,
        0,1,
            2,
              3,
            2,3,
        0,
          1,
        0,
          1,
            2,
              3 ];
    let val : &[f64] = &[
        2.0,1.0,3.0,1.0,
        1.0,1.0,1.0,1.0,
        1.0,-2.0,
        1.0,
        1.0,
        1.0,-3.0,
        1.0,
        1.0,
        1.0,
        1.0,
        1.0,
        1.0 ];

    /* Create the optimization task. */
    msk::Task::new()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {
                // Append free variables
                task.append_vars(numvar)?;
                task.put_var_bound_slice_value(0,numvar, f64::NEG_INFINITY,f64::INFINITY)?;

                task.append_rows(12)?;
                task.put_row_slice(0,rowlen,subj,val)?;

                task.put_obj_row(0)?;
                task.put_obj_sense(msk::ObjSense::MINIMIZE);

                let dom_rplus  = task.get_domain_rplus()?;
                let dom_rminus = task.get_domain_rminus()?;
                let dom_rzero  = task.get_domain_rzero()?;

                let rowidx = &[ 1,2,3,4,5,6,7,8,9,10,11 ];
                let domidx = &[ dom_rplus,
                                dom_rminus, dom_rzero, dom_rzero,
                                dom_rminus, dom_rzero, dom_rzero,
                                dom_rzero,
                                dom_rzero,
                                dom_rzero,
                                dom_rzero ];
                let rhs    = &[ -10.0,
                                -1.0,0.0,0.0,
                                -2.0,0.0,0.0,
                                 2.5,
                                 2.5,
                                 2.5,
                                 2.5 ];
                let termsize = &[ 3,3,1,1,1,1 ];

                task.append_con(domidx[0], &rowidx[0..1], Some(&rhs[0..1]))?;

                //task.append_djc(6, 6, 2, domidx+1, termsize, rowidx+1, rhs+1);
                task.append_djc(&domidx[1..7], &termsize[0..2], &rowidx[1..7], &rhs[1..7])?;
                task.append_djc(&domidx[7..], &termsize[2..], &rowidx[7..], &rhs[7..])?;

                // Solve the problem
                let _trmcode = task.optimize()?;

                /* Print a summary containing information
                    about the solution for debugging purposes. */
                task.solution_summary(msk::StreamType::LOG)?;

                let solidx = 0;
                let (psolsta,_dsolsta) = task.get_sol_status(solidx)?;
                match psolsta {
                    msk::SolSta::OPTIMAL|msk::SolSta::INTEGER_OPTIMAL => {
                        let mut xx = vec![0.0;numvar as usize];
                        task.get_sol_xx_slice(solidx,0,&mut xx)?;
                        println!("Optimal primal solution: {:?}",xx);
                    }
                    _ => println!("Another solution status.")
                }
                Ok(())
            })
}

fn main() {
    djc1().unwrap();
}

#[cfg(test)]
mod test {
    #[test]
    fn test() { super::main(); }
}
