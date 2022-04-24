; vim: set ft=tasm:
version m520
.model large,stdcall
.486

DOSSEG
CODESEG

extern drv_disp_info: proc
extern drv_disp_open: proc
extern drv_disp_close: proc
extern drv_disp_text: proc
extern drv_disp_graph: proc
extern drv_disp_set_pos: proc
extern drv_disp_write: proc
extern drv_disp_zone: proc
extern drv_disp_set_bg: proc
extern drv_disp_clear: proc
extern drv_disp_fg_clear: proc
extern drv_disp_copy: proc
extern drv_disp_size: proc
extern drv_disp_fit: proc
extern drv_disp_curs_off: proc
extern drv_disp_curs_on: proc
extern drv_disp_curs_type: proc
extern drv_disp_grph_process: proc
extern drv_disp_grph_txt_size: proc
extern drv_disp_set_pos_hpu: proc
extern drv_disp_grph_compute_view: proc
extern drv_disp_unlock: proc
extern drv_disp_lock: proc
extern drv_disp_grph_set_cur_view: proc
extern drv_disp_grph_txt_fit: proc
extern drv_disp_sync: proc

start           dd drv_disp_info
                dd drv_disp_open
                dd drv_disp_close
                dd drv_disp_text
                dd drv_disp_graph
                dd drv_disp_set_pos
                dd drv_disp_write
                dd drv_disp_zone
                dd drv_disp_set_bg
                dd drv_disp_clear
                dd drv_disp_fg_clear
                dd drv_disp_copy
                dd drv_disp_size
                dd drv_disp_fit
                dd drv_disp_curs_off
                dd drv_disp_curs_on
                dd drv_disp_curs_type
                dd drv_disp_grph_process
                dd drv_disp_grph_txt_size
                dd drv_disp_set_pos_hpu
                dd drv_disp_grph_compute_view
                dd drv_disp_unlock
                dd drv_disp_lock
                dd drv_disp_grph_set_cur_view
                dd drv_disp_grph_txt_fit
                dd drv_disp_sync
                dw seg _DATA

; This is just here to satisfy the crt, it should never be called.
global _main: proc
_main proc far
        mov     ax, 4141h
        int     3
_main endp

; There is an object file that screws up my alignment in the CRT, and
; I don't even need it.
;
; If you define a symbol, and then use /NOE, the linkers uses this one,
; so this stops that nuisance object from being linked into my executable.
global __nullcheck: proc
__nullcheck proc far
        mov     ax, 4242h
        int     3
__nullcheck endp

end start
