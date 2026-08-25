//!
//!  File : portfolio_5_card.rs
//!
//!  Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//!  Description :  Implements a basic portfolio optimization model
//!                 with cardinality constraints on number of assets traded.
//!

use itertools::izip;
use mosek_stable_api as msk;




/// The portfolio model with cardinality constraints
///
/// ```
/// maximize μ'x
/// s.t.     risk:   ɣ > ||G'x||
///          budget: sum x_i = U
///          zabs:   z > x-x0
///          zabs:   z > x0-x
///          switch: z_i - Uy_i < 0
///          cardinality: sum y_i < K
/// ```
#[allow(non_snake_case)]
fn markowitz_with_card(
    n     : i32,
    k     : i32,
    x0    : &[f64],
    w     : f64,
    gamma : f64,
    mu    : &[f64],
    GT    : &[f64],
    K     : i32,
    xx    : &mut [f64],
    y     : &mut [f64]) -> Result<(),msk::APIError>
{
    let mskapi = msk::initialize()?;

    mskapi.task()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {

                // Offset of variables.
                let voff_x : i32 = 0;
                let voff_z : i32 = voff_x+n;
                let voff_y : i32 = voff_z+n;
                let numvar : i32 = voff_y+n;

                // Offset of constraints.
                let roff_obj  : i64 = 0;
                let roff_bud  : i64 = roff_obj+1;
                let roff_abs1 : i64 = roff_bud+1;
                let roff_abs2 : i64 = roff_abs1 + n as i64;
                let roff_swi  : i64 = roff_abs2 + n as i64;
                let roff_card : i64 = roff_swi + n as i64;
                let roff_risk : i64 = roff_card + 1;
                let numrow    : i64 = roff_risk + k as i64 + 1;

                // Variables (vector of x, z, y)
                task.append_vars(numvar)?;

                for (j,xj,zj,yj) in izip!(0..n,voff_x..,voff_z..,voff_y..)
                {
                    /* Optionally we can give the variables names */
                    task.put_var_name(xj, &format!("x[{}]",1+j))?;
                    task.put_var_name(yj, &format!("y[{}]",1+j))?;
                    task.put_var_name(zj, &format!("z[{}]",1+j))?;

                    /* Apply variable bounds (x >= 0, z free, y binary) */
                    task.put_var_bound(xj, 0.0, f64::INFINITY)?;
                    task.put_var_bound(zj, f64::NEG_INFINITY, f64::INFINITY)?;
                    task.put_var_bound(yj, 0.0, 1.0)?;
                    task.put_var_type(yj, msk::VariableType::INTEGER)?;
                }

                task.append_rows(numrow)?;
                // Linear constraints
                // - Total budget
                for xj in (voff_x..).take(n as usize) {
                    /* Coefficients in the first row of A */
                    task.put_ijc(roff_bud, xj, 1.0)?;
                }

                let dom_rzero = task.get_domain_rzero()?;

                let U : f64 = w + x0.iter().sum::<f64>();
                {
                    let coni = task.get_num_con();
                    task.append_con(dom_rzero, &[roff_bud], Some(&[U]))?;
                    task.put_con_name(coni, "budget")?;
                }

                // - Absolute value
                let dom_rplus = task.get_domain_rplus()?;
                for (i,coni,rabs1,rabs2,xj,zj,x0j) in izip!(0..n,(task.get_num_con()..).step_by(2),roff_abs1..,roff_abs2..,  voff_x..,voff_z.., x0.iter().cloned()) {
                    task.append_con(dom_rplus, &[rabs1], None)?;
                    task.append_con(dom_rplus, &[rabs2], None)?;

                    task.put_ijc(rabs1,xj,1.0)?;
                    task.put_ijc(rabs1,zj,1.0)?;
                    task.put_row_g(rabs1,-x0j)?;

                    task.put_ijc(rabs2,xj,-1.0)?;
                    task.put_ijc(rabs2,zj,1.0)?;
                    task.put_row_g(rabs2,x0j)?;

                    task.put_con_name(coni, &format!("zabs1[{}]",i+1))?;
                    task.put_con_name(coni+1, &format!("zabs2[{}]",i+1))?;
                }

                // - Switch
                let dom_rminus = task.get_domain_rminus()?;
                for (i,coni,rowi,zi,yi) in izip!(0..n,task.get_num_con()..,roff_swi..,voff_z..,voff_y..)
                {
                    task.append_con(dom_rminus, &[rowi], None)?;
                    task.put_con_name(coni, &format!("switch[{}]",i+1))?;
                    task.put_ijc(rowi, zi, 1.0)?;
                    task.put_ijc(rowi, yi, -U)?;
                }

                // - Cardinality
                for yj in (voff_y..).take(n as usize) {
                    task.put_ijc(roff_card, yj, 1.0)?;
                }
                let card_con_i = task.get_num_con();
                task.append_con(dom_rminus, &[roff_card], Some(&[K as f64]))?;
                task.put_con_name(card_con_i,"cardinality")?;


                // - Risk
                task.put_row_g(roff_risk, gamma)?;
                let subj : Vec<i32> = (voff_x..voff_x+n).collect();
                for (rowi,GTi) in izip!(roff_risk+1.., GT.chunks(n as usize)) {
                    task.put_row(rowi, &subj, GTi)?;
                }
                let risk_con_i = task.get_num_con();
                let dom_quad = task.get_domain_quadratic_cone(1+k as i64)?;
                task.append_con(dom_quad, &(roff_risk..roff_risk+k as i64+1).collect::<Vec<i64>>(), None)?;
                task.put_con_name(risk_con_i,"risk")?;


                // Objective: maximize expected return mu^T x
                for (&muj,xj) in izip!(mu.iter(),voff_x..) {
                    task.put_ijc(roff_obj,xj,muj)?;
                }
                task.put_obj_row(roff_obj)?;
                task.put_obj_sense(msk::ObjSense::MAXIMIZE);


                let _trmcode = task.optimize()?;
                task.put_int_param("ipar_ptf_write_solutions", 1)?;
                task.write_task_to_file(&format!("dump-{}.jtask",K))?;

                /* Display the solution summary for quick inspection of results. */
                task.solution_summary(msk::StreamType::MSG)?;

                // Check if the interior point solution is an optimal point
                let solidx = 0;
                let (psolsta,_dsolsta) = task.get_sol_status(solidx)?;
                match psolsta {
                    msk::SolSta::INTEGER_OPTIMAL => {
                        task.get_sol_xx_slice(solidx, voff_x, xx)?;
                        task.get_sol_xx_slice(solidx, voff_y, y)?;
                    },
                    _ => {
                        // See https://docs.mosek.com/latest/capi/accessing-solution.html about handling solution statuses.
                        println!("Unexpected solution status: {:?}", psolsta);
                    }
                }
            Ok(())
        })
}

#[allow(non_snake_case)]
fn main()
{
    let mu = &[0.07197, 0.15518, 0.17535, 0.08981, 0.42896, 0.39292, 0.32171, 0.18379];
    let n = mu.len();
    // GT must have size n rows
    let GT = &[
        0.30758, 0.12146, 0.11341, 0.11327, 0.17625, 0.11973, 0.10435, 0.10638,
        0.0,     0.25042, 0.09946, 0.09164, 0.06692, 0.08706, 0.09173, 0.08506,
        0.0,     0.0,     0.19914, 0.05867, 0.06453, 0.07367, 0.06468, 0.01914,
        0.0,     0.0,     0.0,     0.20876, 0.04933, 0.03651, 0.09381, 0.07742,
        0.0,     0.0,     0.0,     0.0,     0.36096, 0.12574, 0.10157, 0.0571 ,
        0.0,     0.0,     0.0,     0.0,     0.0,     0.21552, 0.05663, 0.06187,
        0.0,     0.0,     0.0,     0.0,     0.0,     0.0,     0.22514, 0.03327,
        0.0,     0.0,     0.0,     0.0,     0.0,     0.0,     0.0,     0.2202 ];
    let k = GT.len()/n as usize;
    let x0 = &[0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];
    let w  = 1.0;
    let gamma = 0.25;



    let res =
        (1..=n).map(|K| {
            let mut xx = vec![0.0;n];
            let mut y = vec![0.0;n];
            markowitz_with_card(n as i32, k as i32, x0, w, gamma, mu, GT, K as i32, &mut xx,&mut y).unwrap();
            let expret : f64 = xx.iter().zip(mu.iter()).map(|(&xj,&mj)| mj*xj).sum();
            (K,expret,xx,y)
        }).collect::<Vec<(usize,f64,Vec<f64>,Vec<f64>)>>();


    for (K,expret,xx,y) in res {
        println!("Bound {}:  x = {:?}", K,izip!(0..,xx.iter(),y.iter()).filter_map(|(i,x,y)| if *y > 0.5 { Some((i,*x)) } else { None }).collect::<Vec<(usize,f64)>>());
        println!("  Return:  {:.5}", expret);
    }
}
