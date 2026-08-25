//!
//!    File : portfolio_4_transcost.rs
//!
//!    Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//!    Description :  Implements a basic portfolio optimization model
//!    with fixed setup costs and transaction costs
//!    as a mixed-integer problem.
//!

use mosek_stable_api as msk;
use itertools::izip;

#[allow(non_snake_case)]
fn portfolio() -> Result<(),msk::APIError> {
    let mskapi = msk::initialize()?;

    let mu    = &[0.07197, 0.15518, 0.17535, 0.08981, 0.42896, 0.39292, 0.32171, 0.18379];
    let n     = mu.len();
    // GT must have size n rows
    let  GT = &[
        0.30758, 0.12146, 0.11341, 0.11327, 0.17625, 0.11973, 0.10435, 0.10638,
        0.0,     0.25042, 0.09946, 0.09164, 0.06692, 0.08706, 0.09173, 0.08506,
        0.0,     0.0,     0.19914, 0.05867, 0.06453, 0.07367, 0.06468, 0.01914,
        0.0,     0.0,     0.0,     0.20876, 0.04933, 0.03651, 0.09381, 0.07742,
        0.0,     0.0,     0.0,     0.0,     0.36096, 0.12574, 0.10157, 0.0571 ,
        0.0,     0.0,     0.0,     0.0,     0.0,     0.21552, 0.05663, 0.06187,
        0.0,     0.0,     0.0,     0.0,     0.0,     0.0,     0.22514, 0.03327,
        0.0,     0.0,     0.0,     0.0,     0.0,     0.0,     0.0,     0.2202];
    let k = GT.len()/n as usize;
    let x0    = &[0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];
    let w     = 1.0;
    let gamma = 0.36;



    mskapi.task()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {
                let f = vec![0.01; n];
                let g = vec![0.001; n];

                // Offset of variables.
                let numvar = 3 * n;
                let voff_x : i32 = 0;
                let voff_z = voff_x + n as i32;
                let voff_y = voff_z + n as i32;

                // Offset of constraints.
                let numcon      = 3 * n + 1;
                let roff_obj : i64 = 0;             // 1 row
                let roff_bud : i64    = roff_obj+1;    // 1 row
                let roff_z : i64      = roff_bud+1;    // 1 row
                let roff_deltax : i64 = roff_z+n as i64;      // n rows
                let roff_swi : i64    = roff_deltax+n as i64 ; // n rows
                let roff_risk : i64   = roff_swi+n as i64;    // k+1 rows
                let rownum : i64      = roff_risk+k as i64+1;

                let U = w + x0.iter().sum::<f64>();

                let dom_rzero = task.get_domain_rzero()?;
                let dom_rminus = task.get_domain_rminus()?;
                let dom_quad = task.get_domain_quadratic_cone(k as i64+1)?;
                let dom_q2 = task.get_domain_quadratic_cone(2)?;

                // Variables (vector of x, z, y)
                task.append_vars(numvar as i32)?;

                for (j,xj,yj,zj) in izip!(0..n,voff_x..,voff_y..,voff_z..) {
                    /* Optionally we can give the variables names */
                    task.put_var_name(xj, &format!("x[{}]",j+1))?;
                    task.put_var_name(zj, &format!("z[{}]",j+1))?;
                    task.put_var_name(yj, &format!("y[{}]",j+1))?;

                    /* Apply variable bounds (x >= 0, z free, y binary) */
                    task.put_var_bound(xj, 0.0,               f64::INFINITY)?;
                    task.put_var_bound(zj, f64::NEG_INFINITY, f64::INFINITY)?;
                    task.put_var_bound(yj, 0.0,               1.0)?;
                    task.put_var_type(yj, msk::VariableType::INTEGER)?;
                }


                // Linear constraints
                // - Total budget
                // sum(x-x0) = w
                task.append_rows(rownum)?;

                let coni = task.get_num_con();
                task.append_con(dom_rzero, &[roff_bud], Some(&[U]))?;
                task.put_con_name(coni, "budget")?;


                for (xj,zj,yj,&gj,&fj) in izip!(voff_x..,voff_z..,voff_y..,g.iter(),f.iter()).take(n as usize) {
                    /* Coefficients in the first row of A */
                    task.put_ijc(roff_bud, xj, 1.0)?;
                    task.put_ijc(roff_bud, zj, gj)?;
                    task.put_ijc(roff_bud, yj, fj)?;

                }

                // - Absolute value
                // z_i > |x_i - x0_i|

                for (i,coni,rowzi,rowdxi) in izip!(0..n,task.get_num_con()..,roff_z..,roff_deltax..)
                {
                    task.append_con(dom_q2, &[rowzi,rowdxi], None)?;
                    task.put_con_name(coni, &format!("zabs[{}]",i+1))?;
                }

                // - Switch
                // z_i - U*y_i < 0.0
                for (i,coni,rowi,zj,yj) in izip!(0..n,task.get_num_con()..,roff_swi..,voff_z..,voff_y..)
                {
                    task.append_con(dom_rminus, &[rowi], None)?;
                    task.put_ijc(rowi, zj, 1.0)?;
                    task.put_ijc(rowi, yj, -U)?;
                    task.put_con_name(coni, &format!("switch[{}]",i+1))?;
                }

                // for (i,coni,rowi,zi,yi) in izip!(0..n,task.get_num_con()..,roff_swi..,voff_z..,voff_y..)
                // {
                //     task.append_con(dom_rminus,&[rowi], None)?;
                //     task.put_ijc(rowi, zi, 1.0)?;
                //     task.put_ijc(rowi, ti, -U)?;
                //     task.put_con_name(coni, &format!("switch[{}]",1+i))?;
                // }

                // for (i,coni,rowi,zi,yi) in izip!(0..n,task.get_num_con()..,roff_swi..,roff_z..,roff_y..)
                // {
                //     task.append_con(dom_rminus, &[rowi], None)?;
                //     task.put_ijc(rowi, zi, 1.0)?;
                //     task.put_ijc(rowi, yo, -U)?;
                //     task.put_con_name(coni, &format!("switch[{}]",1+i))?;
                // }

                // for (i,coni,rowi,zi,yi) in izip!(0..n,task.get_num_con()..,roff_swi..,voff_z..,voff_y..)
                // {
                //     task.append_con(dom_rminus,&[rowidx], None)?;
                //     task.put_ijc(rowi, zi, 1.0)?;
                //     task.put_ijc(rowi, yi, -U)?;
                //     task.put_con_name(coni, &format!("switch[{}]"),1+i)?;
                // }

                // for (int i = 0; i < n && MSK120_RES_OK == r; ++i) {
                //     int64_t coni = MSK120_get_num_con(task);
                //     int64_t rowidx = roff_swi+i;
                //     double rhs = 0.0;
                //     task.append_con(dom_rminus,1, &rowidx, &rhs);
                //         task.put_ijc(roff_swi+i, voff_z+i, 1.0);
                //         task.put_ijc(roff_swi+i, voff_y+i, -U);
                //     sprintf(buf, "switch[%d]", 1 + i);
                //         task.put_con_name(coni, buf);
                // }



                // - Risk
                // - (gamma, GTx) in Q(k+1)
                // The part of F and g for variable x:
                //     ⎡0,  0, 0⎤      ⎡gamma⎤
                // F = ⎣GT, 0, 0⎦, g = ⎣0    ⎦
                {
                    let coni = task.get_num_con();

                    //for (int i = 0; i < k && MSK120_RES_OK == r; ++i)
                    for (i,rowi,GTi) in izip!(0..k,roff_risk+1..,GT.chunks(n as usize)) {
                        task.put_row(rowi, &(voff_x..voff_x+k as i32).collect::<Vec<i32>>(), GTi)?;
                    }

                    task.put_row_g(roff_risk, gamma)?;

                    task.append_con(dom_quad, &(roff_risk..roff_risk+k as i64+1).collect::<Vec<i64>>(),None)?;
                    task.put_con_name(coni, "risk")?;
                }

                // Objective: maximize expected return mu^T x
                for (xj,&muj) in izip!(voff_x..,mu.iter()).take(n as usize)
                {
                    task.put_ijc(0,xj, muj)?;
                    task.put_obj_sense(msk::ObjSense::MAXIMIZE);
                    task.put_obj_row(0)?;

                }

                let _trmcode = task.optimize()?;

                /* Display the solution summary for quick inspection of results. */
                task.solution_summary(msk::StreamType::MSG)?;

                // Check if the interior point solution is an optimal point
                let (psolsta,_dsolsta) = task.get_sol_status(0)?;

                match psolsta {
                    msk::SolSta::INTEGER_OPTIMAL => {
                        let mut xx = vec![0.0; n as usize];
                        task.get_sol_xx_slice(0,0,&mut xx);
                        let expret = xx.iter().zip(mu.iter()).map(|(&xj,&muj)| xj*muj).sum::<f64>();

                        println!("Expected return {:.4e} for gamma {:.4e}", expret, gamma);
                    },
                    _ => {
                        // See https://docs.mosek.com/latest/capi/accessing-solution.html about handling solution statuses.
                        println!("Unexpected solution status: {:?}", psolsta);
                    }
                }

                Ok(())
        })
}

fn main() {
    portfolio().unwrap();
}
