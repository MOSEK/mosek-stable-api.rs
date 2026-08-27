
//!
//!  Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//!  File: `portfolio_6_factor.rs`
//!
//!  Description :  Implements a portfolio optimization model using factor model.
//!
extern crate itertools;

use itertools::{iproduct, izip};
use mosek_stable_api::{self as msk, APIError};

/// Factor portfolio model
///
/// ```
/// maximize μ'x
/// s.t.     budget: e'x = w + w'x0
///          risk:   x'(θ + β'Rβ)x < ɣ
/// ```
/// where θ is a diagonal matrix. Turned into a conic problem:
/// ```
/// maximize μ'x
/// s.t.     budget: e'x = w + w'x0
///          risk:   ⎛⎡ Pβ  ⎤   ⎞ 2
///                  ⎢⎢     ⎥ x ⎥   < ɣ²
///                  ⎝⎣ θ^½ ⎦   ⎠
/// ```
/// where `P'P = R
#[allow(non_snake_case)]
fn portfolio() -> Result<(),msk::APIError> {
    let mskapi : msk::MosekCoreAPI = msk::initialize().unwrap();
    let w  = 1.0;
    let mu = &[0.07197, 0.15518, 0.17535, 0.08981, 0.42896, 0.39292, 0.32171, 0.18379];
    let x0 = &[0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];
    let n  = mu.len();

    // Factor exposure matrix
    let beta = Matrix::from_rows(&[
        0.4256,  0.1869,
        0.2413,  0.3877,
        0.2235,  0.3697,
        0.1503,  0.4612,
        1.5325, -0.2633,
        1.2741, -0.2613,
        0.6939,  0.2372,
        0.5425,  0.2116 ], n,2);

    // Factor covariance matrix
    let R = Matrix::from_columns(&[0.0620, 0.0577,
                                   0.0577, 0.0908 ], 2, 2);

    // Specific risk components
    let theta : &[f64] = &[0.0720, 0.0508, 0.0377, 0.0394, 0.0663, 0.0224, 0.0417, 0.0459];
    let G_factor = beta.matrix_mul(&R.cholesky()?)?; assert!(! G_factor.row_major);

    _ = (beta,R);

    let k = G_factor.num_cols();

    let gammas = &[0.24, 0.28, 0.32, 0.36, 0.4, 0.44, 0.48];

    let total_budget = w + x0.iter().cloned().sum::<f64>();

    /* Initial setup. */
    mskapi.task()?
        .with_stream_callback(
            msk::StreamType::MSG,
            |msg| print!("{}",msg),
            |task| {
                // NOTE: Here we specify matrices as vectors (row major order) to avoid having
                // to initialize them as double(*)[] type, which is incompatible with double**.

                // Offset of variables into the API variable.
                let numvar = n;
                let voff_x = 0;

                // Affine row offset
                let roff_obj       = 0i64;
                let roff_bud       = roff_obj+1;
                let roff_risk      = roff_bud+1;
                let roff_thetadiag = roff_risk + k as i64 + 1;
                let numrow = roff_thetadiag + n as i64;

                // Holding variable x of length n
                // No other auxiliary variables are needed in this formulation
                task.append_vars(numvar as i32)?;

                // Setting up variable x
                for (j,xj) in izip!(0..numvar, voff_x..) {
                    /* Optionally we can give the variables names */
                    task.put_var_name(xj,&format!("x[{}]",j+1))?;
                    /* No short-selling - x^l = 0, x^u = inf */
                    task.put_var_bound(xj, 0.0, f64::INFINITY)?;
                }

                let xsubj : Vec<i32> = (voff_x..).take(n).collect();

                // One linear constraint: total budget
                let dom_rzero = task.get_domain_rzero()?;
                let coni = task.get_num_con();
                task.append_rows(numrow)?;
                task.append_con(dom_rzero, &[roff_bud], Some(&[total_budget]))?;
                task.put_con_name(coni, "budget")?;
                task.put_row(roff_bud, &xsubj, &vec![1.0; n])?;

                // Input (gamma, G_factor_T x, diag(sqrt(theta))*x) in the AFE (affine expression) storage
                // We need k+n+1 rows and we fill them in in three parts

                // 1. The first affine expression = gamma, will be specified later
                // 2. The next k expressions comprise G_factor_T*x, we add them column by column since
                //    G_factor is stored row-wise and we transpose on the fly
                for (i,GT_row) in izip!(roff_risk+1..,G_factor.data.chunks(n)) {
                    task.put_row(i,&xsubj, GT_row)?;
                }

                // 3. The remaining n rows contain sqrt(theta) on the diagonal
                task.put_row_slice(
                    roff_thetadiag,
                    &vec![1; n],
                    &(voff_x..).take(n).collect::<Vec<i32>>(),
                    &theta.iter().map(|t| t.sqrt()).collect::<Vec<f64>>())?;

                // Input the affine conic constraint (gamma, G_factor_T x, diag(sqrt(theta))*x) \in QCone
                // Add the constraint
                {
                    let dom_quad = task.get_domain_quadratic_cone((k+n+1) as i64)?;
                    let coni = task.get_num_con();
                    task.append_con(dom_quad, &(roff_risk..).take(1+n+k).collect::<Vec<i64>>(), None)?;
                    task.put_con_name(coni, "risk")?;
                }

                // Objective: maximize expected return μ'x
                task.put_row(roff_obj, &xsubj, mu)?;
                task.put_obj_sense(msk::ObjSense::MAXIMIZE);
                task.put_obj_row(roff_obj)?;

                let mut res = Vec::new();
                let mut xx  = vec![0.0; n];
                for (i,&gamma) in gammas.iter().enumerate() {
                    // Specify gamma in ACC
                    task.put_row_g(roff_risk, gamma)?;

                    let _trmcode = task.optimize()?;
                    task.write_task_to_file(&format!("dump-{}.ptf",i))?;

                    /* Display the solution summary for quick inspection of results. */
                    task.solution_summary(msk::StreamType::MSG)?;

                    // Check if the interior point solution is an optimal point
                    let solidx = 0;
                    let (psolsta,_dsolsta) = task.get_sol_status(solidx)?;

                    match psolsta {
                        msk::SolSta::OPTIMAL => {
                            task.get_sol_xx_slice(solidx, voff_x, &mut xx)?;
                            // Read the x variables one by one and compute expected return.
                            // Can also be obtained as value of the objective.
                            let expret : f64 = xx.iter().zip(mu.iter()).map(|(&x,&m)| x*m).sum();
                            res.push((gamma,expret));
                        },
                        _ => println!("Warning: Unexpected solution status {:?}",psolsta)
                    }
                }
                println!("Expected returns:\n{:>15} {:>15}","gamma","expret");
                for (gamma,expret) in res {
                    println!("{:15.4} {:15.4}",gamma,expret);
                }

                Ok(())
            })
}

struct Matrix {
    /// data, columns oriented
    data : Vec<f64>,
    /// (rows,columns)
    shape : (usize,usize),
    row_major : bool
}
impl Matrix {
    pub fn from_rows(data : &[f64], rows : usize, columns : usize) -> Matrix {
        assert_eq!(data.len(),rows*columns);
        Matrix { data: data.to_owned(), shape: (rows,columns), row_major : true }
    }
    pub fn from_columns(data : &[f64], rows : usize, columns : usize) -> Matrix {
        assert_eq!(data.len(),rows*columns);
        Matrix { data: data.to_owned(), shape: (rows,columns), row_major : false }
    }
    pub fn num_rows(&self) -> usize { self.shape.0 }
    pub fn num_cols(&self) -> usize { self.shape.1 }

    /// For a symmetric matrix, compute the cholesky factorization
    ///
    /// It is assumed but not checked that the matrix is symmetric. It is checked that the matrix is quadratic.
    pub fn cholesky(&self) -> Result<Matrix,APIError>
    {
        let mskapi : msk::MosekCoreAPI = msk::initialize().unwrap();

        assert_eq!(self.shape.0, self.shape.1);
        let n = self.num_rows();
        let mut data = self.data.clone();
        mskapi.potrf(false, n as i32, &mut data)?;
        // Zero out upper triangular part (MSK_potrf does not use it, original matrix values remain there)
        for ((j,i),v) in iproduct!(0..n,0..n).zip(data.iter_mut()) {
            if j > i { *v = 0.0 }
        }

        Ok(Matrix{data,shape : self.shape, row_major : false})
    }

    // Matrix multiplication. The operand dimensions must match correctly.
    pub fn matrix_mul(&self, b : &Matrix) -> Result<Matrix,APIError>
    {
        let mskapi : msk::MosekCoreAPI = msk::initialize().unwrap();
        assert_eq!(self.num_cols(),b.num_rows());

        let ta = self.row_major;
        let tb = b.row_major;


        let (m,n,k) = (self.num_rows(),b.num_cols(),self.num_cols());
        let mut data = vec![999.0; m*n];

        mskapi.gemm(
            ta,tb,
            m as i32,n as i32,k as i32,
            1.0,
            &self.data,
            &b.data,
            0.0,
            &mut data)?;

        Ok(Matrix{data, shape : (self.num_rows(),b.num_cols()), row_major : false })
    }

    #[allow(dead_code)]
    pub fn make_col_major(&mut self) {
        let mut data = vec![0.0;self.data.len()];
        if self.row_major {
            for (&s,t) in (0..self.num_cols())
                .flat_map(|j| self.data[j..].iter().step_by(self.num_cols()))
                .zip(data.iter_mut())
            {
                *t = s;
            }
        }
        self.data = data;
        self.row_major = false;
    }

    #[allow(dead_code)]
    pub fn transpose_orientation(&self) -> Matrix {
        let mut data = vec![0.0; self.data.len()];
        if self.row_major {
            for (&s,t) in (0..self.num_cols())
                .flat_map(|j| self.data[j..].iter().step_by(self.num_cols()))
                .zip(data.iter_mut())
            {
                *t = s;
            }
        }
        else {
            for (&s,t) in (0..self.num_rows())
                .flat_map(|j| self.data[j..].iter().step_by(self.num_rows()))
                .zip(data.iter_mut())
            {
                *t = s;
            }
        }
        Matrix{data, shape:(self.num_rows(),self.num_cols()), row_major : ! self.row_major }
   }
}



fn main() {
    portfolio().unwrap();
}
