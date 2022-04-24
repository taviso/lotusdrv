#ifndef __LOTCALLS_H
#define __LOTCALLS_H

#pragma pack(1)
#define __lotapi __pascal __far

struct LOTUSFUNCS
{
    int       (__lotapi *drv_max_segsize)(void);
    void far *(__lotapi *alloc_mptr)(int vmr, int size, int tag);
    void      (__lotapi *free_mptr)(void far *ptr, int size);
    void far *(__lotapi *alloc_fixed_block)(int size);
    int       (__lotapi *free_fixed_block)(void far *ptr, int, int size);
    void far *(__lotapi *drv_map_mptr)(void far *m, int v);
    void far *(__lotapi *drv_get_vmr)(int v);
    void far *drv_unmap_vmr;
    void far *drv_yield_test;
    void far *file_name_parse;
    void far *file_exist;
    void far *file_find;
    void far *file_access_read;
    void far *file_get_fileinfo;
    void far *file_get_filepointer;
    void far *file_lseek;
    void far *file_read;
    void far *file_access_finished;
    void far *file_access_write;
    void far *file_write;
    int       (__lotapi *open_rasterizer)(struct DEVDATA far *, int far *);
    void far *rast_init_device;
    void      (__lotapi *set_strip)(int, int);
    int       (__lotapi *raster)(int, void far *ptr);
    void      (__lotapi *close_rasterizer)(int hdl);
    int       (__lotapi *rast_compute_view)(int, void far *ptr);
    int       (__lotapi *rast_txt_size)(int, int, void far *ptr1, void far *ptr2);
    int       (__lotapi *rast_txt_fit)(int, int, void *a, int, void far *b, void far *c);
    void far *p_link_init;
    void far *p_link_transmit;
    void far *p_link_term;
    void far *p_link_start_job;
    void far *p_link_end_job;
    void far *p_signal_user;
    void far *p_print_link_alert;
    void far *p_print_link_message;
    void far *country_reduce_string;
    void far *country_convert_string;
    void far *dyload;
    void far *dyunload;
    void far *drv_wait_nticks;
    unsigned (__lotapi *set_disp_buf)(unsigned far *selector, int base, int limit);
    void far *circle_position;
    void far *drv_display_reset;
    void far *open_system_file;
    void far *drv_largest_avail;
    void far *quit_now;
    void far *is_encoded;
    void far *command;
    void far *x_get_date;
    void far *x_get_time;
    void far *x_sysbeep;
    void far *convert_from_table;
    void far *reduce_from_table;
    void far *country_text_cnv;
    void (__lotapi *drop_disp_buf)(int seg);
    void far *x_cntry_tbl_release2;
    void far *char_size;
};

extern struct LOTUSFUNCS far *callbacks;

extern void __pascal drv_disp_clear(unsigned cols,
                                    unsigned lines,
                                    unsigned char attrs);
extern void __pascal drv_disp_set_pos(int col, int line);

#endif
