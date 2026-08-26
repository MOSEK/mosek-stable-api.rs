
//!  Copyright (c) MOSEK ApS, Denmark. All rights reserved.
//!
//!  File: `portfolio_6_factor.rs`
//!
//!  Description :  Implements a portfolio optimization model using factor model.
extern crate itertools;

use itertools::{iproduct, izip};
use mosek_stable_api::{self as msk, APIError};


#[allow(non_snake_case)]
fn portfolio() -> Result<(),msk::APIError> {
    let mskapi : msk::MosekCoreAPI = msk::initialize().unwrap();
    let w  = 1.0;
    let mu = &[0.07197, 0.15518, 0.17535, 0.08981, 0.42896, 0.39292, 0.32171, 0.18379];
    let x0 = &[0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0];
    let n  = mu.len();

    // Factor exposure matrix
    let vecB =
        &[0.4256, 0.1869,
          0.2413, 0.3877,
          0.2235, 0.3697,
          0.1503, 0.4612,
          1.5325, -0.2633,
          1.2741, -0.2613,
          0.6939, 0.2372,
          0.5425, 0.2116 ];

    let B = Matrix::from_rows(vecB, n, 2);

    // Factor covariance matrix
    let vecS_F = &[
        0.0620, 0.0577,
        0.0577, 0.0908 ];
    let S_F = Matrix::from_rows(vecS_F, 2, 2);

    // Specific risk components
    let theta : &[f64] = &[0.0720, 0.0508, 0.0377, 0.0394, 0.0663, 0.0224, 0.0417, 0.0459];

    let P = S_F.cholesky()?;
    let mut G_factor = B.matrix_mul(&P)?; G_factor.make_row_major();

    _ = B;
    _ = S_F;
    _ = P;

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
                let roff_obj : i64 = 0;
                let roff_bud = roff_obj+1;
                let roff_risk = roff_bud + 1;
                let roff_thetadiag = roff_risk + k as i64+1;
                let numrow = roff_thetadiag + n as i64;

                // Holding variable x of length n
                // No other auxiliary variables are needed in this formulation
                task.append_vars(numvar as i32)?;

                // Setting up variable x
                for (j,xj) in izip!(0..numvar, voff_x..)
                {
                    /* Optionally we can give the variables names */
                    task.put_var_name(xj,&format!("x[{}]",j+1))?;
                    /* No short-selling - x^l = 0, x^u = inf */
                    task.put_var_bound(xj, 0.0, f64::INFINITY)?;
                }

                let dom_rzero = task.get_domain_rzero()?;
                // One linear constraint: total budget
                let coni = task.get_num_con();
                task.append_rows(1)?;
                task.append_con(dom_rzero, &[roff_bud], Some(&[total_budget]))?;
                task.put_con_name(coni, "budget")?;
                for xi in (voff_x..voff_x).take(n) {
                    task.put_ijc(roff_bud, xi, 1.0)?;
                }






                // Input (gamma, G_factor_T x, diag(sqrt(theta))*x) in the AFE (affine expression) storage
                // We need k+n+1 rows and we fill them in in three parts
                task.append_rows(numrow)?;
                //MOSEKCALL(res, MSK_appendafes(task, k + n + 1));
                // 1. The first affine expression = gamma, will be specified later
                // 2. The next k expressions comprise G_factor_T*x, we add them column by column since
                //    G_factor is stored row-wise and we transpose on the fly

                let xsubj : Vec<i32> = (voff_x..).take(n).collect();
                {
                    for (i,GT_row) in izip!(roff_risk+1..,G_factor.data.chunks(n)) {
                        task.put_row(i,&xsubj, GT_row)?;
                    }
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

                // Objective: maximize expected return mu^T x
                task.put_row(roff_obj, &xsubj, mu)?;
                task.put_obj_sense(msk::ObjSense::MAXIMIZE);


                let mut res = Vec::new();
                let mut xx = vec![0.0; n];
                for (_i,&gamma) in izip!(0.., gammas.iter()) {
                    // Specify gamma in ACC
                    task.put_row_g(roff_obj, gamma)?;

                    let _trmcode = task.optimize()?;

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
        mskapi.potrf(false, self.num_rows() as i32, &mut data)?;
        // Zero out upper triangular part (MSK_potrf does not use it, original matrix values remain there)
        for ((j,i),v) in iproduct!(0..n,0..n).zip(data.iter_mut()) {
            if j >= i { *v = 0.0 }
        }

        Ok(Matrix{data,shape : self.shape, row_major : false})
    }

    // Matrix multiplication
    pub fn matrix_mul(&self, b : &Matrix) -> Result<Matrix,APIError>
    {
        let mskapi : msk::MosekCoreAPI = msk::initialize().unwrap();
        assert_eq!(self.num_cols(),b.num_rows());
        let mut data = vec![0.0; self.num_rows()*b.num_cols()];

        let ta = self.row_major;
        let tb = self.row_major;

        let (m,k) = if ta { (self.num_cols(),self.num_rows()) } else { (self.num_rows(),self.num_cols()) };
        let n = if tb { b.num_rows() } else { b.num_cols() };

        mskapi.gemm(
            ta,tb,
            m as i32,n as i32,k as i32,
            1.0,
            &self.data,
            &b.data,
            1.0,
            &mut data)?;

        Ok(Matrix{data, shape : (m,n), row_major : false })
    }

    pub fn make_row_major(&mut self) {
        let mut data = vec![0.0;self.data.len()];
        if ! self.row_major {
            for (&s,t) in (0..self.num_rows())
                .flat_map(|j| self.data[j..].iter().step_by(self.num_rows()))
                .zip(data.iter_mut())
            {
                *t = s;
            }
        }
        self.data = data;
        self.row_major = true;
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
// typedef struct
// {
//   MSKrealt** m;
//   int nr;
//   int nc;
// } matrix;

// static void matrix_print(matrix* m)
// {
//   int i,j;
//   for (i = 0; i < m->nr; ++i)
//   {
//     printf("[");
//     for (j = 0; j < m->nc; ++j)
//     {
//       printf("%f, ", m->m[i][j]);
//     }
//     printf("\b\b]\n");
//   }
// }

// static void array_print(MSKrealt* a, int len)
// {
//   int j;
//   printf("[");
//   for (j = 0; j < len; ++j)
//   {
//     printf("%f, ", a[j]);
//   }
//   printf("\b\b]\n");
// }

// static matrix* matrix_alloc(int dim1, int dim2)
// {
//   int i;
//   matrix* m = (matrix*) malloc(sizeof(matrix));
//   m->nr = dim1;
//   m->nc = dim2;
//   m->m = (MSKrealt**) malloc(dim1 * sizeof(MSKrealt*));
//   for (i = 0; i < dim1; ++i)
//   {
//     m->m[i] = (MSKrealt*) malloc(dim2 * sizeof(MSKrealt));
//   }
//   return m;
// }

// static void matrix_free(matrix* m)
// {
//   int i;
//   for (i = 0; i < m->nr; ++i)
//   {
//     free(m->m[i]);
//   }
//   free(m->m);
//   free(m);
// }

// static MSKrealt* vector_alloc(int dim)
// {
//   MSKrealt* v = (MSKrealt*) malloc(dim * sizeof(MSKrealt));
//   return v;
// }

// static void vector_free(MSKrealt* v)
// {
//   free(v);
// }

// static MSKrealt sum(MSKrealt* x, int n)
// {
//   int i;
//   MSKrealt r = 0.0;
//   for (i = 0; i < n; ++i) r += x[i];
//   return r;
// }

// static MSKrealt dot(MSKrealt* x, MSKrealt* y, int n)
// {
//   int i;
//   MSKrealt r = 0.0;
//   for (i = 0; i < n; ++i) r += x[i] * y[i];
//   return r;
// }

// // Vectorize matrix (column-major order)
// static MSKrealt* mat_to_vec_c(matrix* m)
// {
//   int ni = m->nr;
//   int nj = m->nc;
//   int i,j;

//   MSKrealt* c = vector_alloc(ni * nj);
//   for (j = 0; j < nj; ++j)
//   {
//     for (i = 0; i < ni; ++i)
//     {
//       c[j * ni + i] = m->m[i][j];
//     }
//   }
//   return c;
// }

// // Reshape vector to matrix (column-major order)
// static matrix* vec_to_mat_c(MSKrealt* c, int ni, int nj)
// {
//   int i,j;
//   matrix* m = matrix_alloc(ni, nj);
//   for (j = 0; j < nj; ++j)
//   {
//     for (i = 0; i < ni; ++i)
//     {
//       m->m[i][j] = c[j * ni + i];
//     }
//   }
//   return m;
// }

// // Reshape vector to matrix (row-major order)
// static matrix* vec_to_mat_r(MSKrealt* r, int ni, int nj)
// {
//   int i,j;
//   matrix* m = matrix_alloc(ni, nj);
//   for (i = 0; i < ni; ++i)
//   {
//     for (j = 0; j < nj; ++j)
//     {
//       m->m[i][j] = r[i * nj + j];
//     }
//   }
//   return m;
// }




fn main() {
    portfolio().unwrap();
}
