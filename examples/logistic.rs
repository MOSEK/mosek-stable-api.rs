//!
//! Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//! File: `logistic.rs`
//!
//!
//! Purpose: Implements logistic regression with regulatization.
//!
//!          Demonstrates using the exponential cone and log-sum-exp in Optimizer API.

use mosek_stable_api::{self as moco, APIError};
use itertools::{izip,iproduct};

// Adds constraints for "soft plus",
// $$
//  t_i \\geq \\left\\{
//    \\begin{array}{l}
//      \\log\\left( 1 + e^{θ^T X_i)} \\right)\\ \\mathrm{if}\\ y_i \\\\
//      \\log\\left( 1 + e^{- θ^T X_i} \\right)\\ \\mathrm{if}\\ \\not y_i
//    \\end{array}
// \\right}
//
// $$
// We notice that
// $$
//   t > \\log\\left( 1+e^u \\right)
//   \\Leftrightarrow
//   1 > e^{-t} + e^{u-t}
// $$
// and, introducing auxiliary variable \\(z\\in\\mathbb{R}^2\\)
//     \\begin{array}{l}
//       z_1 > e^{-t}\\. \\Leftrightarrow\. (w_1,1,-t)\\in\\mathcal{C}_{\\mathbb{exp}} \\\\
//       z_2 > e^{u-t}\\. \\Leftrightarrow\. (w_2,1,u-t)\\in\\mathcal{C}_{\\mathbb{exp}}  \\\\
//       1 > z_1+z_2
//     \\end{array}
// $$
//
// # Arguments
// - `d`
// - `n`
// - `voff_theta` Offset of the θ variable subvector of dimension `d`
// - `voff_t` Offset ofthe t subvector of dimension `n`.
// - `X` Data for an `n×n×d` array interpreted as a `n×n` matrix of data points of dimension `d`.
// - `y` Data for a `n×n` of booleans.
//
#[allow(non_snake_case)]
fn soft_plus(task : & mut moco::Task, d : usize, voff_theta : i32, voff_t : i32, X : &[f64], y : &[bool]) -> Result<(), APIError>
{
    let n = X.len() / d;

    assert_eq!(n*d, X.len());
    assert_eq!(n, y.len());

    let var0 = task.get_num_var();

    // auxiliary variables
    let voff_z = var0;
    let numvar = voff_z + 2*n as i32;

    task.append_vars(2*n as i32)?;   // z1, z2
    task.put_var_bound_slice_value(voff_z, 2*n as i32, f64::NEG_INFINITY, f64::INFINITY)?;
    for (i,j) in (voff_z..).step_by(2).take(n).enumerate() {
        task.put_var_name(j, &format!("z1[{}]",i))?;
        task.put_var_name(j+1, &format!("z2[{}]",i))?;
    }

    let roff_0      = task.get_num_row();
    let roff_1      = roff_0;
    let roff_z1z2   = roff_1+1;
    let roff_z1     = roff_z1z2+n  as i64;
    let roff_z2     = roff_z1+n    as i64;
    let roff_minust = roff_z2+n    as i64;
    let roff_theta  = roff_minust+n as i64;
    let numrow = roff_theta+n as i64;

    task.append_rows(numrow)?;

    // Linear expressions
    task.put_row_slice(roff_z1z2,   &vec![2; n], &(voff_z..).take(2*n).collect::<Vec<i32>>(), &vec![1.0; n*2])?;
    task.put_row_slice(roff_z1,     &vec![1; n], &(voff_z..).step_by(2).take(n).collect::<Vec<i32>>(), &vec![1.0;n])?;
    task.put_row_slice(roff_z2,     &vec![1; n], &(voff_z+1..).step_by(2).take(n).collect::<Vec<i32>>(), &vec![1.0;n])?;
    task.put_row_slice(roff_minust, &vec![1; n], &(voff_t..).take(n).collect::<Vec<i32>>(), &vec![-1.0;n])?;
    task.put_row_slice(
        roff_theta,
        &vec![1+d as i32;n],
        &(voff_t..).take(n).flat_map(|ti| std::iter::once(ti).chain(voff_theta..).take(d+1)).collect::<Vec<i32>>(),
        &izip!(y.iter(),X.chunks(d))
            .flat_map(|(yi,Xi)|
                std::iter::once(-1.0).chain(Xi.iter().map(|Xij| if *yi { -Xij } else { *Xij })))
                .collect::<Vec<f64>>())?;

    task.put_row_g(roff_1, 1.0)?;

    let dom_rzero = task.get_domain_rzero()?;
    let dom_pexp  = task.get_domain_primal_exponential_cone()?;

    // Linear constraints 1 = z_1+x_2
        task.append_cons(&vec![dom_rzero; n], &vec![1; n], &(roff_z1z2..).take(n).collect::<Vec<i64>>(), Some(&vec![1.0; n]))?;
    // Cones: z_1 > e^{-t} <=> (z_1,1,-t) ∈ C_exp
    task.append_cons(&vec![dom_pexp; n], &vec![3;n], &interleave3(roff_z1..,std::iter::repeat(roff_1),roff_minust..).take(3*n).collect::<Vec<i64>>(),None  )?;
    // Cones: z_2 > e^{θ'x_i-t} <=> (z_2,1,θ'x_i-t) ∈ C_exp
    {
        let con_first = task.get_num_con();
        task.append_cons(&vec![dom_pexp; n], &vec![3;n], &interleave3(roff_z2..,std::iter::repeat(roff_1),roff_theta..).take(3*n).collect::<Vec<i64>>(),None)?;
        for (i,ci,yi) in izip!(0..,con_first..,y.iter()) {
            task.put_con_name(ci, &if *yi { format!("theta*X[{}]-t[{}]",i,i) } else { format!("-theta*X[{}]-t[{}]",i,i) }   )?;
        }
    }

    Ok(())
}

fn interleave3<I1,I2,I3,T>(i1 : I1, i2 : I2, i3 : I3) -> impl Iterator<Item=T>
    where
        I1 : Iterator<Item=T>,
        I2 : Iterator<Item=T>,
        I3 : Iterator<Item=T>
{
    use std::iter::once;
    izip!(i1,i2,i3).flat_map(|(v1,v2,v3)| once(v1).chain(once(v2).chain(once(v3))))
}

/// Model logistic regression (regularized with full 2-norm of theta)
///
///
/// - `X` n x d matrix of data points
/// - `y` length n vector classifying training points
/// - `lambda` regularization parameter
#[allow(non_snake_case)]
fn logistic_regression(d : usize,    // dimension
                       X : &[f64],
                       y : &[bool],
                       lambda : f64,
                       theta_val : &mut [f64]) -> Result<(),APIError>
{
    let msk = moco::initialize()?;
    let n = X.len()/d;
    assert_eq!(n*d,X.len());
    assert_eq!(n,y.len());
    assert_eq!(d,theta_val.len());

    let voff_r : i32 = 0;
    let voff_theta   = voff_r+1;
    let voff_t       = voff_theta+d as i32;
    let numvar       = voff_t+n as i32;

    let roff_obj : i64 = 0;
    let roff_reg = roff_obj + 1;
    let numrow = roff_reg + d as i64 + 1;

    msk.task()?
        /* Directs the log task stream to the printer function. */
        .with_stream_callback(
            moco::StreamType::MSG,
            |msg| print!("{0}",msg),
            |task| {
                // Variables [r; theta; t]
                task.append_vars(numvar)?;
                task.put_var_bound_slice_value(0, numvar, f64::NEG_INFINITY, f64::INFINITY)?;
                for (i,j) in (voff_t..).take(n).enumerate() {
                    task.put_var_name(j, &format!("t[{}]",i))?;
                }
                for (i,j) in (voff_theta..).take(d).enumerate() {
                    task.put_var_name(j, &format!("theta[{}]",i))?;
                }
                task.put_var_name(voff_r,"r")?;

                task.append_rows(numrow)?;
                // Objective λ*r + sum(t)
                task.put_obj_sense(moco::ObjSense::MINIMIZE);
                task.put_obj_row(roff_obj)?;

                task.put_row(0, &(voff_t..).take(n).collect::<Vec<i32>>(), &vec![1.0;n])?;
                task.put_ijc(0, voff_r, lambda)?;

                // Softplus function constraints
                soft_plus(task, d, voff_theta, voff_t, X, y)?;

                // Regularization
                let dom_quad = task.get_domain_quadratic_cone(d as i64+1)?;
                task.put_ijc(roff_reg, voff_r, 1.0)?;
                task.put_row_slice(roff_reg+1, &vec![1; d], &(voff_theta..).take(d).collect::<Vec<i32>>(), &vec![1.0; d])?;
                task.append_con(dom_quad, &(roff_reg..).take(d+1).collect::<Vec<i64>>(), None)?;

                task.write_task_to_file("dump.ptf")?;
                // Solution
                let _trm = task.optimize()?;
                task.solution_summary(moco::StreamType::MSG)?;

                let solidx = 0;
                task.get_sol_xx_slice(solidx, voff_theta, theta_val)?;

                Ok(())
            })
}

#[allow(non_snake_case)]
fn main() {
    // Test: detect and approximate a circle using degree 2 polynomials
    let n : usize = 30;

    let mut X = vec![0.0; n*n*6];
    for ((i,j),Xij) in iproduct!(0..n,0..n).zip(X.chunks_mut(6)) {
        let (i,j,n) = (i as f64,j as f64,n as f64);
        let x = -1.0 + 2.0 * i/(n-1.0);
        let y = -1.0 + 2.0 * j/(n-1.0);
        Xij.copy_from_slice(&[1.0,x,y,x*y,x*x,y*y]);
    }

    let y : Vec<bool> = iproduct!(0..n,0..n).map(|(i,j)| {
        let (i,j,n) = (i as f64,j as f64,n as f64);

        let x = -1.0 + 2.0 * (i/(n-1.0));
        let y = -1.0 + 2.0 * (j/(n-1.0));
        x*x+y*y >= 0.69  }).collect();

    let mut theta = vec![0.0; 6];

    logistic_regression(6, &X, &y, 0.1, &mut theta).unwrap();

    println!("\ntheta = {:?}",theta);
}
