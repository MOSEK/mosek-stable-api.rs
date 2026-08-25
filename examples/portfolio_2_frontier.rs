//!
//!  Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//! Description :  Implements a basic portfolio optimization model.
//!                Determines points on the efficient frontier.
//!
//! The problem can be formualted on conic form, where the points on the efficient frontier curve are parameterized by ɑᵢ:
//! ```
//! min  μ'x - ɑᵢ s
//! s.t. ∑xᵢ = 1
//!      ⎡  s  ⎤
//!      ⎢  2  ⎥ ∈ Qᵣ
//!      ⎣ G'x ⎦
//!      x > 0
//! ```

use mosek_stable_api::{self as msk};

const LOGLEVEL : i32 = 0;

fn portfolio() -> Result<(),msk::APIError> {
    let mskapi = msk::initialize()?;

    let n = 8;
    let mu = &[0.07197349, 0.15518171, 0.17535435, 0.0898094 , 0.42895777, 0.39291844, 0.32170722, 0.18378628];
    // GT must have size n rows
    let GT = &[
        0.30758, 0.12146, 0.11341, 0.11327, 0.17625, 0.11973, 0.10435, 0.10638,
        0.0,     0.25042, 0.09946, 0.09164, 0.06692, 0.08706, 0.09173, 0.08506,
        0.0,     0.0,     0.19914, 0.05867, 0.06453, 0.07367, 0.06468, 0.01914,
        0.0,     0.0,     0.0,     0.20876, 0.04933, 0.03651, 0.09381, 0.07742,
        0.0,     0.0,     0.0,     0.0,     0.36096, 0.12574, 0.10157, 0.0571 ,
        0.0,     0.0,     0.0,     0.0,     0.0,     0.21552, 0.05663, 0.06187,
        0.0,     0.0,     0.0,     0.0,     0.0,     0.0,     0.22514, 0.03327,
        0.0,     0.0,     0.0,     0.0,     0.0,     0.0,     0.0,     0.2202];

    let k      = GT.len()/n;
    let x0     = &[0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];
    let w      = 1.0;
    let alphas = &[0.0, 0.01, 0.1, 0.25, 0.30, 0.35, 0.4, 0.45, 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 10.0];
    let totalBudget : f64 = x0.iter().sum::<f64>() + w;

    //Offset of variables into the API variable.
    let numvar : i32 = n as i32 + 1;
    let voff_x : i32 = 0;
    let voff_s : i32 = n as i32;

    // Constraints offsets
    let numcon : i32 = 1;
    let coff_bud : i32 = 0;

    mskapi.task()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {
                /* Initial setup. */
                let dom_rzero = task.get_domain_rzero()?;
                let dom_rquad = task.get_domain_rotated_quadratic_cone(k as i64+2)?;

                // Holding variable x of length n
                // No other auxiliary variables are needed in this formulation
                task.append_vars(numvar)?;
                // Setting up variable x
                for j in 0..n {
                    // Optionally we can give the variables names
                    task.put_var_name(voff_x + j as i32, &format!("x[{}]",1+j))?;
                    // No short-selling - x^l = 0, x^u = inf
                    task.put_var_bound(voff_x + j as i32, 0.0, f64::INFINITY)?;
                }



                task.put_var_name(voff_s, "s")?;
                // No short-selling - x^l = 0, x^u = inf
                task.put_var_bound(voff_s, 0.0, f64::INFINITY)?;

                // One linear constraint: total budget
                task.append_rows(k as i64 + 4)?;

                let xidxs = &[0,1,2,3,4,5,6,7];
                //const double ones[]   = { 1,1,1,1,1,1,1,1 };

                // Set objective

                task.put_row(0, xidxs, mu)?;
                task.put_obj_row(0)?;
                task.put_obj_sense(msk::ObjSense::MAXIMIZE);

                // Set budget constraint
                task.put_row(1, xidxs, &[1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0])?;
                task.append_con(dom_rzero,  &[1], Some(&[totalBudget]));

                task.put_con_name(0, "budget")?;

                // Input (2,s, G'x) in the AFE (affine expression) storage
                // We need k+2 rows
                // The first affine expression = gamma
                task.put_row_g(2,2.0)?;
                task.put_ijc(3,voff_s,1.0)?;

                // The remaining k expressions comprise GT*x, we add them row by row
                // In more realisic scenarios it would be better to extract nonzeros and input in sparse form
                for (i,gtrow) in GT.chunks(n).enumerate() {
                    task.put_row(i as i64+4, xidxs, gtrow)?;
                }

                task.append_con(dom_rquad, &[2,3, 4,5,6,7, 8,9,10,11],None)?;
                task.put_con_name(1, "risk")?;

                /* Set the log level */
                task.put_int_param("ipar_log", LOGLEVEL);

                let mut xs = vec![0.0; n+1];
                let mut expret = vec![0.0; alphas.len()];
                let mut stddev = vec![0.0; alphas.len()];

                for (i,alpha) in alphas.iter().enumerate() {
                    println!("LOOP {}, alpha = {:.2}",i,alpha);

                    /* Sets the objective function coefficient for s. */
                    task.put_ijc(0, voff_s + 0, -alpha)?;
                    let trmcode = task.optimize()?;

                    // Check if the interior point solution is an optimal point
                    let (psolsta,dsolsta) = task.get_sol_status(0)?;

                    task.get_sol_xx_slice(0,0,&mut xs)?;

                    match psolsta {
                        msk::SolSta::OPTIMAL => {
                            stddev[i] = xs[voff_s as usize];
                            expret[i] = mu.iter().zip(xs.iter()).map(|(&m,&x)| m*x).sum();
                        },
                        _ => println!("An error occurred when solving for alpha={:e}", alpha)
                    }
                }

                //println!("%-12s  %-12s  %-12s\n", "alpha", "exp ret", "std. dev");
                println!("{:<12}  {:<12}  {:<12}", "alpha", "exp ret", "std. dev");
                for ((alpha,expret),stddev) in alphas.iter().zip(expret.iter()).zip(stddev.iter()) {
                    println!("{:<12.3e}  {:<12.3e}  {:<12.3e}", alpha, expret, stddev.sqrt());
                }
                //for (int i = 0; i < numalpha; ++i) {
                //    //println!("%-12.3e  %-12.3e  %-12.3e\n", alphas[i], expret[i], sqrt(stddev[i]));
                //    println!("%-12.3e  %-12.3e  %-12.3e\n", alphas[i], expret[i], sqrt(stddev[i]));
                // }
                Ok(())
            })
}

fn main() {
    portfolio().unwrap()
}
