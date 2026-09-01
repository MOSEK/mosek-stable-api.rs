//!
//!   Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//!   File:      gp1.rs
//!
//!   Purpose:   Demonstrates how to solve a simple Geometric Program (GP)
//!              cast into conic form with exponential cones and log-sum-exp.
//!
//!              Example from
//!                https://gpkit.readthedocs.io/en/latest/examples.html//maximizing-the-volume-of-a-box
//!
//!   Problem:
//!   ```
//!   maximize     h*w*d
//!   subjecto to  2*(h*w + h*d) <= Awall
//!                w*d <= Afloor
//!                alpha <= h/w <= beta
//!                gamma <= d/w <= delta
//!
//!                h,w,d >= 0
//!   ```
//!              Perform variable substitutions:  h = exp(x), w = exp(y), d = exp(z).
//!   ```
//!   maximize     x+y+z
//!   subject      log( exp(x+y+log(2/Awall)) + exp(x+z+log(2/Awall)) ) <= 0
//!                                y+z <= log(Afloor)
//!                log( alpha ) <= x-y <= log( beta )
//!                log( gamma ) <= z-y <= log( delta )
//!
//!                x,y,z free
//!   ```
//!              We put this in conic form:
//!   ```
//!   maximize     x+y+z
//!   subject      (1) (u1,1,x+y+log(2/Awall)) in K_exp
//!                (2) (u2,1,x+z+log(2/Awall)) in K_exp
//!                (3)                 u1+u2 <= 1
//!                (4)                 y+z <= log(Afloor)
//!                (5) log( alpha ) <= x-y <= log( beta )
//!                (6) log( gamma ) <= z-y <= log( delta )
//!
//!                x,y,z,u1,u2 free
//!   ```


use mosek_stable_api as msk;

#[allow(non_snake_case)]
fn max_volume_box(
    Aw    : f64,
    Af    : f64,
    alpha : f64,
    beta  : f64,
    gamma : f64,
    delta : f64,
    hwd : &mut [f64]) -> Result<(),msk::APIError>
{
    // Basic dimensions of our problem
    const NUMVAR : i32 = 3;  // Variables in original problem
    const NUMROW : i64 = 10;

    // Linear part of the problem involving x, y, z

    let rowlen : &[i32] = &[
        3, // objective: x+y+z
        2, // y+z
        2, // x-y
        1, // z-y
        1, // u1
        2, // u2
        2, // x+y+log(2/Awall)
        2, // x+z+log(2/Awall)
        2, // u1+u2
        0, // 1.0
    ];
    let subj : &[i32] = &[
         /* objective */
        0,1,2,
        /* linear constraints */
        1,2,
        0,1,
        2,1,
        /* conic constraints */
        3,
        4,
        0,1,
        0,2,
        3,4,
    ];
    let val : &[f64] = &[
        1.0,1.0,1.0,

        1.0,1.0,
        1.0,-1.0,
        1.0,-1.0,

        1.0,
        1.0,
        1.0,1.0,
        1.0,1.0,
        1.0,1.0,
    ];
    let rowg : &[f64] = &[
       0.0,        // objective: x+y+z
       0.0,        // y+z
       0.0,        // x-y
       0.0,        // z-y
       0.0,        // u1
       0.0,        // u2
       (2.0/Aw).log10(),  // x+y+log(2/Awall)
       (2.0/Aw).log10(),  // x+z+log(2/Awall)
       0.0,        // u1+u2
       1.0         // 1.0
    ];

    let u1_u2_idx  = &[ 8 ];
    let u1_u2_rhs  = &[ 1.0 ];
    let exp1_idx   = &[ 4,9,6 ];
    let exp1_rhs   = &[ 0.0,0.0,0.0 ];
    let exp2_idx   = &[ 5,9,7 ];
    let exp2_rhs   = &[ 0.0,0.0,0.0 ];


    let mskapi = msk::initialize()?;
    mskapi.task()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {
                let dom_rzero  = task.get_domain_rzero()?;
                let dom_rplus  = task.get_domain_rplus()?;
                let dom_rminus = task.get_domain_rminus()?;
                let dom_pexp   = task.get_domain_primal_exponential_cone()?;

                task.append_vars(NUMVAR+2)?;
                task.put_var_bound_slice_value(0, NUMVAR+2, f64::NEG_INFINITY, f64::INFINITY)?;
                task.append_rows(NUMROW)?;
                task.put_row_slice(0, rowlen, subj, val)?;
                task.put_row_slice_g(0, rowg)?;

                // Objective is the sum of three first variables
                task.put_obj_sense(msk::ObjSense::MAXIMIZE);

                task.put_obj_row(0)?;

                // Add the linear constraints
                {
                    let rhs    = &[ Af.log10(),alpha.log10(),beta.log10(),gamma.log10(),delta.log10() ];
                    let domidx = &[ dom_rminus, dom_rplus, dom_rminus, dom_rplus, dom_rminus ];
                    let rowidx = &[ 1,2,2,3,3 ];

                    task.append_cons(domidx, rowidx, Some(rhs))?;
                }

                // Add conic constraints
                // The constraint u1+u2 ∈ ZERO+1 is added also as an ACC */

                task.append_con(dom_rzero, u1_u2_idx, Some(u1_u2_rhs))?;
                /* (u1, 1, x+y+log(2/Awall)) \in EXP */

                task.append_con(dom_pexp, exp1_idx, Some(exp1_rhs))?;
                /* (u2, 1, x+z+log(2/Awall)) \in EXP */

                task.append_con(dom_pexp, exp2_idx, Some(exp2_rhs))?;

                // Solve and map to original h, w, d
                let trmcode = task.optimize()?;

                let solidx = 0;

                let (psolsta,dsolsta) = task.get_sol_status(solidx)?;

                match (psolsta,dsolsta) {
                    (msk::SolSta::OPTIMAL,msk::SolSta::OPTIMAL) => {
                        let mut xyz = vec![0.0; NUMVAR as usize];
                        task.get_sol_xx_slice(solidx,0,&mut xyz)?;

                        for (t,&s) in hwd.iter_mut().zip(xyz.iter()) { *t = s.exp(); }
                    },
                    _ => panic!("Solution not optimal, termination code {}.\n", mskapi.get_trm_name(trmcode))
                }
                Ok(())
            })
}


#[allow(non_snake_case)]
fn gp1()
{
    let Aw    : f64 = 200.0;
    let Af    : f64 = 50.0;
    let alpha : f64 = 2.0;
    let beta  : f64 = 10.0;
    let gamma : f64 = 2.0;
    let delta : f64 = 10.0;
    let mut hwd = vec![0.0;3];

    max_volume_box(Aw, Af, alpha, beta, gamma, delta, &mut hwd).unwrap();

    println!("Solution h={:.4} w={:.4} d={:.4}\n", hwd[0], hwd[1], hwd[2]);
}


fn main() {
    gp1();
}

#[cfg(test)]
mod test {
    #[test]
    fn test() { super::main(); }
}
