//
// Copyright (c) 2026 MOSEK ApS. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. All advertising materials mentioning features or use of this software must
// display the following acknowledgement:
// This product includes software developed by the the organization.
//
// 4. Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDER "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT
// HOLDER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.

#pragma once
#ifndef _MOSEK_CORE_LOADER_H_
#define _MOSEK_CORE_LOADER_H_

#include <stddef.h>
#include <stdint.h>

#define NULLABLE
#define BOOLEAN

#define MSK12_RES_OK 0
#define MSK12_TRM_OK 0


struct MSK12_Task_s;
typedef struct MSK12_Task_s * MSK12_Task_t;

typedef enum MSK12_DomainType_enum {
  MSK12_DOMAIN_NIL,
  MSK12_DOMAIN_RZERO,
  MSK12_DOMAIN_RPLUS,
  MSK12_DOMAIN_RMINUS,
  MSK12_DOMAIN_R,
  MSK12_DOMAIN_QUADRATIC_CONE,
  MSK12_DOMAIN_ROTATED_QUADRATIC_CONE,
  MSK12_DOMAIN_PRIMAL_EXP_CONE,
  MSK12_DOMAIN_DUAL_EXP_CONE,
  MSK12_DOMAIN_PRIMAL_POWER_CONE,
  MSK12_DOMAIN_DUAL_POWER_CONE,
  MSK12_DOMAIN_PRIMAL_GEOMETRIC_MEAN_CONE,
  MSK12_DOMAIN_DUAL_GEOMETRIC_MEAN_CONE,
  MSK12_DOMAIN_SVEC_PSD_CONE,
} MSK12_DomainType;

typedef enum MSK12_Feature_enum {
  MSK12_FEATURE_PTON,
  MSK12_FEATURE_PTS,
} MSK12_Feature;

typedef enum MSK12_ObjSense_enum {
  MSK12_OBJ_SENSE_MINIMIZE,
  MSK12_OBJ_SENSE_MAXIMIZE,
} MSK12_ObjSense;

typedef enum MSK12_SolType_enum {
  MSK12_SOL_TYPE_BASIC,
  MSK12_SOL_TYPE_INTERIOR,
  MSK12_SOL_TYPE_INTEGER,
  MSK12_SOL_TYPE_UNKNOWN,
} MSK12_SolType;

/**
 * Status of a primal or a dual solution.
 */
typedef enum MSK12_SolSta_enum {
  MSK12_SOL_STA_UNKNOWN,
  MSK12_SOL_STA_UNDEFINED,
  MSK12_SOL_STA_OPTIMAL,
  MSK12_SOL_STA_INTEGER_OPTIMAL,
  MSK12_SOL_STA_FEASIBLE,
  MSK12_SOL_STA_INFEAS_CERT,
  MSK12_SOL_STA_ILLPOSED_CERT,
} MSK12_SolSta;

typedef enum MSK12_ProSta_enum {
  MSK12_PRO_STA_UNKNOWN,
  MSK12_PRO_STA_PRIMAL_AND_DUAL_FEASIBLE,
  MSK12_PRO_STA_PRIMAL_FEASIBLE,
  MSK12_PRO_STA_DUAL_FEASIBLE,
  MSK12_PRO_STA_PRIMAL_INFEASIBLE,
  MSK12_PRO_STA_DUAL_INFEASIBLE,
  MSK12_PRO_STA_PRIMAL_AND_DUAL_INFEASIBLE,
  MSK12_PRO_STA_ILLPOSED,
  MSK12_PRO_STA_PRIMAL_INFEASIBLE_OR_UNBOUNDED,
} MSK12_ProSta;

typedef enum MSK12_Format_enum {
  MSK12_FORMAT_PTF,
  MSK12_FORMAT_TASK,
  MSK12_FORMAT_JTASK,
} MSK12_Format;

typedef enum MSK12_VariableType_enum {
  MSK12_VAR_TYPE_INTEGER,
  MSK12_VAR_TYPE_CONTINUOUS,
} MSK12_VariableType;

typedef enum MSK12_Compression_enum {
  MSK12_COMPRESS_NONE,
  MSK12_COMPRESS_GZIP,
  MSK12_COMPRESS_ZSTD,
} MSK12_Compression;

typedef enum MSK12_SolutionFormat_enum {
  MSK12_SOL_FORMAT_TASK,
  MSK12_SOL_FORMAT_JTASK,
  MSK12_SOL_FORMAT_TEXT,
} MSK12_SolutionFormat;

typedef enum MSK12_StreamType_enum {
  MSK12_STREAM_MSG,
  MSK12_STREAM_WRN,
  MSK12_STREAM_ERR,
  MSK12_STREAM_LOG,
} MSK12_StreamType;


/**
 * A value indicating the result of a function call. 0 indicates success,
 * any other value an error. The exact meaning of the values is undefined,
 * and may change between versions. Use `get_resp_descr` and `get_resp_name`
 * to get description and string representation for the code.
 */
typedef int32_t MSK12_ResCode;
/**
 * Optimizer termination code. 0 indicates normal termination, anything
 * else indicates that the optimizer termianted for other reasons than
 * optimality or valid certificate.
 */
typedef int32_t MSK12_TrmCode;
/**
 * Handle for reading from a stream via function callback.
 */
typedef void* MSK12_ReadHandle;
/**
 * Handle for writing to a stream via function callback. 
 */
typedef void* MSK12_WriteHandle;
/**
 * Stream reader function type. The reader function MUST work as follows:
 * 
 * On end-of-file, the reader function must return 0, and all subsequent calls must also return 0. If `num` is 0, the
 * function must return -1, and it does not indicate an error. Otherwise, the reader function MUST return read at least
 * one byte and at most `num` bytes. The function may perform any number of blocking reads to an underlying stream.
 * 
 * It is forbidden to access the tash object that the callback function is attached to from the callback function.
 */
typedef size_t (*MSK12_ReadFunc)(MSK12_ReadHandle h,void* dest,size_t num);
/**
 * Stream writer function type. The writer function MUST work as follows:
 * 
 * On error, the function must return 0, and all subsequent calls for the same handle must return 0. If `num` is 0, the
 * function must return 0 and it will not indicate an error. Otherwise the function must return the number of bytes written.
 * 
 * It is forbidden to access the tash object that the callback function is attached to from the callback function.
 */
typedef size_t (*MSK12_WriteFunc)(MSK12_WriteHandle h,const void* src,size_t num);
/**
 * Message stream writer function type.
 * The function must write the entire string give or fail silently.
 * It is forbidden to access the tash object that the callback function is attached to from the callback function.
 */
typedef void (*MSK12_StreamFunc)(MSK12_WriteHandle h,const char* src);
/**
 * Handle for callback functions.
 */
typedef void* MSK12_CallbackHandle;
/**
 * Handle for error and warning callback to a stream via function callback. 
 */
typedef void* MSK12_ErrorCallbackHandle;
/**
 * Function type for error and warning callback. This is attached to a task and called whenever a function call to the
 * task produces an error or a warning.
 * 
 * It is forbidden to access the task object that the callback function is attached to from the callback function.
 */
typedef int32_t (*MSK12_ErrorCallbackFunc)(MSK12_ErrorCallbackHandle h,int32_t r,const char* name,const char* desc,const char* message);
/**
 * Information callback function.
 */
typedef int32_t (*MSK12_CallbackFunc)(MSK12_CallbackHandle h,int32_t code,int32_t len_iinf,const int32_t* iinf,int32_t len_liinf,const int64_t* liinf,int32_t len_dinf,const double* dinf);
/**
 * Integer solution callback function.
 */
typedef void (*MSK12_IntSolCallbackFunc)(MSK12_CallbackHandle handle,int32_t num,double primal_obj,const double* xx);
/**
 * Handle for memory allocation functions.
 */
typedef void* MSK12_AllocHandle;
/**
 * Memory allocation function
 */
typedef int8_t* (*MSK12_AllocFunc)(MSK12_AllocHandle handle,size_t num);


#ifdef __cplusplus
extern "C" {
#endif

/**

 * Get the name of a callback code 
 * 
 * # Arguments
 * - `code` 
 */
typedef const char* (*MSK12_get_callback_code_name_func_t)(int32_t code);
extern MSK12_get_callback_code_name_func_t MSK12_get_callback_code_name_ptr;
const char* MSK12_get_callback_code_name(int32_t code);

/**

 * Get string representing the given response code.
 * 
 * # Arguments
 * - `r` 
 */
typedef const char* (*MSK12_get_resp_name_func_t)(MSK12_ResCode r);
extern MSK12_get_resp_name_func_t MSK12_get_resp_name_ptr;
const char* MSK12_get_resp_name(MSK12_ResCode r);

/**

 * Get string with a description of the given response code.
 * 
 * # Arguments
 * - `r` 
 */
typedef const char* (*MSK12_get_resp_descr_func_t)(MSK12_ResCode r);
extern MSK12_get_resp_descr_func_t MSK12_get_resp_descr_ptr;
const char* MSK12_get_resp_descr(MSK12_ResCode r);

/**

 * Get the last error response code 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef MSK12_ResCode (*MSK12_get_last_resp_func_t)(MSK12_Task_t task);
extern MSK12_get_last_resp_func_t MSK12_get_last_resp_ptr;
MSK12_ResCode MSK12_get_last_resp(MSK12_Task_t task);

/**

 * Get a string with name of the last error message recorded in the task.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `buf[buf_len]` (out) Last message will be copied here, truncated to `buf_len` including trailing 0.
 * - `buf_len` Length of target buffer
 */
typedef MSK12_ResCode (*MSK12_get_last_resp_msg_func_t)(MSK12_Task_t task,char* buf,size_t buf_len);
extern MSK12_get_last_resp_msg_func_t MSK12_get_last_resp_msg_ptr;
MSK12_ResCode MSK12_get_last_resp_msg(
    MSK12_Task_t task,
    char* buf,
    size_t buf_len);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef size_t (*MSK12_get_last_resp_msg_len_func_t)(MSK12_Task_t task);
extern MSK12_get_last_resp_msg_len_func_t MSK12_get_last_resp_msg_len_ptr;
size_t MSK12_get_last_resp_msg_len(MSK12_Task_t task);

/**

 * Get string representing the given termination code.
 * 
 * # Arguments
 * - `trm` 
 */
typedef const char* (*MSK12_get_trm_name_func_t)(MSK12_TrmCode trm);
extern MSK12_get_trm_name_func_t MSK12_get_trm_name_ptr;
const char* MSK12_get_trm_name(MSK12_TrmCode trm);

/**

 * Get string with a description if the given termination code.
 * 
 * # Arguments
 * - `trm` 
 */
typedef const char* (*MSK12_get_trm_descr_func_t)(MSK12_TrmCode trm);
extern MSK12_get_trm_descr_func_t MSK12_get_trm_descr_ptr;
const char* MSK12_get_trm_descr(MSK12_TrmCode trm);

/**

 * Create a new task with data from another task. Thus will copy
 * 
 * - All problem data,
 * - All solutions including solution status,
 * - Parameters
 * - Information items
 * 
 * What may NOT be copied
 * 
 * - Callback functions and attached log files
 * - Last error message
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef MSK12_Task_t (*MSK12_new_task_from_task_func_t)(MSK12_Task_t task);
extern MSK12_new_task_from_task_func_t MSK12_new_task_from_task_ptr;
MSK12_Task_t MSK12_new_task_from_task(MSK12_Task_t task);

/**

 * Create a new task from a file. Accepted file formats are PTF, JSON and TASK, and accepted compressions are plain, zstd and gzip.
 */
typedef MSK12_Task_t (*MSK12_new_task_func_t)();
extern MSK12_new_task_func_t MSK12_new_task_ptr;
MSK12_Task_t MSK12_new_task();

/**

 * Delete task. This is assumed to always succeed.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef void (*MSK12_delete_task_func_t)(MSK12_Task_t task);
extern MSK12_delete_task_func_t MSK12_delete_task_ptr;
void MSK12_delete_task(MSK12_Task_t task);

/**

 * Reserve space for scalar variables. This is to be considered a _hint_, not a requirement. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `add_num` Number of item to add
 */
typedef MSK12_ResCode (*MSK12_reserve_num_var_func_t)(MSK12_Task_t task,int32_t add_num);
extern MSK12_reserve_num_var_func_t MSK12_reserve_num_var_ptr;
MSK12_ResCode MSK12_reserve_num_var(
    MSK12_Task_t task,
    int32_t add_num);

/**

 * Reserve space for semidefinite variables. This is to be considered a _hint_, not a requirement.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_barvar` Number of variables
 */
typedef MSK12_ResCode (*MSK12_reserve_num_barvar_func_t)(MSK12_Task_t task,int32_t num_barvar);
extern MSK12_reserve_num_barvar_func_t MSK12_reserve_num_barvar_ptr;
MSK12_ResCode MSK12_reserve_num_barvar(
    MSK12_Task_t task,
    int32_t num_barvar);

/**

 * Reserve space for constraints. This is to be considered a _hint_, not a requirement.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_con` Number of constraints
 */
typedef MSK12_ResCode (*MSK12_reserve_num_con_func_t)(MSK12_Task_t task,int32_t num_con);
extern MSK12_reserve_num_con_func_t MSK12_reserve_num_con_ptr;
MSK12_ResCode MSK12_reserve_num_con(
    MSK12_Task_t task,
    int32_t num_con);

/**

 * Reserve space for afes. This is to be considered a _hint_, not a requirement.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_row` Number of affine rows
 */
typedef MSK12_ResCode (*MSK12_reserve_num_row_func_t)(MSK12_Task_t task,int64_t num_row);
extern MSK12_reserve_num_row_func_t MSK12_reserve_num_row_ptr;
MSK12_ResCode MSK12_reserve_num_row(
    MSK12_Task_t task,
    int64_t num_row);

/**

 * Reserve space for coefficient matrix non-zeros. This is to be considered a _hint_, not a requirement.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_nz` 
 */
typedef MSK12_ResCode (*MSK12_reserve_num_nz_func_t)(MSK12_Task_t task,int64_t num_nz);
extern MSK12_reserve_num_nz_func_t MSK12_reserve_num_nz_ptr;
MSK12_ResCode MSK12_reserve_num_nz(
    MSK12_Task_t task,
    int64_t num_nz);

/**

 * Reserve space for domains. This is to be considered a _hint_, not a requirement.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_dom` Number of domains
 */
typedef MSK12_ResCode (*MSK12_reserve_num_dom_func_t)(MSK12_Task_t task,int64_t num_dom);
extern MSK12_reserve_num_dom_func_t MSK12_reserve_num_dom_ptr;
MSK12_ResCode MSK12_reserve_num_dom(
    MSK12_Task_t task,
    int64_t num_dom);

/**

 * Reserve space for symmetric matrixes. This is to be considered a _hint_, not a requirement.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_symmat` Number of symmetric matrixes
 */
typedef MSK12_ResCode (*MSK12_reserve_num_symmat_func_t)(MSK12_Task_t task,int64_t num_symmat);
extern MSK12_reserve_num_symmat_func_t MSK12_reserve_num_symmat_ptr;
MSK12_ResCode MSK12_reserve_num_symmat(
    MSK12_Task_t task,
    int64_t num_symmat);

/**

 * Reserve space for symmetric matrix variables. This is to be considered a _hint_, not a requirement.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_nz` 
 */
typedef MSK12_ResCode (*MSK12_reserve_num_symmat_nz_func_t)(MSK12_Task_t task,int64_t num_nz);
extern MSK12_reserve_num_symmat_nz_func_t MSK12_reserve_num_symmat_nz_ptr;
MSK12_ResCode MSK12_reserve_num_symmat_nz(
    MSK12_Task_t task,
    int64_t num_nz);

/**

 * Get number of scalar variables. Cannot fail.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int32_t (*MSK12_get_num_var_func_t)(MSK12_Task_t task);
extern MSK12_get_num_var_func_t MSK12_get_num_var_ptr;
int32_t MSK12_get_num_var(MSK12_Task_t task);

/**

 * Get number of semidefinite variables. Cannot fail.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int32_t (*MSK12_get_num_barvar_func_t)(MSK12_Task_t task);
extern MSK12_get_num_barvar_func_t MSK12_get_num_barvar_ptr;
int32_t MSK12_get_num_barvar(MSK12_Task_t task);

/**

 * Get number of domains. Cannot fail. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int64_t (*MSK12_get_num_domain_func_t)(MSK12_Task_t task);
extern MSK12_get_num_domain_func_t MSK12_get_num_domain_ptr;
int64_t MSK12_get_num_domain(MSK12_Task_t task);

/**

 * Get number of affine rows. Cannot fail. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int64_t (*MSK12_get_num_row_func_t)(MSK12_Task_t task);
extern MSK12_get_num_row_func_t MSK12_get_num_row_ptr;
int64_t MSK12_get_num_row(MSK12_Task_t task);

/**

 * Get number of symmetric matrixes. Cannot fail. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int64_t (*MSK12_get_num_symmat_func_t)(MSK12_Task_t task);
extern MSK12_get_num_symmat_func_t MSK12_get_num_symmat_ptr;
int64_t MSK12_get_num_symmat(MSK12_Task_t task);

/**

 * Get number of constraints. Cannot fail. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int64_t (*MSK12_get_num_con_func_t)(MSK12_Task_t task);
extern MSK12_get_num_con_func_t MSK12_get_num_con_ptr;
int64_t MSK12_get_num_con(MSK12_Task_t task);

/**

 * Get number of disjunctive constraints. Cannot fail. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int64_t (*MSK12_get_num_djc_func_t)(MSK12_Task_t task);
extern MSK12_get_num_djc_func_t MSK12_get_num_djc_ptr;
int64_t MSK12_get_num_djc(MSK12_Task_t task);

/**

 * Append scalar variables.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_var` Number of variables
 */
typedef MSK12_ResCode (*MSK12_append_vars_func_t)(MSK12_Task_t task,int32_t num_var);
extern MSK12_append_vars_func_t MSK12_append_vars_ptr;
MSK12_ResCode MSK12_append_vars(
    MSK12_Task_t task,
    int32_t num_var);

/**

 * Append a number of empty affine rows
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_row` Number of affine rows
 */
typedef MSK12_ResCode (*MSK12_append_rows_func_t)(MSK12_Task_t task,int64_t num_row);
extern MSK12_append_rows_func_t MSK12_append_rows_ptr;
MSK12_ResCode MSK12_append_rows(
    MSK12_Task_t task,
    int64_t num_row);

/**

 * Append a single positive semi-definite variable.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dim` Dimension
 */
typedef MSK12_ResCode (*MSK12_append_barvar_func_t)(MSK12_Task_t task,int32_t dim);
extern MSK12_append_barvar_func_t MSK12_append_barvar_ptr;
MSK12_ResCode MSK12_append_barvar(
    MSK12_Task_t task,
    int32_t dim);

/**

 * Append multiple positive semi-definite variables with the given dimensions.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_barvar` Number of variables
 * - `dims[num_barvar]` (in) Array of dimensionms
 */
typedef MSK12_ResCode (*MSK12_append_barvars_func_t)(MSK12_Task_t task,int32_t num_barvar,const int32_t* dims);
extern MSK12_append_barvars_func_t MSK12_append_barvars_ptr;
MSK12_ResCode MSK12_append_barvars(
    MSK12_Task_t task,
    int32_t num_barvar,
    const int32_t* dims);

/**

 * Append a single symmetric matrix.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dim` Dimension
 * - `nnz` Number of nonzeros
 * - `symmat_i[nnz]` (in) Symmetric matrix row subscripts
 * - `symmat_j[nnz]` (in) Symmetric matrix column subscripts
 * - `symmat_val[nnz]` (in) Symmetric matrix values
 */
typedef MSK12_ResCode (*MSK12_append_symmat_func_t)(MSK12_Task_t task,int32_t dim,int64_t nnz,const int32_t* symmat_i,const int32_t* symmat_j,const double* symmat_val);
extern MSK12_append_symmat_func_t MSK12_append_symmat_ptr;
MSK12_ResCode MSK12_append_symmat(
    MSK12_Task_t task,
    int32_t dim,
    int64_t nnz,
    const int32_t* symmat_i,
    const int32_t* symmat_j,
    const double* symmat_val);

/**

 * Append a list of symmetric matrixes.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_symmat` Number of symmetric matrixes
 * - `dim[num_symmat]` (in) Dimension
 * - `nnz[num_symmat]` (in) Number of nonzeros
 * - `symmat_i[nnz]` (in) Symmetric matrix row subscripts
 * - `symmat_j[nnz]` (in) Symmetric matrix column subscripts
 * - `symmat_val[nnz]` (in) Symmetric matrix values
 */
typedef MSK12_ResCode (*MSK12_append_symmats_func_t)(MSK12_Task_t task,int64_t num_symmat,const int32_t* dim,const int64_t* nnz,const int32_t* symmat_i,const int32_t* symmat_j,const double* symmat_val);
extern MSK12_append_symmats_func_t MSK12_append_symmats_ptr;
MSK12_ResCode MSK12_append_symmats(
    MSK12_Task_t task,
    int64_t num_symmat,
    const int32_t* dim,
    const int64_t* nnz,
    const int32_t* symmat_i,
    const int32_t* symmat_j,
    const double* symmat_val);

/**

 * Append a number of empty constraints, initially they will have domain `null`.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_con` Number of constraints
 */
typedef MSK12_ResCode (*MSK12_append_empty_cons_func_t)(MSK12_Task_t task,int64_t num_con);
extern MSK12_append_empty_cons_func_t MSK12_append_empty_cons_ptr;
MSK12_ResCode MSK12_append_empty_cons(
    MSK12_Task_t task,
    int64_t num_con);

/**

 * Append a number of empty disjunctive constraints.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_djc` Number of disjunctive constraints
 */
typedef MSK12_ResCode (*MSK12_append_empty_djcs_func_t)(MSK12_Task_t task,int64_t num_djc);
extern MSK12_append_empty_djcs_func_t MSK12_append_empty_djcs_ptr;
MSK12_ResCode MSK12_append_empty_djcs(
    MSK12_Task_t task,
    int64_t num_djc);

/**

 * Set variable type to integer or continuous.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `j` 
 * - `var_type` 
 */
typedef MSK12_ResCode (*MSK12_put_var_type_func_t)(MSK12_Task_t task,int32_t j,MSK12_VariableType var_type);
extern MSK12_put_var_type_func_t MSK12_put_var_type_ptr;
MSK12_ResCode MSK12_put_var_type(
    MSK12_Task_t task,
    int32_t j,
    MSK12_VariableType var_type);

/**

 * Set variable types in a slice to integer or continuous.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `var_types[num_var]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_var_type_slice_func_t)(MSK12_Task_t task,int32_t first_var,int32_t num_var,const MSK12_VariableType* var_types);
extern MSK12_put_var_type_slice_func_t MSK12_put_var_type_slice_ptr;
MSK12_ResCode MSK12_put_var_type_slice(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    const MSK12_VariableType* var_types);

/**

 * Set variable types for all entries in a slice to a single value.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `var_type` 
 */
typedef MSK12_ResCode (*MSK12_put_var_type_slice_value_func_t)(MSK12_Task_t task,int32_t first_var,int32_t num_var,MSK12_VariableType var_type);
extern MSK12_put_var_type_slice_value_func_t MSK12_put_var_type_slice_value_ptr;
MSK12_ResCode MSK12_put_var_type_slice_value(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    MSK12_VariableType var_type);

/**

 * Set variable types in a list to integer or continuous.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_var` Number of variables
 * - `var_idxs[num_var]` (in) Array of variable indexes
 * - `var_types[num_var]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_var_type_list_func_t)(MSK12_Task_t task,int32_t num_var,const int32_t* var_idxs,const MSK12_VariableType* var_types);
extern MSK12_put_var_type_list_func_t MSK12_put_var_type_list_ptr;
MSK12_ResCode MSK12_put_var_type_list(
    MSK12_Task_t task,
    int32_t num_var,
    const int32_t* var_idxs,
    const MSK12_VariableType* var_types);

/**

 * Get variable type as integer or continuous.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `var_idx` Variable index
 * - `var_type[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_var_type_func_t)(MSK12_Task_t task,int32_t var_idx,MSK12_VariableType var_type[1]);
extern MSK12_get_var_type_func_t MSK12_get_var_type_ptr;
MSK12_ResCode MSK12_get_var_type(
    MSK12_Task_t task,
    int32_t var_idx,
    MSK12_VariableType var_type[1]);

/**

 * Get variable slice types as integer or continuous.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `var_types[num_var]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_var_type_slice_func_t)(MSK12_Task_t task,int32_t first_var,int32_t num_var,MSK12_VariableType* var_types);
extern MSK12_get_var_type_slice_func_t MSK12_get_var_type_slice_ptr;
MSK12_ResCode MSK12_get_var_type_slice(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    MSK12_VariableType* var_types);

/**

 * Set variable bounds.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `var_idx` Variable index
 * - `low` Lower bound
 * - `upr` Upper bound
 */
typedef MSK12_ResCode (*MSK12_put_var_bound_func_t)(MSK12_Task_t task,int32_t var_idx,double low,double upr);
extern MSK12_put_var_bound_func_t MSK12_put_var_bound_ptr;
MSK12_ResCode MSK12_put_var_bound(
    MSK12_Task_t task,
    int32_t var_idx,
    double low,
    double upr);

/**

 * Set variable bounds for a slice.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `low[num_var]` (in) Lower bound
 * - `upr[num_var]` (in) Upper bound
 */
typedef MSK12_ResCode (*MSK12_put_var_bound_slice_func_t)(MSK12_Task_t task,int32_t first_var,int32_t num_var,const double* low,const double* upr);
extern MSK12_put_var_bound_slice_func_t MSK12_put_var_bound_slice_ptr;
MSK12_ResCode MSK12_put_var_bound_slice(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    const double* low,
    const double* upr);

/**

 * Set identical bound for a slice of variables.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `low` Lower bound
 * - `upr` Upper bound
 */
typedef MSK12_ResCode (*MSK12_put_var_bound_slice_value_func_t)(MSK12_Task_t task,int32_t first_var,int32_t num_var,double low,double upr);
extern MSK12_put_var_bound_slice_value_func_t MSK12_put_var_bound_slice_value_ptr;
MSK12_ResCode MSK12_put_var_bound_slice_value(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    double low,
    double upr);

/**

 * Set variable bounds.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `var_idx` Variable index
 * - `low` Lower bound
 */
typedef MSK12_ResCode (*MSK12_put_var_low_bound_func_t)(MSK12_Task_t task,int32_t var_idx,double low);
extern MSK12_put_var_low_bound_func_t MSK12_put_var_low_bound_ptr;
MSK12_ResCode MSK12_put_var_low_bound(
    MSK12_Task_t task,
    int32_t var_idx,
    double low);

/**

 * Set variable bounds for a slice.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `low[num_var]` (in) Lower bound
 */
typedef MSK12_ResCode (*MSK12_put_var_low_bound_slice_func_t)(MSK12_Task_t task,int32_t first_var,int32_t num_var,const double* low);
extern MSK12_put_var_low_bound_slice_func_t MSK12_put_var_low_bound_slice_ptr;
MSK12_ResCode MSK12_put_var_low_bound_slice(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    const double* low);

/**

 * Set identical bound for a slice of variables.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `low` Lower bound
 */
typedef MSK12_ResCode (*MSK12_put_var_low_bound_slice_value_func_t)(MSK12_Task_t task,int32_t first_var,int32_t num_var,double low);
extern MSK12_put_var_low_bound_slice_value_func_t MSK12_put_var_low_bound_slice_value_ptr;
MSK12_ResCode MSK12_put_var_low_bound_slice_value(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    double low);

/**

 * Set variable bounds.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `var_idx` Variable index
 * - `upr` Upper bound
 */
typedef MSK12_ResCode (*MSK12_put_var_upr_bound_func_t)(MSK12_Task_t task,int32_t var_idx,double upr);
extern MSK12_put_var_upr_bound_func_t MSK12_put_var_upr_bound_ptr;
MSK12_ResCode MSK12_put_var_upr_bound(
    MSK12_Task_t task,
    int32_t var_idx,
    double upr);

/**

 * Set variable bounds for a slice.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `upr[num_var]` (in) Upper bound
 */
typedef MSK12_ResCode (*MSK12_put_var_upr_bound_slice_func_t)(MSK12_Task_t task,int32_t first_var,int32_t num_var,const double* upr);
extern MSK12_put_var_upr_bound_slice_func_t MSK12_put_var_upr_bound_slice_ptr;
MSK12_ResCode MSK12_put_var_upr_bound_slice(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    const double* upr);

/**

 * Set identical bound for a slice of variables.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `upr` Upper bound
 */
typedef MSK12_ResCode (*MSK12_put_var_upr_bound_slice_value_func_t)(MSK12_Task_t task,int32_t first_var,int32_t num_var,double upr);
extern MSK12_put_var_upr_bound_slice_value_func_t MSK12_put_var_upr_bound_slice_value_ptr;
MSK12_ResCode MSK12_put_var_upr_bound_slice_value(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    double upr);

/**

 * Get variable bounds.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `var_idx` Variable index
 * - `low[1]` (out) Lower bound
 * - `upr[1]` (out) Upper bound
 */
typedef MSK12_ResCode (*MSK12_get_var_bound_func_t)(MSK12_Task_t task,int32_t var_idx,double low[1],double upr[1]);
extern MSK12_get_var_bound_func_t MSK12_get_var_bound_ptr;
MSK12_ResCode MSK12_get_var_bound(
    MSK12_Task_t task,
    int32_t var_idx,
    double low[1],
    double upr[1]);

/**

 * Get variable bounds for a slice.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `low[num_var]` (out) Lower bound
 * - `upr[num_var]` (out) Upper bound
 */
typedef MSK12_ResCode (*MSK12_get_var_bound_slice_func_t)(MSK12_Task_t task,int32_t first_var,int32_t num_var,double* low,double* upr);
extern MSK12_get_var_bound_slice_func_t MSK12_get_var_bound_slice_ptr;
MSK12_ResCode MSK12_get_var_bound_slice(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    double* low,
    double* upr);

/**

 * Compute the number of scalar elements in a slice of semidefinite variables.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_barvar` First semidefinite variable index in a slice
 * - `num_barvar` Number of variables
 * - `num_elm[1]` (out) Number of positive semidefinite non-zero entries
 */
typedef MSK12_ResCode (*MSK12_get_barvar_slice_num_elm_func_t)(MSK12_Task_t task,int32_t first_barvar,int32_t num_barvar,int64_t num_elm[1]);
extern MSK12_get_barvar_slice_num_elm_func_t MSK12_get_barvar_slice_num_elm_ptr;
MSK12_ResCode MSK12_get_barvar_slice_num_elm(
    MSK12_Task_t task,
    int32_t first_barvar,
    int32_t num_barvar,
    int64_t num_elm[1]);

/**

 * Get dimension of semidefinite variable
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `barvar_idx` Positive semi-definite variable index
 * - `dim[1]` (out) Dimension
 */
typedef MSK12_ResCode (*MSK12_get_barvar_dim_func_t)(MSK12_Task_t task,int32_t barvar_idx,int32_t dim[1]);
extern MSK12_get_barvar_dim_func_t MSK12_get_barvar_dim_ptr;
MSK12_ResCode MSK12_get_barvar_dim(
    MSK12_Task_t task,
    int32_t barvar_idx,
    int32_t dim[1]);

/**

 * Get dimensions of a slice of semidefinite variables.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_barvar` First semidefinite variable index in a slice
 * - `num_barvar` Number of variables
 * - `dim[num_barvar]` (out) Dimension
 */
typedef MSK12_ResCode (*MSK12_get_barvar_slice_dims_func_t)(MSK12_Task_t task,int32_t first_barvar,int32_t num_barvar,int32_t* dim);
extern MSK12_get_barvar_slice_dims_func_t MSK12_get_barvar_slice_dims_ptr;
MSK12_ResCode MSK12_get_barvar_slice_dims(
    MSK12_Task_t task,
    int32_t first_barvar,
    int32_t num_barvar,
    int32_t* dim);

/**

 * Return index of a domain of the requested type
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_type` 
 * - `dim` Dimension
 * - `num_alpha` Number of alpha values in array
 * - `alpha[dim]` (out) Array if alpha values for power cone domain
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_domain_func_t)(MSK12_Task_t task,MSK12_DomainType dom_type,int64_t dim,int32_t num_alpha,double* alpha,int64_t dom_idx[1]);
extern MSK12_get_domain_func_t MSK12_get_domain_ptr;
MSK12_ResCode MSK12_get_domain(
    MSK12_Task_t task,
    MSK12_DomainType dom_type,
    int64_t dim,
    int32_t num_alpha,
    double* alpha,
    int64_t dom_idx[1]);

/**

 * Return index of the empty domain. Only one empty domain is created, so if one already exists, that one is returned instead of creating a new domain
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_domain_empty_func_t)(MSK12_Task_t task,int64_t dom_idx[1]);
extern MSK12_get_domain_empty_func_t MSK12_get_domain_empty_ptr;
MSK12_ResCode MSK12_get_domain_empty(
    MSK12_Task_t task,
    int64_t dom_idx[1]);

/**

 * Return index of an `rzero` domain. Only one `rzero` domain is created, so if one already exists, that one is returned instead of creating a new domain
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_domain_rzero_func_t)(MSK12_Task_t task,int64_t dom_idx[1]);
extern MSK12_get_domain_rzero_func_t MSK12_get_domain_rzero_ptr;
MSK12_ResCode MSK12_get_domain_rzero(
    MSK12_Task_t task,
    int64_t dom_idx[1]);

/**

 * Return index of an `rplus` domain. Only one `rplus` domain is created, so if one already exists, that one is returned instead of creating a new domain
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_domain_rplus_func_t)(MSK12_Task_t task,int64_t dom_idx[1]);
extern MSK12_get_domain_rplus_func_t MSK12_get_domain_rplus_ptr;
MSK12_ResCode MSK12_get_domain_rplus(
    MSK12_Task_t task,
    int64_t dom_idx[1]);

/**

 * Return index of an `rminus` domain. Only one `rminus` domain is created, so if one already exists, that one is returned instead of creating a new domain
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_domain_rminus_func_t)(MSK12_Task_t task,int64_t dom_idx[1]);
extern MSK12_get_domain_rminus_func_t MSK12_get_domain_rminus_ptr;
MSK12_ResCode MSK12_get_domain_rminus(
    MSK12_Task_t task,
    int64_t dom_idx[1]);

/**

 * Return index of an `r` domain. Only one `r` domain is created, so if one already exists, that one is returned instead of creating a new domain
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_domain_r_func_t)(MSK12_Task_t task,int64_t dom_idx[1]);
extern MSK12_get_domain_r_func_t MSK12_get_domain_r_ptr;
MSK12_ResCode MSK12_get_domain_r(
    MSK12_Task_t task,
    int64_t dom_idx[1]);

/**

 * Return the index of a quadratic cone domain of the given size.
 * 
 * The quadratic cone of size \\(n\\) is defined as
 * $$
 * \\left\\{x\\in\\real^n~:~x_0 \\geq \\sqrt{\\sum_{i=1}^{n-1} x_i^2}\\right\\}
 * $$
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `n` 
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_domain_quadratic_cone_func_t)(MSK12_Task_t task,int64_t n,int64_t dom_idx[1]);
extern MSK12_get_domain_quadratic_cone_func_t MSK12_get_domain_quadratic_cone_ptr;
MSK12_ResCode MSK12_get_domain_quadratic_cone(
    MSK12_Task_t task,
    int64_t n,
    int64_t dom_idx[1]);

/**

 * Return the index of a rotated quadratic cone domain of the given size.
 * 
 * The rotated quadratic cone of size \\(n\\) is defined as
 * $$
 * \\left\{ x\\in \\real^3 ~:~ x_0 \\geq x_1 e^{x_2/x_1},\\ x_0,x_1\\geq; 0 \\right\\}
 * $$
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `n` 
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_domain_rotated_quadratic_cone_func_t)(MSK12_Task_t task,int64_t n,int64_t dom_idx[1]);
extern MSK12_get_domain_rotated_quadratic_cone_func_t MSK12_get_domain_rotated_quadratic_cone_ptr;
MSK12_ResCode MSK12_get_domain_rotated_quadratic_cone(
    MSK12_Task_t task,
    int64_t n,
    int64_t dom_idx[1]);

/**

 * Return index if an `primal_exponential` domain. Only one
 * `primal_exponential` domain is created, so if one already exists,
 * that one is returned instead of creating a new domain.
 * 
 * The primal exponential cone is defined as
 * $$
 * \\left\{ x\\in \\real^3 ~:~ x_0 \\geq x_1 e^{x_2/x_1},\\ x_0,x_1> 0 \\right\\}
 * $$
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_domain_primal_exponential_cone_func_t)(MSK12_Task_t task,int64_t dom_idx[1]);
extern MSK12_get_domain_primal_exponential_cone_func_t MSK12_get_domain_primal_exponential_cone_ptr;
MSK12_ResCode MSK12_get_domain_primal_exponential_cone(
    MSK12_Task_t task,
    int64_t dom_idx[1]);

/**

 * Return index if an `dual_exponential` domain. Only one
 * `dual_exponential` domain is created, so if one already exists, that
 * one is returned instead of creating a new domain
 * 
 * The dual exponential cone is defined as
 * $$
 * \\left\\{ x\\in \\real^3 ~:~ x_0 \\geq -x_2 e^{-1} e^{x_1/x_2},\\ x_0> 0,\\ x_2< 0 \\right\\}
 * $$
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_domain_dual_exponential_cone_func_t)(MSK12_Task_t task,int64_t dom_idx[1]);
extern MSK12_get_domain_dual_exponential_cone_func_t MSK12_get_domain_dual_exponential_cone_ptr;
MSK12_ResCode MSK12_get_domain_dual_exponential_cone(
    MSK12_Task_t task,
    int64_t dom_idx[1]);

/**

 * Return the index of a new primal power cone.
 * 
 * The primal power cone domain of dimension \\(n\\), with \\(n_\\ell\\) variables appearing on the left-hand side, where \\(n_\\ell\\) is the length of \\(\\alpha\\), and with a homogenous sequence of exponents \\(\\alpha_0,\\ldots,\\alpha_{n_\\ell-1}\\).
 * 
 * Formally, let \\(s = \\sum_i \\alpha_i\\) and \\(\\beta_i = \\alpha_i / s\\), so that \\(\\sum_i \\beta_i=1\\). Then the primal power cone is defined as follows:
 * 
 * $$
 * \\left\\{ x\\in \\real^n ~:~ \\prod_{i=0}^{n_\\ell-1} x_i^{\\beta_i} \\geq \\sqrt{\\sum_{j=n_\\ell}^{n-1}x_j^2},\\ x_0\\ldots,x_{n_\\ell-1}\\geq 0 \\right\\}
 * $$
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `n` 
 * - `num_alpha` Number of alpha values in array
 * - `alpha[num_alpha]` (in) Array if alpha values for power cone domain
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_domain_primal_power_cone_func_t)(MSK12_Task_t task,int64_t n,int64_t num_alpha,const double* alpha,int64_t dom_idx[1]);
extern MSK12_get_domain_primal_power_cone_func_t MSK12_get_domain_primal_power_cone_ptr;
MSK12_ResCode MSK12_get_domain_primal_power_cone(
    MSK12_Task_t task,
    int64_t n,
    int64_t num_alpha,
    const double* alpha,
    int64_t dom_idx[1]);

/**

 * Return the index of a new dual power cone.
 * 
 * Appends the dual power cone domain of dimension \\(n\\), with \\(n_\\ell\\) variables appearing on the left-hand side, where \\(n_\\ell\\) is the length of \\(\\alpha\\), and with a homogenous sequence of exponents \\(\\alpha_0,\\ldots,\\alpha_{n_\\ell-1}\\).
 * 
 * Formally, let \\(s = \\sum_i \\alpha_i\\) and \\(\\beta_i = \\alpha_i / s\\), so that \\(\\sum_i \\beta_i=1\\). Then the dual power cone is defined as follows:
 * 
 * $$
 * \\left\\{ x\\in \\real^n ~:~ \\prod_{i=0}^{n_\\ell-1} \\left(\\frac{x_i}{\\beta_i}\\right)^{\\beta_i} \\geq \\sqrt{\\sum_{j=n_\\ell}^{n-1}x_j^2},\\ x_0\\ldots,x_{n_\\ell-1}\\geq 0 \\right\\}
 * $$
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `n` 
 * - `num_alpha` Number of alpha values in array
 * - `alpha[num_alpha]` (in) Array if alpha values for power cone domain
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_domain_dual_power_cone_func_t)(MSK12_Task_t task,int64_t n,int64_t num_alpha,const double* alpha,int64_t dom_idx[1]);
extern MSK12_get_domain_dual_power_cone_func_t MSK12_get_domain_dual_power_cone_ptr;
MSK12_ResCode MSK12_get_domain_dual_power_cone(
    MSK12_Task_t task,
    int64_t n,
    int64_t num_alpha,
    const double* alpha,
    int64_t dom_idx[1]);

/**

 * Get the index of a new primal geometric mean cone.
 * 
 * The primal geometric mean cone is defined as
 * $$
 * \\left\\{ x\\in \\real^n ~:~ \\left(\\prod_{i=0}^{n-2} x_i\\right)^{1/(n-1)} \\geq |x_{n-1}|,\\ x_0\\ldots,x_{n-2}\\geq 0 \\right\\}
 * $$
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `n` 
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_domain_primal_geometric_mean_cone_func_t)(MSK12_Task_t task,int64_t n,int64_t dom_idx[1]);
extern MSK12_get_domain_primal_geometric_mean_cone_func_t MSK12_get_domain_primal_geometric_mean_cone_ptr;
MSK12_ResCode MSK12_get_domain_primal_geometric_mean_cone(
    MSK12_Task_t task,
    int64_t n,
    int64_t dom_idx[1]);

/**

 * Get the index of a new dual geometric mean cone.
 * 
 * The dual geometric mean cone is defined as
 * $$
 * \\left\\{ x\\in \\real^n ~:~ (n-1) \\left(\\prod_{i=0}^{n-2} x_i\\right)^{1/(n-1)} \\geq |x_{n-1}|,\\ x_0,\\ldots,x_{n-2}\\geq 0 \\right\\}
 * $$
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `n` 
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_domain_dual_geometric_mean_cone_func_t)(MSK12_Task_t task,int64_t n,int64_t dom_idx[1]);
extern MSK12_get_domain_dual_geometric_mean_cone_func_t MSK12_get_domain_dual_geometric_mean_cone_ptr;
MSK12_ResCode MSK12_get_domain_dual_geometric_mean_cone(
    MSK12_Task_t task,
    int64_t n,
    int64_t dom_idx[1]);

/**

 * Get the index of a new scaled vectorized PSD cone.
 * 
 * The domain consisting of vectors of length \\(n=d(d+1)/2\\) defined as follows
 * 
 * $$
 * \\{(x_1,\\ldots,x_{d(d+1)/2})\\in \\real^n~:~ \\mathrm{sMat}(x)\\in\\PSD^d\\} = \\{\\mathrm{sVec}(X)~:~X\\in\\PSD^d\\},
 * $$
 * 
 * where
 * 
 * $$
 * \\mathrm{sVec}(X) = (X_{11},\\sqrt{2}X_{21},\\ldots,\\sqrt{2}X_{d1},X_{22},\\sqrt{2}X_{32},\\ldots,X_{dd}),
 * $$
 * 
 * and
 * 
 * $$
 *     \\mathrm{sMat}(x) = \\left[\\begin{array}{cccc}
 *         x_1             & x_2/\\sqrt{2}      & \\cdots & x_{d}/\\sqrt{2} \\\\
 *         x_2/\\sqrt{2}   & x_{d+1}            & \\cdots & x_{2d-1}/\\sqrt{2} \\\\
 *         \\cdots         & \\cdots            & \\cdots & \\cdots \\\\
 *         x_{d}/\\sqrt{2} & x_{2d-1}/\\sqrt{2} & \\cdots & x_{d(d+1)/2}
 *     \\end{array}\\right].
 * $$
 * 
 * In other words, the domain consists of vectorizations of the lower-triangular part of a positive semidefinite matrix, with the non-diagonal elements additionally rescaled.
 * 
 * This domain is a self-dual cone.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `n` The cone dimension. Note that only values such that \\(n\\cdot(n+1)/2\\) for some integer \\(d\\) are valid.
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_domain_svecpsd_cone_func_t)(MSK12_Task_t task,int64_t n,int64_t dom_idx[1]);
extern MSK12_get_domain_svecpsd_cone_func_t MSK12_get_domain_svecpsd_cone_ptr;
MSK12_ResCode MSK12_get_domain_svecpsd_cone(
    MSK12_Task_t task,
    int64_t n,
    int64_t dom_idx[1]);

/**

 * Get domain information
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_idx` Index of the domain
 * - `dom_type[1]` (out) 
 * - `size[1]` (out) 
 * - `num_alpha[1]` (out) Number of alpha values in array
 */
typedef MSK12_ResCode (*MSK12_get_domain_info_func_t)(MSK12_Task_t task,int64_t dom_idx,MSK12_DomainType dom_type[1],int64_t size[1],int32_t num_alpha[1]);
extern MSK12_get_domain_info_func_t MSK12_get_domain_info_ptr;
MSK12_ResCode MSK12_get_domain_info(
    MSK12_Task_t task,
    int64_t dom_idx,
    MSK12_DomainType dom_type[1],
    int64_t size[1],
    int32_t num_alpha[1]);

/**

 * For primal and dual power domains, get domain alpha vector
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_idx` Index of the domain
 * - `num_alpha` Number of alpha values in array
 * - `alpha[num_alpha]` (out) Array if alpha values for power cone domain
 */
typedef MSK12_ResCode (*MSK12_get_domain_alpha_func_t)(MSK12_Task_t task,int64_t dom_idx,int64_t num_alpha,double* alpha);
extern MSK12_get_domain_alpha_func_t MSK12_get_domain_alpha_ptr;
MSK12_ResCode MSK12_get_domain_alpha(
    MSK12_Task_t task,
    int64_t dom_idx,
    int64_t num_alpha,
    double* alpha);

/**

 * Input linear terms for a single affine row.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `row_idx` Index of the affine row
 * - `num_nz` Number of nonzeros
 * - `subj[num_nz]` (in) Column indexes
 * - `cof[num_nz]` (in) Coefficients
 */
typedef MSK12_ResCode (*MSK12_put_row_func_t)(MSK12_Task_t task,int64_t row_idx,int32_t num_nz,const int32_t* subj,const double* cof);
extern MSK12_put_row_func_t MSK12_put_row_ptr;
MSK12_ResCode MSK12_put_row(
    MSK12_Task_t task,
    int64_t row_idx,
    int32_t num_nz,
    const int32_t* subj,
    const double* cof);

/**

 * Input linear terms for a slice of affine rows.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_row` Index of first affine row
 * - `num_row` Number of rows in the slice
 * - `row_num_nz[num_row]` (in) Number of non-zeros per row.
 * - `subj` (in) Column subscripts
 * - `cof` (in) Coefficients
 */
typedef MSK12_ResCode (*MSK12_put_row_slice_func_t)(MSK12_Task_t task,int64_t first_row,int64_t num_row,const int32_t* row_num_nz,const int32_t* subj,const double* cof);
extern MSK12_put_row_slice_func_t MSK12_put_row_slice_ptr;
MSK12_ResCode MSK12_put_row_slice(
    MSK12_Task_t task,
    int64_t first_row,
    int64_t num_row,
    const int32_t* row_num_nz,
    const int32_t* subj,
    const double* cof);

/**

 * Input linear terms for a list of affine rows. This accepts subscripts and coefficients where rows are non-continuous.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_row` Number of rows in list
 * - `row_idxs[num_row]` (in) Row indexes
 * - `row_num_nz[num_row]` (in) Number of non-zeros per row.
 * - `subj` (in) List of pointers to subscripts.
 * - `cof` (in) Coefficients
 */
typedef MSK12_ResCode (*MSK12_put_row_list_func_t)(MSK12_Task_t task,int64_t num_row,const int64_t* row_idxs,const int32_t* row_num_nz,const int32_t** subj,const double** cof);
extern MSK12_put_row_list_func_t MSK12_put_row_list_ptr;
MSK12_ResCode MSK12_put_row_list(
    MSK12_Task_t task,
    int64_t num_row,
    const int64_t* row_idxs,
    const int32_t* row_num_nz,
    const int32_t** subj,
    const double** cof);

/**

 * Input the constant term for a single affine row
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `row_idx` Index of the affine row
 * - `g` Row fixed term
 */
typedef MSK12_ResCode (*MSK12_put_row_g_func_t)(MSK12_Task_t task,int64_t row_idx,double g);
extern MSK12_put_row_g_func_t MSK12_put_row_g_ptr;
MSK12_ResCode MSK12_put_row_g(
    MSK12_Task_t task,
    int64_t row_idx,
    double g);

/**

 * Input the constant terms for a slice of affine rows.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_row` Index of the first affine row in a slice
 * - `num_row` Number of affine rows
 * - `g[num_row]` (in) Row fixed term
 */
typedef MSK12_ResCode (*MSK12_put_row_slice_g_func_t)(MSK12_Task_t task,int64_t first_row,int64_t num_row,const double* g);
extern MSK12_put_row_slice_g_func_t MSK12_put_row_slice_g_ptr;
MSK12_ResCode MSK12_put_row_slice_g(
    MSK12_Task_t task,
    int64_t first_row,
    int64_t num_row,
    const double* g);

/**

 * Input the constant terms for a list of affine rows.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_row` Number of affine rows
 * - `row_idxs[num_row]` (in) Array of row indexes
 * - `g[num_row]` (in) Row fixed term
 */
typedef MSK12_ResCode (*MSK12_put_row_list_g_func_t)(MSK12_Task_t task,int64_t num_row,const int64_t* row_idxs,const double* g);
extern MSK12_put_row_list_g_func_t MSK12_put_row_list_g_ptr;
MSK12_ResCode MSK12_put_row_list_g(
    MSK12_Task_t task,
    int64_t num_row,
    const int64_t* row_idxs,
    const double* g);

/**

 * Put a single column.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `col_idx` Variable index
 * - `num_nz` 
 * - `row_idxs[num_nz]` (in) Array of row indexes
 * - `cof[num_nz]` (in) Coefficients
 */
typedef MSK12_ResCode (*MSK12_put_col_func_t)(MSK12_Task_t task,int32_t col_idx,int64_t num_nz,const int64_t* row_idxs,const double* cof);
extern MSK12_put_col_func_t MSK12_put_col_ptr;
MSK12_ResCode MSK12_put_col(
    MSK12_Task_t task,
    int32_t col_idx,
    int64_t num_nz,
    const int64_t* row_idxs,
    const double* cof);

/**

 * Put a slice of columns.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_col` Index of the first columns index in a slice
 * - `num_col` Number of columns
 * - `col_len[num_col]` (in) 
 * - `row_idxs` (in) Array of row indexes
 * - `cof` (in) Coefficients
 */
typedef MSK12_ResCode (*MSK12_put_col_slice_func_t)(MSK12_Task_t task,int32_t first_col,int32_t num_col,const int64_t* col_len,const int64_t* row_idxs,const double* cof);
extern MSK12_put_col_slice_func_t MSK12_put_col_slice_ptr;
MSK12_ResCode MSK12_put_col_slice(
    MSK12_Task_t task,
    int32_t first_col,
    int32_t num_col,
    const int64_t* col_len,
    const int64_t* row_idxs,
    const double* cof);

/**

 * Put a list of columns where the individual columns can be non-contiguous. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_col` Number of columns
 * - `col_idxs[num_col]` (in) Array of variable indexes
 * - `col_lens[num_col]` (in) Array of columns lengths
 * - `row_idxs` (in) Array of row indexes
 * - `cof` (in) Coefficients
 */
typedef MSK12_ResCode (*MSK12_put_col_list_func_t)(MSK12_Task_t task,int32_t num_col,const int32_t* col_idxs,const int64_t* col_lens,const int64_t** row_idxs,const double** cof);
extern MSK12_put_col_list_func_t MSK12_put_col_list_ptr;
MSK12_ResCode MSK12_put_col_list(
    MSK12_Task_t task,
    int32_t num_col,
    const int32_t* col_idxs,
    const int64_t* col_lens,
    const int64_t** row_idxs,
    const double** cof);

/**

 * Put a non-zero triplet
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `row_idx` Index of the affine row
 * - `var_idx` Variable index
 * - `cof` Coefficients
 */
typedef MSK12_ResCode (*MSK12_put_ijc_func_t)(MSK12_Task_t task,int64_t row_idx,int32_t var_idx,double cof);
extern MSK12_put_ijc_func_t MSK12_put_ijc_ptr;
MSK12_ResCode MSK12_put_ijc(
    MSK12_Task_t task,
    int64_t row_idx,
    int32_t var_idx,
    double cof);

/**

 * Put a list of non-zero triplets
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_nz` 
 * - `row_idxs[num_nz]` (in) Array of row indexes
 * - `col_idxs[num_nz]` (in) Array of variable indexes
 * - `cof[num_nz]` (in) Coefficients
 */
typedef MSK12_ResCode (*MSK12_put_ijc_list_func_t)(MSK12_Task_t task,int64_t num_nz,const int64_t* row_idxs,const int32_t* col_idxs,const double* cof);
extern MSK12_put_ijc_list_func_t MSK12_put_ijc_list_ptr;
MSK12_ResCode MSK12_put_ijc_list(
    MSK12_Task_t task,
    int64_t num_nz,
    const int64_t* row_idxs,
    const int32_t* col_idxs,
    const double* cof);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `row_idx` Index of the affine row
 * - `num_nz[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_row_num_nz_func_t)(MSK12_Task_t task,int64_t row_idx,int32_t num_nz[1]);
extern MSK12_get_row_num_nz_func_t MSK12_get_row_num_nz_ptr;
MSK12_ResCode MSK12_get_row_num_nz(
    MSK12_Task_t task,
    int64_t row_idx,
    int32_t num_nz[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_row` Index of the first affine row in a slice
 * - `num_row` Number of affine rows
 * - `num_nz[num_row]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_row_slice_num_nz_func_t)(MSK12_Task_t task,int64_t first_row,int64_t num_row,int64_t* num_nz);
extern MSK12_get_row_slice_num_nz_func_t MSK12_get_row_slice_num_nz_ptr;
MSK12_ResCode MSK12_get_row_slice_num_nz(
    MSK12_Task_t task,
    int64_t first_row,
    int64_t num_row,
    int64_t* num_nz);

/**

 * Get nonzeros from a single row.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `row_idx` Index of the affine row
 * - `nnz` Number of nonzeros
 * - `subj[nnz]` (out) Variable indexes
 * - `cof[nnz]` (out) Coefficients
 */
typedef MSK12_ResCode (*MSK12_get_row_func_t)(MSK12_Task_t task,int64_t row_idx,int32_t nnz,int32_t* subj,double* cof);
extern MSK12_get_row_func_t MSK12_get_row_ptr;
MSK12_ResCode MSK12_get_row(
    MSK12_Task_t task,
    int64_t row_idx,
    int32_t nnz,
    int32_t* subj,
    double* cof);

/**

 * Get nonzeros from a slice of rows.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_row` Index of the first affine row in a slice
 * - `num_row` Number of affine rows
 * - `nnz` Number of nonzeros
 * - `row_len[num_row]` (out) 
 * - `subj[nnz]` (out) Variable indexes
 * - `cof[nnz]` (out) Coefficients
 */
typedef MSK12_ResCode (*MSK12_get_row_slice_func_t)(MSK12_Task_t task,int64_t first_row,int64_t num_row,int64_t nnz,int32_t* row_len,int32_t* subj,double* cof);
extern MSK12_get_row_slice_func_t MSK12_get_row_slice_ptr;
MSK12_ResCode MSK12_get_row_slice(
    MSK12_Task_t task,
    int64_t first_row,
    int64_t num_row,
    int64_t nnz,
    int32_t* row_len,
    int32_t* subj,
    double* cof);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `col_idx` Variable index
 * - `num_nz[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_col_num_nz_func_t)(MSK12_Task_t task,int32_t col_idx,int64_t num_nz[1]);
extern MSK12_get_col_num_nz_func_t MSK12_get_col_num_nz_ptr;
MSK12_ResCode MSK12_get_col_num_nz(
    MSK12_Task_t task,
    int32_t col_idx,
    int64_t num_nz[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_col` Index of the first columns index in a slice
 * - `num_col` Number of columns
 * - `num_nz[num_col]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_col_slice_num_nz_func_t)(MSK12_Task_t task,int32_t first_col,int32_t num_col,int64_t* num_nz);
extern MSK12_get_col_slice_num_nz_func_t MSK12_get_col_slice_num_nz_ptr;
MSK12_ResCode MSK12_get_col_slice_num_nz(
    MSK12_Task_t task,
    int32_t first_col,
    int32_t num_col,
    int64_t* num_nz);

/**

 * Get nonzeros from a single column.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `col_idx` Variable index
 * - `nnz` Number of nonzeros
 * - `subi[nnz]` (out) 
 * - `cof[nnz]` (out) Coefficients
 */
typedef MSK12_ResCode (*MSK12_get_col_func_t)(MSK12_Task_t task,int32_t col_idx,int64_t nnz,int64_t* subi,double* cof);
extern MSK12_get_col_func_t MSK12_get_col_ptr;
MSK12_ResCode MSK12_get_col(
    MSK12_Task_t task,
    int32_t col_idx,
    int64_t nnz,
    int64_t* subi,
    double* cof);

/**

 * Get nonzeros from a slice of columns.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_col` Index of the first columns index in a slice
 * - `num_col` Number of columns
 * - `nnz` Number of nonzeros
 * - `col_len[num_col]` (out) 
 * - `subi[nnz]` (out) 
 * - `cof[nnz]` (out) Coefficients
 */
typedef MSK12_ResCode (*MSK12_get_col_slice_func_t)(MSK12_Task_t task,int32_t first_col,int32_t num_col,int64_t nnz,int64_t* col_len,int64_t* subi,double* cof);
extern MSK12_get_col_slice_func_t MSK12_get_col_slice_ptr;
MSK12_ResCode MSK12_get_col_slice(
    MSK12_Task_t task,
    int32_t first_col,
    int32_t num_col,
    int64_t nnz,
    int64_t* col_len,
    int64_t* subi,
    double* cof);

/**

 * Input a single bar entry.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `row_idx` Index of the affine row
 * - `barvar_idx` Positive semi-definite variable index
 * - `num_weight` 
 * - `matrix_idx[num_weight]` (in) 
 * - `weight[num_weight]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_bar_entry_func_t)(MSK12_Task_t task,int64_t row_idx,int32_t barvar_idx,int64_t num_weight,const int64_t* matrix_idx,const double* weight);
extern MSK12_put_bar_entry_func_t MSK12_put_bar_entry_ptr;
MSK12_ResCode MSK12_put_bar_entry(
    MSK12_Task_t task,
    int64_t row_idx,
    int32_t barvar_idx,
    int64_t num_weight,
    const int64_t* matrix_idx,
    const double* weight);

/**

 * Input a list of bar entries.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_bar_entry` Number of entries
 * - `row_idx[num_bar_entry]` (in) Row index list
 * - `barvar_idx[num_bar_entry]` (in) Bar variable index list
 * - `num_weight[num_bar_entry]` (in) Per entry, the number of terms
 * - `matrix_idx` (in) Matrix indexes
 * - `weight` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_bar_entry_list_func_t)(MSK12_Task_t task,int64_t num_bar_entry,const int64_t* row_idx,const int32_t* barvar_idx,const int64_t* num_weight,const int64_t* matrix_idx,const double* weight);
extern MSK12_put_bar_entry_list_func_t MSK12_put_bar_entry_list_ptr;
MSK12_ResCode MSK12_put_bar_entry_list(
    MSK12_Task_t task,
    int64_t num_bar_entry,
    const int64_t* row_idx,
    const int32_t* barvar_idx,
    const int64_t* num_weight,
    const int64_t* matrix_idx,
    const double* weight);

/**

 * Put bar entries for a single row, replacing all existing entries
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `row_idx` Index of the affine row
 * - `num_bar_entry` 
 * - `barvar_idx[num_bar_entry]` (in) Positive semi-definite variable index
 * - `num_weight[num_bar_entry]` (in) 
 * - `matrix_idx` (in) 
 * - `weight` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_bar_row_func_t)(MSK12_Task_t task,int64_t row_idx,int32_t num_bar_entry,const int32_t* barvar_idx,const int64_t* num_weight,const int64_t* matrix_idx,const double* weight);
extern MSK12_put_bar_row_func_t MSK12_put_bar_row_ptr;
MSK12_ResCode MSK12_put_bar_row(
    MSK12_Task_t task,
    int64_t row_idx,
    int32_t num_bar_entry,
    const int32_t* barvar_idx,
    const int64_t* num_weight,
    const int64_t* matrix_idx,
    const double* weight);

/**

 * Get information on symmetric matrix.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `symmat_idx` Symmetric matrix index
 * - `dim[1]` (out) Dimension
 * - `nnz[1]` (out) Number of nonzeros
 */
typedef MSK12_ResCode (*MSK12_get_symmat_info_func_t)(MSK12_Task_t task,int64_t symmat_idx,int32_t dim[1],int64_t nnz[1]);
extern MSK12_get_symmat_info_func_t MSK12_get_symmat_info_ptr;
MSK12_ResCode MSK12_get_symmat_info(
    MSK12_Task_t task,
    int64_t symmat_idx,
    int32_t dim[1],
    int64_t nnz[1]);

/**

 * Get symmetric matrix data
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `symmat_idx` Symmetric matrix index
 * - `nnz` Number of nonzeros
 * - `symmat_i[nnz]` (out) Symmetric matrix row subscripts
 * - `symmat_j[nnz]` (out) Symmetric matrix column subscripts
 * - `symmat_val[nnz]` (out) Symmetric matrix values
 */
typedef MSK12_ResCode (*MSK12_get_symmat_func_t)(MSK12_Task_t task,int64_t symmat_idx,int64_t nnz,int32_t* symmat_i,int32_t* symmat_j,double* symmat_val);
extern MSK12_get_symmat_func_t MSK12_get_symmat_ptr;
MSK12_ResCode MSK12_get_symmat(
    MSK12_Task_t task,
    int64_t symmat_idx,
    int64_t nnz,
    int32_t* symmat_i,
    int32_t* symmat_j,
    double* symmat_val);

/**

 * Get information on a slice of symmetric matrixes.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_symmat` Index of the first symmetric matrix in a slice
 * - `num_symmat` Number of symmetric matrixes
 * - `dim[num_symmat]` (out) Dimension
 * - `nnz[num_symmat]` (out) Number of nonzeros
 */
typedef MSK12_ResCode (*MSK12_get_symmat_slice_info_func_t)(MSK12_Task_t task,int64_t first_symmat,int64_t num_symmat,int32_t* dim,int64_t* nnz);
extern MSK12_get_symmat_slice_info_func_t MSK12_get_symmat_slice_info_ptr;
MSK12_ResCode MSK12_get_symmat_slice_info(
    MSK12_Task_t task,
    int64_t first_symmat,
    int64_t num_symmat,
    int32_t* dim,
    int64_t* nnz);

/**

 * Get information on a slice of symmetric matrixes.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_symmat` First symmetric matrix in slice
 * - `num_symmat` Number of symmetric matrixes in alice
 * - `total_nnz` Expected number of nonzeros
 * - `symmat_i[total_nnz]` (out) Symmetric matrix row subscripts
 * - `symmat_j[total_nnz]` (out) Symmetric matrix column subscripts
 * - `symmat_val[total_nnz]` (out) Symmetric matrix values
 */
typedef MSK12_ResCode (*MSK12_get_symmat_slice_func_t)(MSK12_Task_t task,int64_t first_symmat,int64_t num_symmat,int64_t total_nnz,int32_t* symmat_i,int32_t* symmat_j,double* symmat_val);
extern MSK12_get_symmat_slice_func_t MSK12_get_symmat_slice_ptr;
MSK12_ResCode MSK12_get_symmat_slice(
    MSK12_Task_t task,
    int64_t first_symmat,
    int64_t num_symmat,
    int64_t total_nnz,
    int32_t* symmat_i,
    int32_t* symmat_j,
    double* symmat_val);

/**

 * Append constraints.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_idx` Index of the domain
 * - `num_rows` Row count for the constraint block.
 * - `row_idxs[num_rows]` (in) Array of row indexes
 * - `con_offset[num_rows]` (in, nullable) Constraint right-hand-side offset vector, where NULL means all zeros 
 */
typedef MSK12_ResCode (*MSK12_append_con_func_t)(MSK12_Task_t task,int64_t dom_idx,int64_t num_rows,const int64_t* row_idxs,NULLABLE const double* con_offset);
extern MSK12_append_con_func_t MSK12_append_con_ptr;
MSK12_ResCode MSK12_append_con(
    MSK12_Task_t task,
    int64_t dom_idx,
    int64_t num_rows,
    const int64_t* row_idxs,
    NULLABLE const double* con_offset);

/**

 * Append constraints.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_con` Number of constraint blocks to add.
 * - `dom_idxs[num_con]` (in) Index of the domain to use. The domain's size must be exactly `num_rows`.
 * - `num_rows` Total number of rows we are using.
 * - `row_idxs[num_rows]` (in) Array of row indexes
 * - `con_offset[num_rows]` (in, nullable) Constraint right-hand-side offset vector, where NULL means all zeros 
 */
typedef MSK12_ResCode (*MSK12_append_cons_func_t)(MSK12_Task_t task,int64_t num_con,const int64_t* dom_idxs,int64_t num_rows,const int64_t* row_idxs,NULLABLE const double* con_offset);
extern MSK12_append_cons_func_t MSK12_append_cons_ptr;
MSK12_ResCode MSK12_append_cons(
    MSK12_Task_t task,
    int64_t num_con,
    const int64_t* dom_idxs,
    int64_t num_rows,
    const int64_t* row_idxs,
    NULLABLE const double* con_offset);

/**

 * Put domain and row indexes for constraint `index`.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `con_idx` Index of the constraint to change.
 * - `num_rows` Total number of scalar rows we will input.
 * - `dom_idx` Index of the domain to use. The domain's size must be exactly `num_rows`. 
 * - `row_idxs[num_rows]` (in) Array of row indexes
 * - `rhs_offset[num_rows]` (in, nullable) Domain offset
 */
typedef MSK12_ResCode (*MSK12_put_con_func_t)(MSK12_Task_t task,int64_t con_idx,int64_t num_rows,int64_t dom_idx,const int64_t* row_idxs,NULLABLE const double* rhs_offset);
extern MSK12_put_con_func_t MSK12_put_con_ptr;
MSK12_ResCode MSK12_put_con(
    MSK12_Task_t task,
    int64_t con_idx,
    int64_t num_rows,
    int64_t dom_idx,
    const int64_t* row_idxs,
    NULLABLE const double* rhs_offset);

/**

 * Put domain and row indexes for a single scalar constraint `index`. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `con_idx` Index of the constraint to change.
 * - `dom_idx` Index of the domain to use. The domain's size must be exactly 1. 
 * - `row_idx` Index of the affine row
 * - `rhs_offset` Domain offset
 */
typedef MSK12_ResCode (*MSK12_put_scalar_con_func_t)(MSK12_Task_t task,int64_t con_idx,int64_t dom_idx,int64_t row_idx,double rhs_offset);
extern MSK12_put_scalar_con_func_t MSK12_put_scalar_con_ptr;
MSK12_ResCode MSK12_put_scalar_con(
    MSK12_Task_t task,
    int64_t con_idx,
    int64_t dom_idx,
    int64_t row_idx,
    double rhs_offset);

/**

 * Put domains and row indexes for a slice of constraints.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_con` Index of the constraint to change.
 * - `num_con` Index of the constraint to change.
 * - `num_rows` Total number of scalar rows we will input.
 * - `dom_idx[num_con]` (in) Indexes of the domains to use. The sum of domain sizees must be exactly `num_rows`. 
 * - `row_idx[num_rows]` (in) Indexes of the scalar affine rows
 * - `rhs_offset[num_rows]` (in, nullable) Domain offset
 */
typedef MSK12_ResCode (*MSK12_put_con_slice_func_t)(MSK12_Task_t task,int64_t first_con,int64_t num_con,int64_t num_rows,const int64_t* dom_idx,const int64_t* row_idx,NULLABLE const double* rhs_offset);
extern MSK12_put_con_slice_func_t MSK12_put_con_slice_ptr;
MSK12_ResCode MSK12_put_con_slice(
    MSK12_Task_t task,
    int64_t first_con,
    int64_t num_con,
    int64_t num_rows,
    const int64_t* dom_idx,
    const int64_t* row_idx,
    NULLABLE const double* rhs_offset);

/**

 * Get domains from a slice of constraints
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_con` First constraint
 * - `num_con` Number of constraints
 * - `dom_idx[num_con]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_con_slice_domains_func_t)(MSK12_Task_t task,int64_t first_con,int64_t num_con,int64_t* dom_idx);
extern MSK12_get_con_slice_domains_func_t MSK12_get_con_slice_domains_ptr;
MSK12_ResCode MSK12_get_con_slice_domains(
    MSK12_Task_t task,
    int64_t first_con,
    int64_t num_con,
    int64_t* dom_idx);

/**

 * Get number of rows in a slice of constraints.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_con` First constraint index in a slice
 * - `num_con` Number of constraints
 * - `num_row[1]` (out) Number of affine rows
 */
typedef MSK12_ResCode (*MSK12_get_con_slice_num_row_func_t)(MSK12_Task_t task,int64_t first_con,int64_t num_con,int64_t num_row[1]);
extern MSK12_get_con_slice_num_row_func_t MSK12_get_con_slice_num_row_ptr;
MSK12_ResCode MSK12_get_con_slice_num_row(
    MSK12_Task_t task,
    int64_t first_con,
    int64_t num_con,
    int64_t num_row[1]);

/**

 * Get domains and indexes for a slice of constraints. Note that if the output is longer than the provided buffers, the result is silently truncated.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_con` Constraint index
 * - `num_con` Number of constraint
 * - `num_row` Length of row_index.
 * - `row_idx[num_row]` (out) Index of the affine row
 * - `rhs_offset[num_row]` (out) Domain offset
 * - `dom_idx[num_con]` (out) Index of the domain
 */
typedef MSK12_ResCode (*MSK12_get_con_slice_func_t)(MSK12_Task_t task,int64_t first_con,int64_t num_con,int64_t num_row,int64_t* row_idx,double* rhs_offset,int64_t* dom_idx);
extern MSK12_get_con_slice_func_t MSK12_get_con_slice_ptr;
MSK12_ResCode MSK12_get_con_slice(
    MSK12_Task_t task,
    int64_t first_con,
    int64_t num_con,
    int64_t num_row,
    int64_t* row_idx,
    double* rhs_offset,
    int64_t* dom_idx);

/**

 * Put domain and row indexes for constraint `index`.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_rows` Total number of scalar rows we will input.
 * - `num_dom` Number of domains
 * - `num_terms` Number of terms
 * - `dom_idx[num_dom]` (in) Index of the domains to use. The sum of the domain sizes must sum to `num_rows`.
 * - `term_size[num_terms]` (in) Term sizes. Each entry denotes the number of domains in the corresponding term. These must sum to `num_domains`
 * - `row_idx[num_rows]` (in) Indexes of the scalar affine rows
 * - `rhs_offset[num_rows]` (in) Domain offset
 */
typedef MSK12_ResCode (*MSK12_append_djc_func_t)(MSK12_Task_t task,int64_t num_rows,int64_t num_dom,int64_t num_terms,const int64_t* dom_idx,const int64_t* term_size,const int64_t* row_idx,const double* rhs_offset);
extern MSK12_append_djc_func_t MSK12_append_djc_ptr;
MSK12_ResCode MSK12_append_djc(
    MSK12_Task_t task,
    int64_t num_rows,
    int64_t num_dom,
    int64_t num_terms,
    const int64_t* dom_idx,
    const int64_t* term_size,
    const int64_t* row_idx,
    const double* rhs_offset);

/**

 * Put domain and row indexes for constraint `index`.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `djc_idx` Index of the djc to change.
 * - `num_rows` Total number of scalar rows we will input.
 * - `num_dom` Number of domains
 * - `num_terms` Number of terms
 * - `dom_idx[num_dom]` (in) Index of the domains to use. The sum of the domain sizes must sum to `num_rows`.
 * - `term_size[num_terms]` (in) Term sizes. Each entry denotes the number of domains in the corresponding term. These must sum to `num_domains`
 * - `row_idx[num_rows]` (in) Indexes of the scalar affine rows
 * - `rhs_offset[num_rows]` (in) Domain offset
 */
typedef MSK12_ResCode (*MSK12_put_djc_func_t)(MSK12_Task_t task,int64_t djc_idx,int64_t num_rows,int64_t num_dom,int64_t num_terms,const int64_t* dom_idx,const int64_t* term_size,const int64_t* row_idx,const double* rhs_offset);
extern MSK12_put_djc_func_t MSK12_put_djc_ptr;
MSK12_ResCode MSK12_put_djc(
    MSK12_Task_t task,
    int64_t djc_idx,
    int64_t num_rows,
    int64_t num_dom,
    int64_t num_terms,
    const int64_t* dom_idx,
    const int64_t* term_size,
    const int64_t* row_idx,
    const double* rhs_offset);

/**

 * Put domains and row indexes for a slice of constraints.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_djc` Index of the constraint to change.
 * - `num_djc` Index of the constraint to change.
 * - `num_rows` Total number of scalar rows we will input.
 * - `num_dom` Number of domains
 * - `num_terms` Number of terms
 * - `dom_idx[num_dom]` (in) Index of the domains to use. The sum of the domain sizes must sum to `num_rows`.
 * - `term_size[num_terms]` (in) Term sizes. Each entry denotes the number of domains in the corresponding term. These must sum to `num_domains` 
 * - `row_idx[num_rows]` (in) Indexes of the scalar affine rows
 * - `rhs_offset[num_rows]` (in) Right-hand-side offset
 * - `djc_numterm[num_djc]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_djc_slice_func_t)(MSK12_Task_t task,int64_t first_djc,int64_t num_djc,int64_t num_rows,int64_t num_dom,int64_t num_terms,const int64_t* dom_idx,const int64_t* term_size,const int64_t* row_idx,const double* rhs_offset,const int64_t* djc_numterm);
extern MSK12_put_djc_slice_func_t MSK12_put_djc_slice_ptr;
MSK12_ResCode MSK12_put_djc_slice(
    MSK12_Task_t task,
    int64_t first_djc,
    int64_t num_djc,
    int64_t num_rows,
    int64_t num_dom,
    int64_t num_terms,
    const int64_t* dom_idx,
    const int64_t* term_size,
    const int64_t* row_idx,
    const double* rhs_offset,
    const int64_t* djc_numterm);

/**

 * Get information in a single DJC.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `djc_idx` DJC index
 * - `num_term[1]` (out) number of terms in DJC
 * - `num_dom[1]` (out) Number of domains
 * - `num_row[1]` (out) Number of affine rows
 */
typedef MSK12_ResCode (*MSK12_get_djc_info_func_t)(MSK12_Task_t task,int64_t djc_idx,int64_t num_term[1],int64_t num_dom[1],int64_t num_row[1]);
extern MSK12_get_djc_info_func_t MSK12_get_djc_info_ptr;
MSK12_ResCode MSK12_get_djc_info(
    MSK12_Task_t task,
    int64_t djc_idx,
    int64_t num_term[1],
    int64_t num_dom[1],
    int64_t num_row[1]);

/**

 * Get DJC data. Together with `MSK120_get_djc_info` this extracts DJC data.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `djc_idx` DJC index
 * - `num_terms` Expect number of terms
 * - `num_dom` Expect number of domain indexes
 * - `num_row` Expect number of rows
 * - `term_size[num_terms]` (out) 
 * - `dom_idx[num_dom]` (out) Index of the domain
 * - `row_idx[num_row]` (out) Index of the affine row
 * - `rhs_offset[num_row]` (out) Domain offset
 */
typedef MSK12_ResCode (*MSK12_get_djc_func_t)(MSK12_Task_t task,int64_t djc_idx,int64_t num_terms,int64_t num_dom,int64_t num_row,int64_t* term_size,int64_t* dom_idx,int64_t* row_idx,double* rhs_offset);
extern MSK12_get_djc_func_t MSK12_get_djc_ptr;
MSK12_ResCode MSK12_get_djc(
    MSK12_Task_t task,
    int64_t djc_idx,
    int64_t num_terms,
    int64_t num_dom,
    int64_t num_row,
    int64_t* term_size,
    int64_t* dom_idx,
    int64_t* row_idx,
    double* rhs_offset);

/**

 * Get information on a slice of DJCs. This will return total sizes for terms, clauses and rows. To get information on the number of clauses and rows for each DJC, call `MSK120_get_djc_slice_term`.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_djc` first DJC index
 * - `num_djc` number of DJCs
 * - `num_term[1]` (out) total number of terms
 * - `num_dom[1]` (out) total number of clauses/domains
 * - `num_row[1]` (out) total number of rows
 */
typedef MSK12_ResCode (*MSK12_get_djc_slice_info_func_t)(MSK12_Task_t task,int64_t first_djc,int64_t num_djc,int64_t num_term[1],int64_t num_dom[1],int64_t num_row[1]);
extern MSK12_get_djc_slice_info_func_t MSK12_get_djc_slice_info_ptr;
MSK12_ResCode MSK12_get_djc_slice_info(
    MSK12_Task_t task,
    int64_t first_djc,
    int64_t num_djc,
    int64_t num_term[1],
    int64_t num_dom[1],
    int64_t num_row[1]);

/**

 * Get information on a terms of a slice of DJCs. This will return total sizes for terms, clauses and rows. Together with `MSK120_get_djc_slice_info` this extracts DJC slice data.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_djc` first DJC index
 * - `num_djc` number of DJCs
 * - `num_term` Expect number of terms
 * - `num_dom` Expect number of domain indexes
 * - `num_row` Expect number of rows
 * - `term_size[num_term]` (out) 
 * - `dom_idx[num_dom]` (out) Index of the domain
 * - `row_idx[num_row]` (out) Index of the affine row
 * - `rhs_offset[num_row]` (out) Domain offset
 * - `djc_num_term[num_djc]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_djc_slice_func_t)(MSK12_Task_t task,int64_t first_djc,int64_t num_djc,int64_t num_term,int64_t num_dom,int64_t num_row,int64_t* term_size,int64_t* dom_idx,int64_t* row_idx,double* rhs_offset,int64_t* djc_num_term);
extern MSK12_get_djc_slice_func_t MSK12_get_djc_slice_ptr;
MSK12_ResCode MSK12_get_djc_slice(
    MSK12_Task_t task,
    int64_t first_djc,
    int64_t num_djc,
    int64_t num_term,
    int64_t num_dom,
    int64_t num_row,
    int64_t* term_size,
    int64_t* dom_idx,
    int64_t* row_idx,
    double* rhs_offset,
    int64_t* djc_num_term);

/**

 * Input objective sense.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sense` 
 */
typedef void (*MSK12_put_obj_sense_func_t)(MSK12_Task_t task,MSK12_ObjSense sense);
extern MSK12_put_obj_sense_func_t MSK12_put_obj_sense_ptr;
void MSK12_put_obj_sense(
    MSK12_Task_t task,
    MSK12_ObjSense sense);

/**

 * Get objectiev sense. This cannot fail.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef MSK12_ObjSense (*MSK12_get_obj_sense_func_t)(MSK12_Task_t task);
extern MSK12_get_obj_sense_func_t MSK12_get_obj_sense_ptr;
MSK12_ObjSense MSK12_get_obj_sense(MSK12_Task_t task);

/**

 * Set the affine row to use as objective
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `row_idx` Index of the affine row
 */
typedef MSK12_ResCode (*MSK12_put_obj_row_func_t)(MSK12_Task_t task,int64_t row_idx);
extern MSK12_put_obj_row_func_t MSK12_put_obj_row_ptr;
MSK12_ResCode MSK12_put_obj_row(
    MSK12_Task_t task,
    int64_t row_idx);

/**

 * Get objective row index. If the objective row is set, `asgn[0]` will be set to 1 and `index[0]` is set to the row index, otherwise `asgn[0]` is set to 0.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `row_idx[1]` (out) Index of the affine row
 * - `asgn[1]` (out) Returns non-zero to indicate that a value was assigned, or zero if it was not
 */
typedef void (*MSK12_get_obj_row_func_t)(MSK12_Task_t task,int64_t row_idx[1],int asgn[1]);
extern MSK12_get_obj_row_func_t MSK12_get_obj_row_ptr;
void MSK12_get_obj_row(
    MSK12_Task_t task,
    int64_t row_idx[1],
    int asgn[1]);

/**

 * Call optimizer. On return, all input solutions have been cleared.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `trm[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_optimize_func_t)(MSK12_Task_t task,MSK12_TrmCode trm[1]);
extern MSK12_optimize_func_t MSK12_optimize_ptr;
MSK12_ResCode MSK12_optimize(
    MSK12_Task_t task,
    MSK12_TrmCode trm[1]);

/**

 * Prints a short summary of the current solutions. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `whichstream` 
 */
typedef MSK12_ResCode (*MSK12_solution_summary_func_t)(MSK12_Task_t task,MSK12_StreamType whichstream);
extern MSK12_solution_summary_func_t MSK12_solution_summary_ptr;
MSK12_ResCode MSK12_solution_summary(
    MSK12_Task_t task,
    MSK12_StreamType whichstream);

/**

 * Call optimizer and provide callback functions. On return, all input solutions have been cleared.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `trm[1]` (out) Return the termination code here. Will always be set on return.
 * - `cb_handle` 
 * - `cb_func` 
 * - `int_cb_handle` 
 * - `int_cb_func` 
 */
typedef MSK12_ResCode (*MSK12_optimize_callback_func_t)(MSK12_Task_t task,MSK12_TrmCode trm[1],MSK12_CallbackHandle cb_handle,MSK12_CallbackFunc cb_func,MSK12_CallbackHandle int_cb_handle,MSK12_IntSolCallbackFunc int_cb_func);
extern MSK12_optimize_callback_func_t MSK12_optimize_callback_ptr;
MSK12_ResCode MSK12_optimize_callback(
    MSK12_Task_t task,
    MSK12_TrmCode trm[1],
    MSK12_CallbackHandle cb_handle,
    MSK12_CallbackFunc cb_func,
    MSK12_CallbackHandle int_cb_handle,
    MSK12_IntSolCallbackFunc int_cb_func);

/**

 * Specify a remote OptServer to use instead of built-in solver.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `server[.cstring]` (in) Server name with protocol and port, e.g. `https://optserver.mydomain:9876`.
 * - `cert[.cstring]` (in, nullable) 
 */
typedef void (*MSK12_put_remote_solver_func_t)(MSK12_Task_t task,const char* server,NULLABLE const char* cert);
extern MSK12_put_remote_solver_func_t MSK12_put_remote_solver_ptr;
void MSK12_put_remote_solver(
    MSK12_Task_t task,
    const char* server,
    NULLABLE const char* cert);

/**

 * Specify access token to be used for remote solving.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `token[.cstring]` (in, nullable) 
 */
typedef void (*MSK12_put_optserver_access_token_func_t)(MSK12_Task_t task,NULLABLE const char* token);
extern MSK12_put_optserver_access_token_func_t MSK12_put_optserver_access_token_ptr;
void MSK12_put_optserver_access_token(
    MSK12_Task_t task,
    NULLABLE const char* token);

/**

 * Get number of solutions. This cannot fail.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int32_t (*MSK12_get_num_sol_func_t)(MSK12_Task_t task);
extern MSK12_get_num_sol_func_t MSK12_get_num_sol_ptr;
int32_t MSK12_get_num_sol(MSK12_Task_t task);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `sol_type[1]` (out) Returns the type of the solution requested
 */
typedef MSK12_ResCode (*MSK12_get_sol_type_func_t)(MSK12_Task_t task,int32_t sol_idx,MSK12_SolType sol_type[1]);
extern MSK12_get_sol_type_func_t MSK12_get_sol_type_ptr;
MSK12_ResCode MSK12_get_sol_type(
    MSK12_Task_t task,
    int32_t sol_idx,
    MSK12_SolType sol_type[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `primal_sol_sta[1]` (out) 
 * - `dual_sol_sta[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_status_func_t)(MSK12_Task_t task,int32_t sol_idx,MSK12_SolSta primal_sol_sta[1],MSK12_SolSta dual_sol_sta[1]);
extern MSK12_get_sol_status_func_t MSK12_get_sol_status_ptr;
MSK12_ResCode MSK12_get_sol_status(
    MSK12_Task_t task,
    int32_t sol_idx,
    MSK12_SolSta primal_sol_sta[1],
    MSK12_SolSta dual_sol_sta[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `pro_sta[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_problem_status_func_t)(MSK12_Task_t task,int32_t sol_idx,MSK12_ProSta pro_sta[1]);
extern MSK12_get_problem_status_func_t MSK12_get_problem_status_ptr;
MSK12_ResCode MSK12_get_problem_status(
    MSK12_Task_t task,
    int32_t sol_idx,
    MSK12_ProSta pro_sta[1]);

/**

 * Get primal objective value for solution `sol_idx`.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `obj_val[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_primal_obj_func_t)(MSK12_Task_t task,int32_t sol_idx,double obj_val[1]);
extern MSK12_get_primal_obj_func_t MSK12_get_primal_obj_ptr;
MSK12_ResCode MSK12_get_primal_obj(
    MSK12_Task_t task,
    int32_t sol_idx,
    double obj_val[1]);

/**

 * Get dual objective value for solution `sol_idx`.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `obj_val[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_dual_obj_func_t)(MSK12_Task_t task,int32_t sol_idx,double obj_val[1]);
extern MSK12_get_dual_obj_func_t MSK12_get_dual_obj_ptr;
MSK12_ResCode MSK12_get_dual_obj(
    MSK12_Task_t task,
    int32_t sol_idx,
    double obj_val[1]);

/**

 * Get primal variable solution slice. The value is always available, even if the primal solution status is unknown or undefined.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index.
 * - `first_var` First in slice.
 * - `num_var` Number of elements in slice.
 * - `xx[num_var]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_xx_slice_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t first_var,int32_t num_var,double* xx);
extern MSK12_get_sol_xx_slice_func_t MSK12_get_sol_xx_slice_ptr;
MSK12_ResCode MSK12_get_sol_xx_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_var,
    int32_t num_var,
    double* xx);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `slx[num_var]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_slx_slice_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t first_var,int32_t num_var,double* slx);
extern MSK12_get_sol_slx_slice_func_t MSK12_get_sol_slx_slice_ptr;
MSK12_ResCode MSK12_get_sol_slx_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_var,
    int32_t num_var,
    double* slx);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `sux[num_var]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_sux_slice_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t first_var,int32_t num_var,double* sux);
extern MSK12_get_sol_sux_slice_func_t MSK12_get_sol_sux_slice_ptr;
MSK12_ResCode MSK12_get_sol_sux_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_var,
    int32_t num_var,
    double* sux);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `barvar_idx` Positive semi-definite variable index
 * - `num_var` Number of variables
 * - `barx[num_var]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_barxj_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t barvar_idx,int64_t num_var,double* barx);
extern MSK12_get_sol_barxj_func_t MSK12_get_sol_barxj_ptr;
MSK12_ResCode MSK12_get_sol_barxj(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t barvar_idx,
    int64_t num_var,
    double* barx);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `barvar_idx` Positive semi-definite variable index
 * - `num_elm` Number of positive semidefinite non-zero entries
 * - `bars[num_elm]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_barsj_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t barvar_idx,int64_t num_elm,double* bars);
extern MSK12_get_sol_barsj_func_t MSK12_get_sol_barsj_ptr;
MSK12_ResCode MSK12_get_sol_barsj(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t barvar_idx,
    int64_t num_elm,
    double* bars);

/**

 * Get primal semidefinite variable solution slice. This get the primal value for a slice of semidefinite variables. Note that the number of elements in the slice can be obtained with `MSK120_barvar_slice_num_elm`.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index.
 * - `first_barvar` First in slice.
 * - `num_barvar` Number of variables in slice.
 * - `num_elm` Number of positive semidefinite non-zero entries
 * - `barx[num_elm]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_barx_slice_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t first_barvar,int32_t num_barvar,int64_t num_elm,double* barx);
extern MSK12_get_sol_barx_slice_func_t MSK12_get_sol_barx_slice_ptr;
MSK12_ResCode MSK12_get_sol_barx_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_barvar,
    int32_t num_barvar,
    int64_t num_elm,
    double* barx);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `first_barvar` First semidefinite variable index in a slice
 * - `num_barvar` Number of variables
 * - `num_elm` Number of positive semidefinite non-zero entries
 * - `bars[num_elm]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_bars_slice_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t first_barvar,int32_t num_barvar,int64_t num_elm,double* bars);
extern MSK12_get_sol_bars_slice_func_t MSK12_get_sol_bars_slice_ptr;
MSK12_ResCode MSK12_get_sol_bars_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_barvar,
    int32_t num_barvar,
    int64_t num_elm,
    double* bars);

/**

 * Get basis indicator for a single variable.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` index of the constraint
 * - `var_idx` index of the variable
 * - `basic[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_basic_xj_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t var_idx,int basic[1]);
extern MSK12_get_sol_basic_xj_func_t MSK12_get_sol_basic_xj_ptr;
MSK12_ResCode MSK12_get_sol_basic_xj(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t var_idx,
    int basic[1]);

/**

 * Get basis indicator for a single semidefinite variable. At the time of writing, it is not well-defined what a basic PSD variable is, exactly.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` index of the constraint
 * - `barvar_idx` index of the variable
 * - `basic[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_basic_barx_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t barvar_idx,int basic[1]);
extern MSK12_get_sol_basic_barx_func_t MSK12_get_sol_basic_barx_ptr;
MSK12_ResCode MSK12_get_sol_basic_barx(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t barvar_idx,
    int basic[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `con_idx` Constraint index 
 * - `basic[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_basic_con_func_t)(MSK12_Task_t task,int32_t sol_idx,int64_t con_idx,int basic[1]);
extern MSK12_get_sol_basic_con_func_t MSK12_get_sol_basic_con_ptr;
MSK12_ResCode MSK12_get_sol_basic_con(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t con_idx,
    int basic[1]);

/**

 * Get bound status indicator for a single variable. Return a value for upper and lower bounds indicating if they are binding or non-binding.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Index of the constraint
 * - `var_idx` Index of the variable
 * - `low_binding[1]` (out) 
 * - `upr_binding[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_sta_var_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t var_idx,int low_binding[1],int upr_binding[1]);
extern MSK12_get_sol_sta_var_func_t MSK12_get_sol_sta_var_ptr;
MSK12_ResCode MSK12_get_sol_sta_var(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t var_idx,
    int low_binding[1],
    int upr_binding[1]);

/**

 * Get bound status indicator for a single semidefinite variable. *
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Index of the constraint
 * - `barvar_idx` Index of the variable
 * - `binding[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_sta_barx_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t barvar_idx,int binding[1]);
extern MSK12_get_sol_sta_barx_func_t MSK12_get_sol_sta_barx_ptr;
MSK12_ResCode MSK12_get_sol_sta_barx(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t barvar_idx,
    int binding[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `con_idx` Constraint index 
 * - `binding[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_sta_con_func_t)(MSK12_Task_t task,int32_t sol_idx,int64_t con_idx,int binding[1]);
extern MSK12_get_sol_sta_con_func_t MSK12_get_sol_sta_con_ptr;
MSK12_ResCode MSK12_get_sol_sta_con(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t con_idx,
    int binding[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `basic[num_var]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_basic_x_slice_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t first_var,int32_t num_var,int* basic);
extern MSK12_get_sol_basic_x_slice_func_t MSK12_get_sol_basic_x_slice_ptr;
MSK12_ResCode MSK12_get_sol_basic_x_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_var,
    int32_t num_var,
    int* basic);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `basic[num_var]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_basic_barx_slice_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t first_var,int32_t num_var,int* basic);
extern MSK12_get_sol_basic_barx_slice_func_t MSK12_get_sol_basic_barx_slice_ptr;
MSK12_ResCode MSK12_get_sol_basic_barx_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_var,
    int32_t num_var,
    int* basic);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `first_con` First constraint index in a slice
 * - `num_con` Number of constraints
 * - `basic[num_con]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_basic_con_slice_func_t)(MSK12_Task_t task,int32_t sol_idx,int64_t first_con,int64_t num_con,int* basic);
extern MSK12_get_sol_basic_con_slice_func_t MSK12_get_sol_basic_con_slice_ptr;
MSK12_ResCode MSK12_get_sol_basic_con_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t first_con,
    int64_t num_con,
    int* basic);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `low_binding[num_var]` (out) 
 * - `upr_binding[num_var]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_sta_var_slice_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t first_var,int32_t num_var,int* low_binding,int* upr_binding);
extern MSK12_get_sol_sta_var_slice_func_t MSK12_get_sol_sta_var_slice_ptr;
MSK12_ResCode MSK12_get_sol_sta_var_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_var,
    int32_t num_var,
    int* low_binding,
    int* upr_binding);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `first_barvar` First semidefinite variable index in a slice
 * - `num_barvar` Number of variables
 * - `bindnig[num_barvar]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_sta_barx_slice_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t first_barvar,int32_t num_barvar,int* bindnig);
extern MSK12_get_sol_sta_barx_slice_func_t MSK12_get_sol_sta_barx_slice_ptr;
MSK12_ResCode MSK12_get_sol_sta_barx_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_barvar,
    int32_t num_barvar,
    int* bindnig);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `first_con` First constraint index in a slice
 * - `num_con` Number of constraints
 * - `binding[num_con]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_sta_con_slice_func_t)(MSK12_Task_t task,int32_t sol_idx,int64_t first_con,int64_t num_con,int* binding);
extern MSK12_get_sol_sta_con_slice_func_t MSK12_get_sol_sta_con_slice_ptr;
MSK12_ResCode MSK12_get_sol_sta_con_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t first_con,
    int64_t num_con,
    int* binding);

/**

 * Get primal solution for a slice of constraints, effectively the constraint expressions evaluated in solution.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index.
 * - `first_con` First constraint.
 * - `num_con` Number of constraints.
 * - `num_elm` Total number of scalar elements in constraint slice.
 * - `xc[num_elm]` (out) Constraint value at the given solution.
 */
typedef MSK12_ResCode (*MSK12_get_sol_xc_slice_func_t)(MSK12_Task_t task,int32_t sol_idx,int64_t first_con,int64_t num_con,int64_t num_elm,double* xc);
extern MSK12_get_sol_xc_slice_func_t MSK12_get_sol_xc_slice_ptr;
MSK12_ResCode MSK12_get_sol_xc_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t first_con,
    int64_t num_con,
    int64_t num_elm,
    double* xc);

/**

 * Get dual solution for a slice of constraints.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index.
 * - `first_con` First constraint.
 * - `num_con` Number of constraints.
 * - `num_elm` Total number of scalar elements in constraint slice.
 * - `y[num_elm]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_sol_y_slice_func_t)(MSK12_Task_t task,int32_t sol_idx,int64_t first_con,int64_t num_con,int64_t num_elm,double* y);
extern MSK12_get_sol_y_slice_func_t MSK12_get_sol_y_slice_ptr;
MSK12_ResCode MSK12_get_sol_y_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t first_con,
    int64_t num_con,
    int64_t num_elm,
    double* y);

/**

 * Get current number of input solutions.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int32_t (*MSK12_get_num_input_solutions_func_t)(MSK12_Task_t task);
extern MSK12_get_num_input_solutions_func_t MSK12_get_num_input_solutions_ptr;
int32_t MSK12_get_num_input_solutions(MSK12_Task_t task);

/**

 * Copy an output solution to the input solutions.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 */
typedef MSK12_ResCode (*MSK12_copy_sol_to_input_func_t)(MSK12_Task_t task,int32_t sol_idx);
extern MSK12_copy_sol_to_input_func_t MSK12_copy_sol_to_input_ptr;
MSK12_ResCode MSK12_copy_sol_to_input(
    MSK12_Task_t task,
    int32_t sol_idx);

/**

 * Append an empty input solution.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `soltype` 
 */
typedef MSK12_ResCode (*MSK12_append_sol_func_t)(MSK12_Task_t task,MSK12_SolType soltype);
extern MSK12_append_sol_func_t MSK12_append_sol_ptr;
MSK12_ResCode MSK12_append_sol(
    MSK12_Task_t task,
    MSK12_SolType soltype);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `num` Number of items
 * - `val[num]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_sol_xx_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t num,const double* val);
extern MSK12_put_sol_xx_func_t MSK12_put_sol_xx_ptr;
MSK12_ResCode MSK12_put_sol_xx(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t num,
    const double* val);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `num` Number of items
 * - `val[num]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_sol_slx_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t num,const double* val);
extern MSK12_put_sol_slx_func_t MSK12_put_sol_slx_ptr;
MSK12_ResCode MSK12_put_sol_slx(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t num,
    const double* val);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `num` Number of items
 * - `val[num]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_sol_sux_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t num,const double* val);
extern MSK12_put_sol_sux_func_t MSK12_put_sol_sux_ptr;
MSK12_ResCode MSK12_put_sol_sux(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t num,
    const double* val);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `num` Number of items
 * - `val[num]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_sol_basic_x_func_t)(MSK12_Task_t task,int32_t sol_idx,int32_t num,const int32_t* val);
extern MSK12_put_sol_basic_x_func_t MSK12_put_sol_basic_x_ptr;
MSK12_ResCode MSK12_put_sol_basic_x(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t num,
    const int32_t* val);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `num` Number of items
 * - `val[num]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_sol_barx_func_t)(MSK12_Task_t task,int32_t sol_idx,int64_t num,const double* val);
extern MSK12_put_sol_barx_func_t MSK12_put_sol_barx_ptr;
MSK12_ResCode MSK12_put_sol_barx(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t num,
    const double* val);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `num` Number of items
 * - `xx[num]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_sol_bars_func_t)(MSK12_Task_t task,int32_t sol_idx,int64_t num,const double* xx);
extern MSK12_put_sol_bars_func_t MSK12_put_sol_bars_ptr;
MSK12_ResCode MSK12_put_sol_bars(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t num,
    const double* xx);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `i` 
 * - `num` Number of items
 * - `xx[num]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_sol_yi_func_t)(MSK12_Task_t task,int32_t sol_idx,int64_t i,int64_t num,const double* xx);
extern MSK12_put_sol_yi_func_t MSK12_put_sol_yi_ptr;
MSK12_ResCode MSK12_put_sol_yi(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t i,
    int64_t num,
    const double* xx);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `num` Number of items
 * - `val[num]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_sol_basic_c_func_t)(MSK12_Task_t task,int32_t sol_idx,int64_t num,const int32_t* val);
extern MSK12_put_sol_basic_c_func_t MSK12_put_sol_basic_c_ptr;
MSK12_ResCode MSK12_put_sol_basic_c(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t num,
    const int32_t* val);

typedef int32_t (*MSK12_get_num_iinf_func_t)();
extern MSK12_get_num_iinf_func_t MSK12_get_num_iinf_ptr;
int32_t MSK12_get_num_iinf();

typedef int32_t (*MSK12_get_num_liinf_func_t)();
extern MSK12_get_num_liinf_func_t MSK12_get_num_liinf_ptr;
int32_t MSK12_get_num_liinf();

typedef int32_t (*MSK12_get_num_dinf_func_t)();
extern MSK12_get_num_dinf_func_t MSK12_get_num_dinf_ptr;
int32_t MSK12_get_num_dinf();

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `par_idx` 
 * - `value[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_iinf_func_t)(MSK12_Task_t task,int32_t par_idx,int32_t value[1]);
extern MSK12_get_iinf_func_t MSK12_get_iinf_ptr;
MSK12_ResCode MSK12_get_iinf(
    MSK12_Task_t task,
    int32_t par_idx,
    int32_t value[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `par_idx` 
 * - `value[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_liinf_func_t)(MSK12_Task_t task,int32_t par_idx,int64_t value[1]);
extern MSK12_get_liinf_func_t MSK12_get_liinf_ptr;
MSK12_ResCode MSK12_get_liinf(
    MSK12_Task_t task,
    int32_t par_idx,
    int64_t value[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `par_idx` 
 * - `value[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_dinf_func_t)(MSK12_Task_t task,int32_t par_idx,double value[1]);
extern MSK12_get_dinf_func_t MSK12_get_dinf_ptr;
MSK12_ResCode MSK12_get_dinf(
    MSK12_Task_t task,
    int32_t par_idx,
    double value[1]);

/**

 * Get the index of the integer information item corresponding to the given name. If the index is invalid, NULL is returned.
 * 
 * # Arguments
 * - `par_idx` 
 */
typedef const char* (*MSK12_get_iinf_name_func_t)(int32_t par_idx);
extern MSK12_get_iinf_name_func_t MSK12_get_iinf_name_ptr;
const char* MSK12_get_iinf_name(int32_t par_idx);

/**

 * Get the index of the long integer information item corresponding to the given name. If the index is invalid, NULL is returned.
 * 
 * # Arguments
 * - `par_idx` 
 */
typedef const char* (*MSK12_get_liinf_name_func_t)(int32_t par_idx);
extern MSK12_get_liinf_name_func_t MSK12_get_liinf_name_ptr;
const char* MSK12_get_liinf_name(int32_t par_idx);

/**

 * Get the index of the long integer information item corresponding to the given name. If the index is invalid, NULL is returned.
 * 
 * # Arguments
 * - `par_idx` 
 */
typedef const char* (*MSK12_get_dinf_name_func_t)(int32_t par_idx);
extern MSK12_get_dinf_name_func_t MSK12_get_dinf_name_ptr;
const char* MSK12_get_dinf_name(int32_t par_idx);

/**

 * This will retur the index of the integer item orresponding to name, or -1 if the name is not recognized.
 * 
 * # Arguments
 * - `par_name[.cstring]` (in) Name of the parameter
 */
typedef int32_t (*MSK12_get_iinf_index_func_t)(const char* par_name);
extern MSK12_get_iinf_index_func_t MSK12_get_iinf_index_ptr;
int32_t MSK12_get_iinf_index(const char* par_name);

/**

 * This will retur the index of the long integer item orresponding to name, or -1 if the name is not recognized.
 * 
 * # Arguments
 * - `par_name[.cstring]` (in) Name of the parameter
 */
typedef int32_t (*MSK12_get_liinf_index_func_t)(const char* par_name);
extern MSK12_get_liinf_index_func_t MSK12_get_liinf_index_ptr;
int32_t MSK12_get_liinf_index(const char* par_name);

/**

 * This will retur the index of the double item orresponding to name, or -1 if the name is not recognized.
 * 
 * # Arguments
 * - `name[.cstring]` (in) 
 */
typedef int32_t (*MSK12_get_dinf_index_func_t)(const char* name);
extern MSK12_get_dinf_index_func_t MSK12_get_dinf_index_ptr;
int32_t MSK12_get_dinf_index(const char* name);

/**

 * Get the current value of a named parameter. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `par_name[.cstring]` (in) Name of the parameter
 * - `value[1]` (out) 
 */
typedef int32_t (*MSK12_get_double_param_func_t)(MSK12_Task_t task,const char* par_name,double value[1]);
extern MSK12_get_double_param_func_t MSK12_get_double_param_ptr;
int32_t MSK12_get_double_param(
    MSK12_Task_t task,
    const char* par_name,
    double value[1]);

/**

 * Get the index corresponding to a double parameter name.
 * 
 * # Arguments
 * - `par_name[.cstring]` (in) Name of the parameter
 */
typedef int32_t (*MSK12_get_double_param_index_func_t)(const char* par_name);
extern MSK12_get_double_param_index_func_t MSK12_get_double_param_index_ptr;
int32_t MSK12_get_double_param_index(const char* par_name);

/**

 * Get the index corresponding to a double parameter name.
 * 
 * # Arguments
 * - `par_idx` 
 */
typedef const char* (*MSK12_get_double_param_name_func_t)(int32_t par_idx);
extern MSK12_get_double_param_name_func_t MSK12_get_double_param_name_ptr;
const char* MSK12_get_double_param_name(int32_t par_idx);

/**

 * Get the index corresponding to a double parameter name.
 */
typedef int32_t (*MSK12_get_num_double_param_func_t)();
extern MSK12_get_num_double_param_func_t MSK12_get_num_double_param_ptr;
int32_t MSK12_get_num_double_param();

/**

 * Get the index corresponding to a double parameter name.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `buflen` 
 * - `buf[buflen]` (out) Target buffer
 */
typedef void (*MSK12_get_all_double_params_func_t)(MSK12_Task_t task,int32_t buflen,double* buf);
extern MSK12_get_all_double_params_func_t MSK12_get_all_double_params_ptr;
void MSK12_get_all_double_params(
    MSK12_Task_t task,
    int32_t buflen,
    double* buf);

/**

 * Set all double parameters.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_par` 
 * - `params[num_par]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_all_double_params_func_t)(MSK12_Task_t task,int32_t num_par,const double* params);
extern MSK12_put_all_double_params_func_t MSK12_put_all_double_params_ptr;
MSK12_ResCode MSK12_put_all_double_params(
    MSK12_Task_t task,
    int32_t num_par,
    const double* params);

/**

 * Get the index corresponding to a integer parameter name.
 * 
 * # Arguments
 * - `par_name[.cstring]` (in) Name of the parameter
 */
typedef int32_t (*MSK12_get_int_param_index_func_t)(const char* par_name);
extern MSK12_get_int_param_index_func_t MSK12_get_int_param_index_ptr;
int32_t MSK12_get_int_param_index(const char* par_name);

/**

 * Get the index corresponding to a integer parameter name.
 * 
 * # Arguments
 * - `par_idx` 
 */
typedef const char* (*MSK12_get_int_param_name_func_t)(int32_t par_idx);
extern MSK12_get_int_param_name_func_t MSK12_get_int_param_name_ptr;
const char* MSK12_get_int_param_name(int32_t par_idx);

/**

 * Get the index corresponding to a double parameter name.
 */
typedef int32_t (*MSK12_get_num_int_param_func_t)();
extern MSK12_get_num_int_param_func_t MSK12_get_num_int_param_ptr;
int32_t MSK12_get_num_int_param();

/**

 * Get the index corresponding to a double parameter name.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `buflen` 
 * - `buf[buflen]` (out) Target buffer
 */
typedef void (*MSK12_get_all_int_params_func_t)(MSK12_Task_t task,int32_t buflen,int32_t* buf);
extern MSK12_get_all_int_params_func_t MSK12_get_all_int_params_ptr;
void MSK12_get_all_int_params(
    MSK12_Task_t task,
    int32_t buflen,
    int32_t* buf);

/**

 * Set all integer parameters.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_par` 
 * - `params[num_par]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_all_int_params_func_t)(MSK12_Task_t task,int32_t num_par,const int32_t* params);
extern MSK12_put_all_int_params_func_t MSK12_put_all_int_params_ptr;
MSK12_ResCode MSK12_put_all_int_params(
    MSK12_Task_t task,
    int32_t num_par,
    const int32_t* params);

/**

 * Get the current value of a named parameter. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `par_name[.cstring]` (in) Name of the parameter
 * - `value[1]` (out) 
 */
typedef int32_t (*MSK12_get_int_param_func_t)(MSK12_Task_t task,const char* par_name,int32_t value[1]);
extern MSK12_get_int_param_func_t MSK12_get_int_param_ptr;
int32_t MSK12_get_int_param(
    MSK12_Task_t task,
    const char* par_name,
    int32_t value[1]);

/**

 * Get the length of the string representation of the current value of a named parameter. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `par_name[.cstring]` (in) Name of the parameter
 */
typedef int32_t (*MSK12_get_param_str_len_func_t)(MSK12_Task_t task,const char* par_name);
extern MSK12_get_param_str_len_func_t MSK12_get_param_str_len_ptr;
int32_t MSK12_get_param_str_len(
    MSK12_Task_t task,
    const char* par_name);

/**

 * Get the string representation of the current value of a named parameter. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `name[.cstring]` (in) 
 * - `length` 
 * - `buf[length]` (out) If the parameter is not recognized, the returned string is 0.
 */
typedef void (*MSK12_get_param_str_func_t)(MSK12_Task_t task,const char* name,int32_t length,char* buf);
extern MSK12_get_param_str_func_t MSK12_get_param_str_ptr;
void MSK12_get_param_str(
    MSK12_Task_t task,
    const char* name,
    int32_t length,
    char* buf);

/**

 * Set the value of a named parameter.
 * 
 * The function will succeed if either the parameter is _not_ recognized or if the parameter is
 * recognized and the given value is within valid bounds it. If the parameter is recognized but
 * the value is invalid, it will fail.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `par_name[.cstring]` (in) The parameter name is the lower case name of the MOSEK parameter without "MSK_" prefix
 * - `value` 
 * - `ok[1]` (out) Returns true if the parameter was found and successfully set, otherwise false. 
 */
typedef MSK12_ResCode (*MSK12_put_double_param_func_t)(MSK12_Task_t task,const char* par_name,double value,int ok[1]);
extern MSK12_put_double_param_func_t MSK12_put_double_param_ptr;
MSK12_ResCode MSK12_put_double_param(
    MSK12_Task_t task,
    const char* par_name,
    double value,
    int ok[1]);

/**

 * Set the value of a named parameter.
 * 
 * The function will succeed if either the parameter is _not_ recognized or if the parameter is
 * recognized and the given value is within valid bounds it. If the parameter is recognized but
 * the value is invalid, it will fail.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `par_name[.cstring]` (in) The parameter name is the lower case name of the MOSEK parameter without "MSK_" prefix
 * - `value` 
 * - `ok[1]` (out) Returns true if the parameter was found and successfully set, otherwise false. 
 */
typedef MSK12_ResCode (*MSK12_put_int_param_func_t)(MSK12_Task_t task,const char* par_name,int32_t value,int ok[1]);
extern MSK12_put_int_param_func_t MSK12_put_int_param_ptr;
MSK12_ResCode MSK12_put_int_param(
    MSK12_Task_t task,
    const char* par_name,
    int32_t value,
    int ok[1]);

/**

 * Set the value of a named parameter as a string.
 * 
 * The function will succeed if either the parameter is _not_ recognized or if the parameter is
 * recognized and the given value is within valid bounds it. If the parameter is recognized but
 * the value is invalid, it will fail.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `par_name[.cstring]` (in) The parameter name is the lower case name of the MOSEK parameter without "MSK_" prefix
 * - `value[.cstring]` (in) The string representation of the parameter value. For double
parameters, this is the ascii string representation of the
floating point value. For integer parameters this can be
either the ascii representation of the integer value or, for
parameters that accept symbolic values, the lower case value
name without "MSK_" prefix.
 * - `ok[1]` (out) Returns true if the parameter was found and successfully set, otherwise false. 
 */
typedef MSK12_ResCode (*MSK12_put_param_str_func_t)(MSK12_Task_t task,const char* par_name,const char* value,int ok[1]);
extern MSK12_put_param_str_func_t MSK12_put_param_str_ptr;
MSK12_ResCode MSK12_put_param_str(
    MSK12_Task_t task,
    const char* par_name,
    const char* value,
    int ok[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int32_t (*MSK12_get_task_name_len_func_t)(MSK12_Task_t task);
extern MSK12_get_task_name_len_func_t MSK12_get_task_name_len_ptr;
int32_t MSK12_get_task_name_len(MSK12_Task_t task);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int32_t (*MSK12_get_obj_name_len_func_t)(MSK12_Task_t task);
extern MSK12_get_obj_name_len_func_t MSK12_get_obj_name_len_ptr;
int32_t MSK12_get_obj_name_len(MSK12_Task_t task);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `capacity` 
 * - `buf[capacity]` (out) Target buffer
 */
typedef void (*MSK12_get_task_name_func_t)(MSK12_Task_t task,int32_t capacity,char* buf);
extern MSK12_get_task_name_func_t MSK12_get_task_name_ptr;
void MSK12_get_task_name(
    MSK12_Task_t task,
    int32_t capacity,
    char* buf);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `capacity` 
 * - `buf[capacity]` (out) Target buffer
 */
typedef void (*MSK12_get_obj_name_func_t)(MSK12_Task_t task,int32_t capacity,char* buf);
extern MSK12_get_obj_name_func_t MSK12_get_obj_name_ptr;
void MSK12_get_obj_name(
    MSK12_Task_t task,
    int32_t capacity,
    char* buf);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `name[.cstring]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_task_name_func_t)(MSK12_Task_t task,const char* name);
extern MSK12_put_task_name_func_t MSK12_put_task_name_ptr;
MSK12_ResCode MSK12_put_task_name(
    MSK12_Task_t task,
    const char* name);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `name[.cstring]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_obj_name_func_t)(MSK12_Task_t task,const char* name);
extern MSK12_put_obj_name_func_t MSK12_put_obj_name_ptr;
MSK12_ResCode MSK12_put_obj_name(
    MSK12_Task_t task,
    const char* name);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `var_idx` Variable index
 * - `name_len[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_var_name_len_func_t)(MSK12_Task_t task,int32_t var_idx,int32_t name_len[1]);
extern MSK12_get_var_name_len_func_t MSK12_get_var_name_len_ptr;
MSK12_ResCode MSK12_get_var_name_len(
    MSK12_Task_t task,
    int32_t var_idx,
    int32_t name_len[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `var_idx` Variable index
 */
typedef int32_t (*MSK12_get_var_name_len2_func_t)(MSK12_Task_t task,int32_t var_idx);
extern MSK12_get_var_name_len2_func_t MSK12_get_var_name_len2_ptr;
int32_t MSK12_get_var_name_len2(
    MSK12_Task_t task,
    int32_t var_idx);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `barvar_idx` Positive semi-definite variable index
 * - `name_len[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_barvar_name_len_func_t)(MSK12_Task_t task,int32_t barvar_idx,int32_t name_len[1]);
extern MSK12_get_barvar_name_len_func_t MSK12_get_barvar_name_len_ptr;
MSK12_ResCode MSK12_get_barvar_name_len(
    MSK12_Task_t task,
    int32_t barvar_idx,
    int32_t name_len[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `barvar_idx` Positive semi-definite variable index
 */
typedef int32_t (*MSK12_get_barvar_name_len2_func_t)(MSK12_Task_t task,int32_t barvar_idx);
extern MSK12_get_barvar_name_len2_func_t MSK12_get_barvar_name_len2_ptr;
int32_t MSK12_get_barvar_name_len2(
    MSK12_Task_t task,
    int32_t barvar_idx);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `var_idx` Variable index
 * - `capacity` 
 * - `buf[capacity]` (out) Target buffer
 */
typedef MSK12_ResCode (*MSK12_get_var_name_func_t)(MSK12_Task_t task,int32_t var_idx,int32_t capacity,char* buf);
extern MSK12_get_var_name_func_t MSK12_get_var_name_ptr;
MSK12_ResCode MSK12_get_var_name(
    MSK12_Task_t task,
    int32_t var_idx,
    int32_t capacity,
    char* buf);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `barvar_idx` Positive semi-definite variable index
 * - `capacity` 
 * - `buf[capacity]` (out) Target buffer
 */
typedef MSK12_ResCode (*MSK12_get_barvar_name_func_t)(MSK12_Task_t task,int32_t barvar_idx,int32_t capacity,char* buf);
extern MSK12_get_barvar_name_func_t MSK12_get_barvar_name_ptr;
MSK12_ResCode MSK12_get_barvar_name(
    MSK12_Task_t task,
    int32_t barvar_idx,
    int32_t capacity,
    char* buf);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `var_idx` Variable index
 * - `name[.cstring]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_var_name_func_t)(MSK12_Task_t task,int32_t var_idx,const char* name);
extern MSK12_put_var_name_func_t MSK12_put_var_name_ptr;
MSK12_ResCode MSK12_put_var_name(
    MSK12_Task_t task,
    int32_t var_idx,
    const char* name);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `barvar_idx` Positive semi-definite variable index
 * - `name[.cstring]` (in) 
 */
typedef MSK12_ResCode (*MSK12_put_barvar_name_func_t)(MSK12_Task_t task,int32_t barvar_idx,const char* name);
extern MSK12_put_barvar_name_func_t MSK12_put_barvar_name_ptr;
MSK12_ResCode MSK12_put_barvar_name(
    MSK12_Task_t task,
    int32_t barvar_idx,
    const char* name);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `con_idx` Constraint index 
 * - `len[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_con_name_len_func_t)(MSK12_Task_t task,int64_t con_idx,int32_t len[1]);
extern MSK12_get_con_name_len_func_t MSK12_get_con_name_len_ptr;
MSK12_ResCode MSK12_get_con_name_len(
    MSK12_Task_t task,
    int64_t con_idx,
    int32_t len[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `djc_idx` Disjunctive constraint index
 * - `len[1]` (out) 
 */
typedef MSK12_ResCode (*MSK12_get_djc_name_len_func_t)(MSK12_Task_t task,int64_t djc_idx,int32_t len[1]);
extern MSK12_get_djc_name_len_func_t MSK12_get_djc_name_len_ptr;
MSK12_ResCode MSK12_get_djc_name_len(
    MSK12_Task_t task,
    int64_t djc_idx,
    int32_t len[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `con_idx` Constraint index 
 */
typedef int32_t (*MSK12_get_con_name_len2_func_t)(MSK12_Task_t task,int64_t con_idx);
extern MSK12_get_con_name_len2_func_t MSK12_get_con_name_len2_ptr;
int32_t MSK12_get_con_name_len2(
    MSK12_Task_t task,
    int64_t con_idx);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `djc_idx` Disjunctive constraint index
 */
typedef int32_t (*MSK12_get_djc_name_len2_func_t)(MSK12_Task_t task,int64_t djc_idx);
extern MSK12_get_djc_name_len2_func_t MSK12_get_djc_name_len2_ptr;
int32_t MSK12_get_djc_name_len2(
    MSK12_Task_t task,
    int64_t djc_idx);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `con_idx` Constraint index 
 * - `capacity` 
 * - `buf[capacity]` (out) Target buffer
 */
typedef MSK12_ResCode (*MSK12_get_con_name_func_t)(MSK12_Task_t task,int64_t con_idx,int32_t capacity,char* buf);
extern MSK12_get_con_name_func_t MSK12_get_con_name_ptr;
MSK12_ResCode MSK12_get_con_name(
    MSK12_Task_t task,
    int64_t con_idx,
    int32_t capacity,
    char* buf);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `djc_idx` Disjunctive constraint index
 * - `capacity` 
 * - `buf[capacity]` (out) Target buffer
 */
typedef MSK12_ResCode (*MSK12_get_djc_name_func_t)(MSK12_Task_t task,int64_t djc_idx,int32_t capacity,char* buf);
extern MSK12_get_djc_name_func_t MSK12_get_djc_name_ptr;
MSK12_ResCode MSK12_get_djc_name(
    MSK12_Task_t task,
    int64_t djc_idx,
    int32_t capacity,
    char* buf);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `con_idx` Constraint index 
 * - `buf[.cstring]` (in) Target buffer
 */
typedef MSK12_ResCode (*MSK12_put_con_name_func_t)(MSK12_Task_t task,int64_t con_idx,const char* buf);
extern MSK12_put_con_name_func_t MSK12_put_con_name_ptr;
MSK12_ResCode MSK12_put_con_name(
    MSK12_Task_t task,
    int64_t con_idx,
    const char* buf);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `djc_idx` Disjunctive constraint index
 * - `buf[.cstring]` (in) Target buffer
 */
typedef MSK12_ResCode (*MSK12_put_djc_name_func_t)(MSK12_Task_t task,int64_t djc_idx,const char* buf);
extern MSK12_put_djc_name_func_t MSK12_put_djc_name_ptr;
MSK12_ResCode MSK12_put_djc_name(
    MSK12_Task_t task,
    int64_t djc_idx,
    const char* buf);

/**

 * Write task, base the format at on the file name extension.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `filename[.cstring]` (in) 
 */
typedef MSK12_ResCode (*MSK12_write_task_to_file_func_t)(MSK12_Task_t task,const char* filename);
extern MSK12_write_task_to_file_func_t MSK12_write_task_to_file_ptr;
MSK12_ResCode MSK12_write_task_to_file(
    MSK12_Task_t task,
    const char* filename);

/**

 * Write task via a function and a handle.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `format` 
 * - `compress` 
 * - `handle` Callback handle specified when inputting the callback function
 * - `func` 
 */
typedef MSK12_ResCode (*MSK12_write_task_to_handle_func_t)(MSK12_Task_t task,MSK12_Format format,MSK12_Compression compress,MSK12_WriteHandle handle,MSK12_WriteFunc func);
extern MSK12_write_task_to_handle_func_t MSK12_write_task_to_handle_ptr;
MSK12_ResCode MSK12_write_task_to_handle(
    MSK12_Task_t task,
    MSK12_Format format,
    MSK12_Compression compress,
    MSK12_WriteHandle handle,
    MSK12_WriteFunc func);

/**

 * Write solution, base the format at on the file name extension.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `filename[.cstring]` (in) 
 */
typedef MSK12_ResCode (*MSK12_write_solution_to_file_func_t)(MSK12_Task_t task,const char* filename);
extern MSK12_write_solution_to_file_func_t MSK12_write_solution_to_file_ptr;
MSK12_ResCode MSK12_write_solution_to_file(
    MSK12_Task_t task,
    const char* filename);

/**

 * Write solution via a function and a handle.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `format` 
 * - `compress` 
 * - `handle` Callback handle specified when inputting the callback function
 * - `func` 
 */
typedef MSK12_ResCode (*MSK12_write_solution_to_handle_func_t)(MSK12_Task_t task,MSK12_SolutionFormat format,MSK12_Compression compress,MSK12_WriteHandle handle,MSK12_WriteFunc func);
extern MSK12_write_solution_to_handle_func_t MSK12_write_solution_to_handle_ptr;
MSK12_ResCode MSK12_write_solution_to_handle(
    MSK12_Task_t task,
    MSK12_SolutionFormat format,
    MSK12_Compression compress,
    MSK12_WriteHandle handle,
    MSK12_WriteFunc func);

/**

 * Reset task and read data from file, base the format on the file extension.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `filename[.cstring]` (in) 
 */
typedef MSK12_ResCode (*MSK12_read_from_file_func_t)(MSK12_Task_t task,const char* filename);
extern MSK12_read_from_file_func_t MSK12_read_from_file_ptr;
MSK12_ResCode MSK12_read_from_file(
    MSK12_Task_t task,
    const char* filename);

/**

 * Reset task and read data from stream.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `format` 
 * - `compress` 
 * - `handle` Callback handle specified when inputting the callback function
 * - `func` 
 */
typedef MSK12_ResCode (*MSK12_read_from_handle_func_t)(MSK12_Task_t task,MSK12_Format format,MSK12_Compression compress,MSK12_ReadHandle handle,MSK12_ReadFunc func);
extern MSK12_read_from_handle_func_t MSK12_read_from_handle_ptr;
MSK12_ResCode MSK12_read_from_handle(
    MSK12_Task_t task,
    MSK12_Format format,
    MSK12_Compression compress,
    MSK12_ReadHandle handle,
    MSK12_ReadFunc func);

/**

 * Write a stream to a file. This will open the file and either append to it or write a clear and rewrite it.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `whichstream` 
 * - `filename[.cstring]` (in) 
 * - `append` 
 */
typedef MSK12_ResCode (*MSK12_put_stream_file_func_t)(MSK12_Task_t task,MSK12_StreamType whichstream,const char* filename,int append);
extern MSK12_put_stream_file_func_t MSK12_put_stream_file_ptr;
MSK12_ResCode MSK12_put_stream_file(
    MSK12_Task_t task,
    MSK12_StreamType whichstream,
    const char* filename,
    int append);

/**

 * Close file attached to a stream.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `whichstream` 
 */
typedef MSK12_ResCode (*MSK12_clear_stream_file_func_t)(MSK12_Task_t task,MSK12_StreamType whichstream);
extern MSK12_clear_stream_file_func_t MSK12_clear_stream_file_ptr;
MSK12_ResCode MSK12_clear_stream_file(
    MSK12_Task_t task,
    MSK12_StreamType whichstream);

/**

 * Attach a log stream writer callback function to a stream. At most
 * one file and one callback can be attached to the same stream.
 * 
 * The callback method can be called at any point during a call to a
 * task method. It is forbidden to access the task it
 * is attached to from the callback function.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `whichstream` 
 * - `handle` Callback handle specified when inputting the callback function
 * - `func` 
 */
typedef MSK12_ResCode (*MSK12_put_stream_callback_func_t)(MSK12_Task_t task,MSK12_StreamType whichstream,MSK12_WriteHandle handle,MSK12_StreamFunc func);
extern MSK12_put_stream_callback_func_t MSK12_put_stream_callback_ptr;
MSK12_ResCode MSK12_put_stream_callback(
    MSK12_Task_t task,
    MSK12_StreamType whichstream,
    MSK12_WriteHandle handle,
    MSK12_StreamFunc func);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `whichstream` 
 */
typedef MSK12_ResCode (*MSK12_clear_stream_callback_func_t)(MSK12_Task_t task,MSK12_StreamType whichstream);
extern MSK12_clear_stream_callback_func_t MSK12_clear_stream_callback_ptr;
MSK12_ResCode MSK12_clear_stream_callback(
    MSK12_Task_t task,
    MSK12_StreamType whichstream);

/**

 * Attach an error callback. This will be called whenever a task
 * function produces an error. It is forbidden to access
 * the task it is attached to from the callback function.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `handle` Callback handle specified when inputting the callback function
 * - `func` 
 */
typedef MSK12_ResCode (*MSK12_put_error_callback_func_t)(MSK12_Task_t task,MSK12_ErrorCallbackHandle handle,MSK12_ErrorCallbackFunc func);
extern MSK12_put_error_callback_func_t MSK12_put_error_callback_ptr;
MSK12_ResCode MSK12_put_error_callback(
    MSK12_Task_t task,
    MSK12_ErrorCallbackHandle handle,
    MSK12_ErrorCallbackFunc func);

/**

 * Attach a warning callback. This will be called whenever a task
 * function produces an error. It is forbidden to access the task it is
 * attached to from the callback function.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `handle` Callback handle specified when inputting the callback function
 * - `func` 
 */
typedef MSK12_ResCode (*MSK12_put_warning_callback_func_t)(MSK12_Task_t task,MSK12_ErrorCallbackHandle handle,MSK12_ErrorCallbackFunc func);
extern MSK12_put_warning_callback_func_t MSK12_put_warning_callback_ptr;
MSK12_ResCode MSK12_put_warning_callback(
    MSK12_Task_t task,
    MSK12_ErrorCallbackHandle handle,
    MSK12_ErrorCallbackFunc func);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef MSK12_ResCode (*MSK12_clear_error_callback_func_t)(MSK12_Task_t task);
extern MSK12_clear_error_callback_func_t MSK12_clear_error_callback_ptr;
MSK12_ResCode MSK12_clear_error_callback(MSK12_Task_t task);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef MSK12_ResCode (*MSK12_clear_warning_callback_func_t)(MSK12_Task_t task);
extern MSK12_clear_warning_callback_func_t MSK12_clear_warning_callback_ptr;
MSK12_ResCode MSK12_clear_warning_callback(MSK12_Task_t task);

/**

 * Stops all threads and deletes all handles used by the license system. If this
 * function is called, it must be called as the last MOSEK API call. No other
 * MOSEK API calls are valid after this.
 */
typedef void (*MSK12_license_cleanup_func_t)();
extern MSK12_license_cleanup_func_t MSK12_license_cleanup_ptr;
void MSK12_license_cleanup();

/**

 * If MOSEK is using a global threadpool, attempt to shut
 * this down. If there are currently jobs running, this will do
 * nothing.
 */
typedef void (*MSK12_shutdown_global_threadpool_func_t)();
extern MSK12_shutdown_global_threadpool_func_t MSK12_shutdown_global_threadpool_ptr;
void MSK12_shutdown_global_threadpool();

/**

 * Computes vector addition and multiplication by a scalar. 
 * 
 * # Arguments
 * - `n` Length of the vectors. 
 * - `alpha` The scalar that multiplies x. 
 * - `x[n]` (in) The x vector. 
 * - `y[n]` (in-out) The y vector. 
 */
typedef MSK12_ResCode (*MSK12_axpy_func_t)(int32_t n,double alpha,const double* x,double* y);
extern MSK12_axpy_func_t MSK12_axpy_ptr;
MSK12_ResCode MSK12_axpy(
    int32_t n,
    double alpha,
    const double* x,
    double* y);

/**

 * Computes the inner product of two vectors. 
 * 
 * # Arguments
 * - `n` Length of the vectors. 
 * - `x[n]` (in) The x vector. 
 * - `y[n]` (in) The y vector. 
 * - `xty[1]` (out) The result of the inner product. 
 */
typedef MSK12_ResCode (*MSK12_dot_func_t)(int32_t n,const double* x,const double* y,double xty[1]);
extern MSK12_dot_func_t MSK12_dot_ptr;
MSK12_ResCode MSK12_dot(
    int32_t n,
    const double* x,
    const double* y,
    double xty[1]);

/**

 * Computes the multiplication of a scaled dense matrix times a dense vector, plus a scaled dense vector. Precisely, if `transa` is false then the update is
 * 
 * $$
 * y := \\alpha A x + \\beta y,
 * $$
 * 
 * and if `transa` is true
 * 
 * $$
 * y := \\alpha A^T x + \\beta y,
 * $$
 * 
 * where \\(\\alpha,\\beta\\) are scalar values and \\(A\\) is a matrix with \\(m\\) rows and \\(n\\) columns.
 * 
 * Note that the result is stored overwriting \\(y\\). It must not overlap with the other input arrays.
 * 
 * # Arguments
 * - `transa` Indicates whether the matrix A must be transposed. 
 * - `m` Specifies the number of rows of the matrix A. 
 * - `n` Specifies the number of columns of the matrix A. 
 * - `alpha` A scalar value multiplying the matrix A. 
 * - `a` (in) A pointer to the array storing matrix A in a column-major format. 
 * - `x` (in) A pointer to the array storing the vector x. 
 * - `beta` A scalar value multiplying the vector y. 
 * - `y` (in-out) A pointer to the array storing the vector y. 
 */
typedef MSK12_ResCode (*MSK12_gemv_func_t)(int transa,int32_t m,int32_t n,double alpha,const double* a,const double* x,double beta,double* y);
extern MSK12_gemv_func_t MSK12_gemv_ptr;
MSK12_ResCode MSK12_gemv(
    int transa,
    int32_t m,
    int32_t n,
    double alpha,
    const double* a,
    const double* x,
    double beta,
    double* y);

/**

 * Performs a matrix multiplication plus addition of dense matrices.
 * 
 * Given
 * \\(A\\), \\(B\\) and \\(C\\) of compatible dimensions, this function
 * computes
 * 
 * .. math:: C:= \\alpha \\mathrm{op}(A)\\mathrm{op}(B) + \\beta C
 * 
 * where \\(\\alpha,\\beta\\) are two scalar values. The function \\(\mathrm{op}(X)\\)
 * denotes \\(X\\) if transX is false, or \\(X^T\\) if set to true. The matrix \\(C\\) has \\(m\\) rows and \\(n\\) columns, and the other matrices must have compatible dimensions.
 * 
 * The result of this operation is stored in \\(C\\). It must not overlap with the other input arrays.
 * 
 * # Arguments
 * - `transa` Indicates whether the matrix A must be transposed. 
 * - `transb` Indicates whether the matrix B must be transposed. 
 * - `m` Indicates the number of rows of matrix C. 
 * - `n` Indicates the number of columns of matrix C. 
 * - `k` Specifies the common dimension along which op(A) and op(B) are multiplied. 
 * - `alpha` A scalar value multiplying the result of the matrix multiplication. 
 * - `a` (in) The pointer to the array storing matrix A in a column-major format. 
 * - `b` (in) The pointer to the array storing matrix B in a column-major format.  
 * - `beta` A scalar value that multiplies C. 
 * - `c` (in-out) The pointer to the array storing matrix C in a column-major format. 
 */
typedef MSK12_ResCode (*MSK12_gemm_func_t)(int transa,int transb,int32_t m,int32_t n,int32_t k,double alpha,const double* a,const double* b,double beta,double* c);
extern MSK12_gemm_func_t MSK12_gemm_ptr;
MSK12_ResCode MSK12_gemm(
    int transa,
    int transb,
    int32_t m,
    int32_t n,
    int32_t k,
    double alpha,
    const double* a,
    const double* b,
    double beta,
    double* c);

/**

 * Performs a symmetric rank-\\(k\\) update for a symmetric matrix.
 * 
 * Given a symmetric matrix \\(C\\in \\real^{n\\times n}\\), two scalars
 * \\(\\alpha,\\beta\\) and a matrix \\(A\\) of rank \\(k\\leq n\\), it
 * computes either
 * 
 * .. math:: C := \\alpha A A^T + \\beta C,
 * 
 * when `trans` is set to false and \\(A\\in \\real^{n\\times k}\\), or
 * 
 * .. math:: C := \\alpha A^T A + \\beta C,
 * 
 * when `trans` is set to true and \\(A\\in \\real^{k\\times n}\\).
 * 
 * Only the part of \\(C\\) indicated by `is_upr` is used and only that part is updated with the result. It must not overlap with the other input arrays.
 * 
 * # Arguments
 * - `is_upr` Indicates whether the upper or lower triangular part of C is used. 
 * - `trans` Indicates whether the matrix A must be transposed. 
 * - `n` Specifies the order of \\(C\\).
 * - `k` Indicates the number of rows or columns of \\(A\\), depending on whether or not it is transposed, and its rank.
 * - `alpha` A scalar value multiplying the result of the matrix multiplication. 
 * - `a` (in) The pointer to the array storing matrix A in a column-major format. 
 * - `beta` A scalar value that multiplies C. 
 * - `c` (in-out) The pointer to the array storing matrix C in a column-major format. 
 */
typedef MSK12_ResCode (*MSK12_syrk_func_t)(int is_upr,int trans,int32_t n,int32_t k,double alpha,const double* a,double beta,double* c);
extern MSK12_syrk_func_t MSK12_syrk_ptr;
MSK12_ResCode MSK12_syrk(
    int is_upr,
    int trans,
    int32_t n,
    int32_t k,
    double alpha,
    const double* a,
    double beta,
    double* c);

/**

 * The function solves a triangular system of the form
 * 
 * .. math:: L x = b
 * 
 * or
 * 
 * .. math:: L^T x = b
 * 
 * where \\(L\\) is a sparse lower triangular nonsingular matrix. This implies in particular that diagonals in \\(L\\) are nonzero.
 * 
 * # Arguments
 * - `transposed` Controls whether the solve is with L or the transposed L. 
 * - `n` Specifies the dimension of L. 
 * - `lnzc[n]` (in) `lnzc[j]` is the number of nonzeros in column j. 
 * - `lsubc` (in) Row indexes for each column stored sequentially. 
 * - `lvalc` (in) The value corresponding to row indexed stored lsubc. 
 * - `b[n]` (in-out) The right-hand side of linear equation system to be solved as a dense vector. 
 */
typedef MSK12_ResCode (*MSK12_sparse_triangular_solve_dense_func_t)(int transposed,int32_t n,const int32_t* lnzc,const int32_t* lsubc,const double* lvalc,double* b);
extern MSK12_sparse_triangular_solve_dense_func_t MSK12_sparse_triangular_solve_dense_ptr;
MSK12_ResCode MSK12_sparse_triangular_solve_dense(
    int transposed,
    int32_t n,
    const int32_t* lnzc,
    const int32_t* lsubc,
    const double* lvalc,
    double* b);

/**

 * Computes a Cholesky factorization of a real symmetric positive definite dense matrix.
 * 
 * # Arguments
 * - `is_upr` Indicates whether the upper or lower triangular part of the matrix is stored. 
 * - `n` Dimension of the symmetric matrix. 
 * - `a` (in-out) A symmetric matrix stored in column-major order. 
 */
typedef MSK12_ResCode (*MSK12_potrf_func_t)(int is_upr,int32_t n,double* a);
extern MSK12_potrf_func_t MSK12_potrf_ptr;
MSK12_ResCode MSK12_potrf(
    int is_upr,
    int32_t n,
    double* a);

/**

 * Computes all eigenvalues of a real symmetric matrix \\(A\\). Given a matrix \\(A\\in\\real^{n\\times n}\\) it returns a vector \\(w\\in\\real^n\\) containing the eigenvalues of \\(A\\). 
 * 
 * # Arguments
 * - `is_upr` Indicates whether the upper or lower triangular part is used. 
 * - `n` Dimension of the symmetric input matrix. 
 * - `a` (in) Input matrix A. 
 * - `w[n]` (out) Array of length at least n containing the eigenvalues of A. 
 */
typedef MSK12_ResCode (*MSK12_syeig_func_t)(int is_upr,int32_t n,const double* a,double* w);
extern MSK12_syeig_func_t MSK12_syeig_ptr;
MSK12_ResCode MSK12_syeig(
    int is_upr,
    int32_t n,
    const double* a,
    double* w);

/**

 * Computes all the eigenvalues and eigenvectors a real symmetric matrix.
 * Given the input matrix \\(A\\in \\real^{n\\times n}\\), this function returns a
 * vector \\(w\\in \\real^n\\) containing the eigenvalues of \\(A\\) and it also computes the eigenvectors
 * of \\(A\\). Therefore, this function computes the eigenvalue decomposition of \\(A\\) as
 * 
 * .. math:: A= U V U^T,
 * 
 * where \\(V=\\diag(w)\\) and \\(U\\) contains the eigenvectors of \\(A\\).
 * 
 * Note that the matrix \\(U\\) overwrites the input data \\(A\\).
 * 
 * # Arguments
 * - `is_upr` Indicates whether the upper or lower triangular part is used. 
 * - `n` Dimension of the symmetric input matrix. 
 * - `a` (in-out) Input matrix A. 
 * - `w[n]` (in-out) Array of length at least n containing the eigenvalues of A. 
 */
typedef MSK12_ResCode (*MSK12_syevd_func_t)(int is_upr,int32_t n,double* a,double* w);
extern MSK12_syevd_func_t MSK12_syevd_ptr;
MSK12_ResCode MSK12_syevd(
    int is_upr,
    int32_t n,
    double* a,
    double* w);

/**

 * The function computes a Cholesky factorization of a sparse positive semidefinite matrix. Sparsity is exploited
 * during the computations to reduce the amount of space and work required. Both the input and output matrices
 * are represented using the sparse format.
 * 
 * To be precise, given a symmetric matrix \\(A \\in \\real^{n\\times n}\\) the function computes a nonsingular lower triangular matrix \\(L\\), a diagonal matrix \\(D\\) and a permutation matrix \\(P\\) such that
 * 
 * $$
 * LL^T - D = P A P^T
 * $$
 * 
 * If ``order_method`` is zero then reordering heuristics are not employed and \\(P\\) is the identity.
 * 
 * If a pivot during the computation of the Cholesky factorization is less than
 * 
 * $$
 * -\\rho\\cdot\\max((PAP^T)_{jj},1.0)
 * $$
 * 
 * then the matrix is declared negative semidefinite. On the hand if a pivot is smaller than
 * 
 * $$
 * \\rho\\cdot\\max((PAP^T)_{jj},1.0),
 * $$
 * 
 * then \\(D_{jj}\\) is increased from zero to
 * 
 * $$
 * \\rho\\cdot\\max((PAP^T)_{jj},1.0).
 * $$
 * 
 * Therefore, if \\(A\\) is sufficiently positive definite then \\(D\\) will be the zero matrix.
 * Here \\(\\rho\\) is set equal to value of ``tol_singular``.
 * 
 * # Arguments
 * - `num_threads` The number threads that can be used to do the computation. 0 means the code makes the choice. 
 * - `order_method` If nonzero, then a sparsity preserving ordering will be employed. 
 * - `tol_singular` A positive parameter controlling when a pivot is declared zero. 
 * - `n` Specifies the order of \\(A\\). 
 * - `a_col_num_nonzero[n]` (in) `a_col_num_nonzero[j]` is a pointer to the first element in column \\(j\\). 
 * - `a_subi` (in) Row indexes for each column stored in increasing order. 
 * - `a_val` (in) The value corresponding to row indexed stored in asubc. 
 * - `perm[n]` (out) Permutation array used to specify the permutation matrix \\(P\\) computed by the function. 
 * - `diag[n]` (out) The diagonal elements of matrix \\(D\\). 
 * - `alloc` Memory allocation function for allocating the result. If `null`, the system `malloc` function is used. 
 * - `alloc_handle` Handle passed to the memory allocation function 
 * - `l_col_num_nonzero[n]` (out) `l_col_num_nonzero[j]` is the number of non zero elements in column \\(j\\) of \\(L\\). 
 * - `l_subi` (out) Row indexes for each column stored in increasing order. The returned array is guaranteed to be allocated with the allocation function `alloc`.

Notice that upon return, whether the function failed or suceeded, if a non-null value is returned here, it means that it was allocated and it must be deallocated acordingly.
 * - `l_val` (out) The values corresponding to row indexed stored in lsubc. The returned array is guaranteed to be allocated with the allocation function `alloc`.

Notice that upon return, whether the function failed or suceeded, if a non-null value is returned here, it means that it was allocated and it must be deallocated acordingly.
 */
typedef MSK12_ResCode (*MSK12_compute_sparse_cholesky_func_t)(int32_t num_threads,int order_method,double tol_singular,int32_t n,const int32_t* a_col_num_nonzero,const int32_t* a_subi,const double* a_val,int32_t* perm,double* diag,MSK12_AllocFunc alloc,MSK12_AllocHandle alloc_handle,int32_t* l_col_num_nonzero,int32_t** l_subi,double** l_val);
extern MSK12_compute_sparse_cholesky_func_t MSK12_compute_sparse_cholesky_ptr;
MSK12_ResCode MSK12_compute_sparse_cholesky(
    int32_t num_threads,
    int order_method,
    double tol_singular,
    int32_t n,
    const int32_t* a_col_num_nonzero,
    const int32_t* a_subi,
    const double* a_val,
    int32_t* perm,
    double* diag,
    MSK12_AllocFunc alloc,
    MSK12_AllocHandle alloc_handle,
    int32_t* l_col_num_nonzero,
    int32_t** l_subi,
    double** l_val);

/**

 * Optimize a number of tasks in parallel using a specified number of threads. All
 * callbacks and log output streams are disabled.
 * 
 * Assuming that each task takes about same time and there many more tasks than number of
 * threads then a linear speedup can be achieved, also known as strong scaling. A typical
 * application of this method is to solve many small tasks of similar type; in this case
 * it is recommended that each of them is allocated a single thread by setting parameter `ipar_num_threads` to 1.
 * 
 * If the parameters `is_race` or `max_time_sec` are used, then the result may not be deterministic, in the sense that the tasks which complete first may vary between runs.
 * 
 * The remaining behavior, including termination and response codes returned for each task, are the same as if each task was optimized separately.
 * 
 * # Arguments
 * - `is_race` If nonzero, then the function is terminated after the first task has been completed.
 * - `max_time_sec` Time limit for the function in seconds. 
 * - `num_threads` Number of threads to be employed.
 * - `num_task` Number of tasks to optimize. 
 * - `tasks[num_task]` (in) An array of tasks to optimize in parallel. 
 * - `trm_code[num_task]` (out) The termination code for each task. 
 * - `res_code[num_task]` (out) The response code for each task. 
 */
typedef MSK12_ResCode (*MSK12_optimize_batch_func_t)(int is_race,double max_time_sec,int32_t num_threads,int64_t num_task,const MSK12_Task_t* tasks,MSK12_TrmCode* trm_code,MSK12_ResCode* res_code);
extern MSK12_optimize_batch_func_t MSK12_optimize_batch_ptr;
MSK12_ResCode MSK12_optimize_batch(
    int is_race,
    double max_time_sec,
    int32_t num_threads,
    int64_t num_task,
    const MSK12_Task_t* tasks,
    MSK12_TrmCode* trm_code,
    MSK12_ResCode* res_code);

/**

 * Checks out a license feature from the license server. Normally the required
 * license features will be automatically checked out the first time they are needed
 * by the function [optimize](#func-optimize). This function can be used to check out one
 * or more features ahead of time.
 * 
 * The feature will remain checked out until the environment is deleted or the function
 * [check_in_license](func:check_in_license) is called.
 * 
 * If a given feature is already checked out when this function is called, the call has no effect.
 * 
 * # Arguments
 * - `feature` Feature to check out from the license system. 
 */
typedef MSK12_ResCode (*MSK12_check_out_license_func_t)(MSK12_Feature feature);
extern MSK12_check_out_license_func_t MSK12_check_out_license_ptr;
MSK12_ResCode MSK12_check_out_license(MSK12_Feature feature);

/**

 * Check in a license feature to the license server. By default all licenses
 * consumed by functions using a single environment are kept checked out for the
 * lifetime of the MOSEK environment. This function checks in a given license
 * feature back to the license server immediately.
 * 
 * If the given license feature is not checked out at all, or it is in use by a call to
 * [optimize](func:optimize), calling this function has no effect.
 * 
 * Please note that returning a license to the license server incurs a small
 * overhead, so frequent calls to this function should be avoided.
 * 
 * # Arguments
 * - `feature` Feature to check in to the license system. 
 */
typedef MSK12_ResCode (*MSK12_check_in_license_func_t)(MSK12_Feature feature);
extern MSK12_check_in_license_func_t MSK12_check_in_license_ptr;
MSK12_ResCode MSK12_check_in_license(MSK12_Feature feature);

/**

 * Check in all unused license features to the license token server. 
 */
typedef MSK12_ResCode (*MSK12_check_in_all_func_t)();
extern MSK12_check_in_all_func_t MSK12_check_in_all_ptr;
MSK12_ResCode MSK12_check_in_all();

/**

 * Prints an intro to message stream. 
 * 
 * # Arguments
 * - `long_ver` If non-zero, then the intro is slightly longer. 
 */
typedef MSK12_ResCode (*MSK12_echo_intro_func_t)(int long_ver);
extern MSK12_echo_intro_func_t MSK12_echo_intro_ptr;
MSK12_ResCode MSK12_echo_intro(int long_ver);

/**

 * Obtains MOSEK version information. 
 * 
 * # Arguments
 * - `major[1]` (out) Major version number. 
 * - `minor[1]` (out) Minor version number. 
 * - `revision[1]` (out) Revision number. 
 */
typedef void (*MSK12_get_version_func_t)(int32_t major[1],int32_t minor[1],int32_t revision[1]);
extern MSK12_get_version_func_t MSK12_get_version_ptr;
void MSK12_get_version(
    int32_t major[1],
    int32_t minor[1],
    int32_t revision[1]);

/**

 * Enables debug information for the license system. If `lic_debug` is non-zero, then MOSEK will print debug info regarding the license checkout.  
 * 
 * # Arguments
 * - `lic_debug` Whether license checkout debug info should be printed.  
 */
typedef MSK12_ResCode (*MSK12_put_license_debug_func_t)(int lic_debug);
extern MSK12_put_license_debug_func_t MSK12_put_license_debug_ptr;
MSK12_ResCode MSK12_put_license_debug(int lic_debug);

/**

 * Input a runtime license code.  This function has an effect only before the first optimization. 
 * 
 * # Arguments
 * - `code[21]` (in, nullable) A license key string. 
 */
typedef MSK12_ResCode (*MSK12_put_license_code_func_t)(NULLABLE const int32_t code[21]);
extern MSK12_put_license_code_func_t MSK12_put_license_code_ptr;
MSK12_ResCode MSK12_put_license_code(NULLABLE const int32_t code[21]);

/**

 * Control whether MOSEK should wait for an available license if no license is available. If `lic_wait` is non-zero, then MOSEK will wait for `lic_wait-1` milliseconds between each check for an available license.
 * 
 * # Arguments
 * - `lic_wait` Enable waiting for a license until it is available. 
 */
typedef MSK12_ResCode (*MSK12_put_license_wait_func_t)(int32_t lic_wait);
extern MSK12_put_license_wait_func_t MSK12_put_license_wait_ptr;
MSK12_ResCode MSK12_put_license_wait(int32_t lic_wait);

/**

 * Set the path to the license file. This function has an effect only before the first optimization. 
 * 
 * # Arguments
 * - `license_path[.cstring]` (in, nullable) A path specifying where to search for the license. 
 */
typedef MSK12_ResCode (*MSK12_put_license_path_func_t)(NULLABLE const char* license_path);
extern MSK12_put_license_path_func_t MSK12_put_license_path_ptr;
MSK12_ResCode MSK12_put_license_path(NULLABLE const char* license_path);


int MSK12_initialize_library_with_paths(const char * paths[]);
int MSK12_library_initialized();
int MSK12_initialize_library();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
