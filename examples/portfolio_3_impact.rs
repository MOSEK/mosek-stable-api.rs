//
//  File : portfolio_3_impact.rs
//
//  Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//
//  Description :  Implements a basic portfolio optimization model
//                 with transaction costs of order x^(3/2).
//
//  Problem:
//  ```
//  max  μ'x
//  s.t. ∑xᵢ + m't = W+∑x0ᵢ
//       (ɣ,G'x) ∈ Q
//       (tⱼ, 1, zⱼ) ∈ P₃[1/3,2/3], j=1..n
//       |xⱼ-x0ⱼ| <= zⱼ
//       x >= 0
//  ```
extern crate itertools;

use mosek_stable_api as msk;
use itertools::izip;

#[allow(non_snake_case)]
fn portfolio() -> Result<(),msk::APIError> {
    let mskapi = msk::initialize()?;

    let n : i32 = 8;
    let mu = &[0.07197, 0.15518, 0.17535, 0.08981, 0.42896, 0.39292, 0.32171, 0.18379];
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
    let m = &[0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01];

    let   k     = GT.len()/n as usize;
    let   x0    = &[0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];
    let   w     = 1.0;
    let   gamma = 0.36;
    let total_budget : f64 = x0.iter().sum::<f64>() + w;

    // Offset of variables into the API variable.
    let numvar      : i32 = 3 * n;
    let voff_x      : i32  = 0;
    let voff_t      : i32  = voff_x+n;
    let voff_z      : i32  = voff_t+n;

    let roff_obj     : i64 = 0;             // 1 row
    let roff_budget  : i64 = roff_obj+1;    // 1 row
    let roff_gamma   : i64 = roff_budget+1; // 1 row
    let roff_GT      : i64 = roff_gamma+1;  // GT k rows
    let roff_z       : i64 = roff_GT+k as i64; // n rows
    let roff_t       : i64 = roff_z+n as i64;      // n rows
    let roff_1       : i64 = roff_t+n as i64;      // 1 row
    let roff_deltax  : i64 = roff_1+1 as i64;      // n rows
    let roff_rdeltax : i64 = roff_deltax+n as i64; // k rows
    let numrow = roff_rdeltax+n as i64;

    mskapi.task()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {
                /* Initial setup. */
                let dom_rzero = task.get_domain_rzero()?;
                let dom_rplus = task.get_domain_rplus()?;
                let dom_quad  = task.get_domain_quadratic_cone(k as i64+1)?;
                let dom_pow3 = task.get_domain_primal_power_cone(3,&[2.0,1.0])?;

                // Append rows.
                //
                // First row is the objective,
                // Second is the budget sum
                // Next is the GT matrix
                // Finally, the impact terms
                task.append_rows(numrow)?;
                // Variables (vector of x, c, z)
                task.append_vars(numvar)?;

                for (j,x,t,z) in izip!(0..n, voff_x..,voff_t..,voff_z..) {
                    /* Optionally we can give the variables names */
                    task.put_var_name(x, &format!("x[{}]",1+j))?;
                    task.put_var_name(t, &format!("t[{}]",1+j))?;
                    task.put_var_name(z, &format!("z[{}]",1+j))?;

                    /* Apply variable bounds (x >= 0, c and z free) */
                    task.put_var_bound(x,  0.0,              f64::INFINITY)?;
                    task.put_var_bound(t, f64::NEG_INFINITY, f64::INFINITY)?;
                    task.put_var_bound(z, f64::NEG_INFINITY, f64::INFINITY)?;
                }

                // - Total budget
                {
                    let c : Vec<f64> = std::iter::repeat_n(1.0, n as usize).chain(m.iter().cloned()).collect();
                    let j : Vec<i32> = (voff_x..voff_x+n).chain(voff_t..voff_t+n).collect();

                    task.put_row(1, &j, &c)?;
                }
                let coff_budget = task.get_num_con();

                task.append_con(dom_rzero,  &[roff_budget], Some(&[total_budget]))?;
                task.put_con_name(coff_budget, "budget")?;

                // - Risk
                {
                    let coni = task.get_num_con();
                    let xj = &[0,1,2,3,4,5,6,7];
                    task.put_row_g(roff_gamma,gamma)?;
                    for (rowi,gtrow) in (roff_GT..).zip(GT.chunks(n as usize)) {
                        task.put_row(rowi,xj,gtrow)?;
                    }

                    task.append_con(dom_quad,&[ roff_gamma,roff_GT,roff_GT+1,roff_GT+2,roff_GT+3,roff_GT+4,roff_GT+5,roff_GT+6,roff_GT+7 ],None)?;
                    task.put_con_name(coni,"risk")?;
                }

                println!("x0 = {:?}",x0);
                // - Absolute values
                //       |xⱼ-x0ⱼ| <= zⱼ
                for (i,coni,rowzi,rowdeltaxi,rowrdeltaxi,rowti,zi,xi,ti,&x0i) in
                    izip!(0..,(task.get_num_con()..).step_by(2),roff_z..,roff_deltax..,roff_rdeltax..,roff_t..,voff_z..,voff_x..,voff_t..,x0.iter())
                {
                    task.append_con(dom_rplus,&[rowdeltaxi], None)?;
                    task.append_con(dom_rplus,&[rowrdeltaxi], None)?;
                    // z_i - x_i + x_0i > 0
                    task.put_ijc(rowdeltaxi,zi,1.0)?;
                    task.put_ijc(rowdeltaxi,xi,-1.0)?;
                    task.put_row_g(rowdeltaxi,x0i)?;

                    // z_i + x_i - x_0i > 0
                    task.put_ijc(rowrdeltaxi,zi,1.0)?;
                    task.put_ijc(rowrdeltaxi,xi,1.0)?;
                    task.put_row_g(rowrdeltaxi,-x0i)?;

                    task.put_ijc(rowti,ti,1.0)?;
                    task.put_ijc(rowzi,zi,1.0)?;

                    task.put_con_name(coni,&format!("abs_1[{}]",i))?;
                    task.put_con_name(coni+1,&format!("abs_2[{}]",i))?;
                }

                // - Market impact
                //       tⱼ >= 1^{1/3} zⱼ^{2/3}
                //       (tⱼ, 1, zⱼ) ∈ P₃[1/3,2/3], j=1..n
                task.put_row_g(roff_1,1.0)?;
                for (i,coni,rowzi,rowti) in izip!(0..n,task.get_num_con()..,roff_z..,roff_t..) {
                    task.append_con(dom_pow3, &[ rowti, roff_1, rowzi ], None)?;
                    task.put_con_name(coni, &format!("market-impact[{}]",i))?;
                }

                // - Objective
                for j in voff_x..voff_x+n {
                    task.put_ijc(0,j, 1.0)?;
                }

                task.put_obj_name("expected-return")?;
                task.put_obj_sense(msk::ObjSense::MAXIMIZE);
                task.put_obj_row(0)?;

                task.write_task_to_file("portfolio_3.ptf")?;
                let trmcode = task.optimize()?;

                /* Display the solution summary for quick inspection of results. */
                task.solution_summary(msk::StreamType::MSG)?;

                let solidx = 0;
                // Check if the interior point solution is an optimal point
                let (psolsta,_dsolsta) = task.get_sol_status(solidx)?;

                match (psolsta) {
                    msk::SolSta::OPTIMAL => {
                        let mut xx = vec![0.0; n as usize];
                        task.get_sol_xx_slice(solidx,voff_x,&mut xx)?;

                        let expret = mu.iter().zip(xx.iter()).map(|(&mu,&x)| mu*x).sum::<f64>();
                        println!("Expected return {:e} for gamma {:e}", expret, gamma);
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
