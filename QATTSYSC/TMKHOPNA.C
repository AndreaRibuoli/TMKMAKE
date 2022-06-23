/* ================================================================= */
/*                                                                   */
/*  MODULE:    TMKHOPNA                                              */
/*                                                                   */
/*                                                                   */
/*  MODULE-TYPE: C Library                                           */
/*                                                                   */
/*  Processor:  C                                                    */
/*                                                                   */
/*  Purpose:    Makefile processing functions - header file          */
/*                                                                   */
/* ================================================================= */

#ifndef _TMKHOPNA_H
#define _TMKHOPNA_H

#ifdef __ILEC400__
   #include <except.h>
#endif

/*********************************************************************/
/*  PROTOTYPES FOR CALLING OPENNESS MODULES                          */
/*********************************************************************/

#pragma linkage (QUSCRTUS, OS)
void QUSCRTUS(char *, char *, int, char *, char *, char *, ...);

#pragma linkage (QUSRTVUS, OS)
void QUSRTVUS(char *, int, int, char *, ...);

#pragma linkage (QUSCHGUS, OS)
void QUSCHGUS(char *, int, int, char *, char *, ...);

#pragma linkage (QUSPTRUS, OS)
void QUSPTRUS(char *, void *, ...);

#pragma linkage (QUSCRTUQ, OS)
void QUSCRTUQ(char *, char *, char *, int, int,
              int, int, char *, char *, ...);

#pragma linkage (QUSCRTUI, OS)
void QUSCRTUI(char *, char *, char *, int, char *, int,
              char *, char *, char *, char *, ...);

#pragma linkage (QUSLOBJ, OS)
void QUSLOBJ(char *, char *, char *, char *, ...);

#pragma linkage (QUSROBJD, OS)
void QUSROBJD(char *, int, char *, char *, char *, ...);

#pragma linkage (QUSLMBR, OS)
void QUSLMBR(char *, char *, char *, char *, char *, ...);

#pragma linkage (QUSRMBRD, OS)
void QUSRMBRD(char *, int, char *, char *, char *, char *, ...);

#pragma linkage (QUSLRCD, OS)
void QUSLRCD(char *, char *, char *, char *, ...);

#pragma linkage (QUSLFLD, OS)
void QUSLFLD(char *, char *, char *, char *, char *, ...);

#pragma linkage (QUSLSPL, OS)
void QUSLSPL(char *, char *, char *, char *, char *, char *, ...);

#pragma linkage (QUSRSPLA, OS)
void QUSRSPLA(char *, int, char *, char *, char *, char *,
              char *, int, ...);

#pragma linkage (QUSLJOB, OS)
void QUSLJOB(char *, char *, char *, char *, ...);

#pragma linkage (QUSRJOBI, OS)
void QUSRJOBI(char *, int, char *, char *, char *, ...);

#pragma linkage (QUSCHGPA, OS)
void QUSCHGPA(int, int, int, ...);

#pragma linkage (QPRCRTPG, OS)
void QPRCRTPG(char *, int, char *, char *, char *, char *,
              char *, char *, int, char *, char *, int, ...);

#pragma linkage (QUSDLTUI, OS)
void QUSDLTUI(char *, void *);

#pragma linkage (QUSDLTUQ, OS)
void QUSDLTUQ(char *, void *);

#pragma linkage (QUSDLTUS, OS)
void QUSDLTUS(char *, void *);

#pragma linkage (QMHSNDPM, OS)
void QMHSNDPM(char *, char *, char *, int, char *, char*, int,
              char *, char *);

#define MSGT_COMPLETE                 "*COMP     "
#define MSGT_DIAG                     "*DIAG     "
#define MSGT_ESCAPE                   "*ESCAPE   "
#define MSGT_INFO                     "*INFO     "
#define MSGT_INQ                      "*INQ      "
#define MSGT_NOTIFY                   "*NOTIFY   "
#define MSGT_RQS                      "*RQS      "
#define MSGT_STATUS                   "*STATUS   "

typedef struct error_rtn {
     int  rtn_area_sz;
     int  exception_area_sz;
     char exception_id[7];
     char reserve;
     char exception_data[1];
} error_rtn;

#ifdef __ILEC400__
  #pragma linkage (QMHCHGEM, OS)
  void QMHCHGEM ( _INVPTR *, int, unsigned int, char *,
                 char *, int, error_rtn * );
#endif

#define MOD_HANDLE               "*HANDLE   "
#define MOD_RMVLOG               "*REMOVE   "
#define MOD_REPLY                "*REPLY    "

/********************************************************************/
/*  Generic header returned in userspaces following a call to       */
/*  a "LIST" module.                                                */
/********************************************************************/

typedef struct header_struct {
     char user_data[64];
     int  generic_header_size;
     char header_version[4];
     char format_name[8];
     char program_name[10];
     char time_generated[13];
     char information_status;
     int  usrspc_used;
     int  parm_section_offset;
     int  parm_section_size;
     int  header_section_offset;
     int  header_section_size;
     int  list_section_offset;
     int  list_section_size;
     int  number_of_entries;
     int  size_of_entry;
} header_struct;

/********************************************************************/
/*  Structures returned by the QUSROBJD API                         */
/********************************************************************/

typedef struct D0100FMT {
                 int bytes_returned;
                 int bytes_available;
                 char object_name[10];
                 char object_library[10];
                 char object_type[10];
                 char return_library[10];
                 int asp_number;
                 char object_owner[10];
                 char object_domain[2];
                 char creation_date[13];
                 char change_date[13];
               } D0100FMT;

typedef struct D0200FMT {
                 int bytes_returned;
                 int bytes_available;
                 char object_name[10];
                 char object_library[10];
                 char object_type[10];
                 char return_library[10];
                 int asp_number;
                 char object_owner[10];
                 char object_domain[2];
                 char creation_date[13];
                 char change_date[13];
                 char extended_attribute[10];
                 char oirtext[50];
                 char source_file[10];
                 char source_file_lib[10];
                 char source_file_mbr[10];
               } D0200FMT;

typedef struct D0300FMT {
                 int bytes_returned;
                 int bytes_available;
                 char object_name[10];
                 char object_library[10];
                 char object_type[10];
                 char return_library[10];
                 int asp_number;
                 char object_owner[10];
                 char object_domain[2];
                 char creation_date[13];
                 char change_date[13];
                 char extended_attribute[10];
                 char oirtext[50];
                 char source_file[10];
                 char source_file_lib[10];
                 char source_file_mbr[10];
                 char source_file_update[13];
                 char savedate[13];
                 char restore_date[13];
                 char creating_userprf[10];
                 char system_created_on[8];
                 char reset_date[7];
                 int save_size;
                 int save_sequence_number;
                 char storage[10];
                 char save_command[10];
                 char save_volid[71];
                 char save_device[10];
                 char save_file[10];
                 char save_file_lib[10];
                 char save_label[17];
                 char system_level[9];
                 char compiler[16];
                 char object_level[8];
                 char user_change;
                 char licensed_program[16];
                 char ptf[10];
                 char apar[10];
               } D0300FMT;

typedef struct D0400FMT {
                 int bytes_returned;
                 int bytes_available;
                 char object_name[10];
                 char object_library[10];
                 char object_type[10];
                 char return_library[10];
                 int asp_number;
                 char object_owner[10];
                 char object_domain[2];
                 char creation_date[13];
                 char change_date[13];
                 char extended_attribute[10];
                 char oirtext[50];
                 char source_file[10];
                 char source_file_lib[10];
                 char source_file_mbr[10];
                 char source_file_update[13];
                 char savedate[13];
                 char restore_date[13];
                 char creating_userprf[10];
                 char system_created_on[8];
                 char reset_date[7];
                 int save_size;
                 int save_sequence_number;
                 char storage[10];
                 char save_command[10];
                 char save_volid[71];
                 char save_device[10];
                 char save_file[10];
                 char save_file_lib[10];
                 char save_label[17];
                 char system_level[9];
                 char compiler[16];
                 char object_level[8];
                 char user_change;
                 char licensed_program[16];
                 char ptf[10];
                 char apar[10];
                 char last_use_date[7];
                 char update_object_usage;
                 int  use_count;
                 int object_size;
                 int object_multiplier;
                 char compress;
               } D0400FMT;

/********************************************************************/
/*  Structures returned by the QUSLOBJ API                          */
/********************************************************************/

typedef struct lobj_input_parms {
                 char user_space_name[10];
                 char user_space_library[10];
                 char format_name[8];
                 char object_name_specified[10];
                 char library_name_specified[10];
                 char object_type_specified[10];
               } lobj_input_parms;

typedef struct lobj_header_section {
                 char object_name_specified[10];
                 char library_name_specified[10];
                 char object_type_specified[10];
               } lobj_header_section;

typedef struct OBJL0100 {
                 char object_name[10];
                 char library_name[10];
                 char object_type[10];
               } OBJL0100;
/********************************************************************/
/*  Structures returned by the QUSLJOB API                          */
/********************************************************************/

typedef struct ljob_input_parms {
                 char job_name_specified[10];
                 char user_name_specified[10];
                 char job_number_specified[6];
                 char status_specified[10];
               } ljob_input_parms;

typedef struct ljob_header_section {
                 char job_name_searched_for[10];
                 char user_name_searched_for[10];
                 char job_number_searched_for[6];
               } ljob_header_section;

typedef struct JOBL0100 {
                 char job_name[10];
                 char user_name[10];
                 char job_number[6];
                 char internal_job_id[16];
                 char status[10];
               } JOBL0100;
/*******************************************************************/
/* Input structure for QUSLMBR header section                      */
/*******************************************************************/
typedef struct mbrl_input {
     char user_space[20];
     char format[8];
     char file_and_lib[20];
     char member_name[10];
     char override;
} mbrl_input;

/*******************************************************************/
/* Record structure for QUSLMBR header section                     */
/*******************************************************************/
typedef struct mbrl_header {
     char file_name[10];
     char file_lib[10];
     char file_attr[10];
     char file_txt[50];
     int  tot_members;
     char source_file;
} mbrl_header;


/*******************************************************************/
/* Record structure for QUSLMBR MBRL0100 format                    */
/*******************************************************************/
typedef struct mbrl0100 {
     char member_name[10];
} mbrl0100;

/*******************************************************************/
/* Record structure for QUSLMBR MBRL0200 format                    */
/*******************************************************************/
typedef struct mbrl0200 {
     char member_name[10];
     char source_type[10];
     char crt_date[13];
     char change_date[13];
     char text_desc[50];
} mbrl0200;

/*******************************************************************/
/* Record structure for QUSRMBRD MBRD0100 format                   */
/*******************************************************************/
typedef struct mbrd0100 {
     int  bytes_returned;
     int  bytes_available;
     char db_file_name[10];
     char db_file_lib[10];
     char member_name[10];
     char file_attr[10];
     char src_type[10];
     char crt_date[13];
     char change_date[13];
     char text_desc[50];
     char src_file;
} mbrd0100;


/*******************************************************************/
/* Record structure for QUSRMBRD MBRD0200 format                   */
/*******************************************************************/
typedef struct mbrd0200 {
     int  bytes_returned;
     int  bytes_available;
     char db_file_name[10];
     char db_file_lib[10];
     char member_name[10];
     char file_attr[10];
     char src_type[10];
     char crt_date[13];
     char src_change_date[13];
     char text_desc[50];
     char src_file;
     char ext_file;
     char log_file;
     char OPD_share;
     char RESERVED[2];
     int  num_cur_rec;
     int  num_dlt_rec;
     int  dat_spc_size;
     int  acc_pth_size;
     int  num_dat_mbr;
     char change_date[13];
     char save_date[13];
     char rest_date[13];
     char exp_date[13];
     int  nbr_days_user;
     char date_lst_used[7];
     char use_reset_date[7];
} mbrd0200;

/*******************************************************************/
/* Input structure for QUSLRCD header section                      */
/*******************************************************************/
typedef struct rcdl_input {
     char user_space_name[20];
     char format_name[8];
     char file_name_and_library[20];
     char override;
} rcdl_input;

/*******************************************************************/
/* Record structure for QUSLRCD header section                     */
/*******************************************************************/
typedef struct rcdl_header {
     char file_name[10];
     char file_lib[10];
     char file_type[10];
     char file_txt_desc[50];
} rcdl_header;

/*******************************************************************/
/* Record structure for QUSLRCD RCDL0100 format                    */
/*******************************************************************/
typedef struct rcdl0100 {
     char record_fmt_name[10];
} rcdl0100;

/*******************************************************************/
/* Record structure for QUSLRCD RCDL0200 format                    */
/*******************************************************************/
typedef struct rcdl0200 {
     char record_fmt_name[10];
     char record_fmt_ID[13];
     char RESERVED1;
     int  record_length;
     int  number_fields;
     char rcrd_txt_desc[50];
} rcdl0200;

/*******************************************************************/
/* Input structure for QUSLFLD header section                      */
/*******************************************************************/
typedef struct fldl_input {
     char space_name[10];
     char space_lib[10];
     char format[8];
     char file_name[10];
     char file_lib[10];
     char format_format_name[10];
     char override;
} fldl_input;

/*******************************************************************/
/* Record structure for QUSLFLD header section                     */
/*******************************************************************/
typedef struct fldl_header {
     char file_name[10];
     char file_lib[10];
     char file_type[10];
     char rcd_fmt_name[10];
     int  rcd_length;
     char record_fmt_ID[13];
     char rcrd_txt_desc[50];
} fldl_header;


/*******************************************************************/
/* Record structure for QUSLFLD FLDL0100 format                    */
/*******************************************************************/
typedef struct fldl0100 {
     char field_name[10];
     char data_type;
     char usage;
     int  output_buff_pos;
     int  input_buff_pos;
     int  field_length;
     int  field_digits;
     int  field_dec_pos;
     char field_desc[50];
     char edit_code[2];
     int  edit_wrd_lngth;
     char edit_word[64];
     char col_head_1[20];
     char col_head_2[20];
     char col_head_3[20];
} fldl0100;


/*******************************************************************/
/* Input structure for QUSLSPL header section                      */
/*******************************************************************/
typedef struct splf_input {
     char user_space[20];
     char format_name[8];
     char user_name[10];
     char outq_name_and_lib[20];
     char form_type[10];
     char user_data[10];
} splf_input;

/*******************************************************************/
/* Record structure for QUSLSPL header section                     */
/*******************************************************************/
typedef struct splf_header {
     char usr_name[10];
     char outq_name[10];
     char outq_lib[10];
} splf_header;


/*******************************************************************/
/* Record structure for QUSLSPL SPLF0100 format                    */
/*******************************************************************/
typedef struct splf0100 {
     char usr_name[10];
     char outq_name[10];
     char outq_lib[10];
     char form_type[10];
     char usr_data[10];
     char int_job_ID[16];
     char int_splf_ID[16];
} splf0100;


/*******************************************************************/
/* Record structure for QUSRSPLA SPLA0100 format                   */
/*******************************************************************/
typedef struct spla0100 {
     int  bytes_return;
     int  bytes_avail;
     char int_job_ID[16];
     char int_splf_ID[16];
     char job_name[10];
     char usr_name[10];
     char job_number[6];
     char splf_name[10];
     int  splf_number;
     char form_type[10];
     char usr_data[10];
     char status[10];
     char file_avail[10];
     char hold_file[10];
     char save_file[10];
     int  total_pages;
     int  curr_page;
     int  start_page;
     int  end_page;
     int  last_page_print;
     int  rest_page;
     int  total_copies;
     int  copies_rem;
     int  lines_per_inch;
     int  char_per_inch;
     char output_priority[2];
     char outq_name[10];
     char outq_lib[10];
     char date_file_open[7];
     char time_file_open[6];
     char dev_file_name[10];
     char dev_file_lib[10];
     char pgm_name[10];
     char pgm_lib[10];
     char count_code[15];
     char print_text[30];
     int  record_length;
     int  max_records;
     char dev_type[10];
     char ptr_dev_type[10];
     char doc_name[12];
     char folder_name[64];
     char s36_proc_name[8];
     char print_fidel[10];
     char repl_unprint;
     char repl_char;
     int  page_length;
     int  page_width;
     int  number_seperate;
     int  overflow_line_nm;
     char DBCS_data[10];
     char DBCS_ext_chars[10];
     char DBCS_SOSI[10];
     char DBCS_char_rotate[10];
     int  DBCS_cpi;
     char grph_char_set[10];
     char code_page[10];
     char form_def_name[10];
     char form_def_lib[10];
     int  source_drawer;
     char print_font[10];
     char s36_spl_ID[6];
     int  page_rotate;
     int  justification;
     char duplex[10];
     char fold[10];
     char ctrl_char[10];
     char align_forms[10];
     char print_quality[10];
     char form_feed[10];
     char disk_volume[71];
     char disk_label[17];
     char exch_type[10];
     char char_code[10];
     int  nmbr_disk_rcrds;
     int  multiup;
     char frnt_ovrly_name[10];
     char frnt_ovrly_lib_name[10];
     char frnt_ovrly_off_dn[8];
     char frnt_ovrly_off_across[8];
     char bck_ovrly_name[10];
     char bck_ovrly_lib_name[10];
     char bck_ovrly_off_dn[8];
     char bck_ovrly_off_across[8];
     char unit_measure[10];
     char page_definition[10];
     char page_definition_lib[10];
     char line_spacing[10];
     char point_size[8];
} spla0100;


/*******************************************************************/
/* Record structure for QUSRJOBI JOBI0100 format                   */
/*******************************************************************/
typedef struct jobi0100 {
     int  bytes_return;
     int  bytes_avail;
     char qual_job_name[26];
     char int_job_ID[16];
     char job_status[10];
     char job_type;
     char job_subtype;
     char RESERVED[2];
     int  run_priority;
     int  time_slice;
     int  default_wait;
     char purge[10];
} jobi0100;


/*******************************************************************/
/* Record structure for QUSRJOBI JOBI0150 format                   */
/*******************************************************************/
typedef struct jobi0150 {
     int  bytes_return;
     int  bytes_avail;
     char qual_job_name[26];
     char int_job_ID[16];
     char job_status[10];
     char job_type;
     char job_subtype;
     char RESERVED[2];
     int  run_priority;
     int  time_slice;
     int  default_wait;
     char purge[10];
     char time_slice_end_pool[10];
     int  CPU_used;
     int  system_pool_ID;
} jobi0150;


/*******************************************************************/
/* Record structure for QUSRJOBI JOBI0200 format                   */
/*******************************************************************/
typedef struct jobi0200 {
     int  bytes_return;
     int  bytes_avail;
     char qual_job_name[26];
     char int_job_ID[16];
     char job_status[10];
     char job_type;
     char job_subtype;
     char subsys_name[10];
     int  run_priority;
     int  system_pool_ID;
     int  CPU_used;
     int  aux_IO_request;
     int  interact_trans;
     int  response_time;
     char function_type;
     char function_name[10];
     char active_job_stat[4];
} jobi0200;


/*******************************************************************/
/* Record structure for QUSRJOBI JOBI0300 format                   */
/*******************************************************************/
typedef struct jobi0300 {
     int  bytes_return;
     int  bytes_avail;
     char qual_job_name[26];
     char int_job_ID[16];
     char job_status[10];
     char job_type;
     char job_subtype;
     char jobq_name[10];
     char jobq_lib[10];
     char jobq_priority[2];
     char outq_name[10];
     char outq_lib[10];
     char outq_priority[2];
     char prn_dev_name[10];
     char subm_name[26];
     char subm_msgq_name[20];
} jobi0300;


/*******************************************************************/
/* Record structure for QUSRJOBI JOBI0400 format                   */
/*******************************************************************/
typedef struct jobi0400 {
     int  bytes_return;
     int  bytes_avail;
     char qual_job_name[26];
     char int_job_ID[16];
     char job_status[10];
     char job_type;
     char job_subtype;
     char date_enter_sys[13];
     char date_job_active[13];
     char accounting_code[15];
     char job_desc_name[10];
     char job_desc_lib[10];
     char unit_work_ID[24];
     char mode_name[8];
     char inq_msg_reply[10];
     char log_cl_pgms[10];
     char brk_msg_hndl[10];
     char status_msg_hndl[10];
     char device_recov_act[13];
     char DDM_conv_handle[10];
     char date_seperate;
     int  date_format;
     char print_text[30];
     char subm_job_name[26];
     char subm_msgq_name[20];
} jobi0400;


/*******************************************************************/
/* Record structure for QUSRJOBI JOBI0500 format                   */
/*******************************************************************/
typedef struct jobi0500 {
     int  bytes_return;
     int  bytes_avail;
     char qual_job_name[26];
     char int_job_ID[16];
     char job_status[10];
     char job_type;
     char job_subtype;
     char RESERVED[2];
     int  end_severity;
     int  log_severity;
     char logging_lvl;
     char logging_type[10];
} jobi0500;


/*******************************************************************/
/* Record structure for QUSRJOBI JOBI0600 format                   */
/*******************************************************************/
typedef struct jobi0600 {
     int  bytes_return;
     int  bytes_avail;
     char qual_job_name[26];
     char int_job_ID[16];
     char job_status[10];
     char job_type;
     char job_subtype;
     char job_switch[8];
     char end_status;
     char subsys_desc[20];
     char curr_usrprf_name[10];
} jobi0600;


/*******************************************************************/
/* Record structure for QUSRJOBI JOBI0700 format                   */
/*******************************************************************/
typedef struct jobi0700 {
     int  bytes_return;
     int  bytes_avail;
     char qual_job_name[26];
     char int_job_ID[16];
     char job_status[10];
     char job_type;
     char job_subtype;
     char RESERVED[2];
     int  lib_in_syslibl;
     int  prod_libs;
     int  curr_libs;
     int  libs_in_usrlibl;
     char libs_in_libl[43][11];
} jobi0700;

#endif  /* _TMKHOPNA_H */

