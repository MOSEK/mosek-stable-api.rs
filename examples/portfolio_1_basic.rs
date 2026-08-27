//!
//! Copyright: Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//! File: portfolio_1_basic.rs
//!
//! # Description
//!
//! Implements a basic portfolio optimization model: Maximize expected profit for a portfolio subject to a bound on the
//! risk. The risk of a portfolio `x` is measured as `x'Qx` for a covariance matrix `Q`. `W` is the total wealth we have
//! to invest.
//!
//! # Problem
//!
//! ```
//! min  μ'x
//! s.t. ∑xᵢ = W
//!      x'Qx <= ɣ
//!      x < 0
//! ```
//!
//! `Q` must be summetric positive semidefinite and can this be factored into
//! `Q=GG'`, with `G` being a lower triangular matrix.
//!
//! The problem can now be formualted on conic form:
//! ```
//! min  μ'x
//! s.t. ∑xᵢ = W
//!      |G'x|_2 <= ɣ
//!      x > 0
//! ```

use mosek_stable_api as moco;

fn portfolio() -> Result<(),moco::APIError>
{
    let msk = moco::initialize()?;

    let n : i32     = 8;
    let gamma : f64 = 36.0;
    let mu : &[f64] = &[ 0.07197349, 0.15518171, 0.17535435, 0.0898094 , 0.42895777, 0.39291844, 0.32170722, 0.18378628 ];
    // GT must have size n rows
    #[allow(non_snake_case)]
    let GT : &[&[f64]] = &[
        &[0.30758, 0.12146, 0.11341, 0.11327, 0.17625, 0.11973, 0.10435, 0.10638],
        &[0.0,     0.25042, 0.09946, 0.09164, 0.06692, 0.08706, 0.09173, 0.08506],
        &[0.0,     0.0,     0.19914, 0.05867, 0.06453, 0.07367, 0.06468, 0.01914],
        &[0.0,     0.0,     0.0,     0.20876, 0.04933, 0.03651, 0.09381, 0.07742],
        &[0.0,     0.0,     0.0,     0.0,     0.36096, 0.12574, 0.10157, 0.0571 ],
        &[0.0,     0.0,     0.0,     0.0,     0.0,     0.21552, 0.05663, 0.06187],
        &[0.0,     0.0,     0.0,     0.0,     0.0,     0.0,     0.22514, 0.03327],
        &[0.0,     0.0,     0.0,     0.0,     0.0,     0.0,     0.0,     0.2202 ] ];

    let k : usize   = GT.len();
    let x0 : &[f64] = &[8.0, 5.0, 3.0, 5.0, 2.0, 9.0, 3.0, 6.0];
    let w           = 59.0;
    let total_budget : f64 = x0.iter().sum::<f64>() + w;

    // Offset of variables into the API variable.
    let numvar : i32 = n;
    let voff_x : i32 = 0;

    // Constraints offsets

    msk.task()?
        /* Directs the log task stream to the printer function. */
        .with_stream_callback(
            moco::StreamType::MSG,
            |msg| print!("{0}",msg),
            |task| {

            let dom_rzero = task.get_domain_rzero()?;
            let dom_quad = task.get_domain_quadratic_cone((k+1) as i64)?;

            // Holding variable x of length n
            // No other auxiliary variables are needed in this formulation
            task.append_vars(numvar)?;

            // Setting up variable x
            for j in 0..n {
                /* Optionally we can give the variables names */
                task.put_var_name(voff_x + j, format!("x[{}]",1+j).as_str())?;
                /* No short-selling - x^l = 0, x^u = inf */
                task.put_var_bound(voff_x + j, 0.0, f64::INFINITY)?;
            }

            // One linear constraint: total budget
            task.append_rows((k+3) as i64)?;

            let xidxs : &[i32] = &[ 0,1,2,3,4,5,6,7 ];
            let ones  : &[f64] = &[ 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0 ];

            // Set objective
            task.put_row(0, xidxs, mu)?;
            task.put_obj_row(0)?;
            task.put_obj_sense(moco::ObjSense::MAXIMIZE);

            // Set budget constraint
            task.put_row(1, xidxs, ones)?;
            task.append_con(dom_rzero, &[1], Some(&[total_budget]))?;

            task.put_con_name(0, "budget")?;

            // Input (gamma, GTx) in the AFE (affine expression) storage
            // We need k+1 rows
            // The first affine expression = gamma
            task.put_row_g(2, gamma)?;
            // The remaining k expressions comprise GT*x, we add them row by row
            // In more realisic scenarios it would be better to extract nonzeros and input in sparse form
            for i in 0..k {
                task.put_row((i+3) as i64, xidxs, GT[i])?;
            }

            task.append_con(dom_quad, &[ 2,3,4,5,6,7,8,9,10 ], None)?;
            task.put_con_name(1, "risk")?;

            let _trmcode = task.optimize()?;

            /* Display the solution summary for quick inspection of results. */
            task.solution_summary(moco::StreamType::MSG)?;

            // Check if the interior point solution is an optimal point
            let solidx = 0;
            match task.get_sol_status(solidx)? {
                (moco::SolSta::OPTIMAL,_) => {
                    /* Read the x variables one by one and compute expected return. */
                    /* Can also be obtained as value of the objective. */
                    let mut xx = vec![0.0; n as usize];
                    task.get_sol_xx_slice(solidx,0,&mut xx)?;
                    let expret : f64 = mu.iter().zip(xx.iter()).map(|(a,b)| a*b).sum();

                    /* Read the value of s. This should be gamma. */
                    println!("\nExpected return {:.4} for gamma {:.2}\n", expret, gamma);
                },
                (psolsta,_) => {
                    panic!("Unexpected solution status: {:?}", psolsta);
                }
            }

            Ok(())
        })
}

fn main() {
    portfolio().unwrap();
}
