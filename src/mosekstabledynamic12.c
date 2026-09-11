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



/* NOTES on compiling and linking:
 * To build the MOSEK Core dymalic loader functions this file must be included in the final binary or library.
 * On Linux and OSX the final library or binary must be linked with libdl.
 *
 */

#ifdef _WIN32
    #include <windows.h>
    #include <libloaderapi.h>
    #define libname "mosekstable12.dll"
    typedef HMODULE libhandle_t;

    libhandle_t __dlopen(const char * filename, const char ** errmsg) {
        libhandle_t h = LoadLibraryA(filename);
        if (!h) {
            *errmsg = "Failed to load MOSEK Core library";
        }
        return h;
    }
    void __dlclose(libhandle_t h) {
        FreeLibrary(h);
    }

    void * __loadsym(libhandle_t h, const char * symname, const char *(* errmsg)) {
        void * symaddr = GetProcAddress(h,symname);
        if (!symaddr)
            *errmsg = "Failed to load symbol";
        return symaddr;
    }
    #define PATH_SEP '\\'
#else
    #include <dlfcn.h>
    typedef void * libhandle_t;
    #if defined(__APPLE__)
        #define libname "libmosekstable12.dylib"
    #else
        #define libname "libmosekstable12.so"
    #endif


    libhandle_t __dlopen(const char * filename, const char ** errmsg) {
        libhandle_t h = dlopen(filename,RTLD_NOW);
        if (!h) {
            *errmsg = dlerror();
        }
        return h;
    }
    void __dlclose(libhandle_t h) {
        dlclose(h);
    }

    void * __loadsym(libhandle_t h, const char * symname, const char ** errmsg) {
        void * symaddr = dlsym(h,symname);
        if (!symaddr)
            *errmsg = dlerror();
        return symaddr;
    }

    #define PATH_SEP '/'
#endif
#include "mosekstabledynamic12.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static libhandle_t libmosek_handle = NULL;
MSK12_get_callback_code_name_func_t MSK12_get_callback_code_name_ptr;
const char* MSK12_get_callback_code_name(int32_t code) { return MSK12_get_callback_code_name_ptr(code); }
MSK12_get_resp_name_func_t MSK12_get_resp_name_ptr;
const char* MSK12_get_resp_name(MSK12_ResCode r) { return MSK12_get_resp_name_ptr(r); }
MSK12_get_resp_descr_func_t MSK12_get_resp_descr_ptr;
const char* MSK12_get_resp_descr(MSK12_ResCode r) { return MSK12_get_resp_descr_ptr(r); }
MSK12_get_last_resp_func_t MSK12_get_last_resp_ptr;
MSK12_ResCode MSK12_get_last_resp(MSK12_Task_t task) { return MSK12_get_last_resp_ptr(task); }
MSK12_get_last_resp_msg_func_t MSK12_get_last_resp_msg_ptr;
MSK12_ResCode MSK12_get_last_resp_msg(
    MSK12_Task_t task,
    char* buf,
    size_t buf_len) { return MSK12_get_last_resp_msg_ptr(task,buf,buf_len); }
MSK12_get_last_resp_msg_len_func_t MSK12_get_last_resp_msg_len_ptr;
size_t MSK12_get_last_resp_msg_len(MSK12_Task_t task) { return MSK12_get_last_resp_msg_len_ptr(task); }
MSK12_get_trm_name_func_t MSK12_get_trm_name_ptr;
const char* MSK12_get_trm_name(MSK12_TrmCode trm) { return MSK12_get_trm_name_ptr(trm); }
MSK12_get_trm_descr_func_t MSK12_get_trm_descr_ptr;
const char* MSK12_get_trm_descr(MSK12_TrmCode trm) { return MSK12_get_trm_descr_ptr(trm); }
MSK12_new_task_from_task_func_t MSK12_new_task_from_task_ptr;
MSK12_Task_t MSK12_new_task_from_task(MSK12_Task_t task) { return MSK12_new_task_from_task_ptr(task); }
MSK12_new_task_func_t MSK12_new_task_ptr;
MSK12_Task_t MSK12_new_task() { return MSK12_new_task_ptr(); }
MSK12_delete_task_func_t MSK12_delete_task_ptr;
void MSK12_delete_task(MSK12_Task_t task) { MSK12_delete_task_ptr(task); }
MSK12_reserve_num_var_func_t MSK12_reserve_num_var_ptr;
MSK12_ResCode MSK12_reserve_num_var(
    MSK12_Task_t task,
    int32_t add_num) { return MSK12_reserve_num_var_ptr(task,add_num); }
MSK12_reserve_num_barvar_func_t MSK12_reserve_num_barvar_ptr;
MSK12_ResCode MSK12_reserve_num_barvar(
    MSK12_Task_t task,
    int32_t num_barvar) { return MSK12_reserve_num_barvar_ptr(task,num_barvar); }
MSK12_reserve_num_con_func_t MSK12_reserve_num_con_ptr;
MSK12_ResCode MSK12_reserve_num_con(
    MSK12_Task_t task,
    int32_t num_con) { return MSK12_reserve_num_con_ptr(task,num_con); }
MSK12_reserve_num_row_func_t MSK12_reserve_num_row_ptr;
MSK12_ResCode MSK12_reserve_num_row(
    MSK12_Task_t task,
    int64_t num_row) { return MSK12_reserve_num_row_ptr(task,num_row); }
MSK12_reserve_num_nz_func_t MSK12_reserve_num_nz_ptr;
MSK12_ResCode MSK12_reserve_num_nz(
    MSK12_Task_t task,
    int64_t num_nz) { return MSK12_reserve_num_nz_ptr(task,num_nz); }
MSK12_reserve_num_dom_func_t MSK12_reserve_num_dom_ptr;
MSK12_ResCode MSK12_reserve_num_dom(
    MSK12_Task_t task,
    int64_t num_dom) { return MSK12_reserve_num_dom_ptr(task,num_dom); }
MSK12_reserve_num_symmat_func_t MSK12_reserve_num_symmat_ptr;
MSK12_ResCode MSK12_reserve_num_symmat(
    MSK12_Task_t task,
    int64_t num_symmat) { return MSK12_reserve_num_symmat_ptr(task,num_symmat); }
MSK12_reserve_num_symmat_nz_func_t MSK12_reserve_num_symmat_nz_ptr;
MSK12_ResCode MSK12_reserve_num_symmat_nz(
    MSK12_Task_t task,
    int64_t num_nz) { return MSK12_reserve_num_symmat_nz_ptr(task,num_nz); }
MSK12_get_num_var_func_t MSK12_get_num_var_ptr;
int32_t MSK12_get_num_var(MSK12_Task_t task) { return MSK12_get_num_var_ptr(task); }
MSK12_get_num_barvar_func_t MSK12_get_num_barvar_ptr;
int32_t MSK12_get_num_barvar(MSK12_Task_t task) { return MSK12_get_num_barvar_ptr(task); }
MSK12_get_num_domain_func_t MSK12_get_num_domain_ptr;
int64_t MSK12_get_num_domain(MSK12_Task_t task) { return MSK12_get_num_domain_ptr(task); }
MSK12_get_num_row_func_t MSK12_get_num_row_ptr;
int64_t MSK12_get_num_row(MSK12_Task_t task) { return MSK12_get_num_row_ptr(task); }
MSK12_get_num_symmat_func_t MSK12_get_num_symmat_ptr;
int64_t MSK12_get_num_symmat(MSK12_Task_t task) { return MSK12_get_num_symmat_ptr(task); }
MSK12_get_num_con_func_t MSK12_get_num_con_ptr;
int64_t MSK12_get_num_con(MSK12_Task_t task) { return MSK12_get_num_con_ptr(task); }
MSK12_get_num_djc_func_t MSK12_get_num_djc_ptr;
int64_t MSK12_get_num_djc(MSK12_Task_t task) { return MSK12_get_num_djc_ptr(task); }
MSK12_append_vars_func_t MSK12_append_vars_ptr;
MSK12_ResCode MSK12_append_vars(
    MSK12_Task_t task,
    int32_t num_var) { return MSK12_append_vars_ptr(task,num_var); }
MSK12_append_rows_func_t MSK12_append_rows_ptr;
MSK12_ResCode MSK12_append_rows(
    MSK12_Task_t task,
    int64_t num_row) { return MSK12_append_rows_ptr(task,num_row); }
MSK12_append_barvar_func_t MSK12_append_barvar_ptr;
MSK12_ResCode MSK12_append_barvar(
    MSK12_Task_t task,
    int32_t dim) { return MSK12_append_barvar_ptr(task,dim); }
MSK12_append_barvars_func_t MSK12_append_barvars_ptr;
MSK12_ResCode MSK12_append_barvars(
    MSK12_Task_t task,
    int32_t num_barvar,
    const int32_t* dims) { return MSK12_append_barvars_ptr(task,num_barvar,dims); }
MSK12_append_symmat_func_t MSK12_append_symmat_ptr;
MSK12_ResCode MSK12_append_symmat(
    MSK12_Task_t task,
    int32_t dim,
    int64_t nnz,
    const int32_t* symmat_i,
    const int32_t* symmat_j,
    const double* symmat_val) { return MSK12_append_symmat_ptr(task,dim,nnz,symmat_i,symmat_j,symmat_val); }
MSK12_append_symmats_func_t MSK12_append_symmats_ptr;
MSK12_ResCode MSK12_append_symmats(
    MSK12_Task_t task,
    int64_t num_symmat,
    const int32_t* dim,
    const int64_t* nnz,
    const int32_t* symmat_i,
    const int32_t* symmat_j,
    const double* symmat_val) { return MSK12_append_symmats_ptr(task,num_symmat,dim,nnz,symmat_i,symmat_j,symmat_val); }
MSK12_append_empty_cons_func_t MSK12_append_empty_cons_ptr;
MSK12_ResCode MSK12_append_empty_cons(
    MSK12_Task_t task,
    int64_t num_con) { return MSK12_append_empty_cons_ptr(task,num_con); }
MSK12_append_empty_djcs_func_t MSK12_append_empty_djcs_ptr;
MSK12_ResCode MSK12_append_empty_djcs(
    MSK12_Task_t task,
    int64_t num_djc) { return MSK12_append_empty_djcs_ptr(task,num_djc); }
MSK12_put_var_type_func_t MSK12_put_var_type_ptr;
MSK12_ResCode MSK12_put_var_type(
    MSK12_Task_t task,
    int32_t j,
    MSK12_VariableType var_type) { return MSK12_put_var_type_ptr(task,j,var_type); }
MSK12_put_var_type_slice_func_t MSK12_put_var_type_slice_ptr;
MSK12_ResCode MSK12_put_var_type_slice(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    const MSK12_VariableType* var_types) { return MSK12_put_var_type_slice_ptr(task,first_var,num_var,var_types); }
MSK12_put_var_type_slice_value_func_t MSK12_put_var_type_slice_value_ptr;
MSK12_ResCode MSK12_put_var_type_slice_value(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    MSK12_VariableType var_type) { return MSK12_put_var_type_slice_value_ptr(task,first_var,num_var,var_type); }
MSK12_put_var_type_list_func_t MSK12_put_var_type_list_ptr;
MSK12_ResCode MSK12_put_var_type_list(
    MSK12_Task_t task,
    int32_t num_var,
    const int32_t* var_idxs,
    const MSK12_VariableType* var_types) { return MSK12_put_var_type_list_ptr(task,num_var,var_idxs,var_types); }
MSK12_get_var_type_func_t MSK12_get_var_type_ptr;
MSK12_ResCode MSK12_get_var_type(
    MSK12_Task_t task,
    int32_t var_idx,
    MSK12_VariableType var_type[1]) { return MSK12_get_var_type_ptr(task,var_idx,var_type); }
MSK12_get_var_type_slice_func_t MSK12_get_var_type_slice_ptr;
MSK12_ResCode MSK12_get_var_type_slice(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    MSK12_VariableType* var_types) { return MSK12_get_var_type_slice_ptr(task,first_var,num_var,var_types); }
MSK12_put_var_bound_func_t MSK12_put_var_bound_ptr;
MSK12_ResCode MSK12_put_var_bound(
    MSK12_Task_t task,
    int32_t var_idx,
    double low,
    double upr) { return MSK12_put_var_bound_ptr(task,var_idx,low,upr); }
MSK12_put_var_bound_slice_func_t MSK12_put_var_bound_slice_ptr;
MSK12_ResCode MSK12_put_var_bound_slice(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    const double* low,
    const double* upr) { return MSK12_put_var_bound_slice_ptr(task,first_var,num_var,low,upr); }
MSK12_put_var_bound_slice_value_func_t MSK12_put_var_bound_slice_value_ptr;
MSK12_ResCode MSK12_put_var_bound_slice_value(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    double low,
    double upr) { return MSK12_put_var_bound_slice_value_ptr(task,first_var,num_var,low,upr); }
MSK12_put_var_low_bound_func_t MSK12_put_var_low_bound_ptr;
MSK12_ResCode MSK12_put_var_low_bound(
    MSK12_Task_t task,
    int32_t var_idx,
    double low) { return MSK12_put_var_low_bound_ptr(task,var_idx,low); }
MSK12_put_var_low_bound_slice_func_t MSK12_put_var_low_bound_slice_ptr;
MSK12_ResCode MSK12_put_var_low_bound_slice(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    const double* low) { return MSK12_put_var_low_bound_slice_ptr(task,first_var,num_var,low); }
MSK12_put_var_low_bound_slice_value_func_t MSK12_put_var_low_bound_slice_value_ptr;
MSK12_ResCode MSK12_put_var_low_bound_slice_value(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    double low) { return MSK12_put_var_low_bound_slice_value_ptr(task,first_var,num_var,low); }
MSK12_put_var_upr_bound_func_t MSK12_put_var_upr_bound_ptr;
MSK12_ResCode MSK12_put_var_upr_bound(
    MSK12_Task_t task,
    int32_t var_idx,
    double upr) { return MSK12_put_var_upr_bound_ptr(task,var_idx,upr); }
MSK12_put_var_upr_bound_slice_func_t MSK12_put_var_upr_bound_slice_ptr;
MSK12_ResCode MSK12_put_var_upr_bound_slice(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    const double* upr) { return MSK12_put_var_upr_bound_slice_ptr(task,first_var,num_var,upr); }
MSK12_put_var_upr_bound_slice_value_func_t MSK12_put_var_upr_bound_slice_value_ptr;
MSK12_ResCode MSK12_put_var_upr_bound_slice_value(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    double upr) { return MSK12_put_var_upr_bound_slice_value_ptr(task,first_var,num_var,upr); }
MSK12_get_var_bound_func_t MSK12_get_var_bound_ptr;
MSK12_ResCode MSK12_get_var_bound(
    MSK12_Task_t task,
    int32_t var_idx,
    double low[1],
    double upr[1]) { return MSK12_get_var_bound_ptr(task,var_idx,low,upr); }
MSK12_get_var_bound_slice_func_t MSK12_get_var_bound_slice_ptr;
MSK12_ResCode MSK12_get_var_bound_slice(
    MSK12_Task_t task,
    int32_t first_var,
    int32_t num_var,
    double* low,
    double* upr) { return MSK12_get_var_bound_slice_ptr(task,first_var,num_var,low,upr); }
MSK12_get_barvar_slice_num_elm_func_t MSK12_get_barvar_slice_num_elm_ptr;
MSK12_ResCode MSK12_get_barvar_slice_num_elm(
    MSK12_Task_t task,
    int32_t first_barvar,
    int32_t num_barvar,
    int64_t num_elm[1]) { return MSK12_get_barvar_slice_num_elm_ptr(task,first_barvar,num_barvar,num_elm); }
MSK12_get_barvar_dim_func_t MSK12_get_barvar_dim_ptr;
MSK12_ResCode MSK12_get_barvar_dim(
    MSK12_Task_t task,
    int32_t barvar_idx,
    int32_t dim[1]) { return MSK12_get_barvar_dim_ptr(task,barvar_idx,dim); }
MSK12_get_barvar_slice_dims_func_t MSK12_get_barvar_slice_dims_ptr;
MSK12_ResCode MSK12_get_barvar_slice_dims(
    MSK12_Task_t task,
    int32_t first_barvar,
    int32_t num_barvar,
    int32_t* dim) { return MSK12_get_barvar_slice_dims_ptr(task,first_barvar,num_barvar,dim); }
MSK12_get_domain_func_t MSK12_get_domain_ptr;
MSK12_ResCode MSK12_get_domain(
    MSK12_Task_t task,
    MSK12_DomainType dom_type,
    int64_t dim,
    int32_t num_alpha,
    double* alpha,
    int64_t dom_idx[1]) { return MSK12_get_domain_ptr(task,dom_type,dim,num_alpha,alpha,dom_idx); }
MSK12_get_domain_empty_func_t MSK12_get_domain_empty_ptr;
MSK12_ResCode MSK12_get_domain_empty(
    MSK12_Task_t task,
    int64_t dom_idx[1]) { return MSK12_get_domain_empty_ptr(task,dom_idx); }
MSK12_get_domain_rzero_func_t MSK12_get_domain_rzero_ptr;
MSK12_ResCode MSK12_get_domain_rzero(
    MSK12_Task_t task,
    int64_t dom_idx[1]) { return MSK12_get_domain_rzero_ptr(task,dom_idx); }
MSK12_get_domain_rplus_func_t MSK12_get_domain_rplus_ptr;
MSK12_ResCode MSK12_get_domain_rplus(
    MSK12_Task_t task,
    int64_t dom_idx[1]) { return MSK12_get_domain_rplus_ptr(task,dom_idx); }
MSK12_get_domain_rminus_func_t MSK12_get_domain_rminus_ptr;
MSK12_ResCode MSK12_get_domain_rminus(
    MSK12_Task_t task,
    int64_t dom_idx[1]) { return MSK12_get_domain_rminus_ptr(task,dom_idx); }
MSK12_get_domain_r_func_t MSK12_get_domain_r_ptr;
MSK12_ResCode MSK12_get_domain_r(
    MSK12_Task_t task,
    int64_t dom_idx[1]) { return MSK12_get_domain_r_ptr(task,dom_idx); }
MSK12_get_domain_quadratic_cone_func_t MSK12_get_domain_quadratic_cone_ptr;
MSK12_ResCode MSK12_get_domain_quadratic_cone(
    MSK12_Task_t task,
    int64_t n,
    int64_t dom_idx[1]) { return MSK12_get_domain_quadratic_cone_ptr(task,n,dom_idx); }
MSK12_get_domain_rotated_quadratic_cone_func_t MSK12_get_domain_rotated_quadratic_cone_ptr;
MSK12_ResCode MSK12_get_domain_rotated_quadratic_cone(
    MSK12_Task_t task,
    int64_t n,
    int64_t dom_idx[1]) { return MSK12_get_domain_rotated_quadratic_cone_ptr(task,n,dom_idx); }
MSK12_get_domain_primal_exponential_cone_func_t MSK12_get_domain_primal_exponential_cone_ptr;
MSK12_ResCode MSK12_get_domain_primal_exponential_cone(
    MSK12_Task_t task,
    int64_t dom_idx[1]) { return MSK12_get_domain_primal_exponential_cone_ptr(task,dom_idx); }
MSK12_get_domain_dual_exponential_cone_func_t MSK12_get_domain_dual_exponential_cone_ptr;
MSK12_ResCode MSK12_get_domain_dual_exponential_cone(
    MSK12_Task_t task,
    int64_t dom_idx[1]) { return MSK12_get_domain_dual_exponential_cone_ptr(task,dom_idx); }
MSK12_get_domain_primal_power_cone_func_t MSK12_get_domain_primal_power_cone_ptr;
MSK12_ResCode MSK12_get_domain_primal_power_cone(
    MSK12_Task_t task,
    int64_t n,
    int64_t num_alpha,
    const double* alpha,
    int64_t dom_idx[1]) { return MSK12_get_domain_primal_power_cone_ptr(task,n,num_alpha,alpha,dom_idx); }
MSK12_get_domain_dual_power_cone_func_t MSK12_get_domain_dual_power_cone_ptr;
MSK12_ResCode MSK12_get_domain_dual_power_cone(
    MSK12_Task_t task,
    int64_t n,
    int64_t num_alpha,
    const double* alpha,
    int64_t dom_idx[1]) { return MSK12_get_domain_dual_power_cone_ptr(task,n,num_alpha,alpha,dom_idx); }
MSK12_get_domain_primal_geometric_mean_cone_func_t MSK12_get_domain_primal_geometric_mean_cone_ptr;
MSK12_ResCode MSK12_get_domain_primal_geometric_mean_cone(
    MSK12_Task_t task,
    int64_t n,
    int64_t dom_idx[1]) { return MSK12_get_domain_primal_geometric_mean_cone_ptr(task,n,dom_idx); }
MSK12_get_domain_dual_geometric_mean_cone_func_t MSK12_get_domain_dual_geometric_mean_cone_ptr;
MSK12_ResCode MSK12_get_domain_dual_geometric_mean_cone(
    MSK12_Task_t task,
    int64_t n,
    int64_t dom_idx[1]) { return MSK12_get_domain_dual_geometric_mean_cone_ptr(task,n,dom_idx); }
MSK12_get_domain_svecpsd_cone_func_t MSK12_get_domain_svecpsd_cone_ptr;
MSK12_ResCode MSK12_get_domain_svecpsd_cone(
    MSK12_Task_t task,
    int64_t n,
    int64_t dom_idx[1]) { return MSK12_get_domain_svecpsd_cone_ptr(task,n,dom_idx); }
MSK12_get_domain_info_func_t MSK12_get_domain_info_ptr;
MSK12_ResCode MSK12_get_domain_info(
    MSK12_Task_t task,
    int64_t dom_idx,
    MSK12_DomainType dom_type[1],
    int64_t size[1],
    int32_t num_alpha[1]) { return MSK12_get_domain_info_ptr(task,dom_idx,dom_type,size,num_alpha); }
MSK12_get_domain_alpha_func_t MSK12_get_domain_alpha_ptr;
MSK12_ResCode MSK12_get_domain_alpha(
    MSK12_Task_t task,
    int64_t dom_idx,
    int64_t num_alpha,
    double* alpha) { return MSK12_get_domain_alpha_ptr(task,dom_idx,num_alpha,alpha); }
MSK12_put_row_func_t MSK12_put_row_ptr;
MSK12_ResCode MSK12_put_row(
    MSK12_Task_t task,
    int64_t row_idx,
    int32_t num_nz,
    const int32_t* subj,
    const double* cof) { return MSK12_put_row_ptr(task,row_idx,num_nz,subj,cof); }
MSK12_put_row_slice_func_t MSK12_put_row_slice_ptr;
MSK12_ResCode MSK12_put_row_slice(
    MSK12_Task_t task,
    int64_t first_row,
    int64_t num_row,
    const int32_t* row_num_nz,
    const int32_t* subj,
    const double* cof) { return MSK12_put_row_slice_ptr(task,first_row,num_row,row_num_nz,subj,cof); }
MSK12_put_row_list_func_t MSK12_put_row_list_ptr;
MSK12_ResCode MSK12_put_row_list(
    MSK12_Task_t task,
    int64_t num_row,
    const int64_t* row_idxs,
    const int32_t* row_num_nz,
    const int32_t** subj,
    const double** cof) { return MSK12_put_row_list_ptr(task,num_row,row_idxs,row_num_nz,subj,cof); }
MSK12_put_row_g_func_t MSK12_put_row_g_ptr;
MSK12_ResCode MSK12_put_row_g(
    MSK12_Task_t task,
    int64_t row_idx,
    double g) { return MSK12_put_row_g_ptr(task,row_idx,g); }
MSK12_put_row_slice_g_func_t MSK12_put_row_slice_g_ptr;
MSK12_ResCode MSK12_put_row_slice_g(
    MSK12_Task_t task,
    int64_t first_row,
    int64_t num_row,
    const double* g) { return MSK12_put_row_slice_g_ptr(task,first_row,num_row,g); }
MSK12_put_row_list_g_func_t MSK12_put_row_list_g_ptr;
MSK12_ResCode MSK12_put_row_list_g(
    MSK12_Task_t task,
    int64_t num_row,
    const int64_t* row_idxs,
    const double* g) { return MSK12_put_row_list_g_ptr(task,num_row,row_idxs,g); }
MSK12_put_col_func_t MSK12_put_col_ptr;
MSK12_ResCode MSK12_put_col(
    MSK12_Task_t task,
    int32_t col_idx,
    int64_t num_nz,
    const int64_t* row_idxs,
    const double* cof) { return MSK12_put_col_ptr(task,col_idx,num_nz,row_idxs,cof); }
MSK12_put_col_slice_func_t MSK12_put_col_slice_ptr;
MSK12_ResCode MSK12_put_col_slice(
    MSK12_Task_t task,
    int32_t first_col,
    int32_t num_col,
    const int64_t* col_len,
    const int64_t* row_idxs,
    const double* cof) { return MSK12_put_col_slice_ptr(task,first_col,num_col,col_len,row_idxs,cof); }
MSK12_put_col_list_func_t MSK12_put_col_list_ptr;
MSK12_ResCode MSK12_put_col_list(
    MSK12_Task_t task,
    int32_t num_col,
    const int32_t* col_idxs,
    const int64_t* col_lens,
    const int64_t** row_idxs,
    const double** cof) { return MSK12_put_col_list_ptr(task,num_col,col_idxs,col_lens,row_idxs,cof); }
MSK12_put_ijc_func_t MSK12_put_ijc_ptr;
MSK12_ResCode MSK12_put_ijc(
    MSK12_Task_t task,
    int64_t row_idx,
    int32_t var_idx,
    double cof) { return MSK12_put_ijc_ptr(task,row_idx,var_idx,cof); }
MSK12_put_ijc_list_func_t MSK12_put_ijc_list_ptr;
MSK12_ResCode MSK12_put_ijc_list(
    MSK12_Task_t task,
    int64_t num_nz,
    const int64_t* row_idxs,
    const int32_t* col_idxs,
    const double* cof) { return MSK12_put_ijc_list_ptr(task,num_nz,row_idxs,col_idxs,cof); }
MSK12_get_row_num_nz_func_t MSK12_get_row_num_nz_ptr;
MSK12_ResCode MSK12_get_row_num_nz(
    MSK12_Task_t task,
    int64_t row_idx,
    int32_t num_nz[1]) { return MSK12_get_row_num_nz_ptr(task,row_idx,num_nz); }
MSK12_get_row_slice_num_nz_func_t MSK12_get_row_slice_num_nz_ptr;
MSK12_ResCode MSK12_get_row_slice_num_nz(
    MSK12_Task_t task,
    int64_t first_row,
    int64_t num_row,
    int64_t* num_nz) { return MSK12_get_row_slice_num_nz_ptr(task,first_row,num_row,num_nz); }
MSK12_get_row_func_t MSK12_get_row_ptr;
MSK12_ResCode MSK12_get_row(
    MSK12_Task_t task,
    int64_t row_idx,
    int32_t nnz,
    int32_t* subj,
    double* cof) { return MSK12_get_row_ptr(task,row_idx,nnz,subj,cof); }
MSK12_get_row_slice_func_t MSK12_get_row_slice_ptr;
MSK12_ResCode MSK12_get_row_slice(
    MSK12_Task_t task,
    int64_t first_row,
    int64_t num_row,
    int64_t nnz,
    int32_t* row_len,
    int32_t* subj,
    double* cof) { return MSK12_get_row_slice_ptr(task,first_row,num_row,nnz,row_len,subj,cof); }
MSK12_get_col_num_nz_func_t MSK12_get_col_num_nz_ptr;
MSK12_ResCode MSK12_get_col_num_nz(
    MSK12_Task_t task,
    int32_t col_idx,
    int64_t num_nz[1]) { return MSK12_get_col_num_nz_ptr(task,col_idx,num_nz); }
MSK12_get_col_slice_num_nz_func_t MSK12_get_col_slice_num_nz_ptr;
MSK12_ResCode MSK12_get_col_slice_num_nz(
    MSK12_Task_t task,
    int32_t first_col,
    int32_t num_col,
    int64_t* num_nz) { return MSK12_get_col_slice_num_nz_ptr(task,first_col,num_col,num_nz); }
MSK12_get_col_func_t MSK12_get_col_ptr;
MSK12_ResCode MSK12_get_col(
    MSK12_Task_t task,
    int32_t col_idx,
    int64_t nnz,
    int64_t* subi,
    double* cof) { return MSK12_get_col_ptr(task,col_idx,nnz,subi,cof); }
MSK12_get_col_slice_func_t MSK12_get_col_slice_ptr;
MSK12_ResCode MSK12_get_col_slice(
    MSK12_Task_t task,
    int32_t first_col,
    int32_t num_col,
    int64_t nnz,
    int64_t* col_len,
    int64_t* subi,
    double* cof) { return MSK12_get_col_slice_ptr(task,first_col,num_col,nnz,col_len,subi,cof); }
MSK12_put_bar_entry_func_t MSK12_put_bar_entry_ptr;
MSK12_ResCode MSK12_put_bar_entry(
    MSK12_Task_t task,
    int64_t row_idx,
    int32_t barvar_idx,
    int64_t num_weight,
    const int64_t* matrix_idx,
    const double* weight) { return MSK12_put_bar_entry_ptr(task,row_idx,barvar_idx,num_weight,matrix_idx,weight); }
MSK12_put_bar_entry_list_func_t MSK12_put_bar_entry_list_ptr;
MSK12_ResCode MSK12_put_bar_entry_list(
    MSK12_Task_t task,
    int64_t num_bar_entry,
    const int64_t* row_idx,
    const int32_t* barvar_idx,
    const int64_t* num_weight,
    const int64_t* matrix_idx,
    const double* weight) { return MSK12_put_bar_entry_list_ptr(task,num_bar_entry,row_idx,barvar_idx,num_weight,matrix_idx,weight); }
MSK12_put_bar_row_func_t MSK12_put_bar_row_ptr;
MSK12_ResCode MSK12_put_bar_row(
    MSK12_Task_t task,
    int64_t row_idx,
    int32_t num_bar_entry,
    const int32_t* barvar_idx,
    const int64_t* num_weight,
    const int64_t* matrix_idx,
    const double* weight) { return MSK12_put_bar_row_ptr(task,row_idx,num_bar_entry,barvar_idx,num_weight,matrix_idx,weight); }
MSK12_get_symmat_info_func_t MSK12_get_symmat_info_ptr;
MSK12_ResCode MSK12_get_symmat_info(
    MSK12_Task_t task,
    int64_t symmat_idx,
    int32_t dim[1],
    int64_t nnz[1]) { return MSK12_get_symmat_info_ptr(task,symmat_idx,dim,nnz); }
MSK12_get_symmat_func_t MSK12_get_symmat_ptr;
MSK12_ResCode MSK12_get_symmat(
    MSK12_Task_t task,
    int64_t symmat_idx,
    int64_t nnz,
    int32_t* symmat_i,
    int32_t* symmat_j,
    double* symmat_val) { return MSK12_get_symmat_ptr(task,symmat_idx,nnz,symmat_i,symmat_j,symmat_val); }
MSK12_get_symmat_slice_info_func_t MSK12_get_symmat_slice_info_ptr;
MSK12_ResCode MSK12_get_symmat_slice_info(
    MSK12_Task_t task,
    int64_t first_symmat,
    int64_t num_symmat,
    int32_t* dim,
    int64_t* nnz) { return MSK12_get_symmat_slice_info_ptr(task,first_symmat,num_symmat,dim,nnz); }
MSK12_get_symmat_slice_func_t MSK12_get_symmat_slice_ptr;
MSK12_ResCode MSK12_get_symmat_slice(
    MSK12_Task_t task,
    int64_t first_symmat,
    int64_t num_symmat,
    int64_t total_nnz,
    int32_t* symmat_i,
    int32_t* symmat_j,
    double* symmat_val) { return MSK12_get_symmat_slice_ptr(task,first_symmat,num_symmat,total_nnz,symmat_i,symmat_j,symmat_val); }
MSK12_append_con_func_t MSK12_append_con_ptr;
MSK12_ResCode MSK12_append_con(
    MSK12_Task_t task,
    int64_t dom_idx,
    int64_t num_rows,
    const int64_t* row_idxs,
    NULLABLE const double* con_offset) { return MSK12_append_con_ptr(task,dom_idx,num_rows,row_idxs,con_offset); }
MSK12_append_cons_func_t MSK12_append_cons_ptr;
MSK12_ResCode MSK12_append_cons(
    MSK12_Task_t task,
    int64_t num_con,
    const int64_t* dom_idxs,
    int64_t num_rows,
    const int64_t* row_idxs,
    NULLABLE const double* con_offset) { return MSK12_append_cons_ptr(task,num_con,dom_idxs,num_rows,row_idxs,con_offset); }
MSK12_append_cons_seq_func_t MSK12_append_cons_seq_ptr;
MSK12_ResCode MSK12_append_cons_seq(
    MSK12_Task_t task,
    int64_t num_con,
    int64_t dom_idx,
    int64_t first_row,
    int64_t num_rows,
    NULLABLE const double* con_offset) { return MSK12_append_cons_seq_ptr(task,num_con,dom_idx,first_row,num_rows,con_offset); }
MSK12_put_con_func_t MSK12_put_con_ptr;
MSK12_ResCode MSK12_put_con(
    MSK12_Task_t task,
    int64_t con_idx,
    int64_t num_rows,
    int64_t dom_idx,
    const int64_t* row_idxs,
    NULLABLE const double* rhs_offset) { return MSK12_put_con_ptr(task,con_idx,num_rows,dom_idx,row_idxs,rhs_offset); }
MSK12_put_scalar_con_func_t MSK12_put_scalar_con_ptr;
MSK12_ResCode MSK12_put_scalar_con(
    MSK12_Task_t task,
    int64_t con_idx,
    int64_t dom_idx,
    int64_t row_idx,
    double rhs_offset) { return MSK12_put_scalar_con_ptr(task,con_idx,dom_idx,row_idx,rhs_offset); }
MSK12_put_con_slice_func_t MSK12_put_con_slice_ptr;
MSK12_ResCode MSK12_put_con_slice(
    MSK12_Task_t task,
    int64_t first_con,
    int64_t num_con,
    int64_t num_rows,
    const int64_t* dom_idx,
    const int64_t* row_idx,
    NULLABLE const double* rhs_offset) { return MSK12_put_con_slice_ptr(task,first_con,num_con,num_rows,dom_idx,row_idx,rhs_offset); }
MSK12_get_con_slice_domains_func_t MSK12_get_con_slice_domains_ptr;
MSK12_ResCode MSK12_get_con_slice_domains(
    MSK12_Task_t task,
    int64_t first_con,
    int64_t num_con,
    int64_t* dom_idx) { return MSK12_get_con_slice_domains_ptr(task,first_con,num_con,dom_idx); }
MSK12_get_con_slice_num_row_func_t MSK12_get_con_slice_num_row_ptr;
MSK12_ResCode MSK12_get_con_slice_num_row(
    MSK12_Task_t task,
    int64_t first_con,
    int64_t num_con,
    int64_t num_row[1]) { return MSK12_get_con_slice_num_row_ptr(task,first_con,num_con,num_row); }
MSK12_get_con_slice_func_t MSK12_get_con_slice_ptr;
MSK12_ResCode MSK12_get_con_slice(
    MSK12_Task_t task,
    int64_t first_con,
    int64_t num_con,
    int64_t num_row,
    int64_t* row_idx,
    double* rhs_offset,
    int64_t* dom_idx) { return MSK12_get_con_slice_ptr(task,first_con,num_con,num_row,row_idx,rhs_offset,dom_idx); }
MSK12_append_djc_func_t MSK12_append_djc_ptr;
MSK12_ResCode MSK12_append_djc(
    MSK12_Task_t task,
    int64_t num_rows,
    int64_t num_dom,
    int64_t num_terms,
    const int64_t* dom_idx,
    const int64_t* term_size,
    const int64_t* row_idx,
    const double* rhs_offset) { return MSK12_append_djc_ptr(task,num_rows,num_dom,num_terms,dom_idx,term_size,row_idx,rhs_offset); }
MSK12_put_djc_func_t MSK12_put_djc_ptr;
MSK12_ResCode MSK12_put_djc(
    MSK12_Task_t task,
    int64_t djc_idx,
    int64_t num_rows,
    int64_t num_dom,
    int64_t num_terms,
    const int64_t* dom_idx,
    const int64_t* term_size,
    const int64_t* row_idx,
    const double* rhs_offset) { return MSK12_put_djc_ptr(task,djc_idx,num_rows,num_dom,num_terms,dom_idx,term_size,row_idx,rhs_offset); }
MSK12_put_djc_slice_func_t MSK12_put_djc_slice_ptr;
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
    const int64_t* djc_numterm) { return MSK12_put_djc_slice_ptr(task,first_djc,num_djc,num_rows,num_dom,num_terms,dom_idx,term_size,row_idx,rhs_offset,djc_numterm); }
MSK12_get_djc_info_func_t MSK12_get_djc_info_ptr;
MSK12_ResCode MSK12_get_djc_info(
    MSK12_Task_t task,
    int64_t djc_idx,
    int64_t num_term[1],
    int64_t num_dom[1],
    int64_t num_row[1]) { return MSK12_get_djc_info_ptr(task,djc_idx,num_term,num_dom,num_row); }
MSK12_get_djc_func_t MSK12_get_djc_ptr;
MSK12_ResCode MSK12_get_djc(
    MSK12_Task_t task,
    int64_t djc_idx,
    int64_t num_terms,
    int64_t num_dom,
    int64_t num_row,
    int64_t* term_size,
    int64_t* dom_idx,
    int64_t* row_idx,
    double* rhs_offset) { return MSK12_get_djc_ptr(task,djc_idx,num_terms,num_dom,num_row,term_size,dom_idx,row_idx,rhs_offset); }
MSK12_get_djc_slice_info_func_t MSK12_get_djc_slice_info_ptr;
MSK12_ResCode MSK12_get_djc_slice_info(
    MSK12_Task_t task,
    int64_t first_djc,
    int64_t num_djc,
    int64_t num_term[1],
    int64_t num_dom[1],
    int64_t num_row[1]) { return MSK12_get_djc_slice_info_ptr(task,first_djc,num_djc,num_term,num_dom,num_row); }
MSK12_get_djc_slice_func_t MSK12_get_djc_slice_ptr;
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
    int64_t* djc_num_term) { return MSK12_get_djc_slice_ptr(task,first_djc,num_djc,num_term,num_dom,num_row,term_size,dom_idx,row_idx,rhs_offset,djc_num_term); }
MSK12_put_obj_sense_func_t MSK12_put_obj_sense_ptr;
void MSK12_put_obj_sense(
    MSK12_Task_t task,
    MSK12_ObjSense sense) { return MSK12_put_obj_sense_ptr(task,sense); }
MSK12_get_obj_sense_func_t MSK12_get_obj_sense_ptr;
MSK12_ObjSense MSK12_get_obj_sense(MSK12_Task_t task) { return MSK12_get_obj_sense_ptr(task); }
MSK12_put_obj_row_func_t MSK12_put_obj_row_ptr;
MSK12_ResCode MSK12_put_obj_row(
    MSK12_Task_t task,
    int64_t row_idx) { return MSK12_put_obj_row_ptr(task,row_idx); }
MSK12_get_obj_row_func_t MSK12_get_obj_row_ptr;
void MSK12_get_obj_row(
    MSK12_Task_t task,
    int64_t row_idx[1],
    int asgn[1]) { return MSK12_get_obj_row_ptr(task,row_idx,asgn); }
MSK12_optimize_func_t MSK12_optimize_ptr;
MSK12_ResCode MSK12_optimize(
    MSK12_Task_t task,
    MSK12_TrmCode trm[1]) { return MSK12_optimize_ptr(task,trm); }
MSK12_solution_summary_func_t MSK12_solution_summary_ptr;
MSK12_ResCode MSK12_solution_summary(
    MSK12_Task_t task,
    MSK12_StreamType whichstream) { return MSK12_solution_summary_ptr(task,whichstream); }
MSK12_optimize_callback_func_t MSK12_optimize_callback_ptr;
MSK12_ResCode MSK12_optimize_callback(
    MSK12_Task_t task,
    MSK12_TrmCode trm[1],
    MSK12_CallbackHandle cb_handle,
    MSK12_CallbackFunc cb_func,
    MSK12_CallbackHandle int_cb_handle,
    MSK12_IntSolCallbackFunc int_cb_func) { return MSK12_optimize_callback_ptr(task,trm,cb_handle,cb_func,int_cb_handle,int_cb_func); }
MSK12_put_remote_solver_func_t MSK12_put_remote_solver_ptr;
void MSK12_put_remote_solver(
    MSK12_Task_t task,
    const char* server,
    NULLABLE const char* cert) { return MSK12_put_remote_solver_ptr(task,server,cert); }
MSK12_put_optserver_access_token_func_t MSK12_put_optserver_access_token_ptr;
void MSK12_put_optserver_access_token(
    MSK12_Task_t task,
    NULLABLE const char* token) { return MSK12_put_optserver_access_token_ptr(task,token); }
MSK12_get_num_sol_func_t MSK12_get_num_sol_ptr;
int32_t MSK12_get_num_sol(MSK12_Task_t task) { return MSK12_get_num_sol_ptr(task); }
MSK12_get_sol_type_func_t MSK12_get_sol_type_ptr;
MSK12_ResCode MSK12_get_sol_type(
    MSK12_Task_t task,
    int32_t sol_idx,
    MSK12_SolType sol_type[1]) { return MSK12_get_sol_type_ptr(task,sol_idx,sol_type); }
MSK12_get_sol_status_func_t MSK12_get_sol_status_ptr;
MSK12_ResCode MSK12_get_sol_status(
    MSK12_Task_t task,
    int32_t sol_idx,
    MSK12_SolSta primal_sol_sta[1],
    MSK12_SolSta dual_sol_sta[1]) { return MSK12_get_sol_status_ptr(task,sol_idx,primal_sol_sta,dual_sol_sta); }
MSK12_get_problem_status_func_t MSK12_get_problem_status_ptr;
MSK12_ResCode MSK12_get_problem_status(
    MSK12_Task_t task,
    int32_t sol_idx,
    MSK12_ProSta pro_sta[1]) { return MSK12_get_problem_status_ptr(task,sol_idx,pro_sta); }
MSK12_get_primal_obj_func_t MSK12_get_primal_obj_ptr;
MSK12_ResCode MSK12_get_primal_obj(
    MSK12_Task_t task,
    int32_t sol_idx,
    double obj_val[1]) { return MSK12_get_primal_obj_ptr(task,sol_idx,obj_val); }
MSK12_get_dual_obj_func_t MSK12_get_dual_obj_ptr;
MSK12_ResCode MSK12_get_dual_obj(
    MSK12_Task_t task,
    int32_t sol_idx,
    double obj_val[1]) { return MSK12_get_dual_obj_ptr(task,sol_idx,obj_val); }
MSK12_get_sol_xx_slice_func_t MSK12_get_sol_xx_slice_ptr;
MSK12_ResCode MSK12_get_sol_xx_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_var,
    int32_t num_var,
    double* xx) { return MSK12_get_sol_xx_slice_ptr(task,sol_idx,first_var,num_var,xx); }
MSK12_get_sol_slx_slice_func_t MSK12_get_sol_slx_slice_ptr;
MSK12_ResCode MSK12_get_sol_slx_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_var,
    int32_t num_var,
    double* slx) { return MSK12_get_sol_slx_slice_ptr(task,sol_idx,first_var,num_var,slx); }
MSK12_get_sol_sux_slice_func_t MSK12_get_sol_sux_slice_ptr;
MSK12_ResCode MSK12_get_sol_sux_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_var,
    int32_t num_var,
    double* sux) { return MSK12_get_sol_sux_slice_ptr(task,sol_idx,first_var,num_var,sux); }
MSK12_get_sol_barxj_func_t MSK12_get_sol_barxj_ptr;
MSK12_ResCode MSK12_get_sol_barxj(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t barvar_idx,
    int64_t num_var,
    double* barx) { return MSK12_get_sol_barxj_ptr(task,sol_idx,barvar_idx,num_var,barx); }
MSK12_get_sol_barsj_func_t MSK12_get_sol_barsj_ptr;
MSK12_ResCode MSK12_get_sol_barsj(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t barvar_idx,
    int64_t num_elm,
    double* bars) { return MSK12_get_sol_barsj_ptr(task,sol_idx,barvar_idx,num_elm,bars); }
MSK12_get_sol_barx_slice_func_t MSK12_get_sol_barx_slice_ptr;
MSK12_ResCode MSK12_get_sol_barx_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_barvar,
    int32_t num_barvar,
    int64_t num_elm,
    double* barx) { return MSK12_get_sol_barx_slice_ptr(task,sol_idx,first_barvar,num_barvar,num_elm,barx); }
MSK12_get_sol_bars_slice_func_t MSK12_get_sol_bars_slice_ptr;
MSK12_ResCode MSK12_get_sol_bars_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_barvar,
    int32_t num_barvar,
    int64_t num_elm,
    double* bars) { return MSK12_get_sol_bars_slice_ptr(task,sol_idx,first_barvar,num_barvar,num_elm,bars); }
MSK12_get_sol_basic_xj_func_t MSK12_get_sol_basic_xj_ptr;
MSK12_ResCode MSK12_get_sol_basic_xj(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t var_idx,
    int basic[1]) { return MSK12_get_sol_basic_xj_ptr(task,sol_idx,var_idx,basic); }
MSK12_get_sol_basic_barx_func_t MSK12_get_sol_basic_barx_ptr;
MSK12_ResCode MSK12_get_sol_basic_barx(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t barvar_idx,
    int basic[1]) { return MSK12_get_sol_basic_barx_ptr(task,sol_idx,barvar_idx,basic); }
MSK12_get_sol_basic_con_func_t MSK12_get_sol_basic_con_ptr;
MSK12_ResCode MSK12_get_sol_basic_con(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t con_idx,
    int basic[1]) { return MSK12_get_sol_basic_con_ptr(task,sol_idx,con_idx,basic); }
MSK12_get_sol_sta_var_func_t MSK12_get_sol_sta_var_ptr;
MSK12_ResCode MSK12_get_sol_sta_var(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t var_idx,
    int low_binding[1],
    int upr_binding[1]) { return MSK12_get_sol_sta_var_ptr(task,sol_idx,var_idx,low_binding,upr_binding); }
MSK12_get_sol_sta_barx_func_t MSK12_get_sol_sta_barx_ptr;
MSK12_ResCode MSK12_get_sol_sta_barx(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t barvar_idx,
    int binding[1]) { return MSK12_get_sol_sta_barx_ptr(task,sol_idx,barvar_idx,binding); }
MSK12_get_sol_sta_con_func_t MSK12_get_sol_sta_con_ptr;
MSK12_ResCode MSK12_get_sol_sta_con(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t con_idx,
    int binding[1]) { return MSK12_get_sol_sta_con_ptr(task,sol_idx,con_idx,binding); }
MSK12_get_sol_basic_x_slice_func_t MSK12_get_sol_basic_x_slice_ptr;
MSK12_ResCode MSK12_get_sol_basic_x_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_var,
    int32_t num_var,
    int* basic) { return MSK12_get_sol_basic_x_slice_ptr(task,sol_idx,first_var,num_var,basic); }
MSK12_get_sol_basic_barx_slice_func_t MSK12_get_sol_basic_barx_slice_ptr;
MSK12_ResCode MSK12_get_sol_basic_barx_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_var,
    int32_t num_var,
    int* basic) { return MSK12_get_sol_basic_barx_slice_ptr(task,sol_idx,first_var,num_var,basic); }
MSK12_get_sol_basic_con_slice_func_t MSK12_get_sol_basic_con_slice_ptr;
MSK12_ResCode MSK12_get_sol_basic_con_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t first_con,
    int64_t num_con,
    int* basic) { return MSK12_get_sol_basic_con_slice_ptr(task,sol_idx,first_con,num_con,basic); }
MSK12_get_sol_sta_var_slice_func_t MSK12_get_sol_sta_var_slice_ptr;
MSK12_ResCode MSK12_get_sol_sta_var_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_var,
    int32_t num_var,
    int* low_binding,
    int* upr_binding) { return MSK12_get_sol_sta_var_slice_ptr(task,sol_idx,first_var,num_var,low_binding,upr_binding); }
MSK12_get_sol_sta_barx_slice_func_t MSK12_get_sol_sta_barx_slice_ptr;
MSK12_ResCode MSK12_get_sol_sta_barx_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t first_barvar,
    int32_t num_barvar,
    int* bindnig) { return MSK12_get_sol_sta_barx_slice_ptr(task,sol_idx,first_barvar,num_barvar,bindnig); }
MSK12_get_sol_sta_con_slice_func_t MSK12_get_sol_sta_con_slice_ptr;
MSK12_ResCode MSK12_get_sol_sta_con_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t first_con,
    int64_t num_con,
    int* binding) { return MSK12_get_sol_sta_con_slice_ptr(task,sol_idx,first_con,num_con,binding); }
MSK12_get_sol_xc_slice_func_t MSK12_get_sol_xc_slice_ptr;
MSK12_ResCode MSK12_get_sol_xc_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t first_con,
    int64_t num_con,
    int64_t num_elm,
    double* xc) { return MSK12_get_sol_xc_slice_ptr(task,sol_idx,first_con,num_con,num_elm,xc); }
MSK12_get_sol_y_slice_func_t MSK12_get_sol_y_slice_ptr;
MSK12_ResCode MSK12_get_sol_y_slice(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t first_con,
    int64_t num_con,
    int64_t num_elm,
    double* y) { return MSK12_get_sol_y_slice_ptr(task,sol_idx,first_con,num_con,num_elm,y); }
MSK12_get_num_input_solutions_func_t MSK12_get_num_input_solutions_ptr;
int32_t MSK12_get_num_input_solutions(MSK12_Task_t task) { return MSK12_get_num_input_solutions_ptr(task); }
MSK12_copy_sol_to_input_func_t MSK12_copy_sol_to_input_ptr;
MSK12_ResCode MSK12_copy_sol_to_input(
    MSK12_Task_t task,
    int32_t sol_idx) { return MSK12_copy_sol_to_input_ptr(task,sol_idx); }
MSK12_append_sol_func_t MSK12_append_sol_ptr;
MSK12_ResCode MSK12_append_sol(
    MSK12_Task_t task,
    MSK12_SolType soltype) { return MSK12_append_sol_ptr(task,soltype); }
MSK12_put_sol_xx_func_t MSK12_put_sol_xx_ptr;
MSK12_ResCode MSK12_put_sol_xx(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t num,
    const double* val) { return MSK12_put_sol_xx_ptr(task,sol_idx,num,val); }
MSK12_put_sol_slx_func_t MSK12_put_sol_slx_ptr;
MSK12_ResCode MSK12_put_sol_slx(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t num,
    const double* val) { return MSK12_put_sol_slx_ptr(task,sol_idx,num,val); }
MSK12_put_sol_sux_func_t MSK12_put_sol_sux_ptr;
MSK12_ResCode MSK12_put_sol_sux(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t num,
    const double* val) { return MSK12_put_sol_sux_ptr(task,sol_idx,num,val); }
MSK12_put_sol_basic_x_func_t MSK12_put_sol_basic_x_ptr;
MSK12_ResCode MSK12_put_sol_basic_x(
    MSK12_Task_t task,
    int32_t sol_idx,
    int32_t num,
    const int32_t* val) { return MSK12_put_sol_basic_x_ptr(task,sol_idx,num,val); }
MSK12_put_sol_barx_func_t MSK12_put_sol_barx_ptr;
MSK12_ResCode MSK12_put_sol_barx(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t num,
    const double* val) { return MSK12_put_sol_barx_ptr(task,sol_idx,num,val); }
MSK12_put_sol_bars_func_t MSK12_put_sol_bars_ptr;
MSK12_ResCode MSK12_put_sol_bars(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t num,
    const double* xx) { return MSK12_put_sol_bars_ptr(task,sol_idx,num,xx); }
MSK12_put_sol_yi_func_t MSK12_put_sol_yi_ptr;
MSK12_ResCode MSK12_put_sol_yi(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t i,
    int64_t num,
    const double* xx) { return MSK12_put_sol_yi_ptr(task,sol_idx,i,num,xx); }
MSK12_put_sol_basic_c_func_t MSK12_put_sol_basic_c_ptr;
MSK12_ResCode MSK12_put_sol_basic_c(
    MSK12_Task_t task,
    int32_t sol_idx,
    int64_t num,
    const int32_t* val) { return MSK12_put_sol_basic_c_ptr(task,sol_idx,num,val); }
MSK12_get_num_iinf_func_t MSK12_get_num_iinf_ptr;
int32_t MSK12_get_num_iinf() { return MSK12_get_num_iinf_ptr(); }
MSK12_get_num_liinf_func_t MSK12_get_num_liinf_ptr;
int32_t MSK12_get_num_liinf() { return MSK12_get_num_liinf_ptr(); }
MSK12_get_num_dinf_func_t MSK12_get_num_dinf_ptr;
int32_t MSK12_get_num_dinf() { return MSK12_get_num_dinf_ptr(); }
MSK12_get_iinf_func_t MSK12_get_iinf_ptr;
MSK12_ResCode MSK12_get_iinf(
    MSK12_Task_t task,
    int32_t par_idx,
    int32_t value[1]) { return MSK12_get_iinf_ptr(task,par_idx,value); }
MSK12_get_liinf_func_t MSK12_get_liinf_ptr;
MSK12_ResCode MSK12_get_liinf(
    MSK12_Task_t task,
    int32_t par_idx,
    int64_t value[1]) { return MSK12_get_liinf_ptr(task,par_idx,value); }
MSK12_get_dinf_func_t MSK12_get_dinf_ptr;
MSK12_ResCode MSK12_get_dinf(
    MSK12_Task_t task,
    int32_t par_idx,
    double value[1]) { return MSK12_get_dinf_ptr(task,par_idx,value); }
MSK12_get_iinf_name_func_t MSK12_get_iinf_name_ptr;
const char* MSK12_get_iinf_name(int32_t par_idx) { return MSK12_get_iinf_name_ptr(par_idx); }
MSK12_get_liinf_name_func_t MSK12_get_liinf_name_ptr;
const char* MSK12_get_liinf_name(int32_t par_idx) { return MSK12_get_liinf_name_ptr(par_idx); }
MSK12_get_dinf_name_func_t MSK12_get_dinf_name_ptr;
const char* MSK12_get_dinf_name(int32_t par_idx) { return MSK12_get_dinf_name_ptr(par_idx); }
MSK12_get_iinf_index_func_t MSK12_get_iinf_index_ptr;
int32_t MSK12_get_iinf_index(const char* par_name) { return MSK12_get_iinf_index_ptr(par_name); }
MSK12_get_liinf_index_func_t MSK12_get_liinf_index_ptr;
int32_t MSK12_get_liinf_index(const char* par_name) { return MSK12_get_liinf_index_ptr(par_name); }
MSK12_get_dinf_index_func_t MSK12_get_dinf_index_ptr;
int32_t MSK12_get_dinf_index(const char* name) { return MSK12_get_dinf_index_ptr(name); }
MSK12_get_double_param_func_t MSK12_get_double_param_ptr;
int32_t MSK12_get_double_param(
    MSK12_Task_t task,
    const char* par_name,
    double value[1]) { return MSK12_get_double_param_ptr(task,par_name,value); }
MSK12_get_double_param_index_func_t MSK12_get_double_param_index_ptr;
int32_t MSK12_get_double_param_index(const char* par_name) { return MSK12_get_double_param_index_ptr(par_name); }
MSK12_get_double_param_name_func_t MSK12_get_double_param_name_ptr;
const char* MSK12_get_double_param_name(int32_t par_idx) { return MSK12_get_double_param_name_ptr(par_idx); }
MSK12_get_num_double_param_func_t MSK12_get_num_double_param_ptr;
int32_t MSK12_get_num_double_param() { return MSK12_get_num_double_param_ptr(); }
MSK12_get_all_double_params_func_t MSK12_get_all_double_params_ptr;
void MSK12_get_all_double_params(
    MSK12_Task_t task,
    int32_t buflen,
    double* buf) { MSK12_get_all_double_params_ptr(task,buflen,buf); }
MSK12_put_all_double_params_func_t MSK12_put_all_double_params_ptr;
MSK12_ResCode MSK12_put_all_double_params(
    MSK12_Task_t task,
    int32_t num_par,
    const double* params) { return MSK12_put_all_double_params_ptr(task,num_par,params); }
MSK12_get_int_param_index_func_t MSK12_get_int_param_index_ptr;
int32_t MSK12_get_int_param_index(const char* par_name) { return MSK12_get_int_param_index_ptr(par_name); }
MSK12_get_int_param_name_func_t MSK12_get_int_param_name_ptr;
const char* MSK12_get_int_param_name(int32_t par_idx) { return MSK12_get_int_param_name_ptr(par_idx); }
MSK12_get_num_int_param_func_t MSK12_get_num_int_param_ptr;
int32_t MSK12_get_num_int_param() { return MSK12_get_num_int_param_ptr(); }
MSK12_get_all_int_params_func_t MSK12_get_all_int_params_ptr;
void MSK12_get_all_int_params(
    MSK12_Task_t task,
    int32_t buflen,
    int32_t* buf) { MSK12_get_all_int_params_ptr(task,buflen,buf); }
MSK12_put_all_int_params_func_t MSK12_put_all_int_params_ptr;
MSK12_ResCode MSK12_put_all_int_params(
    MSK12_Task_t task,
    int32_t num_par,
    const int32_t* params) { return MSK12_put_all_int_params_ptr(task,num_par,params); }
MSK12_get_int_param_func_t MSK12_get_int_param_ptr;
int32_t MSK12_get_int_param(
    MSK12_Task_t task,
    const char* par_name,
    int32_t value[1]) { return MSK12_get_int_param_ptr(task,par_name,value); }
MSK12_get_param_str_len_func_t MSK12_get_param_str_len_ptr;
int32_t MSK12_get_param_str_len(
    MSK12_Task_t task,
    const char* par_name) { return MSK12_get_param_str_len_ptr(task,par_name); }
MSK12_get_param_str_func_t MSK12_get_param_str_ptr;
void MSK12_get_param_str(
    MSK12_Task_t task,
    const char* name,
    int32_t length,
    char* buf) { MSK12_get_param_str_ptr(task,name,length,buf); }
MSK12_put_double_param_func_t MSK12_put_double_param_ptr;
MSK12_ResCode MSK12_put_double_param(
    MSK12_Task_t task,
    const char* par_name,
    double value,
    int ok[1]) { return MSK12_put_double_param_ptr(task,par_name,value,ok); }
MSK12_put_int_param_func_t MSK12_put_int_param_ptr;
MSK12_ResCode MSK12_put_int_param(
    MSK12_Task_t task,
    const char* par_name,
    int32_t value,
    int ok[1]) { return MSK12_put_int_param_ptr(task,par_name,value,ok); }
MSK12_put_param_str_func_t MSK12_put_param_str_ptr;
MSK12_ResCode MSK12_put_param_str(
    MSK12_Task_t task,
    const char* par_name,
    const char* value,
    int ok[1]) { return MSK12_put_param_str_ptr(task,par_name,value,ok); }
MSK12_get_task_name_len_func_t MSK12_get_task_name_len_ptr;
int32_t MSK12_get_task_name_len(MSK12_Task_t task) { return MSK12_get_task_name_len_ptr(task); }
MSK12_get_obj_name_len_func_t MSK12_get_obj_name_len_ptr;
int32_t MSK12_get_obj_name_len(MSK12_Task_t task) { return MSK12_get_obj_name_len_ptr(task); }
MSK12_get_task_name_func_t MSK12_get_task_name_ptr;
void MSK12_get_task_name(
    MSK12_Task_t task,
    int32_t capacity,
    char* buf) { return MSK12_get_task_name_ptr(task,capacity,buf); }
MSK12_get_obj_name_func_t MSK12_get_obj_name_ptr;
void MSK12_get_obj_name(
    MSK12_Task_t task,
    int32_t capacity,
    char* buf) { return MSK12_get_obj_name_ptr(task,capacity,buf); }
MSK12_put_task_name_func_t MSK12_put_task_name_ptr;
MSK12_ResCode MSK12_put_task_name(
    MSK12_Task_t task,
    const char* name) { return MSK12_put_task_name_ptr(task,name); }
MSK12_put_obj_name_func_t MSK12_put_obj_name_ptr;
MSK12_ResCode MSK12_put_obj_name(
    MSK12_Task_t task,
    const char* name) { return MSK12_put_obj_name_ptr(task,name); }
MSK12_get_var_name_len_func_t MSK12_get_var_name_len_ptr;
MSK12_ResCode MSK12_get_var_name_len(
    MSK12_Task_t task,
    int32_t var_idx,
    int32_t name_len[1]) { return MSK12_get_var_name_len_ptr(task,var_idx,name_len); }
MSK12_get_var_name_len2_func_t MSK12_get_var_name_len2_ptr;
int32_t MSK12_get_var_name_len2(
    MSK12_Task_t task,
    int32_t var_idx) { return MSK12_get_var_name_len2_ptr(task,var_idx); }
MSK12_get_barvar_name_len_func_t MSK12_get_barvar_name_len_ptr;
MSK12_ResCode MSK12_get_barvar_name_len(
    MSK12_Task_t task,
    int32_t barvar_idx,
    int32_t name_len[1]) { return MSK12_get_barvar_name_len_ptr(task,barvar_idx,name_len); }
MSK12_get_barvar_name_len2_func_t MSK12_get_barvar_name_len2_ptr;
int32_t MSK12_get_barvar_name_len2(
    MSK12_Task_t task,
    int32_t barvar_idx) { return MSK12_get_barvar_name_len2_ptr(task,barvar_idx); }
MSK12_get_var_name_func_t MSK12_get_var_name_ptr;
MSK12_ResCode MSK12_get_var_name(
    MSK12_Task_t task,
    int32_t var_idx,
    int32_t capacity,
    char* buf) { return MSK12_get_var_name_ptr(task,var_idx,capacity,buf); }
MSK12_get_barvar_name_func_t MSK12_get_barvar_name_ptr;
MSK12_ResCode MSK12_get_barvar_name(
    MSK12_Task_t task,
    int32_t barvar_idx,
    int32_t capacity,
    char* buf) { return MSK12_get_barvar_name_ptr(task,barvar_idx,capacity,buf); }
MSK12_put_var_name_func_t MSK12_put_var_name_ptr;
MSK12_ResCode MSK12_put_var_name(
    MSK12_Task_t task,
    int32_t var_idx,
    const char* name) { return MSK12_put_var_name_ptr(task,var_idx,name); }
MSK12_put_barvar_name_func_t MSK12_put_barvar_name_ptr;
MSK12_ResCode MSK12_put_barvar_name(
    MSK12_Task_t task,
    int32_t barvar_idx,
    const char* name) { return MSK12_put_barvar_name_ptr(task,barvar_idx,name); }
MSK12_get_con_name_len_func_t MSK12_get_con_name_len_ptr;
MSK12_ResCode MSK12_get_con_name_len(
    MSK12_Task_t task,
    int64_t con_idx,
    int32_t len[1]) { return MSK12_get_con_name_len_ptr(task,con_idx,len); }
MSK12_get_djc_name_len_func_t MSK12_get_djc_name_len_ptr;
MSK12_ResCode MSK12_get_djc_name_len(
    MSK12_Task_t task,
    int64_t djc_idx,
    int32_t len[1]) { return MSK12_get_djc_name_len_ptr(task,djc_idx,len); }
MSK12_get_con_name_len2_func_t MSK12_get_con_name_len2_ptr;
int32_t MSK12_get_con_name_len2(
    MSK12_Task_t task,
    int64_t con_idx) { return MSK12_get_con_name_len2_ptr(task,con_idx); }
MSK12_get_djc_name_len2_func_t MSK12_get_djc_name_len2_ptr;
int32_t MSK12_get_djc_name_len2(
    MSK12_Task_t task,
    int64_t djc_idx) { return MSK12_get_djc_name_len2_ptr(task,djc_idx); }
MSK12_get_con_name_func_t MSK12_get_con_name_ptr;
MSK12_ResCode MSK12_get_con_name(
    MSK12_Task_t task,
    int64_t con_idx,
    int32_t capacity,
    char* buf) { return MSK12_get_con_name_ptr(task,con_idx,capacity,buf); }
MSK12_get_djc_name_func_t MSK12_get_djc_name_ptr;
MSK12_ResCode MSK12_get_djc_name(
    MSK12_Task_t task,
    int64_t djc_idx,
    int32_t capacity,
    char* buf) { return MSK12_get_djc_name_ptr(task,djc_idx,capacity,buf); }
MSK12_put_con_name_func_t MSK12_put_con_name_ptr;
MSK12_ResCode MSK12_put_con_name(
    MSK12_Task_t task,
    int64_t con_idx,
    const char* buf) { return MSK12_put_con_name_ptr(task,con_idx,buf); }
MSK12_put_djc_name_func_t MSK12_put_djc_name_ptr;
MSK12_ResCode MSK12_put_djc_name(
    MSK12_Task_t task,
    int64_t djc_idx,
    const char* buf) { return MSK12_put_djc_name_ptr(task,djc_idx,buf); }
MSK12_write_task_to_file_func_t MSK12_write_task_to_file_ptr;
MSK12_ResCode MSK12_write_task_to_file(
    MSK12_Task_t task,
    const char* filename) { return MSK12_write_task_to_file_ptr(task,filename); }
MSK12_write_task_to_handle_func_t MSK12_write_task_to_handle_ptr;
MSK12_ResCode MSK12_write_task_to_handle(
    MSK12_Task_t task,
    MSK12_Format format,
    MSK12_Compression compress,
    MSK12_WriteHandle handle,
    MSK12_WriteFunc func) { return MSK12_write_task_to_handle_ptr(task,format,compress,handle,func); }
MSK12_write_solution_to_file_func_t MSK12_write_solution_to_file_ptr;
MSK12_ResCode MSK12_write_solution_to_file(
    MSK12_Task_t task,
    const char* filename) { return MSK12_write_solution_to_file_ptr(task,filename); }
MSK12_write_solution_to_handle_func_t MSK12_write_solution_to_handle_ptr;
MSK12_ResCode MSK12_write_solution_to_handle(
    MSK12_Task_t task,
    MSK12_SolutionFormat format,
    MSK12_Compression compress,
    MSK12_WriteHandle handle,
    MSK12_WriteFunc func) { return MSK12_write_solution_to_handle_ptr(task,format,compress,handle,func); }
MSK12_read_from_file_func_t MSK12_read_from_file_ptr;
MSK12_ResCode MSK12_read_from_file(
    MSK12_Task_t task,
    const char* filename) { return MSK12_read_from_file_ptr(task,filename); }
MSK12_read_from_handle_func_t MSK12_read_from_handle_ptr;
MSK12_ResCode MSK12_read_from_handle(
    MSK12_Task_t task,
    MSK12_Format format,
    MSK12_Compression compress,
    MSK12_ReadHandle handle,
    MSK12_ReadFunc func) { return MSK12_read_from_handle_ptr(task,format,compress,handle,func); }
MSK12_put_stream_file_func_t MSK12_put_stream_file_ptr;
MSK12_ResCode MSK12_put_stream_file(
    MSK12_Task_t task,
    MSK12_StreamType whichstream,
    const char* filename,
    int append) { return MSK12_put_stream_file_ptr(task,whichstream,filename,append); }
MSK12_clear_stream_file_func_t MSK12_clear_stream_file_ptr;
MSK12_ResCode MSK12_clear_stream_file(
    MSK12_Task_t task,
    MSK12_StreamType whichstream) { return MSK12_clear_stream_file_ptr(task,whichstream); }
MSK12_put_stream_callback_func_t MSK12_put_stream_callback_ptr;
MSK12_ResCode MSK12_put_stream_callback(
    MSK12_Task_t task,
    MSK12_StreamType whichstream,
    MSK12_WriteHandle handle,
    MSK12_StreamFunc func) { return MSK12_put_stream_callback_ptr(task,whichstream,handle,func); }
MSK12_clear_stream_callback_func_t MSK12_clear_stream_callback_ptr;
MSK12_ResCode MSK12_clear_stream_callback(
    MSK12_Task_t task,
    MSK12_StreamType whichstream) { return MSK12_clear_stream_callback_ptr(task,whichstream); }
MSK12_put_error_callback_func_t MSK12_put_error_callback_ptr;
MSK12_ResCode MSK12_put_error_callback(
    MSK12_Task_t task,
    MSK12_ErrorCallbackHandle handle,
    MSK12_ErrorCallbackFunc func) { return MSK12_put_error_callback_ptr(task,handle,func); }
MSK12_put_warning_callback_func_t MSK12_put_warning_callback_ptr;
MSK12_ResCode MSK12_put_warning_callback(
    MSK12_Task_t task,
    MSK12_ErrorCallbackHandle handle,
    MSK12_ErrorCallbackFunc func) { return MSK12_put_warning_callback_ptr(task,handle,func); }
MSK12_clear_error_callback_func_t MSK12_clear_error_callback_ptr;
MSK12_ResCode MSK12_clear_error_callback(MSK12_Task_t task) { return MSK12_clear_error_callback_ptr(task); }
MSK12_clear_warning_callback_func_t MSK12_clear_warning_callback_ptr;
MSK12_ResCode MSK12_clear_warning_callback(MSK12_Task_t task) { return MSK12_clear_warning_callback_ptr(task); }
MSK12_license_cleanup_func_t MSK12_license_cleanup_ptr;
void MSK12_license_cleanup() { MSK12_license_cleanup_ptr(); }
MSK12_shutdown_global_threadpool_func_t MSK12_shutdown_global_threadpool_ptr;
void MSK12_shutdown_global_threadpool() { MSK12_shutdown_global_threadpool_ptr(); }
MSK12_axpy_func_t MSK12_axpy_ptr;
MSK12_ResCode MSK12_axpy(
    int32_t n,
    double alpha,
    const double* x,
    double* y) { return MSK12_axpy_ptr(n,alpha,x,y); }
MSK12_dot_func_t MSK12_dot_ptr;
MSK12_ResCode MSK12_dot(
    int32_t n,
    const double* x,
    const double* y,
    double xty[1]) { return MSK12_dot_ptr(n,x,y,xty); }
MSK12_gemv_func_t MSK12_gemv_ptr;
MSK12_ResCode MSK12_gemv(
    int transa,
    int32_t m,
    int32_t n,
    double alpha,
    const double* a,
    const double* x,
    double beta,
    double* y) { return MSK12_gemv_ptr(transa,m,n,alpha,a,x,beta,y); }
MSK12_gemm_func_t MSK12_gemm_ptr;
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
    double* c) { return MSK12_gemm_ptr(transa,transb,m,n,k,alpha,a,b,beta,c); }
MSK12_syrk_func_t MSK12_syrk_ptr;
MSK12_ResCode MSK12_syrk(
    int is_upr,
    int trans,
    int32_t n,
    int32_t k,
    double alpha,
    const double* a,
    double beta,
    double* c) { return MSK12_syrk_ptr(is_upr,trans,n,k,alpha,a,beta,c); }
MSK12_sparse_triangular_solve_dense_func_t MSK12_sparse_triangular_solve_dense_ptr;
MSK12_ResCode MSK12_sparse_triangular_solve_dense(
    int transposed,
    int32_t n,
    const int32_t* lnzc,
    const int32_t* lsubc,
    const double* lvalc,
    double* b) { return MSK12_sparse_triangular_solve_dense_ptr(transposed,n,lnzc,lsubc,lvalc,b); }
MSK12_potrf_func_t MSK12_potrf_ptr;
MSK12_ResCode MSK12_potrf(
    int is_upr,
    int32_t n,
    double* a) { return MSK12_potrf_ptr(is_upr,n,a); }
MSK12_syeig_func_t MSK12_syeig_ptr;
MSK12_ResCode MSK12_syeig(
    int is_upr,
    int32_t n,
    const double* a,
    double* w) { return MSK12_syeig_ptr(is_upr,n,a,w); }
MSK12_syevd_func_t MSK12_syevd_ptr;
MSK12_ResCode MSK12_syevd(
    int is_upr,
    int32_t n,
    double* a,
    double* w) { return MSK12_syevd_ptr(is_upr,n,a,w); }
MSK12_compute_sparse_cholesky_func_t MSK12_compute_sparse_cholesky_ptr;
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
    double** l_val) { return MSK12_compute_sparse_cholesky_ptr(num_threads,order_method,tol_singular,n,a_col_num_nonzero,a_subi,a_val,perm,diag,alloc,alloc_handle,l_col_num_nonzero,l_subi,l_val); }
MSK12_optimize_batch_func_t MSK12_optimize_batch_ptr;
MSK12_ResCode MSK12_optimize_batch(
    int is_race,
    double max_time_sec,
    int32_t num_threads,
    int64_t num_task,
    const MSK12_Task_t* tasks,
    MSK12_TrmCode* trm_code,
    MSK12_ResCode* res_code) { return MSK12_optimize_batch_ptr(is_race,max_time_sec,num_threads,num_task,tasks,trm_code,res_code); }
MSK12_check_out_license_func_t MSK12_check_out_license_ptr;
MSK12_ResCode MSK12_check_out_license(MSK12_Feature feature) { return MSK12_check_out_license_ptr(feature); }
MSK12_check_in_license_func_t MSK12_check_in_license_ptr;
MSK12_ResCode MSK12_check_in_license(MSK12_Feature feature) { return MSK12_check_in_license_ptr(feature); }
MSK12_check_in_all_func_t MSK12_check_in_all_ptr;
MSK12_ResCode MSK12_check_in_all() { return MSK12_check_in_all_ptr(); }
MSK12_echo_intro_func_t MSK12_echo_intro_ptr;
MSK12_ResCode MSK12_echo_intro(int long_ver) { return MSK12_echo_intro_ptr(long_ver); }
MSK12_get_version_func_t MSK12_get_version_ptr;
void MSK12_get_version(
    int32_t major[1],
    int32_t minor[1],
    int32_t revision[1]) { MSK12_get_version_ptr(major,minor,revision); }
MSK12_put_license_debug_func_t MSK12_put_license_debug_ptr;
MSK12_ResCode MSK12_put_license_debug(int lic_debug) { return MSK12_put_license_debug_ptr(lic_debug); }
MSK12_put_license_code_func_t MSK12_put_license_code_ptr;
MSK12_ResCode MSK12_put_license_code(NULLABLE const int32_t code[21]) { return MSK12_put_license_code_ptr(code); }
MSK12_put_license_wait_func_t MSK12_put_license_wait_ptr;
MSK12_ResCode MSK12_put_license_wait(int32_t lic_wait) { return MSK12_put_license_wait_ptr(lic_wait); }
MSK12_put_license_path_func_t MSK12_put_license_path_ptr;
MSK12_ResCode MSK12_put_license_path(NULLABLE const char* license_path) { return MSK12_put_license_path_ptr(license_path); }


int MSK12_library_initialized() {
    return libmosek_handle != NULL;
}

// Return 0 to indicate success, otherwize non-zeo
int MSK12_initialize_library_with_paths(const char * paths[]) {
    const char * errmsg = NULL;
    char * buf = NULL;
    if (! libmosek_handle) {
        if (paths && paths[0]) {
            size_t libnamelen = strlen(libname);
            size_t maxpathlen = 0; for (int i = 0; paths[i]; ++i) {
                size_t n = strlen(paths[i]);
                maxpathlen = maxpathlen > n ? maxpathlen : n;
            }
            if (maxpathlen > 0) {
                buf = (char*)calloc(maxpathlen+libnamelen+2,1);
                for (int i = 0; paths[i] && ! libmosek_handle; ++i) {
                    size_t n = strlen(paths[i]);
                    memcpy(buf, paths[i], n);
                    buf[n] = PATH_SEP;
                    memcpy(buf+n+1,libname,libnamelen);
                    buf[n+libnamelen+1] = 0;

                    libmosek_handle = __dlopen(buf,&errmsg);
                }
            }
        }

        if (!libmosek_handle)
            libmosek_handle = __dlopen(libname,&errmsg);

        if (!libmosek_handle) goto EXIT_ERROR;

        if (NULL == (MSK12_get_callback_code_name_ptr = (MSK12_get_callback_code_name_func_t)__loadsym(libmosek_handle,"MSK12_get_callback_code_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_resp_name_ptr = (MSK12_get_resp_name_func_t)__loadsym(libmosek_handle,"MSK12_get_resp_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_resp_descr_ptr = (MSK12_get_resp_descr_func_t)__loadsym(libmosek_handle,"MSK12_get_resp_descr",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_last_resp_ptr = (MSK12_get_last_resp_func_t)__loadsym(libmosek_handle,"MSK12_get_last_resp",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_last_resp_msg_ptr = (MSK12_get_last_resp_msg_func_t)__loadsym(libmosek_handle,"MSK12_get_last_resp_msg",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_last_resp_msg_len_ptr = (MSK12_get_last_resp_msg_len_func_t)__loadsym(libmosek_handle,"MSK12_get_last_resp_msg_len",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_trm_name_ptr = (MSK12_get_trm_name_func_t)__loadsym(libmosek_handle,"MSK12_get_trm_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_trm_descr_ptr = (MSK12_get_trm_descr_func_t)__loadsym(libmosek_handle,"MSK12_get_trm_descr",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_new_task_from_task_ptr = (MSK12_new_task_from_task_func_t)__loadsym(libmosek_handle,"MSK12_new_task_from_task",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_new_task_ptr = (MSK12_new_task_func_t)__loadsym(libmosek_handle,"MSK12_new_task",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_delete_task_ptr = (MSK12_delete_task_func_t)__loadsym(libmosek_handle,"MSK12_delete_task",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_reserve_num_var_ptr = (MSK12_reserve_num_var_func_t)__loadsym(libmosek_handle,"MSK12_reserve_num_var",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_reserve_num_barvar_ptr = (MSK12_reserve_num_barvar_func_t)__loadsym(libmosek_handle,"MSK12_reserve_num_barvar",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_reserve_num_con_ptr = (MSK12_reserve_num_con_func_t)__loadsym(libmosek_handle,"MSK12_reserve_num_con",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_reserve_num_row_ptr = (MSK12_reserve_num_row_func_t)__loadsym(libmosek_handle,"MSK12_reserve_num_row",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_reserve_num_nz_ptr = (MSK12_reserve_num_nz_func_t)__loadsym(libmosek_handle,"MSK12_reserve_num_nz",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_reserve_num_dom_ptr = (MSK12_reserve_num_dom_func_t)__loadsym(libmosek_handle,"MSK12_reserve_num_dom",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_reserve_num_symmat_ptr = (MSK12_reserve_num_symmat_func_t)__loadsym(libmosek_handle,"MSK12_reserve_num_symmat",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_reserve_num_symmat_nz_ptr = (MSK12_reserve_num_symmat_nz_func_t)__loadsym(libmosek_handle,"MSK12_reserve_num_symmat_nz",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_num_var_ptr = (MSK12_get_num_var_func_t)__loadsym(libmosek_handle,"MSK12_get_num_var",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_num_barvar_ptr = (MSK12_get_num_barvar_func_t)__loadsym(libmosek_handle,"MSK12_get_num_barvar",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_num_domain_ptr = (MSK12_get_num_domain_func_t)__loadsym(libmosek_handle,"MSK12_get_num_domain",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_num_row_ptr = (MSK12_get_num_row_func_t)__loadsym(libmosek_handle,"MSK12_get_num_row",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_num_symmat_ptr = (MSK12_get_num_symmat_func_t)__loadsym(libmosek_handle,"MSK12_get_num_symmat",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_num_con_ptr = (MSK12_get_num_con_func_t)__loadsym(libmosek_handle,"MSK12_get_num_con",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_num_djc_ptr = (MSK12_get_num_djc_func_t)__loadsym(libmosek_handle,"MSK12_get_num_djc",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_append_vars_ptr = (MSK12_append_vars_func_t)__loadsym(libmosek_handle,"MSK12_append_vars",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_append_rows_ptr = (MSK12_append_rows_func_t)__loadsym(libmosek_handle,"MSK12_append_rows",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_append_barvar_ptr = (MSK12_append_barvar_func_t)__loadsym(libmosek_handle,"MSK12_append_barvar",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_append_barvars_ptr = (MSK12_append_barvars_func_t)__loadsym(libmosek_handle,"MSK12_append_barvars",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_append_symmat_ptr = (MSK12_append_symmat_func_t)__loadsym(libmosek_handle,"MSK12_append_symmat",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_append_symmats_ptr = (MSK12_append_symmats_func_t)__loadsym(libmosek_handle,"MSK12_append_symmats",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_append_empty_cons_ptr = (MSK12_append_empty_cons_func_t)__loadsym(libmosek_handle,"MSK12_append_empty_cons",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_append_empty_djcs_ptr = (MSK12_append_empty_djcs_func_t)__loadsym(libmosek_handle,"MSK12_append_empty_djcs",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_var_type_ptr = (MSK12_put_var_type_func_t)__loadsym(libmosek_handle,"MSK12_put_var_type",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_var_type_slice_ptr = (MSK12_put_var_type_slice_func_t)__loadsym(libmosek_handle,"MSK12_put_var_type_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_var_type_slice_value_ptr = (MSK12_put_var_type_slice_value_func_t)__loadsym(libmosek_handle,"MSK12_put_var_type_slice_value",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_var_type_list_ptr = (MSK12_put_var_type_list_func_t)__loadsym(libmosek_handle,"MSK12_put_var_type_list",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_var_type_ptr = (MSK12_get_var_type_func_t)__loadsym(libmosek_handle,"MSK12_get_var_type",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_var_type_slice_ptr = (MSK12_get_var_type_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_var_type_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_var_bound_ptr = (MSK12_put_var_bound_func_t)__loadsym(libmosek_handle,"MSK12_put_var_bound",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_var_bound_slice_ptr = (MSK12_put_var_bound_slice_func_t)__loadsym(libmosek_handle,"MSK12_put_var_bound_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_var_bound_slice_value_ptr = (MSK12_put_var_bound_slice_value_func_t)__loadsym(libmosek_handle,"MSK12_put_var_bound_slice_value",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_var_low_bound_ptr = (MSK12_put_var_low_bound_func_t)__loadsym(libmosek_handle,"MSK12_put_var_low_bound",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_var_low_bound_slice_ptr = (MSK12_put_var_low_bound_slice_func_t)__loadsym(libmosek_handle,"MSK12_put_var_low_bound_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_var_low_bound_slice_value_ptr = (MSK12_put_var_low_bound_slice_value_func_t)__loadsym(libmosek_handle,"MSK12_put_var_low_bound_slice_value",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_var_upr_bound_ptr = (MSK12_put_var_upr_bound_func_t)__loadsym(libmosek_handle,"MSK12_put_var_upr_bound",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_var_upr_bound_slice_ptr = (MSK12_put_var_upr_bound_slice_func_t)__loadsym(libmosek_handle,"MSK12_put_var_upr_bound_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_var_upr_bound_slice_value_ptr = (MSK12_put_var_upr_bound_slice_value_func_t)__loadsym(libmosek_handle,"MSK12_put_var_upr_bound_slice_value",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_var_bound_ptr = (MSK12_get_var_bound_func_t)__loadsym(libmosek_handle,"MSK12_get_var_bound",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_var_bound_slice_ptr = (MSK12_get_var_bound_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_var_bound_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_barvar_slice_num_elm_ptr = (MSK12_get_barvar_slice_num_elm_func_t)__loadsym(libmosek_handle,"MSK12_get_barvar_slice_num_elm",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_barvar_dim_ptr = (MSK12_get_barvar_dim_func_t)__loadsym(libmosek_handle,"MSK12_get_barvar_dim",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_barvar_slice_dims_ptr = (MSK12_get_barvar_slice_dims_func_t)__loadsym(libmosek_handle,"MSK12_get_barvar_slice_dims",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_ptr = (MSK12_get_domain_func_t)__loadsym(libmosek_handle,"MSK12_get_domain",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_empty_ptr = (MSK12_get_domain_empty_func_t)__loadsym(libmosek_handle,"MSK12_get_domain_empty",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_rzero_ptr = (MSK12_get_domain_rzero_func_t)__loadsym(libmosek_handle,"MSK12_get_domain_rzero",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_rplus_ptr = (MSK12_get_domain_rplus_func_t)__loadsym(libmosek_handle,"MSK12_get_domain_rplus",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_rminus_ptr = (MSK12_get_domain_rminus_func_t)__loadsym(libmosek_handle,"MSK12_get_domain_rminus",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_r_ptr = (MSK12_get_domain_r_func_t)__loadsym(libmosek_handle,"MSK12_get_domain_r",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_quadratic_cone_ptr = (MSK12_get_domain_quadratic_cone_func_t)__loadsym(libmosek_handle,"MSK12_get_domain_quadratic_cone",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_rotated_quadratic_cone_ptr = (MSK12_get_domain_rotated_quadratic_cone_func_t)__loadsym(libmosek_handle,"MSK12_get_domain_rotated_quadratic_cone",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_primal_exponential_cone_ptr = (MSK12_get_domain_primal_exponential_cone_func_t)__loadsym(libmosek_handle,"MSK12_get_domain_primal_exponential_cone",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_dual_exponential_cone_ptr = (MSK12_get_domain_dual_exponential_cone_func_t)__loadsym(libmosek_handle,"MSK12_get_domain_dual_exponential_cone",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_primal_power_cone_ptr = (MSK12_get_domain_primal_power_cone_func_t)__loadsym(libmosek_handle,"MSK12_get_domain_primal_power_cone",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_dual_power_cone_ptr = (MSK12_get_domain_dual_power_cone_func_t)__loadsym(libmosek_handle,"MSK12_get_domain_dual_power_cone",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_primal_geometric_mean_cone_ptr = (MSK12_get_domain_primal_geometric_mean_cone_func_t)__loadsym(libmosek_handle,"MSK12_get_domain_primal_geometric_mean_cone",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_dual_geometric_mean_cone_ptr = (MSK12_get_domain_dual_geometric_mean_cone_func_t)__loadsym(libmosek_handle,"MSK12_get_domain_dual_geometric_mean_cone",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_svecpsd_cone_ptr = (MSK12_get_domain_svecpsd_cone_func_t)__loadsym(libmosek_handle,"MSK12_get_domain_svecpsd_cone",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_info_ptr = (MSK12_get_domain_info_func_t)__loadsym(libmosek_handle,"MSK12_get_domain_info",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_domain_alpha_ptr = (MSK12_get_domain_alpha_func_t)__loadsym(libmosek_handle,"MSK12_get_domain_alpha",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_row_ptr = (MSK12_put_row_func_t)__loadsym(libmosek_handle,"MSK12_put_row",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_row_slice_ptr = (MSK12_put_row_slice_func_t)__loadsym(libmosek_handle,"MSK12_put_row_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_row_list_ptr = (MSK12_put_row_list_func_t)__loadsym(libmosek_handle,"MSK12_put_row_list",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_row_g_ptr = (MSK12_put_row_g_func_t)__loadsym(libmosek_handle,"MSK12_put_row_g",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_row_slice_g_ptr = (MSK12_put_row_slice_g_func_t)__loadsym(libmosek_handle,"MSK12_put_row_slice_g",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_row_list_g_ptr = (MSK12_put_row_list_g_func_t)__loadsym(libmosek_handle,"MSK12_put_row_list_g",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_col_ptr = (MSK12_put_col_func_t)__loadsym(libmosek_handle,"MSK12_put_col",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_col_slice_ptr = (MSK12_put_col_slice_func_t)__loadsym(libmosek_handle,"MSK12_put_col_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_col_list_ptr = (MSK12_put_col_list_func_t)__loadsym(libmosek_handle,"MSK12_put_col_list",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_ijc_ptr = (MSK12_put_ijc_func_t)__loadsym(libmosek_handle,"MSK12_put_ijc",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_ijc_list_ptr = (MSK12_put_ijc_list_func_t)__loadsym(libmosek_handle,"MSK12_put_ijc_list",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_row_num_nz_ptr = (MSK12_get_row_num_nz_func_t)__loadsym(libmosek_handle,"MSK12_get_row_num_nz",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_row_slice_num_nz_ptr = (MSK12_get_row_slice_num_nz_func_t)__loadsym(libmosek_handle,"MSK12_get_row_slice_num_nz",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_row_ptr = (MSK12_get_row_func_t)__loadsym(libmosek_handle,"MSK12_get_row",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_row_slice_ptr = (MSK12_get_row_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_row_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_col_num_nz_ptr = (MSK12_get_col_num_nz_func_t)__loadsym(libmosek_handle,"MSK12_get_col_num_nz",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_col_slice_num_nz_ptr = (MSK12_get_col_slice_num_nz_func_t)__loadsym(libmosek_handle,"MSK12_get_col_slice_num_nz",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_col_ptr = (MSK12_get_col_func_t)__loadsym(libmosek_handle,"MSK12_get_col",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_col_slice_ptr = (MSK12_get_col_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_col_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_bar_entry_ptr = (MSK12_put_bar_entry_func_t)__loadsym(libmosek_handle,"MSK12_put_bar_entry",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_bar_entry_list_ptr = (MSK12_put_bar_entry_list_func_t)__loadsym(libmosek_handle,"MSK12_put_bar_entry_list",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_bar_row_ptr = (MSK12_put_bar_row_func_t)__loadsym(libmosek_handle,"MSK12_put_bar_row",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_symmat_info_ptr = (MSK12_get_symmat_info_func_t)__loadsym(libmosek_handle,"MSK12_get_symmat_info",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_symmat_ptr = (MSK12_get_symmat_func_t)__loadsym(libmosek_handle,"MSK12_get_symmat",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_symmat_slice_info_ptr = (MSK12_get_symmat_slice_info_func_t)__loadsym(libmosek_handle,"MSK12_get_symmat_slice_info",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_symmat_slice_ptr = (MSK12_get_symmat_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_symmat_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_append_con_ptr = (MSK12_append_con_func_t)__loadsym(libmosek_handle,"MSK12_append_con",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_append_cons_ptr = (MSK12_append_cons_func_t)__loadsym(libmosek_handle,"MSK12_append_cons",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_append_cons_seq_ptr = (MSK12_append_cons_seq_func_t)__loadsym(libmosek_handle,"MSK12_append_cons_seq",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_con_ptr = (MSK12_put_con_func_t)__loadsym(libmosek_handle,"MSK12_put_con",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_scalar_con_ptr = (MSK12_put_scalar_con_func_t)__loadsym(libmosek_handle,"MSK12_put_scalar_con",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_con_slice_ptr = (MSK12_put_con_slice_func_t)__loadsym(libmosek_handle,"MSK12_put_con_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_con_slice_domains_ptr = (MSK12_get_con_slice_domains_func_t)__loadsym(libmosek_handle,"MSK12_get_con_slice_domains",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_con_slice_num_row_ptr = (MSK12_get_con_slice_num_row_func_t)__loadsym(libmosek_handle,"MSK12_get_con_slice_num_row",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_con_slice_ptr = (MSK12_get_con_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_con_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_append_djc_ptr = (MSK12_append_djc_func_t)__loadsym(libmosek_handle,"MSK12_append_djc",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_djc_ptr = (MSK12_put_djc_func_t)__loadsym(libmosek_handle,"MSK12_put_djc",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_djc_slice_ptr = (MSK12_put_djc_slice_func_t)__loadsym(libmosek_handle,"MSK12_put_djc_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_djc_info_ptr = (MSK12_get_djc_info_func_t)__loadsym(libmosek_handle,"MSK12_get_djc_info",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_djc_ptr = (MSK12_get_djc_func_t)__loadsym(libmosek_handle,"MSK12_get_djc",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_djc_slice_info_ptr = (MSK12_get_djc_slice_info_func_t)__loadsym(libmosek_handle,"MSK12_get_djc_slice_info",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_djc_slice_ptr = (MSK12_get_djc_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_djc_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_obj_sense_ptr = (MSK12_put_obj_sense_func_t)__loadsym(libmosek_handle,"MSK12_put_obj_sense",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_obj_sense_ptr = (MSK12_get_obj_sense_func_t)__loadsym(libmosek_handle,"MSK12_get_obj_sense",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_obj_row_ptr = (MSK12_put_obj_row_func_t)__loadsym(libmosek_handle,"MSK12_put_obj_row",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_obj_row_ptr = (MSK12_get_obj_row_func_t)__loadsym(libmosek_handle,"MSK12_get_obj_row",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_optimize_ptr = (MSK12_optimize_func_t)__loadsym(libmosek_handle,"MSK12_optimize",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_solution_summary_ptr = (MSK12_solution_summary_func_t)__loadsym(libmosek_handle,"MSK12_solution_summary",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_optimize_callback_ptr = (MSK12_optimize_callback_func_t)__loadsym(libmosek_handle,"MSK12_optimize_callback",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_remote_solver_ptr = (MSK12_put_remote_solver_func_t)__loadsym(libmosek_handle,"MSK12_put_remote_solver",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_optserver_access_token_ptr = (MSK12_put_optserver_access_token_func_t)__loadsym(libmosek_handle,"MSK12_put_optserver_access_token",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_num_sol_ptr = (MSK12_get_num_sol_func_t)__loadsym(libmosek_handle,"MSK12_get_num_sol",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_type_ptr = (MSK12_get_sol_type_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_type",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_status_ptr = (MSK12_get_sol_status_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_status",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_problem_status_ptr = (MSK12_get_problem_status_func_t)__loadsym(libmosek_handle,"MSK12_get_problem_status",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_primal_obj_ptr = (MSK12_get_primal_obj_func_t)__loadsym(libmosek_handle,"MSK12_get_primal_obj",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_dual_obj_ptr = (MSK12_get_dual_obj_func_t)__loadsym(libmosek_handle,"MSK12_get_dual_obj",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_xx_slice_ptr = (MSK12_get_sol_xx_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_xx_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_slx_slice_ptr = (MSK12_get_sol_slx_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_slx_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_sux_slice_ptr = (MSK12_get_sol_sux_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_sux_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_barxj_ptr = (MSK12_get_sol_barxj_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_barxj",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_barsj_ptr = (MSK12_get_sol_barsj_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_barsj",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_barx_slice_ptr = (MSK12_get_sol_barx_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_barx_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_bars_slice_ptr = (MSK12_get_sol_bars_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_bars_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_basic_xj_ptr = (MSK12_get_sol_basic_xj_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_basic_xj",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_basic_barx_ptr = (MSK12_get_sol_basic_barx_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_basic_barx",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_basic_con_ptr = (MSK12_get_sol_basic_con_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_basic_con",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_sta_var_ptr = (MSK12_get_sol_sta_var_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_sta_var",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_sta_barx_ptr = (MSK12_get_sol_sta_barx_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_sta_barx",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_sta_con_ptr = (MSK12_get_sol_sta_con_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_sta_con",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_basic_x_slice_ptr = (MSK12_get_sol_basic_x_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_basic_x_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_basic_barx_slice_ptr = (MSK12_get_sol_basic_barx_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_basic_barx_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_basic_con_slice_ptr = (MSK12_get_sol_basic_con_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_basic_con_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_sta_var_slice_ptr = (MSK12_get_sol_sta_var_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_sta_var_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_sta_barx_slice_ptr = (MSK12_get_sol_sta_barx_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_sta_barx_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_sta_con_slice_ptr = (MSK12_get_sol_sta_con_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_sta_con_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_xc_slice_ptr = (MSK12_get_sol_xc_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_xc_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_sol_y_slice_ptr = (MSK12_get_sol_y_slice_func_t)__loadsym(libmosek_handle,"MSK12_get_sol_y_slice",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_num_input_solutions_ptr = (MSK12_get_num_input_solutions_func_t)__loadsym(libmosek_handle,"MSK12_get_num_input_solutions",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_copy_sol_to_input_ptr = (MSK12_copy_sol_to_input_func_t)__loadsym(libmosek_handle,"MSK12_copy_sol_to_input",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_append_sol_ptr = (MSK12_append_sol_func_t)__loadsym(libmosek_handle,"MSK12_append_sol",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_sol_xx_ptr = (MSK12_put_sol_xx_func_t)__loadsym(libmosek_handle,"MSK12_put_sol_xx",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_sol_slx_ptr = (MSK12_put_sol_slx_func_t)__loadsym(libmosek_handle,"MSK12_put_sol_slx",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_sol_sux_ptr = (MSK12_put_sol_sux_func_t)__loadsym(libmosek_handle,"MSK12_put_sol_sux",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_sol_basic_x_ptr = (MSK12_put_sol_basic_x_func_t)__loadsym(libmosek_handle,"MSK12_put_sol_basic_x",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_sol_barx_ptr = (MSK12_put_sol_barx_func_t)__loadsym(libmosek_handle,"MSK12_put_sol_barx",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_sol_bars_ptr = (MSK12_put_sol_bars_func_t)__loadsym(libmosek_handle,"MSK12_put_sol_bars",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_sol_yi_ptr = (MSK12_put_sol_yi_func_t)__loadsym(libmosek_handle,"MSK12_put_sol_yi",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_sol_basic_c_ptr = (MSK12_put_sol_basic_c_func_t)__loadsym(libmosek_handle,"MSK12_put_sol_basic_c",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_num_iinf_ptr = (MSK12_get_num_iinf_func_t)__loadsym(libmosek_handle,"MSK12_get_num_iinf",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_num_liinf_ptr = (MSK12_get_num_liinf_func_t)__loadsym(libmosek_handle,"MSK12_get_num_liinf",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_num_dinf_ptr = (MSK12_get_num_dinf_func_t)__loadsym(libmosek_handle,"MSK12_get_num_dinf",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_iinf_ptr = (MSK12_get_iinf_func_t)__loadsym(libmosek_handle,"MSK12_get_iinf",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_liinf_ptr = (MSK12_get_liinf_func_t)__loadsym(libmosek_handle,"MSK12_get_liinf",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_dinf_ptr = (MSK12_get_dinf_func_t)__loadsym(libmosek_handle,"MSK12_get_dinf",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_iinf_name_ptr = (MSK12_get_iinf_name_func_t)__loadsym(libmosek_handle,"MSK12_get_iinf_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_liinf_name_ptr = (MSK12_get_liinf_name_func_t)__loadsym(libmosek_handle,"MSK12_get_liinf_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_dinf_name_ptr = (MSK12_get_dinf_name_func_t)__loadsym(libmosek_handle,"MSK12_get_dinf_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_iinf_index_ptr = (MSK12_get_iinf_index_func_t)__loadsym(libmosek_handle,"MSK12_get_iinf_index",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_liinf_index_ptr = (MSK12_get_liinf_index_func_t)__loadsym(libmosek_handle,"MSK12_get_liinf_index",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_dinf_index_ptr = (MSK12_get_dinf_index_func_t)__loadsym(libmosek_handle,"MSK12_get_dinf_index",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_double_param_ptr = (MSK12_get_double_param_func_t)__loadsym(libmosek_handle,"MSK12_get_double_param",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_double_param_index_ptr = (MSK12_get_double_param_index_func_t)__loadsym(libmosek_handle,"MSK12_get_double_param_index",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_double_param_name_ptr = (MSK12_get_double_param_name_func_t)__loadsym(libmosek_handle,"MSK12_get_double_param_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_num_double_param_ptr = (MSK12_get_num_double_param_func_t)__loadsym(libmosek_handle,"MSK12_get_num_double_param",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_all_double_params_ptr = (MSK12_get_all_double_params_func_t)__loadsym(libmosek_handle,"MSK12_get_all_double_params",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_all_double_params_ptr = (MSK12_put_all_double_params_func_t)__loadsym(libmosek_handle,"MSK12_put_all_double_params",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_int_param_index_ptr = (MSK12_get_int_param_index_func_t)__loadsym(libmosek_handle,"MSK12_get_int_param_index",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_int_param_name_ptr = (MSK12_get_int_param_name_func_t)__loadsym(libmosek_handle,"MSK12_get_int_param_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_num_int_param_ptr = (MSK12_get_num_int_param_func_t)__loadsym(libmosek_handle,"MSK12_get_num_int_param",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_all_int_params_ptr = (MSK12_get_all_int_params_func_t)__loadsym(libmosek_handle,"MSK12_get_all_int_params",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_all_int_params_ptr = (MSK12_put_all_int_params_func_t)__loadsym(libmosek_handle,"MSK12_put_all_int_params",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_int_param_ptr = (MSK12_get_int_param_func_t)__loadsym(libmosek_handle,"MSK12_get_int_param",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_param_str_len_ptr = (MSK12_get_param_str_len_func_t)__loadsym(libmosek_handle,"MSK12_get_param_str_len",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_param_str_ptr = (MSK12_get_param_str_func_t)__loadsym(libmosek_handle,"MSK12_get_param_str",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_double_param_ptr = (MSK12_put_double_param_func_t)__loadsym(libmosek_handle,"MSK12_put_double_param",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_int_param_ptr = (MSK12_put_int_param_func_t)__loadsym(libmosek_handle,"MSK12_put_int_param",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_param_str_ptr = (MSK12_put_param_str_func_t)__loadsym(libmosek_handle,"MSK12_put_param_str",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_task_name_len_ptr = (MSK12_get_task_name_len_func_t)__loadsym(libmosek_handle,"MSK12_get_task_name_len",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_obj_name_len_ptr = (MSK12_get_obj_name_len_func_t)__loadsym(libmosek_handle,"MSK12_get_obj_name_len",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_task_name_ptr = (MSK12_get_task_name_func_t)__loadsym(libmosek_handle,"MSK12_get_task_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_obj_name_ptr = (MSK12_get_obj_name_func_t)__loadsym(libmosek_handle,"MSK12_get_obj_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_task_name_ptr = (MSK12_put_task_name_func_t)__loadsym(libmosek_handle,"MSK12_put_task_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_obj_name_ptr = (MSK12_put_obj_name_func_t)__loadsym(libmosek_handle,"MSK12_put_obj_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_var_name_len_ptr = (MSK12_get_var_name_len_func_t)__loadsym(libmosek_handle,"MSK12_get_var_name_len",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_var_name_len2_ptr = (MSK12_get_var_name_len2_func_t)__loadsym(libmosek_handle,"MSK12_get_var_name_len2",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_barvar_name_len_ptr = (MSK12_get_barvar_name_len_func_t)__loadsym(libmosek_handle,"MSK12_get_barvar_name_len",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_barvar_name_len2_ptr = (MSK12_get_barvar_name_len2_func_t)__loadsym(libmosek_handle,"MSK12_get_barvar_name_len2",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_var_name_ptr = (MSK12_get_var_name_func_t)__loadsym(libmosek_handle,"MSK12_get_var_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_barvar_name_ptr = (MSK12_get_barvar_name_func_t)__loadsym(libmosek_handle,"MSK12_get_barvar_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_var_name_ptr = (MSK12_put_var_name_func_t)__loadsym(libmosek_handle,"MSK12_put_var_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_barvar_name_ptr = (MSK12_put_barvar_name_func_t)__loadsym(libmosek_handle,"MSK12_put_barvar_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_con_name_len_ptr = (MSK12_get_con_name_len_func_t)__loadsym(libmosek_handle,"MSK12_get_con_name_len",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_djc_name_len_ptr = (MSK12_get_djc_name_len_func_t)__loadsym(libmosek_handle,"MSK12_get_djc_name_len",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_con_name_len2_ptr = (MSK12_get_con_name_len2_func_t)__loadsym(libmosek_handle,"MSK12_get_con_name_len2",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_djc_name_len2_ptr = (MSK12_get_djc_name_len2_func_t)__loadsym(libmosek_handle,"MSK12_get_djc_name_len2",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_con_name_ptr = (MSK12_get_con_name_func_t)__loadsym(libmosek_handle,"MSK12_get_con_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_djc_name_ptr = (MSK12_get_djc_name_func_t)__loadsym(libmosek_handle,"MSK12_get_djc_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_con_name_ptr = (MSK12_put_con_name_func_t)__loadsym(libmosek_handle,"MSK12_put_con_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_djc_name_ptr = (MSK12_put_djc_name_func_t)__loadsym(libmosek_handle,"MSK12_put_djc_name",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_write_task_to_file_ptr = (MSK12_write_task_to_file_func_t)__loadsym(libmosek_handle,"MSK12_write_task_to_file",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_write_task_to_handle_ptr = (MSK12_write_task_to_handle_func_t)__loadsym(libmosek_handle,"MSK12_write_task_to_handle",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_write_solution_to_file_ptr = (MSK12_write_solution_to_file_func_t)__loadsym(libmosek_handle,"MSK12_write_solution_to_file",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_write_solution_to_handle_ptr = (MSK12_write_solution_to_handle_func_t)__loadsym(libmosek_handle,"MSK12_write_solution_to_handle",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_read_from_file_ptr = (MSK12_read_from_file_func_t)__loadsym(libmosek_handle,"MSK12_read_from_file",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_read_from_handle_ptr = (MSK12_read_from_handle_func_t)__loadsym(libmosek_handle,"MSK12_read_from_handle",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_stream_file_ptr = (MSK12_put_stream_file_func_t)__loadsym(libmosek_handle,"MSK12_put_stream_file",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_clear_stream_file_ptr = (MSK12_clear_stream_file_func_t)__loadsym(libmosek_handle,"MSK12_clear_stream_file",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_stream_callback_ptr = (MSK12_put_stream_callback_func_t)__loadsym(libmosek_handle,"MSK12_put_stream_callback",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_clear_stream_callback_ptr = (MSK12_clear_stream_callback_func_t)__loadsym(libmosek_handle,"MSK12_clear_stream_callback",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_error_callback_ptr = (MSK12_put_error_callback_func_t)__loadsym(libmosek_handle,"MSK12_put_error_callback",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_warning_callback_ptr = (MSK12_put_warning_callback_func_t)__loadsym(libmosek_handle,"MSK12_put_warning_callback",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_clear_error_callback_ptr = (MSK12_clear_error_callback_func_t)__loadsym(libmosek_handle,"MSK12_clear_error_callback",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_clear_warning_callback_ptr = (MSK12_clear_warning_callback_func_t)__loadsym(libmosek_handle,"MSK12_clear_warning_callback",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_license_cleanup_ptr = (MSK12_license_cleanup_func_t)__loadsym(libmosek_handle,"MSK12_license_cleanup",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_shutdown_global_threadpool_ptr = (MSK12_shutdown_global_threadpool_func_t)__loadsym(libmosek_handle,"MSK12_shutdown_global_threadpool",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_axpy_ptr = (MSK12_axpy_func_t)__loadsym(libmosek_handle,"MSK12_axpy",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_dot_ptr = (MSK12_dot_func_t)__loadsym(libmosek_handle,"MSK12_dot",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_gemv_ptr = (MSK12_gemv_func_t)__loadsym(libmosek_handle,"MSK12_gemv",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_gemm_ptr = (MSK12_gemm_func_t)__loadsym(libmosek_handle,"MSK12_gemm",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_syrk_ptr = (MSK12_syrk_func_t)__loadsym(libmosek_handle,"MSK12_syrk",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_sparse_triangular_solve_dense_ptr = (MSK12_sparse_triangular_solve_dense_func_t)__loadsym(libmosek_handle,"MSK12_sparse_triangular_solve_dense",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_potrf_ptr = (MSK12_potrf_func_t)__loadsym(libmosek_handle,"MSK12_potrf",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_syeig_ptr = (MSK12_syeig_func_t)__loadsym(libmosek_handle,"MSK12_syeig",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_syevd_ptr = (MSK12_syevd_func_t)__loadsym(libmosek_handle,"MSK12_syevd",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_compute_sparse_cholesky_ptr = (MSK12_compute_sparse_cholesky_func_t)__loadsym(libmosek_handle,"MSK12_compute_sparse_cholesky",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_optimize_batch_ptr = (MSK12_optimize_batch_func_t)__loadsym(libmosek_handle,"MSK12_optimize_batch",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_check_out_license_ptr = (MSK12_check_out_license_func_t)__loadsym(libmosek_handle,"MSK12_check_out_license",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_check_in_license_ptr = (MSK12_check_in_license_func_t)__loadsym(libmosek_handle,"MSK12_check_in_license",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_check_in_all_ptr = (MSK12_check_in_all_func_t)__loadsym(libmosek_handle,"MSK12_check_in_all",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_echo_intro_ptr = (MSK12_echo_intro_func_t)__loadsym(libmosek_handle,"MSK12_echo_intro",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_get_version_ptr = (MSK12_get_version_func_t)__loadsym(libmosek_handle,"MSK12_get_version",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_license_debug_ptr = (MSK12_put_license_debug_func_t)__loadsym(libmosek_handle,"MSK12_put_license_debug",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_license_code_ptr = (MSK12_put_license_code_func_t)__loadsym(libmosek_handle,"MSK12_put_license_code",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_license_wait_ptr = (MSK12_put_license_wait_func_t)__loadsym(libmosek_handle,"MSK12_put_license_wait",&errmsg))) goto EXIT_ERROR;
if (NULL == (MSK12_put_license_path_ptr = (MSK12_put_license_path_func_t)__loadsym(libmosek_handle,"MSK12_put_license_path",&errmsg))) goto EXIT_ERROR;

    }
    goto EXIT_OK;
EXIT_ERROR:
    if (libmosek_handle) {
        __dlclose(libmosek_handle);
        libmosek_handle = NULL;
    }
    if (buf) free(buf);
    return 1;
EXIT_OK:
    if (buf) free(buf);
    return 0;
}

int MSK12_initialize_library() {
    return MSK12_initialize_library_with_paths(NULL);
}
