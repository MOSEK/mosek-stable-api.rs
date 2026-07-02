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

#define MSK120_RES_OK 0
#define MSK120_TRM_OK 0


struct MSK120_Task_s;
typedef struct MSK120_Task_s * MSK120_Task_t;

typedef enum MSK120_DomainType_enum {
  MSK120_DOMAIN_NIL,
  MSK120_DOMAIN_RZERO,
  MSK120_DOMAIN_RPLUS,
  MSK120_DOMAIN_RMINUS,
  MSK120_DOMAIN_R,
  MSK120_DOMAIN_QUADRATIC_CONE,
  MSK120_DOMAIN_ROTATED_QUADRATIC_CONE,
  MSK120_DOMAIN_PRIMAL_EXP_CONE,
  MSK120_DOMAIN_DUAL_EXP_CONE,
  MSK120_DOMAIN_PRIMAL_POWER_CONE,
  MSK120_DOMAIN_DUAL_POWER_CONE,
  MSK120_DOMAIN_PRIMAL_GEOMETRIC_MEAN_CONE,
  MSK120_DOMAIN_DUAL_GEOMETRIC_MEAN_CONE,
  MSK120_DOMAIN_SVEC_PSD_CONE,
} MSK120_DomainType;

typedef enum MSK120_Feature_enum {
  MSK120_FEATURE_PTON,
  MSK120_FEATURE_PTS,
} MSK120_Feature;

typedef enum MSK120_ObjSense_enum {
  MSK120_OBJ_SENSE_MINIMIZE,
  MSK120_OBJ_SENSE_MAXIMIZE,
} MSK120_ObjSense;

typedef enum MSK120_SolType_enum {
  MSK120_SOL_TYPE_BASIC,
  MSK120_SOL_TYPE_INTERIOR,
  MSK120_SOL_TYPE_INTEGER,
  MSK120_SOL_TYPE_UNKNOWN,
} MSK120_SolType;

/**
 * Status of a primal or a dual solution.
 */
typedef enum MSK120_SolSta_enum {
  MSK120_SOL_STA_UNKNOWN,
  MSK120_SOL_STA_UNDEFINED,
  MSK120_SOL_STA_OPTIMAL,
  MSK120_SOL_STA_INTEGER_OPTIMAL,
  MSK120_SOL_STA_FEASIBLE,
  MSK120_SOL_STA_INFEAS_CERT,
  MSK120_SOL_STA_ILLPOSED_CERT,
} MSK120_SolSta;

typedef enum MSK120_ProSta_enum {
  MSK120_PRO_STA_UNKNOWN,
  MSK120_PRO_STA_PRIMAL_AND_DUAL_FEASIBLE,
  MSK120_PRO_STA_PRIMAL_FEASIBLE,
  MSK120_PRO_STA_DUAL_FEASIBLE,
  MSK120_PRO_STA_PRIMAL_INFEASIBLE,
  MSK120_PRO_STA_DUAL_INFEASIBLE,
  MSK120_PRO_STA_PRIMAL_AND_DUAL_INFEASIBLE,
  MSK120_PRO_STA_ILLPOSED,
  MSK120_PRO_STA_PRIMAL_INFEASIBLE_OR_UNBOUNDED,
} MSK120_ProSta;

typedef enum MSK120_Format_enum {
  MSK120_FORMAT_PTF,
  MSK120_FORMAT_TASK,
  MSK120_FORMAT_JTASK,
} MSK120_Format;

typedef enum MSK120_VariableType_enum {
  MSK120_VAR_TYPE_INTEGER,
  MSK120_VAR_TYPE_CONTINUOUS,
} MSK120_VariableType;

typedef enum MSK120_Compression_enum {
  MSK120_COMPRESS_NONE,
  MSK120_COMPRESS_GZIP,
  MSK120_COMPRESS_ZSTD,
} MSK120_Compression;

typedef enum MSK120_SolutionFormat_enum {
  MSK120_SOL_FORMAT_TASK,
  MSK120_SOL_FORMAT_JTASK,
  MSK120_SOL_FORMAT_TEXT,
} MSK120_SolutionFormat;

typedef enum MSK120_StreamType_enum {
  MSK120_STREAM_MSG,
  MSK120_STREAM_WRN,
  MSK120_STREAM_ERR,
  MSK120_STREAM_LOG,
} MSK120_StreamType;


/**
 * A value indicating the result of a function call. 0 indicates success,
 * any other value an error. The exact meaning of the values is undefined,
 * and may change between versions. Use `get_resp_descr` and `get_resp_name`
 * to get description and string representation for the code.
 */
typedef int32_t MSK120_ResCode;
/**
 * Optimizer termination code. 0 indicates normal termination, anything
 * else indicates that the optimizer termianted for other reasons than
 * optimality or valid certificate.
 */
typedef int32_t MSK120_TrmCode;
/**
 * Handle for reading from a stream via function callback.
 */
typedef void* MSK120_ReadHandle;
/**
 * Handle for writing to a stream via function callback. 
 */
typedef void* MSK120_WriteHandle;
/**
 * Stream reader function type. The reader function MUST work as follows:
 * 
 * On end-of-file, the reader function must return 0, and all subsequent calls must also return 0. If `num` is 0, the
 * function must return -1, and it does not indicate an error. Otherwise, the reader function MUST return read at least
 * one byte and at most `num` bytes. The function may perform any number of blocking reads to an underlying stream.
 * 
 * It is forbidden to access the tash object that the callback function is attached to from the callback function.
 */
typedef size_t (*MSK120_ReadFunc)(MSK120_ReadHandle h,void* dest,size_t num);
/**
 * Stream writer function type. The writer function MUST work as follows:
 * 
 * On error, the function must return 0, and all subsequent calls for the same handle must return 0. If `num` is 0, the
 * function must return 0 and it will not indicate an error. Otherwise the function must return the number of bytes written.
 * 
 * It is forbidden to access the tash object that the callback function is attached to from the callback function.
 */
typedef size_t (*MSK120_WriteFunc)(MSK120_WriteHandle h,const void* src,size_t num);
/**
 * Message stream writer function type.
 * The function must write the entire string give or fail silently.
 * It is forbidden to access the tash object that the callback function is attached to from the callback function.
 */
typedef void (*MSK120_StreamFunc)(MSK120_WriteHandle h,const char* src);
/**
 * Handle for callback functions.
 */
typedef void* MSK120_CallbackHandle;
/**
 * Handle for error and warning callback to a stream via function callback. 
 */
typedef void* MSK120_ErrorCallbackHandle;
/**
 * Function type for error and warning callback. This is attached to a task and called whenever a function call to the
 * task produces an error or a warning.
 * 
 * It is forbidden to access the task object that the callback function is attached to from the callback function.
 */
typedef void (*MSK120_ErrorCallbackFunc)(MSK120_ErrorCallbackHandle h,int32_t r,const char* name,const char* desc,const char* message);
/**
 * Information callback function.
 */
typedef int32_t (*MSK120_CallbackFunc)(MSK120_CallbackHandle h,int32_t code,int32_t len_iinf,const int32_t* iinf,int32_t len_liinf,const int64_t* liinf,int32_t len_dinf,const double* dinf);
/**
 * Integer solution callback function.
 */
typedef void (*MSK120_IntSolCallbackFunc)(MSK120_CallbackHandle handle,int32_t num,const double* xx);


#ifdef __cplusplus
extern "C" {
#endif

/**

 * Get the name of a callback code 
 * 
 * # Arguments
 * - `code` 
 */
typedef const char* (*MSK120_get_callback_code_name_func_t)(int32_t code);
extern MSK120_get_callback_code_name_func_t MSK120_get_callback_code_name_ptr;
const char* MSK120_get_callback_code_name(int32_t code);

/**

 * Get string representing the given response code.
 * 
 * # Arguments
 * - `r` 
 */
typedef const char* (*MSK120_get_resp_name_func_t)(MSK120_ResCode r);
extern MSK120_get_resp_name_func_t MSK120_get_resp_name_ptr;
const char* MSK120_get_resp_name(MSK120_ResCode r);

/**

 * Get string with a description of the given response code.
 * 
 * # Arguments
 * - `r` 
 */
typedef const char* (*MSK120_get_resp_descr_func_t)(MSK120_ResCode r);
extern MSK120_get_resp_descr_func_t MSK120_get_resp_descr_ptr;
const char* MSK120_get_resp_descr(MSK120_ResCode r);

/**

 * Get the last error response code 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef MSK120_ResCode (*MSK120_get_last_resp_func_t)(MSK120_Task_t task);
extern MSK120_get_last_resp_func_t MSK120_get_last_resp_ptr;
MSK120_ResCode MSK120_get_last_resp(MSK120_Task_t task);

/**

 * Get a string with name of the last error message recorded in the task.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `buf[buf_len]` (out) Last message will be copied here, truncated to `buf_len` including trailing 0.
 * - `buf_len` Length of target buffer
 */
typedef MSK120_ResCode (*MSK120_get_last_resp_msg_func_t)(MSK120_Task_t task,char* buf,size_t buf_len);
extern MSK120_get_last_resp_msg_func_t MSK120_get_last_resp_msg_ptr;
MSK120_ResCode MSK120_get_last_resp_msg(
    MSK120_Task_t task,
    char* buf,
    size_t buf_len);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef size_t (*MSK120_get_last_resp_msg_len_func_t)(MSK120_Task_t task);
extern MSK120_get_last_resp_msg_len_func_t MSK120_get_last_resp_msg_len_ptr;
size_t MSK120_get_last_resp_msg_len(MSK120_Task_t task);

/**

 * Get string representing the given termination code.
 * 
 * # Arguments
 * - `trm` 
 */
typedef const char* (*MSK120_get_trm_name_func_t)(MSK120_TrmCode trm);
extern MSK120_get_trm_name_func_t MSK120_get_trm_name_ptr;
const char* MSK120_get_trm_name(MSK120_TrmCode trm);

/**

 * Get string with a description if the given termination code.
 * 
 * # Arguments
 * - `trm` 
 */
typedef const char* (*MSK120_get_trm_descr_func_t)(MSK120_TrmCode trm);
extern MSK120_get_trm_descr_func_t MSK120_get_trm_descr_ptr;
const char* MSK120_get_trm_descr(MSK120_TrmCode trm);

/**

 * Create new task. On failure NULL is returned.
 */
typedef MSK120_Task_t (*MSK120_new_task_func_t)();
extern MSK120_new_task_func_t MSK120_new_task_ptr;
MSK120_Task_t MSK120_new_task();

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
typedef MSK120_Task_t (*MSK120_new_task_from_task_func_t)(MSK120_Task_t task);
extern MSK120_new_task_from_task_func_t MSK120_new_task_from_task_ptr;
MSK120_Task_t MSK120_new_task_from_task(MSK120_Task_t task);

/**

 * Delete task. This is assumed to always succeed.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef void (*MSK120_delete_task_func_t)(MSK120_Task_t task);
extern MSK120_delete_task_func_t MSK120_delete_task_ptr;
void MSK120_delete_task(MSK120_Task_t task);

/**

 * Reserve space for scalar variables. This is to be considered a _hint_, not a requirement. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `add_num` Number of item to add
 */
typedef MSK120_ResCode (*MSK120_reserve_num_var_func_t)(MSK120_Task_t task,int32_t add_num);
extern MSK120_reserve_num_var_func_t MSK120_reserve_num_var_ptr;
MSK120_ResCode MSK120_reserve_num_var(
    MSK120_Task_t task,
    int32_t add_num);

/**

 * Reserve space for semidefinite variables. This is to be considered a _hint_, not a requirement.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_barvar` Number of variables
 */
typedef MSK120_ResCode (*MSK120_reserve_num_barvar_func_t)(MSK120_Task_t task,int32_t num_barvar);
extern MSK120_reserve_num_barvar_func_t MSK120_reserve_num_barvar_ptr;
MSK120_ResCode MSK120_reserve_num_barvar(
    MSK120_Task_t task,
    int32_t num_barvar);

/**

 * Reserve space for constraints. This is to be considered a _hint_, not a requirement.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_con` Number of constraints
 */
typedef MSK120_ResCode (*MSK120_reserve_num_con_func_t)(MSK120_Task_t task,int32_t num_con);
extern MSK120_reserve_num_con_func_t MSK120_reserve_num_con_ptr;
MSK120_ResCode MSK120_reserve_num_con(
    MSK120_Task_t task,
    int32_t num_con);

/**

 * Reserve space for afes. This is to be considered a _hint_, not a requirement.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_row` Number of affine rows
 */
typedef MSK120_ResCode (*MSK120_reserve_num_row_func_t)(MSK120_Task_t task,int64_t num_row);
extern MSK120_reserve_num_row_func_t MSK120_reserve_num_row_ptr;
MSK120_ResCode MSK120_reserve_num_row(
    MSK120_Task_t task,
    int64_t num_row);

/**

 * Reserve space for coefficient matrix non-zeros. This is to be considered a _hint_, not a requirement.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_nz` 
 */
typedef MSK120_ResCode (*MSK120_reserve_num_nz_func_t)(MSK120_Task_t task,int64_t num_nz);
extern MSK120_reserve_num_nz_func_t MSK120_reserve_num_nz_ptr;
MSK120_ResCode MSK120_reserve_num_nz(
    MSK120_Task_t task,
    int64_t num_nz);

/**

 * Reserve space for domains. This is to be considered a _hint_, not a requirement.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_dom` Number of domains
 */
typedef MSK120_ResCode (*MSK120_reserve_num_dom_func_t)(MSK120_Task_t task,int64_t num_dom);
extern MSK120_reserve_num_dom_func_t MSK120_reserve_num_dom_ptr;
MSK120_ResCode MSK120_reserve_num_dom(
    MSK120_Task_t task,
    int64_t num_dom);

/**

 * Reserve space for symmetric matrixes. This is to be considered a _hint_, not a requirement.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_symmat` Number of symmetric matrixes
 */
typedef MSK120_ResCode (*MSK120_reserve_num_symmat_func_t)(MSK120_Task_t task,int64_t num_symmat);
extern MSK120_reserve_num_symmat_func_t MSK120_reserve_num_symmat_ptr;
MSK120_ResCode MSK120_reserve_num_symmat(
    MSK120_Task_t task,
    int64_t num_symmat);

/**

 * Reserve space for symmetric matrix variables. This is to be considered a _hint_, not a requirement.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_nz` 
 */
typedef MSK120_ResCode (*MSK120_reserve_num_symmat_nz_func_t)(MSK120_Task_t task,int64_t num_nz);
extern MSK120_reserve_num_symmat_nz_func_t MSK120_reserve_num_symmat_nz_ptr;
MSK120_ResCode MSK120_reserve_num_symmat_nz(
    MSK120_Task_t task,
    int64_t num_nz);

/**

 * Get number of scalar variables. Cannot fail.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int32_t (*MSK120_get_num_var_func_t)(MSK120_Task_t task);
extern MSK120_get_num_var_func_t MSK120_get_num_var_ptr;
int32_t MSK120_get_num_var(MSK120_Task_t task);

/**

 * Get number of semidefinite variables. Cannot fail.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int32_t (*MSK120_get_num_barvar_func_t)(MSK120_Task_t task);
extern MSK120_get_num_barvar_func_t MSK120_get_num_barvar_ptr;
int32_t MSK120_get_num_barvar(MSK120_Task_t task);

/**

 * Get number of domains.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int64_t (*MSK120_get_num_domain_func_t)(MSK120_Task_t task);
extern MSK120_get_num_domain_func_t MSK120_get_num_domain_ptr;
int64_t MSK120_get_num_domain(MSK120_Task_t task);

/**

 * Get number of affine rows.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int64_t (*MSK120_get_num_row_func_t)(MSK120_Task_t task);
extern MSK120_get_num_row_func_t MSK120_get_num_row_ptr;
int64_t MSK120_get_num_row(MSK120_Task_t task);

/**

 * Get number of symmetric matrixes
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int64_t (*MSK120_get_num_symmat_func_t)(MSK120_Task_t task);
extern MSK120_get_num_symmat_func_t MSK120_get_num_symmat_ptr;
int64_t MSK120_get_num_symmat(MSK120_Task_t task);

/**

 * Get number of constraints.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int64_t (*MSK120_get_num_con_func_t)(MSK120_Task_t task);
extern MSK120_get_num_con_func_t MSK120_get_num_con_ptr;
int64_t MSK120_get_num_con(MSK120_Task_t task);

/**

 * Get number of disjunctive constraints.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int64_t (*MSK120_get_num_djc_func_t)(MSK120_Task_t task);
extern MSK120_get_num_djc_func_t MSK120_get_num_djc_ptr;
int64_t MSK120_get_num_djc(MSK120_Task_t task);

/**

 * Append scalar variables.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_var` Number of variables
 */
typedef MSK120_ResCode (*MSK120_append_vars_func_t)(MSK120_Task_t task,int32_t num_var);
extern MSK120_append_vars_func_t MSK120_append_vars_ptr;
MSK120_ResCode MSK120_append_vars(
    MSK120_Task_t task,
    int32_t num_var);

/**

 * Append a number of empty affine rows
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_row` Number of affine rows
 */
typedef MSK120_ResCode (*MSK120_append_rows_func_t)(MSK120_Task_t task,int64_t num_row);
extern MSK120_append_rows_func_t MSK120_append_rows_ptr;
MSK120_ResCode MSK120_append_rows(
    MSK120_Task_t task,
    int64_t num_row);

/**

 * Append a single semidefinite variable.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dim` Dimension
 */
typedef MSK120_ResCode (*MSK120_append_barvar_func_t)(MSK120_Task_t task,int32_t dim);
extern MSK120_append_barvar_func_t MSK120_append_barvar_ptr;
MSK120_ResCode MSK120_append_barvar(
    MSK120_Task_t task,
    int32_t dim);

/**

 * Append `num` semidefinite variables with the given dimensions.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_barvar` Number of variables
 * - `dims[num_barvar]` (in) Array of dimensionms
 */
typedef MSK120_ResCode (*MSK120_append_barvars_func_t)(MSK120_Task_t task,int32_t num_barvar,const int32_t* dims);
extern MSK120_append_barvars_func_t MSK120_append_barvars_ptr;
MSK120_ResCode MSK120_append_barvars(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_append_symmat_func_t)(MSK120_Task_t task,int32_t dim,int64_t nnz,const int32_t* symmat_i,const int32_t* symmat_j,const double* symmat_val);
extern MSK120_append_symmat_func_t MSK120_append_symmat_ptr;
MSK120_ResCode MSK120_append_symmat(
    MSK120_Task_t task,
    int32_t dim,
    int64_t nnz,
    const int32_t* symmat_i,
    const int32_t* symmat_j,
    const double* symmat_val);

/**

 * Append a list of symmetric matrix.
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
typedef MSK120_ResCode (*MSK120_append_symmats_func_t)(MSK120_Task_t task,int64_t num_symmat,const int32_t* dim,const int64_t* nnz,const int32_t* symmat_i,const int32_t* symmat_j,const double* symmat_val);
extern MSK120_append_symmats_func_t MSK120_append_symmats_ptr;
MSK120_ResCode MSK120_append_symmats(
    MSK120_Task_t task,
    int64_t num_symmat,
    const int32_t* dim,
    const int64_t* nnz,
    const int32_t* symmat_i,
    const int32_t* symmat_j,
    const double* symmat_val);

/**

 * Append `num` empty constraints, initially it will have domain `null`.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_con` Number of constraints
 */
typedef MSK120_ResCode (*MSK120_append_empty_cons_func_t)(MSK120_Task_t task,int64_t num_con);
extern MSK120_append_empty_cons_func_t MSK120_append_empty_cons_ptr;
MSK120_ResCode MSK120_append_empty_cons(
    MSK120_Task_t task,
    int64_t num_con);

/**

 * Append `num` empty djcs, initially each having 0 terms.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_djc` Number of disjunctive constraints
 */
typedef MSK120_ResCode (*MSK120_append_empty_djcs_func_t)(MSK120_Task_t task,int64_t num_djc);
extern MSK120_append_empty_djcs_func_t MSK120_append_empty_djcs_ptr;
MSK120_ResCode MSK120_append_empty_djcs(
    MSK120_Task_t task,
    int64_t num_djc);

/**

 * Set variable type to integer or continuous.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `j` 
 * - `var_type` 
 */
typedef MSK120_ResCode (*MSK120_put_var_type_func_t)(MSK120_Task_t task,int32_t j,MSK120_VariableType var_type);
extern MSK120_put_var_type_func_t MSK120_put_var_type_ptr;
MSK120_ResCode MSK120_put_var_type(
    MSK120_Task_t task,
    int32_t j,
    MSK120_VariableType var_type);

/**

 * Set variable type to integer or continuous.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `var_types[num_var]` (in) 
 */
typedef MSK120_ResCode (*MSK120_put_var_type_slice_func_t)(MSK120_Task_t task,int32_t first_var,int32_t num_var,const MSK120_VariableType* var_types);
extern MSK120_put_var_type_slice_func_t MSK120_put_var_type_slice_ptr;
MSK120_ResCode MSK120_put_var_type_slice(
    MSK120_Task_t task,
    int32_t first_var,
    int32_t num_var,
    const MSK120_VariableType* var_types);

/**

 * Set variable type to integer or continuous.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `var_type` 
 */
typedef MSK120_ResCode (*MSK120_put_var_type_slice_value_func_t)(MSK120_Task_t task,int32_t first_var,int32_t num_var,MSK120_VariableType var_type);
extern MSK120_put_var_type_slice_value_func_t MSK120_put_var_type_slice_value_ptr;
MSK120_ResCode MSK120_put_var_type_slice_value(
    MSK120_Task_t task,
    int32_t first_var,
    int32_t num_var,
    MSK120_VariableType var_type);

/**

 * Set variable type to integer or continuous.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_var` Number of variables
 * - `var_idxs[num_var]` (in) Array of variable indexes
 * - `var_types[num_var]` (in) 
 */
typedef MSK120_ResCode (*MSK120_put_var_type_list_func_t)(MSK120_Task_t task,int32_t num_var,const int32_t* var_idxs,const MSK120_VariableType* var_types);
extern MSK120_put_var_type_list_func_t MSK120_put_var_type_list_ptr;
MSK120_ResCode MSK120_put_var_type_list(
    MSK120_Task_t task,
    int32_t num_var,
    const int32_t* var_idxs,
    const MSK120_VariableType* var_types);

/**

 * Get variable type as integer or continuous.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `var_idx` Variable index
 * - `var_type[1]` (out) 
 */
typedef MSK120_ResCode (*MSK120_get_var_type_func_t)(MSK120_Task_t task,int32_t var_idx,MSK120_VariableType var_type[1]);
extern MSK120_get_var_type_func_t MSK120_get_var_type_ptr;
MSK120_ResCode MSK120_get_var_type(
    MSK120_Task_t task,
    int32_t var_idx,
    MSK120_VariableType var_type[1]);

/**

 * Get variable slice types as integer or continuous.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `first_var` Variable index
 * - `num_var` Number of variables
 * - `var_types[num_var]` (out) 
 */
typedef MSK120_ResCode (*MSK120_get_var_type_slice_func_t)(MSK120_Task_t task,int32_t first_var,int32_t num_var,MSK120_VariableType* var_types);
extern MSK120_get_var_type_slice_func_t MSK120_get_var_type_slice_ptr;
MSK120_ResCode MSK120_get_var_type_slice(
    MSK120_Task_t task,
    int32_t first_var,
    int32_t num_var,
    MSK120_VariableType* var_types);

/**

 * Set variable bounds.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `var_idx` Variable index
 * - `low` Lower bound
 * - `upr` Upper bound
 */
typedef MSK120_ResCode (*MSK120_put_var_bound_func_t)(MSK120_Task_t task,int32_t var_idx,double low,double upr);
extern MSK120_put_var_bound_func_t MSK120_put_var_bound_ptr;
MSK120_ResCode MSK120_put_var_bound(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_var_bound_slice_func_t)(MSK120_Task_t task,int32_t first_var,int32_t num_var,const double* low,const double* upr);
extern MSK120_put_var_bound_slice_func_t MSK120_put_var_bound_slice_ptr;
MSK120_ResCode MSK120_put_var_bound_slice(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_var_bound_slice_value_func_t)(MSK120_Task_t task,int32_t first_var,int32_t num_var,double low,double upr);
extern MSK120_put_var_bound_slice_value_func_t MSK120_put_var_bound_slice_value_ptr;
MSK120_ResCode MSK120_put_var_bound_slice_value(
    MSK120_Task_t task,
    int32_t first_var,
    int32_t num_var,
    double low,
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
typedef MSK120_ResCode (*MSK120_get_var_bound_func_t)(MSK120_Task_t task,int32_t var_idx,double low[1],double upr[1]);
extern MSK120_get_var_bound_func_t MSK120_get_var_bound_ptr;
MSK120_ResCode MSK120_get_var_bound(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_var_bound_slice_func_t)(MSK120_Task_t task,int32_t first_var,int32_t num_var,double* low,double* upr);
extern MSK120_get_var_bound_slice_func_t MSK120_get_var_bound_slice_ptr;
MSK120_ResCode MSK120_get_var_bound_slice(
    MSK120_Task_t task,
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
 * - `num_elm[num_barvar]` (out) Number of positive semidefinite non-zero entries
 */
typedef MSK120_ResCode (*MSK120_get_barvar_slice_num_elm_func_t)(MSK120_Task_t task,int32_t first_barvar,int32_t num_barvar,int64_t* num_elm);
extern MSK120_get_barvar_slice_num_elm_func_t MSK120_get_barvar_slice_num_elm_ptr;
MSK120_ResCode MSK120_get_barvar_slice_num_elm(
    MSK120_Task_t task,
    int32_t first_barvar,
    int32_t num_barvar,
    int64_t* num_elm);

/**

 * Get dimension of semidefinite variable
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `barvar_idx` Positive semi-definite variable index
 * - `dim[1]` (out) Dimension
 */
typedef MSK120_ResCode (*MSK120_get_barvar_dim_func_t)(MSK120_Task_t task,int32_t barvar_idx,int32_t dim[1]);
extern MSK120_get_barvar_dim_func_t MSK120_get_barvar_dim_ptr;
MSK120_ResCode MSK120_get_barvar_dim(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_barvar_slice_dims_func_t)(MSK120_Task_t task,int32_t first_barvar,int32_t num_barvar,int32_t* dim);
extern MSK120_get_barvar_slice_dims_func_t MSK120_get_barvar_slice_dims_ptr;
MSK120_ResCode MSK120_get_barvar_slice_dims(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_domain_func_t)(MSK120_Task_t task,MSK120_DomainType dom_type,int64_t dim,int32_t num_alpha,double* alpha,int64_t dom_idx[1]);
extern MSK120_get_domain_func_t MSK120_get_domain_ptr;
MSK120_ResCode MSK120_get_domain(
    MSK120_Task_t task,
    MSK120_DomainType dom_type,
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
typedef MSK120_ResCode (*MSK120_get_domain_empty_func_t)(MSK120_Task_t task,int64_t dom_idx[1]);
extern MSK120_get_domain_empty_func_t MSK120_get_domain_empty_ptr;
MSK120_ResCode MSK120_get_domain_empty(
    MSK120_Task_t task,
    int64_t dom_idx[1]);

/**

 * Return index of an `rzero` domain. Only one `rzero` domain is created, so if one already exists, that one is returned instead of creating a new domain
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK120_ResCode (*MSK120_get_domain_rzero_func_t)(MSK120_Task_t task,int64_t dom_idx[1]);
extern MSK120_get_domain_rzero_func_t MSK120_get_domain_rzero_ptr;
MSK120_ResCode MSK120_get_domain_rzero(
    MSK120_Task_t task,
    int64_t dom_idx[1]);

/**

 * Return index of an `rplus` domain. Only one `rplus` domain is created, so if one already exists, that one is returned instead of creating a new domain
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK120_ResCode (*MSK120_get_domain_rplus_func_t)(MSK120_Task_t task,int64_t dom_idx[1]);
extern MSK120_get_domain_rplus_func_t MSK120_get_domain_rplus_ptr;
MSK120_ResCode MSK120_get_domain_rplus(
    MSK120_Task_t task,
    int64_t dom_idx[1]);

/**

 * Return index of an `rminus` domain. Only one `rminus` domain is created, so if one already exists, that one is returned instead of creating a new domain
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK120_ResCode (*MSK120_get_domain_rminus_func_t)(MSK120_Task_t task,int64_t dom_idx[1]);
extern MSK120_get_domain_rminus_func_t MSK120_get_domain_rminus_ptr;
MSK120_ResCode MSK120_get_domain_rminus(
    MSK120_Task_t task,
    int64_t dom_idx[1]);

/**

 * Return index of an `r` domain. Only one `r` domain is created, so if one already exists, that one is returned instead of creating a new domain
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK120_ResCode (*MSK120_get_domain_r_func_t)(MSK120_Task_t task,int64_t dom_idx[1]);
extern MSK120_get_domain_r_func_t MSK120_get_domain_r_ptr;
MSK120_ResCode MSK120_get_domain_r(
    MSK120_Task_t task,
    int64_t dom_idx[1]);

/**

 * Return the index of a quadratic cone domain of the given size.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `n` 
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK120_ResCode (*MSK120_get_domain_quadratic_cone_func_t)(MSK120_Task_t task,int64_t n,int64_t dom_idx[1]);
extern MSK120_get_domain_quadratic_cone_func_t MSK120_get_domain_quadratic_cone_ptr;
MSK120_ResCode MSK120_get_domain_quadratic_cone(
    MSK120_Task_t task,
    int64_t n,
    int64_t dom_idx[1]);

/**

 * Return the index of a rotated quadratic cone domain of the given size.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `n` 
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK120_ResCode (*MSK120_get_domain_rotated_quadratic_cone_func_t)(MSK120_Task_t task,int64_t n,int64_t dom_idx[1]);
extern MSK120_get_domain_rotated_quadratic_cone_func_t MSK120_get_domain_rotated_quadratic_cone_ptr;
MSK120_ResCode MSK120_get_domain_rotated_quadratic_cone(
    MSK120_Task_t task,
    int64_t n,
    int64_t dom_idx[1]);

/**

 * Return index if an `primal_exponential` domain. Only one
 * `primal_exponential` domain is created, so if one already exists,
 * that one is returned instead of creating a new domain
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK120_ResCode (*MSK120_get_domain_primal_exponential_cone_func_t)(MSK120_Task_t task,int64_t dom_idx[1]);
extern MSK120_get_domain_primal_exponential_cone_func_t MSK120_get_domain_primal_exponential_cone_ptr;
MSK120_ResCode MSK120_get_domain_primal_exponential_cone(
    MSK120_Task_t task,
    int64_t dom_idx[1]);

/**

 * Return index if an `dual_exponential` domain. Only one
 * `dual_exponential` domain is created, so if one already exists, that
 * one is returned instead of creating a new domain
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK120_ResCode (*MSK120_get_domain_dual_exponential_cone_func_t)(MSK120_Task_t task,int64_t dom_idx[1]);
extern MSK120_get_domain_dual_exponential_cone_func_t MSK120_get_domain_dual_exponential_cone_ptr;
MSK120_ResCode MSK120_get_domain_dual_exponential_cone(
    MSK120_Task_t task,
    int64_t dom_idx[1]);

/**

 * Return the index of a new primal power cone.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `n` 
 * - `num_alpha` Number of alpha values in array
 * - `alpha[num_alpha]` (in) Array if alpha values for power cone domain
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK120_ResCode (*MSK120_get_domain_primal_power_cone_func_t)(MSK120_Task_t task,int64_t n,int64_t num_alpha,const double* alpha,int64_t dom_idx[1]);
extern MSK120_get_domain_primal_power_cone_func_t MSK120_get_domain_primal_power_cone_ptr;
MSK120_ResCode MSK120_get_domain_primal_power_cone(
    MSK120_Task_t task,
    int64_t n,
    int64_t num_alpha,
    const double* alpha,
    int64_t dom_idx[1]);

/**

 * Return the index of a new dual power cone.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `n` 
 * - `num_alpha` Number of alpha values in array
 * - `alpha[num_alpha]` (in) Array if alpha values for power cone domain
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK120_ResCode (*MSK120_get_domain_dual_power_cone_func_t)(MSK120_Task_t task,int64_t n,int64_t num_alpha,const double* alpha,int64_t dom_idx[1]);
extern MSK120_get_domain_dual_power_cone_func_t MSK120_get_domain_dual_power_cone_ptr;
MSK120_ResCode MSK120_get_domain_dual_power_cone(
    MSK120_Task_t task,
    int64_t n,
    int64_t num_alpha,
    const double* alpha,
    int64_t dom_idx[1]);

/**

 * Get the index of a new primal geometric mean cone.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `n` 
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK120_ResCode (*MSK120_get_domain_primal_geometric_mean_cone_func_t)(MSK120_Task_t task,int64_t n,int64_t dom_idx[1]);
extern MSK120_get_domain_primal_geometric_mean_cone_func_t MSK120_get_domain_primal_geometric_mean_cone_ptr;
MSK120_ResCode MSK120_get_domain_primal_geometric_mean_cone(
    MSK120_Task_t task,
    int64_t n,
    int64_t dom_idx[1]);

/**

 * Get the index of a new dual geometric mean cone.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `n` 
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK120_ResCode (*MSK120_get_domain_dual_geometric_mean_cone_func_t)(MSK120_Task_t task,int64_t n,int64_t dom_idx[1]);
extern MSK120_get_domain_dual_geometric_mean_cone_func_t MSK120_get_domain_dual_geometric_mean_cone_ptr;
MSK120_ResCode MSK120_get_domain_dual_geometric_mean_cone(
    MSK120_Task_t task,
    int64_t n,
    int64_t dom_idx[1]);

/**

 * Get the index of a new scaled vectorized PSD cone.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `n` 
 * - `dom_idx[1]` (out) Index of the domain
 */
typedef MSK120_ResCode (*MSK120_get_domain_svecpsd_cone_func_t)(MSK120_Task_t task,int64_t n,int64_t dom_idx[1]);
extern MSK120_get_domain_svecpsd_cone_func_t MSK120_get_domain_svecpsd_cone_ptr;
MSK120_ResCode MSK120_get_domain_svecpsd_cone(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_domain_info_func_t)(MSK120_Task_t task,int64_t dom_idx,MSK120_DomainType dom_type[1],int64_t size[1],int32_t num_alpha[1]);
extern MSK120_get_domain_info_func_t MSK120_get_domain_info_ptr;
MSK120_ResCode MSK120_get_domain_info(
    MSK120_Task_t task,
    int64_t dom_idx,
    MSK120_DomainType dom_type[1],
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
typedef MSK120_ResCode (*MSK120_get_domain_alpha_func_t)(MSK120_Task_t task,int64_t dom_idx,int64_t num_alpha,double* alpha);
extern MSK120_get_domain_alpha_func_t MSK120_get_domain_alpha_ptr;
MSK120_ResCode MSK120_get_domain_alpha(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_row_func_t)(MSK120_Task_t task,int64_t row_idx,int32_t num_nz,const int32_t* subj,const double* cof);
extern MSK120_put_row_func_t MSK120_put_row_ptr;
MSK120_ResCode MSK120_put_row(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_row_slice_func_t)(MSK120_Task_t task,int64_t first_row,int64_t num_row,const int32_t* row_num_nz,const int32_t* subj,const double* cof);
extern MSK120_put_row_slice_func_t MSK120_put_row_slice_ptr;
MSK120_ResCode MSK120_put_row_slice(
    MSK120_Task_t task,
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
 * - `row_idxs[num_row]` (out) Row indexes
 * - `row_num_nz[num_row]` (in) Number of non-zeros per row.
 * - `subj` (in) List of pointers to subscripts.
 * - `cof` (in) Coefficients
 */
typedef MSK120_ResCode (*MSK120_put_row_list_func_t)(MSK120_Task_t task,int64_t num_row,int64_t* row_idxs,const int32_t* row_num_nz,const int32_t** subj,const double** cof);
extern MSK120_put_row_list_func_t MSK120_put_row_list_ptr;
MSK120_ResCode MSK120_put_row_list(
    MSK120_Task_t task,
    int64_t num_row,
    int64_t* row_idxs,
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
typedef MSK120_ResCode (*MSK120_put_row_g_func_t)(MSK120_Task_t task,int64_t row_idx,double g);
extern MSK120_put_row_g_func_t MSK120_put_row_g_ptr;
MSK120_ResCode MSK120_put_row_g(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_row_slice_g_func_t)(MSK120_Task_t task,int64_t first_row,int64_t num_row,const double* g);
extern MSK120_put_row_slice_g_func_t MSK120_put_row_slice_g_ptr;
MSK120_ResCode MSK120_put_row_slice_g(
    MSK120_Task_t task,
    int64_t first_row,
    int64_t num_row,
    const double* g);

/**

 * Input the constant terms for a list of affine rows.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `num_row` Number of affine rows
 * - `row_idxs[num_row]` (out) Array of row indexes
 * - `g[num_row]` (in) Row fixed term
 */
typedef MSK120_ResCode (*MSK120_put_row_list_g_func_t)(MSK120_Task_t task,int64_t num_row,int64_t* row_idxs,const double* g);
extern MSK120_put_row_list_g_func_t MSK120_put_row_list_g_ptr;
MSK120_ResCode MSK120_put_row_list_g(
    MSK120_Task_t task,
    int64_t num_row,
    int64_t* row_idxs,
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
typedef MSK120_ResCode (*MSK120_put_col_func_t)(MSK120_Task_t task,int32_t col_idx,int64_t num_nz,const int64_t* row_idxs,const double* cof);
extern MSK120_put_col_func_t MSK120_put_col_ptr;
MSK120_ResCode MSK120_put_col(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_col_slice_func_t)(MSK120_Task_t task,int32_t first_col,int32_t num_col,const int64_t* col_len,const int64_t* row_idxs,const double* cof);
extern MSK120_put_col_slice_func_t MSK120_put_col_slice_ptr;
MSK120_ResCode MSK120_put_col_slice(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_col_list_func_t)(MSK120_Task_t task,int32_t num_col,const int32_t* col_idxs,const int64_t* col_lens,const int64_t** row_idxs,const double** cof);
extern MSK120_put_col_list_func_t MSK120_put_col_list_ptr;
MSK120_ResCode MSK120_put_col_list(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_ijc_func_t)(MSK120_Task_t task,int64_t row_idx,int32_t var_idx,double cof);
extern MSK120_put_ijc_func_t MSK120_put_ijc_ptr;
MSK120_ResCode MSK120_put_ijc(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_ijc_list_func_t)(MSK120_Task_t task,int64_t num_nz,const int64_t* row_idxs,const int32_t* col_idxs,const double* cof);
extern MSK120_put_ijc_list_func_t MSK120_put_ijc_list_ptr;
MSK120_ResCode MSK120_put_ijc_list(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_row_num_nz_func_t)(MSK120_Task_t task,int64_t row_idx,int32_t num_nz[1]);
extern MSK120_get_row_num_nz_func_t MSK120_get_row_num_nz_ptr;
MSK120_ResCode MSK120_get_row_num_nz(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_row_slice_num_nz_func_t)(MSK120_Task_t task,int64_t first_row,int64_t num_row,int64_t* num_nz);
extern MSK120_get_row_slice_num_nz_func_t MSK120_get_row_slice_num_nz_ptr;
MSK120_ResCode MSK120_get_row_slice_num_nz(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_row_func_t)(MSK120_Task_t task,int64_t row_idx,int32_t nnz,int32_t* subj,double* cof);
extern MSK120_get_row_func_t MSK120_get_row_ptr;
MSK120_ResCode MSK120_get_row(
    MSK120_Task_t task,
    int64_t row_idx,
    int32_t nnz,
    int32_t* subj,
    double* cof);

/**

 * Get nonzeros from a single row.
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
typedef MSK120_ResCode (*MSK120_get_row_slice_func_t)(MSK120_Task_t task,int64_t first_row,int64_t num_row,int64_t nnz,int32_t* row_len,int32_t* subj,double* cof);
extern MSK120_get_row_slice_func_t MSK120_get_row_slice_ptr;
MSK120_ResCode MSK120_get_row_slice(
    MSK120_Task_t task,
    int64_t first_row,
    int64_t num_row,
    int64_t nnz,
    int32_t* row_len,
    int32_t* subj,
    double* cof);

/**

 * Input a single bar entry.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `row_idx` Index of the affine row
 * - `barvar_idx` Positive semi-definite variable index
 * - `num_weight` 
 * - `matrix_idx[num_weight]` (out) 
 * - `weight[num_weight]` (out) 
 */
typedef MSK120_ResCode (*MSK120_put_bar_entry_func_t)(MSK120_Task_t task,int64_t row_idx,int32_t barvar_idx,int64_t num_weight,int64_t* matrix_idx,double* weight);
extern MSK120_put_bar_entry_func_t MSK120_put_bar_entry_ptr;
MSK120_ResCode MSK120_put_bar_entry(
    MSK120_Task_t task,
    int64_t row_idx,
    int32_t barvar_idx,
    int64_t num_weight,
    int64_t* matrix_idx,
    double* weight);

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
typedef MSK120_ResCode (*MSK120_put_bar_entry_list_func_t)(MSK120_Task_t task,int64_t num_bar_entry,const int64_t* row_idx,const int32_t* barvar_idx,const int64_t* num_weight,const int64_t* matrix_idx,const double* weight);
extern MSK120_put_bar_entry_list_func_t MSK120_put_bar_entry_list_ptr;
MSK120_ResCode MSK120_put_bar_entry_list(
    MSK120_Task_t task,
    int64_t num_bar_entry,
    const int64_t* row_idx,
    const int32_t* barvar_idx,
    const int64_t* num_weight,
    const int64_t* matrix_idx,
    const double* weight);

/**

 * Put bar entries for a single row
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
typedef MSK120_ResCode (*MSK120_put_bar_row_func_t)(MSK120_Task_t task,int64_t row_idx,int32_t num_bar_entry,const int32_t* barvar_idx,const int64_t* num_weight,const int64_t* matrix_idx,const double* weight);
extern MSK120_put_bar_row_func_t MSK120_put_bar_row_ptr;
MSK120_ResCode MSK120_put_bar_row(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_symmat_info_func_t)(MSK120_Task_t task,int64_t symmat_idx,int32_t dim[1],int64_t nnz[1]);
extern MSK120_get_symmat_info_func_t MSK120_get_symmat_info_ptr;
MSK120_ResCode MSK120_get_symmat_info(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_symmat_func_t)(MSK120_Task_t task,int64_t symmat_idx,int64_t nnz,int32_t* symmat_i,int32_t* symmat_j,double* symmat_val);
extern MSK120_get_symmat_func_t MSK120_get_symmat_ptr;
MSK120_ResCode MSK120_get_symmat(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_symmat_slice_info_func_t)(MSK120_Task_t task,int64_t first_symmat,int64_t num_symmat,int32_t* dim,int64_t* nnz);
extern MSK120_get_symmat_slice_info_func_t MSK120_get_symmat_slice_info_ptr;
MSK120_ResCode MSK120_get_symmat_slice_info(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_symmat_slice_func_t)(MSK120_Task_t task,int64_t first_symmat,int64_t num_symmat,int64_t total_nnz,int32_t* symmat_i,int32_t* symmat_j,double* symmat_val);
extern MSK120_get_symmat_slice_func_t MSK120_get_symmat_slice_ptr;
MSK120_ResCode MSK120_get_symmat_slice(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_append_con_func_t)(MSK120_Task_t task,int64_t dom_idx,int64_t num_rows,const int64_t* row_idxs,NULLABLE const double* con_offset);
extern MSK120_append_con_func_t MSK120_append_con_ptr;
MSK120_ResCode MSK120_append_con(
    MSK120_Task_t task,
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
 * - `num_rows[num_con]` (in) List of row counts for each constraint block.
 * - `row_idxs` (in) Array of row indexes
 * - `con_offset` (in, nullable) Constraint right-hand-side offset vector, where NULL means all zeros 
 */
typedef MSK120_ResCode (*MSK120_append_cons_func_t)(MSK120_Task_t task,int64_t num_con,const int64_t* dom_idxs,const int64_t* num_rows,const int64_t* row_idxs,NULLABLE const double* con_offset);
extern MSK120_append_cons_func_t MSK120_append_cons_ptr;
MSK120_ResCode MSK120_append_cons(
    MSK120_Task_t task,
    int64_t num_con,
    const int64_t* dom_idxs,
    const int64_t* num_rows,
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
typedef MSK120_ResCode (*MSK120_put_con_func_t)(MSK120_Task_t task,int64_t con_idx,int64_t num_rows,int64_t dom_idx,const int64_t* row_idxs,NULLABLE const double* rhs_offset);
extern MSK120_put_con_func_t MSK120_put_con_ptr;
MSK120_ResCode MSK120_put_con(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_scalar_con_func_t)(MSK120_Task_t task,int64_t con_idx,int64_t dom_idx,int64_t row_idx,double rhs_offset);
extern MSK120_put_scalar_con_func_t MSK120_put_scalar_con_ptr;
MSK120_ResCode MSK120_put_scalar_con(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_con_slice_func_t)(MSK120_Task_t task,int64_t first_con,int64_t num_con,int64_t num_rows,const int64_t* dom_idx,const int64_t* row_idx,NULLABLE const double* rhs_offset);
extern MSK120_put_con_slice_func_t MSK120_put_con_slice_ptr;
MSK120_ResCode MSK120_put_con_slice(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_con_slice_domains_func_t)(MSK120_Task_t task,int64_t first_con,int64_t num_con,int64_t* dom_idx);
extern MSK120_get_con_slice_domains_func_t MSK120_get_con_slice_domains_ptr;
MSK120_ResCode MSK120_get_con_slice_domains(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_con_slice_num_row_func_t)(MSK120_Task_t task,int64_t first_con,int64_t num_con,int64_t num_row[1]);
extern MSK120_get_con_slice_num_row_func_t MSK120_get_con_slice_num_row_ptr;
MSK120_ResCode MSK120_get_con_slice_num_row(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_con_slice_func_t)(MSK120_Task_t task,int64_t first_con,int64_t num_con,int64_t num_row,int64_t* row_idx,double* rhs_offset,int64_t* dom_idx);
extern MSK120_get_con_slice_func_t MSK120_get_con_slice_ptr;
MSK120_ResCode MSK120_get_con_slice(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_append_djc_func_t)(MSK120_Task_t task,int64_t num_rows,int64_t num_dom,int64_t num_terms,const int64_t* dom_idx,const int64_t* term_size,const int64_t* row_idx,const double* rhs_offset);
extern MSK120_append_djc_func_t MSK120_append_djc_ptr;
MSK120_ResCode MSK120_append_djc(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_djc_func_t)(MSK120_Task_t task,int64_t djc_idx,int64_t num_rows,int64_t num_dom,int64_t num_terms,const int64_t* dom_idx,const int64_t* term_size,const int64_t* row_idx,const double* rhs_offset);
extern MSK120_put_djc_func_t MSK120_put_djc_ptr;
MSK120_ResCode MSK120_put_djc(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_djc_slice_func_t)(MSK120_Task_t task,int64_t first_djc,int64_t num_djc,int64_t num_rows,int64_t num_dom,int64_t num_terms,const int64_t* dom_idx,const int64_t* term_size,const int64_t* row_idx,const double* rhs_offset,const int64_t* djc_numterm);
extern MSK120_put_djc_slice_func_t MSK120_put_djc_slice_ptr;
MSK120_ResCode MSK120_put_djc_slice(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_djc_info_func_t)(MSK120_Task_t task,int64_t djc_idx,int64_t num_term[1],int64_t num_dom[1],int64_t num_row[1]);
extern MSK120_get_djc_info_func_t MSK120_get_djc_info_ptr;
MSK120_ResCode MSK120_get_djc_info(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_djc_func_t)(MSK120_Task_t task,int64_t djc_idx,int64_t num_terms,int64_t num_dom,int64_t num_row,int64_t* term_size,int64_t* dom_idx,int64_t* row_idx,double* rhs_offset);
extern MSK120_get_djc_func_t MSK120_get_djc_ptr;
MSK120_ResCode MSK120_get_djc(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_djc_slice_info_func_t)(MSK120_Task_t task,int64_t first_djc,int64_t num_djc,int64_t num_term[1],int64_t num_dom[1],int64_t num_row[1]);
extern MSK120_get_djc_slice_info_func_t MSK120_get_djc_slice_info_ptr;
MSK120_ResCode MSK120_get_djc_slice_info(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_djc_slice_func_t)(MSK120_Task_t task,int64_t first_djc,int64_t num_djc,int64_t num_term,int64_t num_dom,int64_t num_row,int64_t* term_size,int64_t* dom_idx,int64_t* row_idx,double* rhs_offset,int64_t* djc_num_term);
extern MSK120_get_djc_slice_func_t MSK120_get_djc_slice_ptr;
MSK120_ResCode MSK120_get_djc_slice(
    MSK120_Task_t task,
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
typedef void (*MSK120_put_obj_sense_func_t)(MSK120_Task_t task,MSK120_ObjSense sense);
extern MSK120_put_obj_sense_func_t MSK120_put_obj_sense_ptr;
void MSK120_put_obj_sense(
    MSK120_Task_t task,
    MSK120_ObjSense sense);

/**

 * Get objectiev sense. This cannot fail.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef MSK120_ObjSense (*MSK120_get_obj_sense_func_t)(MSK120_Task_t task);
extern MSK120_get_obj_sense_func_t MSK120_get_obj_sense_ptr;
MSK120_ObjSense MSK120_get_obj_sense(MSK120_Task_t task);

/**

 * Set the affine row to use as objective
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `row_idx` Index of the affine row
 */
typedef MSK120_ResCode (*MSK120_put_obj_row_func_t)(MSK120_Task_t task,int64_t row_idx);
extern MSK120_put_obj_row_func_t MSK120_put_obj_row_ptr;
MSK120_ResCode MSK120_put_obj_row(
    MSK120_Task_t task,
    int64_t row_idx);

/**

 * Get objective row index. If the objective row is set, `asgn[0]` will be set to 1 and `index[0]` is set to the row index, otherwise `asgn[0]` is set to 0.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `row_idx[1]` (out) Index of the affine row
 * - `asgn[1]` (out) Returns non-zero to indicate that a value was assigned, or zero if it was not
 */
typedef void (*MSK120_get_obj_row_func_t)(MSK120_Task_t task,int64_t row_idx[1],int asgn[1]);
extern MSK120_get_obj_row_func_t MSK120_get_obj_row_ptr;
void MSK120_get_obj_row(
    MSK120_Task_t task,
    int64_t row_idx[1],
    int asgn[1]);

/**

 * Call optimizer. On return, all input solutions have been cleared.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `trm[1]` (out) 
 */
typedef MSK120_ResCode (*MSK120_optimize_func_t)(MSK120_Task_t task,MSK120_TrmCode trm[1]);
extern MSK120_optimize_func_t MSK120_optimize_ptr;
MSK120_ResCode MSK120_optimize(
    MSK120_Task_t task,
    MSK120_TrmCode trm[1]);

/**

 * Prints a short summary of the current solutions. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `whichstream` 
 */
typedef MSK120_ResCode (*MSK120_solution_summary_func_t)(MSK120_Task_t task,MSK120_StreamType whichstream);
extern MSK120_solution_summary_func_t MSK120_solution_summary_ptr;
MSK120_ResCode MSK120_solution_summary(
    MSK120_Task_t task,
    MSK120_StreamType whichstream);

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
typedef MSK120_ResCode (*MSK120_optimize_callback_func_t)(MSK120_Task_t task,MSK120_TrmCode trm[1],MSK120_CallbackHandle cb_handle,MSK120_CallbackFunc cb_func,MSK120_CallbackHandle int_cb_handle,MSK120_IntSolCallbackFunc int_cb_func);
extern MSK120_optimize_callback_func_t MSK120_optimize_callback_ptr;
MSK120_ResCode MSK120_optimize_callback(
    MSK120_Task_t task,
    MSK120_TrmCode trm[1],
    MSK120_CallbackHandle cb_handle,
    MSK120_CallbackFunc cb_func,
    MSK120_CallbackHandle int_cb_handle,
    MSK120_IntSolCallbackFunc int_cb_func);

/**

 * Specify a remote OptServer to use instead of built-in solver.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `server[.cstring]` (in) Server name with protocol and port, e.g. `https://optserver.mydomain:9876`.
 * - `cert[.cstring]` (in, nullable) 
 */
typedef void (*MSK120_put_remote_solver_func_t)(MSK120_Task_t task,const char* server,NULLABLE const char* cert);
extern MSK120_put_remote_solver_func_t MSK120_put_remote_solver_ptr;
void MSK120_put_remote_solver(
    MSK120_Task_t task,
    const char* server,
    NULLABLE const char* cert);

/**

 * Specify access token to be used for remote solving.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `token[.cstring]` (in, nullable) 
 */
typedef void (*MSK120_put_optserver_access_token_func_t)(MSK120_Task_t task,NULLABLE const char* token);
extern MSK120_put_optserver_access_token_func_t MSK120_put_optserver_access_token_ptr;
void MSK120_put_optserver_access_token(
    MSK120_Task_t task,
    NULLABLE const char* token);

/**

 * Get number of solutions. This cannot fail.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int32_t (*MSK120_get_num_sol_func_t)(MSK120_Task_t task);
extern MSK120_get_num_sol_func_t MSK120_get_num_sol_ptr;
int32_t MSK120_get_num_sol(MSK120_Task_t task);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `sol_type[1]` (out) Returns the type of the solution requested
 */
typedef MSK120_ResCode (*MSK120_get_sol_type_func_t)(MSK120_Task_t task,int32_t sol_idx,MSK120_SolType sol_type[1]);
extern MSK120_get_sol_type_func_t MSK120_get_sol_type_ptr;
MSK120_ResCode MSK120_get_sol_type(
    MSK120_Task_t task,
    int32_t sol_idx,
    MSK120_SolType sol_type[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `primal_sol_sta[1]` (out) 
 * - `dual_sol_sta[1]` (out) 
 */
typedef MSK120_ResCode (*MSK120_get_sol_status_func_t)(MSK120_Task_t task,int32_t sol_idx,MSK120_SolSta primal_sol_sta[1],MSK120_SolSta dual_sol_sta[1]);
extern MSK120_get_sol_status_func_t MSK120_get_sol_status_ptr;
MSK120_ResCode MSK120_get_sol_status(
    MSK120_Task_t task,
    int32_t sol_idx,
    MSK120_SolSta primal_sol_sta[1],
    MSK120_SolSta dual_sol_sta[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `pro_sta[1]` (out) 
 */
typedef MSK120_ResCode (*MSK120_get_problem_status_func_t)(MSK120_Task_t task,int32_t sol_idx,MSK120_ProSta pro_sta[1]);
extern MSK120_get_problem_status_func_t MSK120_get_problem_status_ptr;
MSK120_ResCode MSK120_get_problem_status(
    MSK120_Task_t task,
    int32_t sol_idx,
    MSK120_ProSta pro_sta[1]);

/**

 * Get primal objective value for solution `sol_idx`.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `obj_val[1]` (out) 
 */
typedef MSK120_ResCode (*MSK120_get_primal_obj_func_t)(MSK120_Task_t task,int32_t sol_idx,double obj_val[1]);
extern MSK120_get_primal_obj_func_t MSK120_get_primal_obj_ptr;
MSK120_ResCode MSK120_get_primal_obj(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_dual_obj_func_t)(MSK120_Task_t task,int32_t sol_idx,double obj_val[1]);
extern MSK120_get_dual_obj_func_t MSK120_get_dual_obj_ptr;
MSK120_ResCode MSK120_get_dual_obj(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_xx_slice_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t first_var,int32_t num_var,double* xx);
extern MSK120_get_sol_xx_slice_func_t MSK120_get_sol_xx_slice_ptr;
MSK120_ResCode MSK120_get_sol_xx_slice(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_slx_slice_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t first_var,int32_t num_var,double* slx);
extern MSK120_get_sol_slx_slice_func_t MSK120_get_sol_slx_slice_ptr;
MSK120_ResCode MSK120_get_sol_slx_slice(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_sux_slice_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t first_var,int32_t num_var,double* sux);
extern MSK120_get_sol_sux_slice_func_t MSK120_get_sol_sux_slice_ptr;
MSK120_ResCode MSK120_get_sol_sux_slice(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_barxj_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t barvar_idx,int64_t num_var,double* barx);
extern MSK120_get_sol_barxj_func_t MSK120_get_sol_barxj_ptr;
MSK120_ResCode MSK120_get_sol_barxj(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_barsj_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t barvar_idx,int64_t num_elm,double* bars);
extern MSK120_get_sol_barsj_func_t MSK120_get_sol_barsj_ptr;
MSK120_ResCode MSK120_get_sol_barsj(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_barx_slice_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t first_barvar,int32_t num_barvar,int64_t num_elm,double* barx);
extern MSK120_get_sol_barx_slice_func_t MSK120_get_sol_barx_slice_ptr;
MSK120_ResCode MSK120_get_sol_barx_slice(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_bars_slice_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t first_barvar,int32_t num_barvar,int64_t num_elm,double* bars);
extern MSK120_get_sol_bars_slice_func_t MSK120_get_sol_bars_slice_ptr;
MSK120_ResCode MSK120_get_sol_bars_slice(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_basic_xj_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t var_idx,int basic[1]);
extern MSK120_get_sol_basic_xj_func_t MSK120_get_sol_basic_xj_ptr;
MSK120_ResCode MSK120_get_sol_basic_xj(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_basic_barx_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t barvar_idx,int basic[1]);
extern MSK120_get_sol_basic_barx_func_t MSK120_get_sol_basic_barx_ptr;
MSK120_ResCode MSK120_get_sol_basic_barx(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_basic_con_func_t)(MSK120_Task_t task,int32_t sol_idx,int64_t con_idx,int basic[1]);
extern MSK120_get_sol_basic_con_func_t MSK120_get_sol_basic_con_ptr;
MSK120_ResCode MSK120_get_sol_basic_con(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_sta_x_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t var_idx,int low_binding[1],int upr_binding[1]);
extern MSK120_get_sol_sta_x_func_t MSK120_get_sol_sta_x_ptr;
MSK120_ResCode MSK120_get_sol_sta_x(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_sta_barx_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t barvar_idx,int binding[1]);
extern MSK120_get_sol_sta_barx_func_t MSK120_get_sol_sta_barx_ptr;
MSK120_ResCode MSK120_get_sol_sta_barx(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_sta_con_func_t)(MSK120_Task_t task,int32_t sol_idx,int64_t con_idx,int binding[1]);
extern MSK120_get_sol_sta_con_func_t MSK120_get_sol_sta_con_ptr;
MSK120_ResCode MSK120_get_sol_sta_con(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_basic_x_slice_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t first_var,int32_t num_var,int* basic);
extern MSK120_get_sol_basic_x_slice_func_t MSK120_get_sol_basic_x_slice_ptr;
MSK120_ResCode MSK120_get_sol_basic_x_slice(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_basic_barx_slice_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t first_var,int32_t num_var,int* basic);
extern MSK120_get_sol_basic_barx_slice_func_t MSK120_get_sol_basic_barx_slice_ptr;
MSK120_ResCode MSK120_get_sol_basic_barx_slice(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_basic_con_slice_func_t)(MSK120_Task_t task,int32_t sol_idx,int64_t first_con,int64_t num_con,int* basic);
extern MSK120_get_sol_basic_con_slice_func_t MSK120_get_sol_basic_con_slice_ptr;
MSK120_ResCode MSK120_get_sol_basic_con_slice(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_sta_x_slice_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t first_var,int32_t num_var,int* low_binding,int* upr_binding);
extern MSK120_get_sol_sta_x_slice_func_t MSK120_get_sol_sta_x_slice_ptr;
MSK120_ResCode MSK120_get_sol_sta_x_slice(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_sta_barx_slice_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t first_barvar,int32_t num_barvar,int* bindnig);
extern MSK120_get_sol_sta_barx_slice_func_t MSK120_get_sol_sta_barx_slice_ptr;
MSK120_ResCode MSK120_get_sol_sta_barx_slice(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_sol_sta_con_slice_func_t)(MSK120_Task_t task,int32_t sol_idx,int64_t first_con,int64_t num_con,int* binding);
extern MSK120_get_sol_sta_con_slice_func_t MSK120_get_sol_sta_con_slice_ptr;
MSK120_ResCode MSK120_get_sol_sta_con_slice(
    MSK120_Task_t task,
    int32_t sol_idx,
    int64_t first_con,
    int64_t num_con,
    int* binding);

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
typedef MSK120_ResCode (*MSK120_get_sol_y_slice_func_t)(MSK120_Task_t task,int32_t sol_idx,int64_t first_con,int64_t num_con,int64_t num_elm,double* y);
extern MSK120_get_sol_y_slice_func_t MSK120_get_sol_y_slice_ptr;
MSK120_ResCode MSK120_get_sol_y_slice(
    MSK120_Task_t task,
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
typedef int32_t (*MSK120_get_num_input_solutions_func_t)(MSK120_Task_t task);
extern MSK120_get_num_input_solutions_func_t MSK120_get_num_input_solutions_ptr;
int32_t MSK120_get_num_input_solutions(MSK120_Task_t task);

/**

 * Copy an output solution to the input solutions.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 */
typedef MSK120_ResCode (*MSK120_copy_sol_to_input_func_t)(MSK120_Task_t task,int32_t sol_idx);
extern MSK120_copy_sol_to_input_func_t MSK120_copy_sol_to_input_ptr;
MSK120_ResCode MSK120_copy_sol_to_input(
    MSK120_Task_t task,
    int32_t sol_idx);

/**

 * Append an empty input solution.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `soltype` 
 */
typedef MSK120_ResCode (*MSK120_append_sol_func_t)(MSK120_Task_t task,MSK120_SolType soltype);
extern MSK120_append_sol_func_t MSK120_append_sol_ptr;
MSK120_ResCode MSK120_append_sol(
    MSK120_Task_t task,
    MSK120_SolType soltype);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `sol_idx` Solution index
 * - `num` Number of items
 * - `val[num]` (in) 
 */
typedef MSK120_ResCode (*MSK120_put_sol_xx_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t num,const double* val);
extern MSK120_put_sol_xx_func_t MSK120_put_sol_xx_ptr;
MSK120_ResCode MSK120_put_sol_xx(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_sol_slx_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t num,const double* val);
extern MSK120_put_sol_slx_func_t MSK120_put_sol_slx_ptr;
MSK120_ResCode MSK120_put_sol_slx(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_sol_sux_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t num,const double* val);
extern MSK120_put_sol_sux_func_t MSK120_put_sol_sux_ptr;
MSK120_ResCode MSK120_put_sol_sux(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_sol_basic_x_func_t)(MSK120_Task_t task,int32_t sol_idx,int32_t num,const int32_t* val);
extern MSK120_put_sol_basic_x_func_t MSK120_put_sol_basic_x_ptr;
MSK120_ResCode MSK120_put_sol_basic_x(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_sol_barx_func_t)(MSK120_Task_t task,int32_t sol_idx,int64_t num,const double* val);
extern MSK120_put_sol_barx_func_t MSK120_put_sol_barx_ptr;
MSK120_ResCode MSK120_put_sol_barx(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_sol_bars_func_t)(MSK120_Task_t task,int32_t sol_idx,int64_t num,const double* xx);
extern MSK120_put_sol_bars_func_t MSK120_put_sol_bars_ptr;
MSK120_ResCode MSK120_put_sol_bars(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_sol_yi_func_t)(MSK120_Task_t task,int32_t sol_idx,int64_t i,int64_t num,const double* xx);
extern MSK120_put_sol_yi_func_t MSK120_put_sol_yi_ptr;
MSK120_ResCode MSK120_put_sol_yi(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_sol_basic_c_func_t)(MSK120_Task_t task,int32_t sol_idx,int64_t num,const int32_t* val);
extern MSK120_put_sol_basic_c_func_t MSK120_put_sol_basic_c_ptr;
MSK120_ResCode MSK120_put_sol_basic_c(
    MSK120_Task_t task,
    int32_t sol_idx,
    int64_t num,
    const int32_t* val);

typedef int32_t (*MSK120_get_num_iinf_func_t)();
extern MSK120_get_num_iinf_func_t MSK120_get_num_iinf_ptr;
int32_t MSK120_get_num_iinf();

typedef int32_t (*MSK120_get_num_liinf_func_t)();
extern MSK120_get_num_liinf_func_t MSK120_get_num_liinf_ptr;
int32_t MSK120_get_num_liinf();

typedef int32_t (*MSK120_get_num_dinf_func_t)();
extern MSK120_get_num_dinf_func_t MSK120_get_num_dinf_ptr;
int32_t MSK120_get_num_dinf();

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `par_idx` 
 * - `value[1]` (out) 
 */
typedef MSK120_ResCode (*MSK120_get_iinf_func_t)(MSK120_Task_t task,int32_t par_idx,int32_t value[1]);
extern MSK120_get_iinf_func_t MSK120_get_iinf_ptr;
MSK120_ResCode MSK120_get_iinf(
    MSK120_Task_t task,
    int32_t par_idx,
    int32_t value[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `par_idx` 
 * - `value[1]` (out) 
 */
typedef MSK120_ResCode (*MSK120_get_liinf_func_t)(MSK120_Task_t task,int32_t par_idx,int64_t value[1]);
extern MSK120_get_liinf_func_t MSK120_get_liinf_ptr;
MSK120_ResCode MSK120_get_liinf(
    MSK120_Task_t task,
    int32_t par_idx,
    int64_t value[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `par_idx` 
 * - `value[1]` (out) 
 */
typedef MSK120_ResCode (*MSK120_get_dinf_func_t)(MSK120_Task_t task,int32_t par_idx,double value[1]);
extern MSK120_get_dinf_func_t MSK120_get_dinf_ptr;
MSK120_ResCode MSK120_get_dinf(
    MSK120_Task_t task,
    int32_t par_idx,
    double value[1]);

/**

 * Get the index of the integer information item corresponding to the given name. If the index is invalid, NULL is returned.
 * 
 * # Arguments
 * - `par_idx` 
 */
typedef const char* (*MSK120_get_iinf_name_func_t)(int32_t par_idx);
extern MSK120_get_iinf_name_func_t MSK120_get_iinf_name_ptr;
const char* MSK120_get_iinf_name(int32_t par_idx);

/**

 * Get the index of the long integer information item corresponding to the given name. If the index is invalid, NULL is returned.
 * 
 * # Arguments
 * - `par_idx` 
 */
typedef const char* (*MSK120_get_liinf_name_func_t)(int32_t par_idx);
extern MSK120_get_liinf_name_func_t MSK120_get_liinf_name_ptr;
const char* MSK120_get_liinf_name(int32_t par_idx);

/**

 * Get the index of the long integer information item corresponding to the given name. If the index is invalid, NULL is returned.
 * 
 * # Arguments
 * - `par_idx` 
 */
typedef const char* (*MSK120_get_dinf_name_func_t)(int32_t par_idx);
extern MSK120_get_dinf_name_func_t MSK120_get_dinf_name_ptr;
const char* MSK120_get_dinf_name(int32_t par_idx);

/**

 * This will retur the index of the integer item orresponding to name, or -1 if the name is not recognized.
 * 
 * # Arguments
 * - `par_name[.cstring]` (in) Name of the parameter
 */
typedef int32_t (*MSK120_get_iinf_index_func_t)(const char* par_name);
extern MSK120_get_iinf_index_func_t MSK120_get_iinf_index_ptr;
int32_t MSK120_get_iinf_index(const char* par_name);

/**

 * This will retur the index of the long integer item orresponding to name, or -1 if the name is not recognized.
 * 
 * # Arguments
 * - `par_name[.cstring]` (in) Name of the parameter
 */
typedef int32_t (*MSK120_get_liinf_index_func_t)(const char* par_name);
extern MSK120_get_liinf_index_func_t MSK120_get_liinf_index_ptr;
int32_t MSK120_get_liinf_index(const char* par_name);

/**

 * This will retur the index of the double item orresponding to name, or -1 if the name is not recognized.
 * 
 * # Arguments
 * - `name[.cstring]` (in) 
 */
typedef int32_t (*MSK120_get_dinf_index_func_t)(const char* name);
extern MSK120_get_dinf_index_func_t MSK120_get_dinf_index_ptr;
int32_t MSK120_get_dinf_index(const char* name);

/**

 * Get the current value of a named parameter. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `par_name[.cstring]` (in) Name of the parameter
 * - `value[1]` (out) 
 */
typedef int32_t (*MSK120_get_double_param_func_t)(MSK120_Task_t task,const char* par_name,double value[1]);
extern MSK120_get_double_param_func_t MSK120_get_double_param_ptr;
int32_t MSK120_get_double_param(
    MSK120_Task_t task,
    const char* par_name,
    double value[1]);

/**

 * Get the index corresponding to a double parameter name.
 * 
 * # Arguments
 * - `par_name[.cstring]` (in) Name of the parameter
 */
typedef int32_t (*MSK120_get_double_param_index_func_t)(const char* par_name);
extern MSK120_get_double_param_index_func_t MSK120_get_double_param_index_ptr;
int32_t MSK120_get_double_param_index(const char* par_name);

/**

 * Get the index corresponding to a double parameter name.
 * 
 * # Arguments
 * - `par_idx` 
 */
typedef const char* (*MSK120_get_double_param_name_func_t)(int32_t par_idx);
extern MSK120_get_double_param_name_func_t MSK120_get_double_param_name_ptr;
const char* MSK120_get_double_param_name(int32_t par_idx);

/**

 * Get the index corresponding to a double parameter name.
 */
typedef int32_t (*MSK120_get_num_double_param_func_t)();
extern MSK120_get_num_double_param_func_t MSK120_get_num_double_param_ptr;
int32_t MSK120_get_num_double_param();

/**

 * Get the index corresponding to a double parameter name.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `buflen` 
 * - `buf[buflen]` (out) Target buffer
 */
typedef void (*MSK120_get_all_double_params_func_t)(MSK120_Task_t task,int32_t buflen,double* buf);
extern MSK120_get_all_double_params_func_t MSK120_get_all_double_params_ptr;
void MSK120_get_all_double_params(
    MSK120_Task_t task,
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
typedef void (*MSK120_put_all_double_params_func_t)(MSK120_Task_t task,int32_t num_par,const double* params);
extern MSK120_put_all_double_params_func_t MSK120_put_all_double_params_ptr;
void MSK120_put_all_double_params(
    MSK120_Task_t task,
    int32_t num_par,
    const double* params);

/**

 * Get the index corresponding to a integer parameter name.
 * 
 * # Arguments
 * - `par_name[.cstring]` (in) Name of the parameter
 */
typedef int32_t (*MSK120_get_int_param_index_func_t)(const char* par_name);
extern MSK120_get_int_param_index_func_t MSK120_get_int_param_index_ptr;
int32_t MSK120_get_int_param_index(const char* par_name);

/**

 * Get the index corresponding to a integer parameter name.
 * 
 * # Arguments
 * - `par_idx` 
 */
typedef const char* (*MSK120_get_int_param_name_func_t)(int32_t par_idx);
extern MSK120_get_int_param_name_func_t MSK120_get_int_param_name_ptr;
const char* MSK120_get_int_param_name(int32_t par_idx);

/**

 * Get the index corresponding to a double parameter name.
 */
typedef int32_t (*MSK120_get_num_int_param_func_t)();
extern MSK120_get_num_int_param_func_t MSK120_get_num_int_param_ptr;
int32_t MSK120_get_num_int_param();

/**

 * Get the index corresponding to a double parameter name.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `buflen` 
 * - `buf[buflen]` (out) Target buffer
 */
typedef void (*MSK120_get_all_int_params_func_t)(MSK120_Task_t task,int32_t buflen,int32_t* buf);
extern MSK120_get_all_int_params_func_t MSK120_get_all_int_params_ptr;
void MSK120_get_all_int_params(
    MSK120_Task_t task,
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
typedef void (*MSK120_put_all_int_params_func_t)(MSK120_Task_t task,int32_t num_par,const int32_t* params);
extern MSK120_put_all_int_params_func_t MSK120_put_all_int_params_ptr;
void MSK120_put_all_int_params(
    MSK120_Task_t task,
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
typedef int32_t (*MSK120_get_int_param_func_t)(MSK120_Task_t task,const char* par_name,int32_t value[1]);
extern MSK120_get_int_param_func_t MSK120_get_int_param_ptr;
int32_t MSK120_get_int_param(
    MSK120_Task_t task,
    const char* par_name,
    int32_t value[1]);

/**

 * Get the length of the string representation of the current value of a named parameter. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `par_name[.cstring]` (in) Name of the parameter
 */
typedef int32_t (*MSK120_get_param_str_len_func_t)(MSK120_Task_t task,const char* par_name);
extern MSK120_get_param_str_len_func_t MSK120_get_param_str_len_ptr;
int32_t MSK120_get_param_str_len(
    MSK120_Task_t task,
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
typedef void (*MSK120_get_param_str_func_t)(MSK120_Task_t task,const char* name,int32_t length,char* buf);
extern MSK120_get_param_str_func_t MSK120_get_param_str_ptr;
void MSK120_get_param_str(
    MSK120_Task_t task,
    const char* name,
    int32_t length,
    char* buf);

/**

 * Get the current value of a named parameter. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `par_name[.cstring]` (in) The parameter name is the lower case name of the MOSEK parameter without "MSK_" prefix
 * - `value` 
 */
typedef int32_t (*MSK120_put_double_param_func_t)(MSK120_Task_t task,const char* par_name,double value);
extern MSK120_put_double_param_func_t MSK120_put_double_param_ptr;
int32_t MSK120_put_double_param(
    MSK120_Task_t task,
    const char* par_name,
    double value);

/**

 * Get the current value of a named parameter. 
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `par_name[.cstring]` (in) The parameter name is the lower case name of the MOSEK parameter without "MSK_" prefix
 * - `value` 
 */
typedef int32_t (*MSK120_put_int_param_func_t)(MSK120_Task_t task,const char* par_name,int32_t value);
extern MSK120_put_int_param_func_t MSK120_put_int_param_ptr;
int32_t MSK120_put_int_param(
    MSK120_Task_t task,
    const char* par_name,
    int32_t value);

/**

 * Get the current value of a named parameter. 
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
 */
typedef int32_t (*MSK120_put_param_str_func_t)(MSK120_Task_t task,const char* par_name,const char* value);
extern MSK120_put_param_str_func_t MSK120_put_param_str_ptr;
int32_t MSK120_put_param_str(
    MSK120_Task_t task,
    const char* par_name,
    const char* value);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int32_t (*MSK120_get_task_name_len_func_t)(MSK120_Task_t task);
extern MSK120_get_task_name_len_func_t MSK120_get_task_name_len_ptr;
int32_t MSK120_get_task_name_len(MSK120_Task_t task);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef int32_t (*MSK120_get_obj_name_len_func_t)(MSK120_Task_t task);
extern MSK120_get_obj_name_len_func_t MSK120_get_obj_name_len_ptr;
int32_t MSK120_get_obj_name_len(MSK120_Task_t task);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `capacity` 
 * - `buf[capacity]` (out) Target buffer
 */
typedef void (*MSK120_get_task_name_func_t)(MSK120_Task_t task,int32_t capacity,char* buf);
extern MSK120_get_task_name_func_t MSK120_get_task_name_ptr;
void MSK120_get_task_name(
    MSK120_Task_t task,
    int32_t capacity,
    char* buf);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `capacity` 
 * - `buf[capacity]` (out) Target buffer
 */
typedef void (*MSK120_get_obj_name_func_t)(MSK120_Task_t task,int32_t capacity,char* buf);
extern MSK120_get_obj_name_func_t MSK120_get_obj_name_ptr;
void MSK120_get_obj_name(
    MSK120_Task_t task,
    int32_t capacity,
    char* buf);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `name[.cstring]` (in) 
 */
typedef MSK120_ResCode (*MSK120_put_task_name_func_t)(MSK120_Task_t task,const char* name);
extern MSK120_put_task_name_func_t MSK120_put_task_name_ptr;
MSK120_ResCode MSK120_put_task_name(
    MSK120_Task_t task,
    const char* name);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `name[.cstring]` (in) 
 */
typedef MSK120_ResCode (*MSK120_put_obj_name_func_t)(MSK120_Task_t task,const char* name);
extern MSK120_put_obj_name_func_t MSK120_put_obj_name_ptr;
MSK120_ResCode MSK120_put_obj_name(
    MSK120_Task_t task,
    const char* name);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `var_idx` Variable index
 * - `name_len[1]` (out) 
 */
typedef MSK120_ResCode (*MSK120_get_var_name_len_func_t)(MSK120_Task_t task,int32_t var_idx,int32_t name_len[1]);
extern MSK120_get_var_name_len_func_t MSK120_get_var_name_len_ptr;
MSK120_ResCode MSK120_get_var_name_len(
    MSK120_Task_t task,
    int32_t var_idx,
    int32_t name_len[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `var_idx` Variable index
 */
typedef int32_t (*MSK120_get_var_name_len2_func_t)(MSK120_Task_t task,int32_t var_idx);
extern MSK120_get_var_name_len2_func_t MSK120_get_var_name_len2_ptr;
int32_t MSK120_get_var_name_len2(
    MSK120_Task_t task,
    int32_t var_idx);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `barvar_idx` Positive semi-definite variable index
 * - `name_len[1]` (out) 
 */
typedef MSK120_ResCode (*MSK120_get_barvar_name_len_func_t)(MSK120_Task_t task,int32_t barvar_idx,int32_t name_len[1]);
extern MSK120_get_barvar_name_len_func_t MSK120_get_barvar_name_len_ptr;
MSK120_ResCode MSK120_get_barvar_name_len(
    MSK120_Task_t task,
    int32_t barvar_idx,
    int32_t name_len[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `barvar_idx` Positive semi-definite variable index
 */
typedef int32_t (*MSK120_get_barvar_name_len2_func_t)(MSK120_Task_t task,int32_t barvar_idx);
extern MSK120_get_barvar_name_len2_func_t MSK120_get_barvar_name_len2_ptr;
int32_t MSK120_get_barvar_name_len2(
    MSK120_Task_t task,
    int32_t barvar_idx);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `var_idx` Variable index
 * - `capacity` 
 * - `buf[capacity]` (out) Target buffer
 */
typedef MSK120_ResCode (*MSK120_get_var_name_func_t)(MSK120_Task_t task,int32_t var_idx,int32_t capacity,char* buf);
extern MSK120_get_var_name_func_t MSK120_get_var_name_ptr;
MSK120_ResCode MSK120_get_var_name(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_barvar_name_func_t)(MSK120_Task_t task,int32_t barvar_idx,int32_t capacity,char* buf);
extern MSK120_get_barvar_name_func_t MSK120_get_barvar_name_ptr;
MSK120_ResCode MSK120_get_barvar_name(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_var_name_func_t)(MSK120_Task_t task,int32_t var_idx,const char* name);
extern MSK120_put_var_name_func_t MSK120_put_var_name_ptr;
MSK120_ResCode MSK120_put_var_name(
    MSK120_Task_t task,
    int32_t var_idx,
    const char* name);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `barvar_idx` Positive semi-definite variable index
 * - `name[.cstring]` (in) 
 */
typedef MSK120_ResCode (*MSK120_put_barvar_name_func_t)(MSK120_Task_t task,int32_t barvar_idx,const char* name);
extern MSK120_put_barvar_name_func_t MSK120_put_barvar_name_ptr;
MSK120_ResCode MSK120_put_barvar_name(
    MSK120_Task_t task,
    int32_t barvar_idx,
    const char* name);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `con_idx` Constraint index 
 * - `len[1]` (out) 
 */
typedef MSK120_ResCode (*MSK120_get_con_name_len_func_t)(MSK120_Task_t task,int64_t con_idx,int32_t len[1]);
extern MSK120_get_con_name_len_func_t MSK120_get_con_name_len_ptr;
MSK120_ResCode MSK120_get_con_name_len(
    MSK120_Task_t task,
    int64_t con_idx,
    int32_t len[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `djc_idx` Disjunctive constraint index
 * - `len[1]` (out) 
 */
typedef MSK120_ResCode (*MSK120_get_djc_name_len_func_t)(MSK120_Task_t task,int64_t djc_idx,int32_t len[1]);
extern MSK120_get_djc_name_len_func_t MSK120_get_djc_name_len_ptr;
MSK120_ResCode MSK120_get_djc_name_len(
    MSK120_Task_t task,
    int64_t djc_idx,
    int32_t len[1]);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `con_idx` Constraint index 
 */
typedef int32_t (*MSK120_get_con_name_len2_func_t)(MSK120_Task_t task,int64_t con_idx);
extern MSK120_get_con_name_len2_func_t MSK120_get_con_name_len2_ptr;
int32_t MSK120_get_con_name_len2(
    MSK120_Task_t task,
    int64_t con_idx);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `djc_idx` Disjunctive constraint index
 */
typedef int32_t (*MSK120_get_djc_name_len2_func_t)(MSK120_Task_t task,int64_t djc_idx);
extern MSK120_get_djc_name_len2_func_t MSK120_get_djc_name_len2_ptr;
int32_t MSK120_get_djc_name_len2(
    MSK120_Task_t task,
    int64_t djc_idx);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `con_idx` Constraint index 
 * - `capacity` 
 * - `buf[capacity]` (out) Target buffer
 */
typedef MSK120_ResCode (*MSK120_get_con_name_func_t)(MSK120_Task_t task,int64_t con_idx,int32_t capacity,char* buf);
extern MSK120_get_con_name_func_t MSK120_get_con_name_ptr;
MSK120_ResCode MSK120_get_con_name(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_get_djc_name_func_t)(MSK120_Task_t task,int64_t djc_idx,int32_t capacity,char* buf);
extern MSK120_get_djc_name_func_t MSK120_get_djc_name_ptr;
MSK120_ResCode MSK120_get_djc_name(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_put_con_name_func_t)(MSK120_Task_t task,int64_t con_idx,const char* buf);
extern MSK120_put_con_name_func_t MSK120_put_con_name_ptr;
MSK120_ResCode MSK120_put_con_name(
    MSK120_Task_t task,
    int64_t con_idx,
    const char* buf);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `djc_idx` Disjunctive constraint index
 * - `buf[.cstring]` (in) Target buffer
 */
typedef MSK120_ResCode (*MSK120_put_djc_name_func_t)(MSK120_Task_t task,int64_t djc_idx,const char* buf);
extern MSK120_put_djc_name_func_t MSK120_put_djc_name_ptr;
MSK120_ResCode MSK120_put_djc_name(
    MSK120_Task_t task,
    int64_t djc_idx,
    const char* buf);

/**

 * Write task, base the format at on the file name extension.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `filename[.cstring]` (in) 
 */
typedef MSK120_ResCode (*MSK120_write_task_to_file_func_t)(MSK120_Task_t task,const char* filename);
extern MSK120_write_task_to_file_func_t MSK120_write_task_to_file_ptr;
MSK120_ResCode MSK120_write_task_to_file(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_write_task_to_handle_func_t)(MSK120_Task_t task,MSK120_Format format,MSK120_Compression compress,MSK120_WriteHandle handle,MSK120_WriteFunc func);
extern MSK120_write_task_to_handle_func_t MSK120_write_task_to_handle_ptr;
MSK120_ResCode MSK120_write_task_to_handle(
    MSK120_Task_t task,
    MSK120_Format format,
    MSK120_Compression compress,
    MSK120_WriteHandle handle,
    MSK120_WriteFunc func);

/**

 * Write solution, base the format at on the file name extension.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `filename[.cstring]` (in) 
 */
typedef MSK120_ResCode (*MSK120_write_solution_to_file_func_t)(MSK120_Task_t task,const char* filename);
extern MSK120_write_solution_to_file_func_t MSK120_write_solution_to_file_ptr;
MSK120_ResCode MSK120_write_solution_to_file(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_write_solution_to_handle_func_t)(MSK120_Task_t task,MSK120_SolutionFormat format,MSK120_Compression compress,MSK120_WriteHandle handle,MSK120_WriteFunc func);
extern MSK120_write_solution_to_handle_func_t MSK120_write_solution_to_handle_ptr;
MSK120_ResCode MSK120_write_solution_to_handle(
    MSK120_Task_t task,
    MSK120_SolutionFormat format,
    MSK120_Compression compress,
    MSK120_WriteHandle handle,
    MSK120_WriteFunc func);

/**

 * Reset task and read data from file, base the format on the file extension.
 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `filename[.cstring]` (in) 
 */
typedef MSK120_ResCode (*MSK120_read_from_file_func_t)(MSK120_Task_t task,const char* filename);
extern MSK120_read_from_file_func_t MSK120_read_from_file_ptr;
MSK120_ResCode MSK120_read_from_file(
    MSK120_Task_t task,
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
typedef MSK120_ResCode (*MSK120_read_from_handle_func_t)(MSK120_Task_t task,MSK120_Format format,MSK120_Compression compress,MSK120_ReadHandle handle,MSK120_ReadFunc func);
extern MSK120_read_from_handle_func_t MSK120_read_from_handle_ptr;
MSK120_ResCode MSK120_read_from_handle(
    MSK120_Task_t task,
    MSK120_Format format,
    MSK120_Compression compress,
    MSK120_ReadHandle handle,
    MSK120_ReadFunc func);

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
typedef MSK120_ResCode (*MSK120_put_stream_callback_func_t)(MSK120_Task_t task,MSK120_StreamType whichstream,MSK120_WriteHandle handle,MSK120_StreamFunc func);
extern MSK120_put_stream_callback_func_t MSK120_put_stream_callback_ptr;
MSK120_ResCode MSK120_put_stream_callback(
    MSK120_Task_t task,
    MSK120_StreamType whichstream,
    MSK120_WriteHandle handle,
    MSK120_StreamFunc func);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 * - `whichstream` 
 */
typedef MSK120_ResCode (*MSK120_clear_stream_callback_func_t)(MSK120_Task_t task,MSK120_StreamType whichstream);
extern MSK120_clear_stream_callback_func_t MSK120_clear_stream_callback_ptr;
MSK120_ResCode MSK120_clear_stream_callback(
    MSK120_Task_t task,
    MSK120_StreamType whichstream);

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
typedef MSK120_ResCode (*MSK120_put_error_callback_func_t)(MSK120_Task_t task,MSK120_ErrorCallbackHandle handle,MSK120_ErrorCallbackFunc func);
extern MSK120_put_error_callback_func_t MSK120_put_error_callback_ptr;
MSK120_ResCode MSK120_put_error_callback(
    MSK120_Task_t task,
    MSK120_ErrorCallbackHandle handle,
    MSK120_ErrorCallbackFunc func);

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
typedef MSK120_ResCode (*MSK120_put_warning_callback_func_t)(MSK120_Task_t task,MSK120_ErrorCallbackHandle handle,MSK120_ErrorCallbackFunc func);
extern MSK120_put_warning_callback_func_t MSK120_put_warning_callback_ptr;
MSK120_ResCode MSK120_put_warning_callback(
    MSK120_Task_t task,
    MSK120_ErrorCallbackHandle handle,
    MSK120_ErrorCallbackFunc func);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef MSK120_ResCode (*MSK120_clear_error_callback_func_t)(MSK120_Task_t task);
extern MSK120_clear_error_callback_func_t MSK120_clear_error_callback_ptr;
MSK120_ResCode MSK120_clear_error_callback(MSK120_Task_t task);

/**

 * 
 * # Arguments
 * - `task` The optimizatioj task object
 */
typedef MSK120_ResCode (*MSK120_clear_warning_callback_func_t)(MSK120_Task_t task);
extern MSK120_clear_warning_callback_func_t MSK120_clear_warning_callback_ptr;
MSK120_ResCode MSK120_clear_warning_callback(MSK120_Task_t task);

/**

 * Stops all threads and deletes all handles used by the license system. If this
 * function is called, it must be called as the last |mosek| API call. No other
 * |mosek| API calls are valid after this.
 */
typedef void (*MSK120_license_cleanup_func_t)();
extern MSK120_license_cleanup_func_t MSK120_license_cleanup_ptr;
void MSK120_license_cleanup();

/**

 * If |mosek| is using a global threadpool, attempt to shut
 * this down. If there are currently jobs running, this will do
 * nothing.
 */
typedef void (*MSK120_shutdown_global_threadpool_func_t)();
extern MSK120_shutdown_global_threadpool_func_t MSK120_shutdown_global_threadpool_ptr;
void MSK120_shutdown_global_threadpool();

/**

 * Computes vector addition and multiplication by a scalar. 
 * 
 * # Arguments
 * - `n` Length of the vectors. 
 * - `alpha` The scalar that multiplies x. 
 * - `x[n]` (in) The x vector. 
 * - `y[n]` (in-out) The y vector. 
 */
typedef MSK120_ResCode (*MSK120_axpy_func_t)(int32_t n,double alpha,const double* x,double* y);
extern MSK120_axpy_func_t MSK120_axpy_ptr;
MSK120_ResCode MSK120_axpy(
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
typedef MSK120_ResCode (*MSK120_dot_func_t)(int32_t n,const double* x,const double* y,double xty[1]);
extern MSK120_dot_func_t MSK120_dot_ptr;
MSK120_ResCode MSK120_dot(
    int32_t n,
    const double* x,
    const double* y,
    double xty[1]);

/**

 * Computes the multiplication of a scaled dense matrix times a dense vector, plus a scaled dense vector. Precisely, if ``trans`` is :msk:const:`transpose.no` then the update is
 * 
 * .. math:: y := \alpha A x + \beta y,
 * 
 * and if ``trans`` is :msk:const:`transpose.yes` then
 * 
 * .. math:: y := \alpha A^T x + \beta y,
 * 
 * where :math:`\alpha,\beta` are scalar values and :math:`A` is a matrix with :math:`m` rows and :math:`n` columns.
 * 
 * Note that the result is stored overwriting :math:`y`. It must not overlap with the other input arrays.
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
typedef MSK120_ResCode (*MSK120_gemv_func_t)(int transa,int32_t m,int32_t n,double alpha,const double* a,const double* x,double beta,double* y);
extern MSK120_gemv_func_t MSK120_gemv_ptr;
MSK120_ResCode MSK120_gemv(
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
 * :math:`A`, :math:`B` and :math:`C` of compatible dimensions, this function
 * computes
 * 
 * .. math:: C:= \alpha op(A)op(B) + \beta C
 * 
 * where :math:`\alpha,\beta` are two scalar values. The function :math:`op(X)`
 * denotes :math:`X` if transX is :msk:const:`transpose.no`, or :math:`X^T` if set to :msk:const:`transpose.yes`. The matrix :math:`C` has :math:`m` rows and :math:`n` columns, and the other matrices must have compatible dimensions.
 * 
 * The result of this operation is stored in :math:`C`. It must not overlap with the other input arrays.
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
typedef MSK120_ResCode (*MSK120_gemm_func_t)(int transa,int transb,int32_t m,int32_t n,int32_t k,double alpha,const double* a,const double* b,double beta,double* c);
extern MSK120_gemm_func_t MSK120_gemm_ptr;
MSK120_ResCode MSK120_gemm(
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

 * Performs a symmetric rank-:math:`k` update for a symmetric matrix.
 * 
 * Given a symmetric matrix :math:`C\in \real^{n\times n}`, two scalars
 * :math:`\alpha,\beta` and a matrix :math:`A` of rank :math:`k\leq n`, it
 * computes either
 * 
 * .. math:: C := \alpha A A^T + \beta C,
 * 
 * when ``trans`` is set to :msk:const:`transpose.no` and :math:`A\in \real^{n\times k}`, or
 * 
 * .. math:: C := \alpha A^T A + \beta C,
 * 
 * when ``trans`` is set to :msk:const:`transpose.yes` and :math:`A\in \real^{k\times n}`.
 * 
 * Only the part of :math:`C` indicated by ``uplo`` is used and only that part is updated with the result. It must not overlap with the other input arrays.
 * 
 * # Arguments
 * - `is_upr` Indicates whether the upper or lower triangular part of C is used. 
 * - `trans` Indicates whether the matrix A must be transposed. 
 * - `n` Specifies the order of :math:`C`.
 * - `k` Indicates the number of rows or columns of :math:`A`, depending on whether or not it is transposed, and its rank.
 * - `alpha` A scalar value multiplying the result of the matrix multiplication. 
 * - `a` (in) The pointer to the array storing matrix A in a column-major format. 
 * - `beta` A scalar value that multiplies C. 
 * - `c` (in-out) The pointer to the array storing matrix C in a column-major format. 
 */
typedef MSK120_ResCode (*MSK120_syrk_func_t)(int is_upr,int trans,int32_t n,int32_t k,double alpha,const double* a,double beta,double* c);
extern MSK120_syrk_func_t MSK120_syrk_ptr;
MSK120_ResCode MSK120_syrk(
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
 * where :math:`L` is a sparse lower triangular nonsingular matrix. This implies in particular that diagonals in :math:`L` are nonzero.
 * 
 * # Arguments
 * - `transposed` Controls whether the solve is with L or the transposed L. 
 * - `n` Specifies the dimension of L. 
 * - `lnzc[n]` (in) lnzc[j] is the number of nonzeros in column j. 
 * - `lptrc[n]` (in) lptrc[j] is a pointer to the first row index and value in column j. 
 * - `nnz` Number of elements in lsubc and lvalc. 
 * - `lsubc[nnz]` (in) Row indexes for each column stored sequentially. 
 * - `lvalc[nnz]` (in) The value corresponding to row indexed stored lsubc. 
 * - `b[n]` (in-out) The right-hand side of linear equation system to be solved as a dense vector. 
 */
typedef MSK120_ResCode (*MSK120_sparse_triangular_solve_dense_func_t)(int transposed,int32_t n,const int32_t* lnzc,const int64_t* lptrc,int64_t nnz,const int32_t* lsubc,const double* lvalc,double* b);
extern MSK120_sparse_triangular_solve_dense_func_t MSK120_sparse_triangular_solve_dense_ptr;
MSK120_ResCode MSK120_sparse_triangular_solve_dense(
    int transposed,
    int32_t n,
    const int32_t* lnzc,
    const int64_t* lptrc,
    int64_t nnz,
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
typedef MSK120_ResCode (*MSK120_potrf_func_t)(int is_upr,int32_t n,double* a);
extern MSK120_potrf_func_t MSK120_potrf_ptr;
MSK120_ResCode MSK120_potrf(
    int is_upr,
    int32_t n,
    double* a);

/**

 * Computes all eigenvalues of a real symmetric matrix :math:`A`. Given a matrix :math:`A\in\real^{n\times n}` it returns a vector :math:`w\in\real^n` containing the eigenvalues of :math:`A`. 
 * 
 * # Arguments
 * - `is_upr` Indicates whether the upper or lower triangular part is used. 
 * - `n` Dimension of the symmetric input matrix. 
 * - `a` (in) Input matrix A. 
 * - `w[n]` (out) Array of length at least n containing the eigenvalues of A. 
 */
typedef MSK120_ResCode (*MSK120_syeig_func_t)(int is_upr,int32_t n,const double* a,double* w);
extern MSK120_syeig_func_t MSK120_syeig_ptr;
MSK120_ResCode MSK120_syeig(
    int is_upr,
    int32_t n,
    const double* a,
    double* w);

/**

 * Computes all the eigenvalues and eigenvectors a real symmetric matrix.
 * Given the input matrix :math:`A\in \real^{n\times n}`, this function returns a
 * vector :math:`w\in \real^n` containing the eigenvalues of :math:`A` and it also computes the eigenvectors
 * of :math:`A`. Therefore, this function computes the eigenvalue decomposition of :math:`A` as
 * 
 * .. math:: A= U V U^T,
 * 
 * where :math:`V=\diag(w)` and :math:`U` contains the eigenvectors of :math:`A`.
 * 
 * Note that the matrix :math:`U` overwrites the input data :math:`A`.
 * 
 * # Arguments
 * - `is_upr` Indicates whether the upper or lower triangular part is used. 
 * - `n` Dimension of the symmetric input matrix. 
 * - `a` (in-out) Input matrix A. 
 * - `w[n]` (in-out) Array of length at least n containing the eigenvalues of A. 
 */
typedef MSK120_ResCode (*MSK120_syevd_func_t)(int is_upr,int32_t n,double* a,double* w);
extern MSK120_syevd_func_t MSK120_syevd_ptr;
MSK120_ResCode MSK120_syevd(
    int is_upr,
    int32_t n,
    double* a,
    double* w);

/**

 * Optimize a number of tasks in parallel using a specified number of threads. All callbacks and log output streams are disabled.
 * 
 * Assuming that each task takes about same time and there many more tasks than number of threads then a linear speedup can be achieved, also known as strong scaling. A typical application of this method is to solve many small tasks of similar type; in this case it is recommended that each of them is allocated a single thread by setting :msk:iparam:`num_threads` to :math:`1`.
 * 
 * If the parameters ``is_race`` or ``max_time`` are used, then the result may not be deterministic, in the sense that the tasks which complete first may vary between runs.
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
typedef MSK120_ResCode (*MSK120_optimize_batch_func_t)(int is_race,double max_time_sec,int32_t num_threads,int64_t num_task,const MSK120_Task_t* tasks,MSK120_TrmCode* trm_code,MSK120_ResCode* res_code);
extern MSK120_optimize_batch_func_t MSK120_optimize_batch_ptr;
MSK120_ResCode MSK120_optimize_batch(
    int is_race,
    double max_time_sec,
    int32_t num_threads,
    int64_t num_task,
    const MSK120_Task_t* tasks,
    MSK120_TrmCode* trm_code,
    MSK120_ResCode* res_code);

/**

 * Checks out a license feature from the license server. Normally the required
 * license features will be automatically checked out the first time they are needed
 * by the function :msk:func:`task.optimize`. This function can be used to check out one
 * or more features ahead of time.
 * 
 * The feature will remain checked out until the environment is deleted or the function
 * :msk:func:`env.checkinlicense` is called.
 * 
 * If a given feature is already checked out when this function is called, the call has no effect.
 * 
 * # Arguments
 * - `feature` Feature to check out from the license system. 
 */
typedef MSK120_ResCode (*MSK120_check_out_license_func_t)(MSK120_Feature feature);
extern MSK120_check_out_license_func_t MSK120_check_out_license_ptr;
MSK120_ResCode MSK120_check_out_license(MSK120_Feature feature);

/**

 * Check in a license feature to the license server. By default all licenses
 * consumed by functions using a single environment are kept checked out for the
 * lifetime of the |mosek| environment. This function checks in a given license
 * feature back to the license server immediately.
 * 
 * If the given license feature is not checked out at all, or it is in use by a call to
 * :msk:func:`task.optimize`, calling this function has no effect.
 * 
 * Please note that returning a license to the license server incurs a small
 * overhead, so frequent calls to this function should be avoided.
 * 
 * # Arguments
 * - `feature` Feature to check in to the license system. 
 */
typedef MSK120_ResCode (*MSK120_check_in_license_func_t)(MSK120_Feature feature);
extern MSK120_check_in_license_func_t MSK120_check_in_license_ptr;
MSK120_ResCode MSK120_check_in_license(MSK120_Feature feature);

/**

 * Check in all unused license features to the license token server. 
 */
typedef MSK120_ResCode (*MSK120_check_in_all_func_t)();
extern MSK120_check_in_all_func_t MSK120_check_in_all_ptr;
MSK120_ResCode MSK120_check_in_all();

/**

 * Prints an intro to message stream. 
 * 
 * # Arguments
 * - `long_ver` If non-zero, then the intro is slightly longer. 
 */
typedef MSK120_ResCode (*MSK120_echo_intro_func_t)(int long_ver);
extern MSK120_echo_intro_func_t MSK120_echo_intro_ptr;
MSK120_ResCode MSK120_echo_intro(int long_ver);

/**

 * Obtains |mosek| version information. 
 * 
 * # Arguments
 * - `major[1]` (out) Major version number. 
 * - `minor[1]` (out) Minor version number. 
 * - `revision[1]` (out) Revision number. 
 */
typedef void (*MSK120_get_version_func_t)(int32_t major[1],int32_t minor[1],int32_t revision[1]);
extern MSK120_get_version_func_t MSK120_get_version_ptr;
void MSK120_get_version(
    int32_t major[1],
    int32_t minor[1],
    int32_t revision[1]);

/**

 * Enables debug information for the license system. If ``licdebug`` is non-zero, then |mosek| will print debug info regarding the license checkout.  
 * 
 * # Arguments
 * - `lic_debug` Whether license checkout debug info should be printed.  
 */
typedef MSK120_ResCode (*MSK120_put_license_debug_func_t)(int lic_debug);
extern MSK120_put_license_debug_func_t MSK120_put_license_debug_ptr;
MSK120_ResCode MSK120_put_license_debug(int lic_debug);

/**

 * Input a runtime license code.  This function has an effect only before the first optimization. 
 * 
 * # Arguments
 * - `code[21]` (in, nullable) A license key string. 
 */
typedef MSK120_ResCode (*MSK120_put_license_code_func_t)(NULLABLE const int32_t code[21]);
extern MSK120_put_license_code_func_t MSK120_put_license_code_ptr;
MSK120_ResCode MSK120_put_license_code(NULLABLE const int32_t code[21]);

/**

 * Control whether |mosek| should wait for an available license if no license is available. If ``licwait`` is non-zero, then |mosek| will wait for ``licwait-1`` milliseconds between each check for an available license.
 * 
 * # Arguments
 * - `lic_wait` Enable waiting for a license until it is available. 
 */
typedef MSK120_ResCode (*MSK120_put_license_wait_func_t)(int lic_wait);
extern MSK120_put_license_wait_func_t MSK120_put_license_wait_ptr;
MSK120_ResCode MSK120_put_license_wait(int lic_wait);

/**

 * Set the path to the license file. This function has an effect only before the first optimization. 
 * 
 * # Arguments
 * - `license_path[.cstring]` (in, nullable) A path specifying where to search for the license. 
 */
typedef MSK120_ResCode (*MSK120_put_license_path_func_t)(NULLABLE const char* license_path);
extern MSK120_put_license_path_func_t MSK120_put_license_path_ptr;
MSK120_ResCode MSK120_put_license_path(NULLABLE const char* license_path);


int MSK120_initialize_library_with_paths(const char * paths[]);
int MSK120_library_initialized();
int MSK120_initialize_library();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
