; trdos.z80s

; This file is part of TR-DOS Disassembled
; By Marcos Cruz (programandala.net), 2016, 2017

  org 0x0000

  include inc/zx_spectrum_rom_routines.z80s
  include inc/zx_spectrum_char_codes.z80s

sys_swap: equ 0x5b00
sys_bankm:  equ 0x5b5c
sys_tstack: equ 0x5bff
sys_kstate: equ 0x5c00
sys_kstate.1: equ 0x5c01
sys_repdel: equ 0x5c09
sys_strms:  equ 0x5c10
sys_chars:  equ 0x5c36
sys_rasp: equ 0x5c38
sys_err_nr: equ 0x5c3a
sys_flags:  equ 0x5c3b
sys_tv_flag:  equ 0x5c3c
sys_err_sp: equ 0x5c3d
sys_newppc: equ 0x5c42
sys_nsppc:  equ 0x5c44
sys_ppc:  equ 0x5c45
sys_bordcr: equ 0x5c48
sys_vars: equ 0x5c4b
sys_chans:  equ 0x5c4f
sys_curchl: equ 0x5c51
sys_prog: equ 0x5c53
sys_datadd: equ 0x5c57
sys_e_line: equ 0x5c59
sys_k_cur:  equ 0x5c5b
sys_ch_add: equ 0x5c5d
sys_worksp: equ 0x5c61
sys_stkbot: equ 0x5c63
sys_stkend: equ 0x5c65
sys_df_sz:  equ 0x5c6b
sys_udg:  equ 0x5c7b
sys_attr_p: equ 0x5c8d
sys_attr_t: equ 0x5c8f
sys_membot: equ 0x5c92
sys_ramtop: equ 0x5cb2
sys_p_ramt: equ 0x5cb4
sys_p_ramt.high:  equ 0x5cb5
trdos_variable.interface_1_zone:  equ 0x5cb6
trdos_variable.0xc9:  equ 0x5cc2
trdos_variable.unknown_23747: equ 0x5cc3
trdos_variable.drive_a_mode:  equ 0x5cc8
trdos_variable.drive_c_mode:  equ 0x5cca
trdos_variable.cat_current_sector:  equ 0x5ccc
trdos_variable.disk_drive_ready:  equ 0x5ccd
trdos_variable.sector_rw_flag:  equ 0x5cce
trdos_variable.stack_sp_copy: equ 0x5ccf
trdos_variable.basic_program_autostart: equ 0x5cd1
trdos_variable.basic_program_autostart.high:  equ 0x5cd2
trdos_variable.unknown_23763: equ 0x5cd3
trdos_variable.move_command_deleted_files:  equ 0x5cd4
trdos_variable.move_command_first_sector: equ 0x5cd5
trdos_variable.bad_sectors_or_move_command_first_track: equ 0x5cd6
trdos_variable.start_file_parameter_or_number_of_tracks:  equ 0x5cd7
trdos_variable.start_file_parameter.high: equ 0x5cd8
trdos_variable.ch_add:  equ 0x5cd9
trdos_variable.ch_add.high: equ 0x5cda
trdos_variable.stream_or_file_size: equ 0x5cdb
trdos_variable.file_size.high:  equ 0x5cdc
trdos_variable.filename:  equ 0x5cdd
trdos_variable.filename.2:  equ 0x5cdf
trdos_variable.filename.4:  equ 0x5ce1
trdos_variable.filename.6:  equ 0x5ce3
trdos_variable.file_type: equ 0x5ce5
trdos_variable.file_start:  equ 0x5ce6
trdos_variable.file_start.high: equ 0x5ce7
trdos_variable.file_length_in_bytes:  equ 0x5ce8
trdos_variable.file_length_in_bytes.high: equ 0x5ce9
trdos_variable.file_length_in_sectors:  equ 0x5cea
trdos_variable.file_first_sector: equ 0x5ceb
trdos_variable.c_file_start_or_b_file_size: equ 0x5ced
trdos_variable.file_2_length_in_bytes:  equ 0x5cef
trdos_variable.file_2_length_in_sectors:  equ 0x5cf1
trdos_variable.file_2_first_sector: equ 0x5cf2
trdos_variable.current_sector:  equ 0x5cf4
trdos_variable.current_track: equ 0x5cf5
trdos_variable.current_temporary_drive: equ 0x5cf6
trdos_variable.cleared: equ 0x5cf7
trdos_variable.two_files_drive: equ 0x5cf8
trdos_variable.cat_drive_or_read_verify_flag: equ 0x5cf9
trdos_variable.drive_a_step_rate: equ 0x5cfa
trdos_variable.drive_c_step_rate: equ 0x5cfc
trdos_variable.last_fdc_command:  equ 0x5cfe
trdos_variable.sector:  equ 0x5cff
trdos_variable.buffer:  equ 0x5d00
trdos_variable.hl_copy: equ 0x5d02
trdos_variable.de_copy: equ 0x5d04
trdos_variable.bytes_compared_by_find_command:  equ 0x5d06
trdos_variable.deleted_files: equ 0x5d07
trdos_variable.filename_first_char: equ 0x5d08
trdos_variable.data_file_mode:  equ 0x5d09
trdos_variable.buffer_flag: equ 0x5d0c
trdos_variable.file_number_for_copying: equ 0x5d0d
trdos_variable.command_mode:  equ 0x5d0e
trdos_variable.error_code:  equ 0x5d0f
trdos_variable.error_code_msb:  equ 0x5d10
trdos_variable.command_line_address:  equ 0x5d11
trdos_variable.err_sp_copy: equ 0x5d13
trdos_variable.show_screen_flag:  equ 0x5d15
trdos_variable.system_register: equ 0x5d16
trdos_variable.show_title_flag: equ 0x5d17
trdos_variable.interface_1_flag:  equ 0x5d18
trdos_variable.default_drive: equ 0x5d19
trdos_variable.internal_address_of_finishing_proc:  equ 0x5d1a
trdos_variable.sp_copy: equ 0x5d1c
trdos_variable.find_command_found_file: equ 0x5d1e
trdos_variable.unknown_23839: equ 0x5d1f
trdos_variable.entered_line_3_first_chars:  equ 0x5d20
trdos_variable.entered_line_3_first_chars.1:  equ 0x5d21
trdos_variable.move_command_blocks: equ 0x5d23
trdos_variable.tmp_buffer_address:  equ 0x5d25
trdos_variable.k_channel.output:  equ 0x5d26
trdos_variable.r_channel.input.high:  equ 0x5d33
trdos_sector_buffer.next_free_sector: equ 0x5e06
trdos_sector_buffer.next_free_track:  equ 0x5e07
trdos_sector_buffer.disk_type:  equ 0x5e08
trdos_sector_buffer.files:  equ 0x5e09
trdos_sector_buffer.free_sectors: equ 0x5e0a
trdos_sector_buffer.sectors_per_track:  equ 0x5e0c
trdos_sector_buffer.deleted_files:  equ 0x5e19
trdos_sector_buffer.disc_title: equ 0x5e1a

  di
  ld de,0xFFFF
  ld a,0x07
  jr l0009h
  nop
l0009h:
  out (0xFE),a
  ld a,0x3F
  jr l0013h
  nop
rst_10:
  jp print_character_A
l0013h:
  ld i,a
  jp l001bh
rst_18:
  jp print_message_at_HL
l001bh:
  nop
  nop
  nop
  jr l0024h
rst_20:
  jp call_zx_spectrum_rom_routine_stored_at_the_address_on_tos
  ret
l0024h:
  ld h,d
  ld l,e
  jr l002bh
  jp l2323h
l002bh:
  ld (hl),0x02
  dec hl
  cp h
  jr nz,l002bh
  jr l003ah

; BLOCK 'not_used_0033' (start 0x0033 end 0x003a)
not_used_0033_start:
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFB
  defb 0xC9
l003ah:
  or a
  sbc hl,de
  add hl,de
  inc hl
  jr nc,l0047h
  dec (hl)
  jr z,l0047h
  dec (hl)
  jr z,l003ah
l0047h:
  dec hl
  ld (sys_p_ramt),hl
  ld de,l3eafh
  ld bc,0x00A8
  ld a,e
  ex de,hl
  ld sp,0x6000
  ld (0x5F00),hl
  ld hl,l0079h
  push hl
  ld hl,trdos_entry.call_pushed_address
  push hl
  ld hl,0xB8ED
  jr l0069h
  jp magic_button_nmi_request
l0069h:
  ld (0x5F10),hl
  push af
  ld a,0xC9
  ld (0x5F12),a
  pop af
  ld hl,(0x5F00)
  jp 0x5F10
l0079h:
  ex de,hl
  inc hl
  ld (sys_udg),hl
  dec hl
  ld bc,0x1E40
  ld (sys_rasp),bc
  ld (sys_ramtop),hl
  ld hl,zx_spectrum_font_char_0
  ld (sys_chars),hl
  ld hl,(sys_ramtop)
  ld (hl),0x3E
  dec hl
  ld sp,hl
  dec hl
  dec hl
  ld (sys_err_sp),hl
  ld de,l1303h
  push de
  im 1
  ld iy,sys_err_nr
  ld hl,trdos_variable.interface_1_zone
  ld (sys_chans),hl
  ld de,l15afh
  ld bc,0x0015
  ex de,hl
  call sub_0117h
  ex de,hl
  dec hl
  ld (sys_datadd),hl
  inc hl
  ld (sys_prog),hl
  ld (sys_vars),hl
  ld (hl),0x80
  inc hl
  ld (sys_e_line),hl
  ld (hl),0x0D
  inc hl
  ld (hl),0x80
  inc hl
  ld (sys_worksp),hl
  ld (sys_stkbot),hl
  ld (sys_stkend),hl
  ld a,0x38
  ld (sys_attr_p),a
  ld (sys_attr_t),a
  ld (sys_bordcr),a
  ld hl,0x0523
  ld (sys_repdel),hl
  dec (iy-0x3A)
  dec (iy-0x36)
  ld hl,0x15C6
  ld de,sys_strms
  ld bc,0x000D+1
  call sub_0117h
  set 1,(iy+0x01)
  ld hl,trdos_variable.0xc9
  ld (hl),0xC9
  rst 0x20
  defw rom_clear_prb
  ld hl,sys_df_sz
  ld (hl),0x02
  ld hl,l128bh
  push hl
  ld a,0xAA
  ld (sys_swap),a
  ei
  jp enter_trdos_from_basic
sub_0117h:
  ld (0x5F00),hl
  ld hl,trdos_entry.call_pushed_address
  push hl
  ld hl,0xB0ED
  ld (0x5F10),hl
  ld hl,(0x5F00)
  jp 0x5F10
l012ah:
  call sub_20e5h
  call cls
  ld hl,(sys_e_line)
  inc hl
  ld e,(hl)
  inc hl
  ld d,(hl)
  ld a,d
  or e
  ex de,hl
  jr z,l0140h
  xor a
  ld (trdos_variable.error_code_msb),a
l0140h:
  push hl
  call sub_0232h
  pop hl
  ld (sys_newppc),hl
  xor a
  ld (sys_nsppc),a
  rst 0x20
  defw rom_set_min
  ld hl,(sys_prog)
  dec hl
  ld (sys_datadd),hl
  ld sp,(sys_err_sp)
  ld a,(trdos_variable.error_code_msb)
  or a
  ld hl,0x1B76
  jr z,l0166h
  rst 0x20
  defw rom_report_0 ; Error report: "OK"
l0166h:
  push hl
  ld hl,trdos_variable.0xc9
  push hl
  ret
do_interpret_rem_command_in_basic:
  call sub_20f1h
  call allocate_the_sector_buffer
  ld a,0xFF
  ld (trdos_variable.show_screen_flag),a
  xor a
  ld (trdos_variable.cleared),a
  ld a,0xAA
  ld (trdos_variable.show_title_flag),a
  ld hl,finishing_proc_0x0201
  ld (trdos_variable.internal_address_of_finishing_proc),hl
  ld hl,0x0000
  add hl,sp
  ld (trdos_variable.sp_copy),hl
  dec hl
  dec hl
  ld sp,hl
  call sub_021dh
  ld hl,(sys_ramtop)
  ld de,(sys_ch_add)
  sbc hl,de
  ex de,hl
  jr nc,l01a5h
  or a
  ld de,0x0100+1
  sbc hl,de
l01a5h:
  ld (sys_ch_add),hl
l01a8h:
  call sub_01c7h
l01abh:
  jp z,exit_dos
  cp 0xEA
  inc hl
  jr nz,l01a8h
  call sub_01c7h
  jr z,l01abh
  cp 0x3A
  jp nz,exit_dos
  inc hl
  call sub_3048h
  ld hl,(trdos_variable.command_line_address)
  jp l030ah
sub_01c7h:
  ld a,(hl)
  cp char.carriage_return
  ret z
  cp 0x80
  ret z
  or a
  ret
  call sub_1e43h
exit_dos:
  ld hl,0x0000
  ld (trdos_variable.two_files_drive),hl
  call sub_20e5h
  call sub_1d63h
  ld hl,trdos_variable.show_title_flag
  ld (hl),0xAA
  ld hl,trdos_variable.unknown_23839
  ld a,(hl)
  or a
  ld (hl),0x00
  jr nz,l01f3h
  call remove_fp
  call empty_command_line
l01f3h:
  ld sp,(trdos_variable.sp_copy)
  ld hl,(trdos_variable.internal_address_of_finishing_proc)
  ld bc,(trdos_variable.error_code)
  ld b,0x00
  jp (hl)
finishing_proc_0x0201:
  call sub_0232h
  bit 7,(iy+0x00)
  ret nz
  ld de,trdos_variable.0xc9
  ld sp,(sys_err_sp)
  push de
  ret
empty_command_line:
  call get_char
  cp char.carriage_return
  ret z
  call next_char
  jr empty_command_line
sub_021dh:
  ld hl,(sys_err_sp)
  ld (trdos_variable.err_sp_copy),hl
  ld hl,(trdos_variable.sp_copy)
  dec hl
  dec hl
  ld (sys_err_sp),hl
  ld de,l3d16h
  ld (hl),e
  inc hl
  ld (hl),d
  ret
sub_0232h:
  ld hl,(trdos_variable.err_sp_copy)
  ld (sys_err_sp),hl
  ret
l0239h:
  ld hl,0x0000
  ld (trdos_variable.cleared),hl
  add hl,sp
  ld (trdos_variable.sp_copy),hl
  dec hl
  dec hl
  ld sp,hl
  call sub_021dh
  ld hl,trdos_variable.show_title_flag
  ld a,(hl)
  cp 0xAA
  ld a,0x00
  ld (trdos_variable.error_code),a
  jp z,finishing_proc_0x02CB
  ld (hl),0xAA
  call cls
  call open_channel_2
  ld hl,trdos_title_start
  rst 0x18
  call sub_106eh
  ld a,(trdos_variable.interface_1_zone)
  cp 0xF4
  jr z,l0271h
  ld hl,message_interface_1_start
  rst 0x18
l0271h:
  ld a,(sys_swap)
  cp 0xAA
  jr nz,finishing_proc_0x02CB
  call sub_20f1h
l027bh:
  ld hl,(sys_e_line)
  ld a,0xFE
  ld (trdos_variable.command_mode),a
  ld (hl),0xF7
  inc hl
  ld (hl),0x22
  inc hl
  ld (hl),0x62
  inc hl
  ld (hl),0x6F
  inc hl
  ld (hl),0x6F
  inc hl
  ld (hl),0x74
  inc hl
  ld (hl),0x22
  inc hl
  ld (sys_k_cur),hl
  ld (hl),0x0D
  inc hl
  ld (hl),0x80
  inc hl
  ld (sys_worksp),hl
  ld (sys_stkbot),hl
  ld (sys_stkend),hl
  set 3,(iy+0x01)
  jr l02efh
sub_02b0h:
  ld b,0x03
l02b2h:
  ld a,(hl)
  ld (de),a
  inc hl
  inc de
  djnz l02b2h
  ret
sub_02b9h:
  ld b,0x20
l02bbh:
  push bc
  xor 0x08
  out (0xFF),a
  push af
  ld a,0x05
  call sub_3dffh
  pop af
  pop bc
  djnz l02bbh
  ret
finishing_proc_0x02CB:
  ld hl,(trdos_variable.sp_copy)
  dec hl
  dec hl
  ld sp,hl
  call sub_20f1h
  call open_channel_0
  ld a,(trdos_variable.system_register)
  or 0x03
  call sub_02b9h
  ld a,(trdos_variable.system_register)
  call sub_02b9h
  xor a
  ld (trdos_variable.show_screen_flag),a
  call sub_2135h
  call sub_3032h
l02efh:
  call cls_lower
  ld hl,finishing_proc_0x02CB
  ld (trdos_variable.internal_address_of_finishing_proc),hl
  xor a
  ld (trdos_variable.error_code),a
  ld hl,(sys_e_line)
  push hl
  ld de,trdos_variable.entered_line_3_first_chars
  call sub_02b0h
  pop hl
  ld (trdos_variable.command_line_address),hl
l030ah:
  ld a,(hl)
  ld b,a
  and 0x80
  ld a,b
  jr z,l031ah
  cp 0xFE
  jr z,l031ah
  push af
  call init_default_disk_drive
  pop af
l031ah:
  ld hl,token_commands_table_start
  dec hl
  ld c,0x00
l0320h:
  inc c
  ld d,a
  ld a,0x15
  cp c
  jp c,exit_dos
  ld a,d
  inc hl
  cp (hl)
  jr nz,l0320h
  cp 0xFE
  call nz,allocate_the_sector_buffer
  ld a,0x09
  ld (trdos_variable.bytes_compared_by_find_command),a
  xor a
  ld (trdos_variable.error_code),a
  ld (trdos_variable.bad_sectors_or_move_command_first_track),a
  ld (trdos_variable.error_code_msb),a
  ld hl,sys_flags
  res 7,(hl)
  ld b,0x00
  ld hl,token_commands_addresses_table_start
  dec c
  sla c
  add hl,bc
  ld e,(hl)
  inc hl
  ld d,(hl)
  ex de,hl
  push hl
  ld de,l0359h
  push de
  jp (hl)
l0359h:
  ld hl,sys_flags
  set 7,(hl)
  pop hl
  jp (hl)

; BLOCK 'trdos_title' (start 0x0360 end 0x03ac)
trdos_title_start:
  defb 0x16
  defb 0x01
  defb 0x05
  defb "*"
  defb " "
  defb "T"
  defb "R"
  defb "-"
  defb "D"
  defb "O"
  defb "S"
  defb " "
  defb "V"
  defb "e"
  defb "r"
  defb " "
  defb "5"
  defb "."
  defb "0"
  defb "3"
  defb " "
  defb "*"
  defb 0x0D
  defb 0x0D
  defb 0x7F
  defb " "
  defb "1"
  defb "9"
  defb "8"
  defb "6"
  defb " "
  defb "T"
  defb "e"
  defb "c"
  defb "h"
  defb "n"
  defb "o"
  defb "l"
  defb "o"
  defb "g"
  defb "y"
  defb " "
  defb "R"
  defb "e"
  defb "s"
  defb "e"
  defb "a"
  defb "r"
  defb "c"
  defb "h"
  defb " "
  defb "L"
  defb "t"
  defb "d"
  defb "."
  defb 0x16
  defb 0x05
  defb 0x0B
  defb "("
  defb "U"
  defb "."
  defb "K"
  defb "."
  defb ")"
  defb 0x16
  defb 0x07
  defb 0x05
  defb "B"
  defb "E"
  defb "T"
  defb "A"
  defb " "
  defb "1"
  defb "2"
  defb "8"
  defb 0x00
l03ach:
  call read_sector_8_of_track_0
  call print_carriage_return
  call print_carriage_return
l03b5h:
  ld bc,(trdos_sector_buffer.free_sectors)
  call print_number_in_BC
  ld hl,message.cr.free
  rst 0x18
jp_exit_dos:
  jp exit_dos
print_error_number_A_whose_message_is_at_HL:
  push af
  ld a,(trdos_variable.command_mode)
  cp 0xFE
  jr nz,l03cdh
  pop af
  ret
l03cdh:
  pop af
  ld (trdos_variable.error_code),a
  ld a,(trdos_variable.show_screen_flag)
  or a
  call z,print_message_at_HL
  ret
print_no_files_error_and_exit_DOS:
  ld hl,message.cr.no_files
  ld a,0x01
  jp print_error_number_A_whose_message_is_at_HL_and_exit_DOS
print_OK_and_exit_DOS:
  ld hl,message.ok
  xor a
  jp print_error_number_A_whose_message_is_at_HL_and_exit_DOS
read_cat_sector_0_into_tmp_buffer:
  xor a
  ld (trdos_variable.cat_current_sector),a
read_current_cat_sector_into_tmp_buffer:
  ld de,(trdos_variable.cat_current_sector)
  ld d,0x00
read_sector_DE_into_tmp_buffer:
  call allocate_the_sector_buffer
  ld hl,trdos_variable.tmp_buffer_address
  ld b,0x01
  jp read_sectors
read_sector_8_of_track_0:
  call allocate_the_sector_buffer
  ld de,0x0008
  jr read_sector_DE_into_tmp_buffer
read_system_track:
  call read_sector_8_of_track_0
  ld a,(trdos_sector_buffer.sectors_per_track)
  cp 0x10
  jr z,sectors_per_track_are_correct_so_disk_is_recognized
  ld hl,message.cr.disc_error
  rst 0x18
  jr jp_exit_dos
sectors_per_track_are_correct_so_disk_is_recognized:
  call sub_3e11h
  res 0,(hl)
  res 1,(hl)
  ld a,(trdos_sector_buffer.disk_type)
  bit 0,a
  jr nz,l0425h
  set 0,(hl)
l0425h:
  bit 3,a
  ret nz
  set 1,(hl)
  ret
z_if_next_command_line_char_is_carriage_return:
  ld hl,(trdos_variable.command_line_address)
  inc hl
  ld a,(hl)
  cp char.carriage_return
  ret
cli_command.cat:
  call z_if_next_command_line_char_is_carriage_return
  ld bc,0x0001+1
  ld (trdos_variable.stream_or_file_size),bc
  jr z,print_catalogue_of_current_temporary_drive
  cp '#' ; channel?
  jr nz,l045dh
  ld (sys_ch_add),hl
  call sub_1e0bh
  call get_char
  cp char.carriage_return
  jr z,print_catalogue_of_current_temporary_drive
  cp 0x2C
  jp nz,error.wrong_drive_letter
  call next_char
  call expt_exp
  jr l0460h
l045dh:
  call parse_expression
l0460h:
  call pop_hl_if_checking_syntax
  call stk_fetch
  ex de,hl
  call parse_possible_drive_letter_in_filename_at_HL
print_catalogue_of_current_temporary_drive:
  call pop_hl_if_checking_syntax
  ld a,(trdos_variable.current_temporary_drive)
  ld (trdos_variable.cat_drive_or_read_verify_flag),a
  call read_system_track
  ld a,(trdos_variable.stream_or_file_size)
print_catalogue_to_stream_A:
  cp 0x02
  push af
  call z,cls
  pop af
  cp 0x11
  jp nc,error.wrong_drive_letter
  call chan_open
  ld a,0xFF
  ld (trdos_variable.two_files_drive),a
  ld hl,message.cr.title
  rst 0x18
  ld hl,trdos_sector_buffer.disc_title
  rst 0x18
  call print_carriage_return
  ld a,(trdos_sector_buffer.files)
  ld hl,trdos_sector_buffer.deleted_files
  sub (hl)
  push hl
  call print_number_in_A
  ld hl,message.files.cr
  rst 0x18
  pop hl
  ld c,(hl)
  call print_number_in_C
  ld hl,message.del_file
  rst 0x18
  call read_cat_sector_0_into_tmp_buffer
  ld hl,trdos_variable.tmp_buffer_address
print_catalogue.line:
  call init_cat_drive
  call print_carriage_return
  ld a,(trdos_variable.current_temporary_drive)
  add a,"A" ; first drive letter
  rst 0x10
  ld b,0x02
print_catalogue.file:
  call init_cat_drive
  push bc
  ld a,":"
  rst 0x10
  push hl
  call print_filename_at_HL
  ld bc,0x000D
  pop hl
  push hl
  add hl,bc
  ld c,(hl)
  push bc
  ld a,c
  ld b,0x02
  cp 0x0A
  jr c,l04dfh
  dec b
l04dfh:
  cp 0x64
  jr nc,l04e8h
l04e3h:
  ld a,0x20
  rst 0x10
  djnz l04e3h
l04e8h:
  pop bc
  call print_number_in_BC
  pop hl
  pop bc
  ld de,0x10
  add hl,de
  djnz print_catalogue.file
  jr print_catalogue.line
init_cat_drive:
  push hl
  push bc
  ld a,(trdos_variable.cat_drive_or_read_verify_flag)
  ld hl,trdos_variable.current_temporary_drive
  cp (hl)
  call nz,init_disk_drive_in_A_register
  pop bc
  pop hl
  jp l2fc6h
sub_0507h:
  ld de,0x10
  add hl,de
  ret
sub_050ch:
  push hl
  push bc
  ld bc,0xA1DB
  add hl,bc
  jr c,l0517h
  pop bc
  pop hl
  ret
l0517h:
  ld hl,trdos_variable.cat_current_sector
  inc (hl)
  call read_current_cat_sector_into_tmp_buffer
  pop bc
  pop hl
  ld hl,trdos_variable.tmp_buffer_address
  ret
check_drive_letter_and_convert_to_number:
  and 0xDF
  sbc a,"A" ; first drive letter
  jp c,error.wrong_drive_letter
  cp 0x04
  jp nc,error.wrong_drive_letter
  ret
fetch_drive_letter_from_the_calculator_stack:
  call stk_fetch
  ld a,c
  cp b
  jp z,error.wrong_drive_letter
  ret
cli_command.new:
  call sub_1dcdh
  call pop_hl_if_checking_syntax
  call sub_102eh
  call sub_1cb0h
  ld a,(trdos_variable.current_temporary_drive)
  ld (trdos_variable.two_files_drive),a
  jp nz,print_no_files_error_and_exit_DOS
  push bc
  call read_file_descriptor_C
  call sub_1cb0h
  push af
  ld a,(trdos_variable.two_files_drive)
  ld hl,trdos_variable.current_temporary_drive
  cp (hl)
  jp nz,error.wrong_drive_letter
  call read_system_track
  pop af
  jp z,error.file_exists
  pop bc
l0569h:
  call write_file_descriptor_C
  call sub_1e43h
  jp print_OK_and_exit_DOS
check_error_code_msb:
  ld a,(trdos_variable.error_code_msb)
  or a
  ret
check_deleted_files_and_exit_DOS:
  ld a,(trdos_variable.deleted_files)
  or a
  jp z,print_no_files_error_and_exit_DOS
  jp print_OK_and_exit_DOS
sub_0581h:
  push bc
  call cls
  ld a,(trdos_variable.current_temporary_drive)
  add a,"A" ; first drive letter
  call print_character_A
  ld a,":"
  call print_character_A
  ld hl,trdos_variable.filename
  call print_filename_at_HL
  ld hl,message.file_exists
  call print_message_at_HL
  call get_uppercase_pressed_key
  cp "Y" ; Yes?
  push af
  call cls
  pop af
  pop bc
  ret nz
  push bc
  call cls
  pop bc
  call sub_0781h
  xor a
  ret
sub_05b4h:
  ld a,(trdos_variable.file_type)
  cp "#" ; data file type?
  jr z,l05bdh
  xor a
  ret
l05bdh:
  ld a,0x0A
  ld (trdos_variable.bytes_compared_by_find_command),a
  call sub_1cb3h
  ld a,0x09
  ld (trdos_variable.bytes_compared_by_find_command),a
  ret
l05cbh:
  ld a,(trdos_variable.filename)
  cp 0x2A
  jp nz,print_no_files_error_and_exit_DOS
  call stk_fetch
  ex de,hl
  call parse_possible_drive_letter_in_filename_at_HL
  ld a,(hl)
  cp 0x2A
  jp nz,error.wrong_drive_letter
  ld a,(trdos_variable.current_temporary_drive)
  ld (trdos_variable.cat_drive_or_read_verify_flag),a
  ld a,(trdos_variable.cat_drive_or_read_verify_flag)
  call init_disk_drive_in_A_register
  call read_system_track
  ld a,0xFF
  ld (trdos_variable.file_number_for_copying),a
l05f4h:
  ld a,(trdos_variable.two_files_drive)
  call init_disk_drive_in_A_register
  call read_system_track
  ld a,(trdos_variable.file_number_for_copying)
  inc a
  ld (trdos_variable.file_number_for_copying),a
  ld c,a
  call read_file_descriptor_C
  ld a,(trdos_variable.filename)
  cp 0x00
  jp z,print_OK_and_exit_DOS
  cp 0x01
  jr z,l05f4h
  ld hl,trdos_variable.file_start
  ld de,trdos_variable.c_file_start_or_b_file_size
  ld bc,0x0007
  ldir
  ld a,(trdos_variable.cat_drive_or_read_verify_flag)
  call init_disk_drive_in_A_register
  call sub_1cb3h
  jr nz,l0634h
  call sub_05b4h
  jr nz,l0634h
  call sub_0581h
  jr nz,l05f4h
l0634h:
  call sub_063ch
  call sub_1e43h
  jr l05f4h
sub_063ch:
  call read_sector_8_of_track_0
  ld a,(trdos_sector_buffer.files)
  cp 0x80
  jp z,error.no_space
  ld hl,trdos_variable.c_file_start_or_b_file_size
  ld de,trdos_variable.file_start
  ld bc,0x0007
  ldir
  ld de,(trdos_variable.file_length_in_sectors)
  ld d,0x00
  or a
  ld hl,(trdos_sector_buffer.free_sectors)
  sbc hl,de
  jp c,error.no_space
  ld (trdos_sector_buffer.free_sectors),hl
  ld hl,(trdos_sector_buffer.next_free_sector)
  ld (trdos_variable.file_first_sector),hl
  push hl
  call sub_072fh
  pop hl
  ld (trdos_variable.file_first_sector),hl
  ld hl,(trdos_variable.current_sector)
  ld (trdos_sector_buffer.next_free_sector),hl
  ld hl,trdos_sector_buffer.files
  inc (hl)
  ld c,(hl)
  dec c
  ld b,0x00
  push bc
  ld de,0x0009
  ld (trdos_variable.current_sector),de
  call sub_1e43h
  pop bc
  call write_file_descriptor_C
  ret
cli_command.copy:
  ld hl,(trdos_variable.command_line_address)
  inc hl
  ld a,(hl)
  and 0xDF
  cp "S" ; `COPY s`?
  jp z,cli_command.copy_s
  cp "B" ; `COPY b`?
  jp z,cli_command.copy_b
  call sub_1dcdh
  call pop_hl_if_checking_syntax
  call sub_166fh
  call sub_102eh
  call sub_1cb0h
  ld a,(trdos_variable.current_temporary_drive)
  ld (trdos_variable.two_files_drive),a
  jp nz,l05cbh
  call read_file_descriptor_C
  ld hl,trdos_variable.file_start
  ld de,trdos_variable.c_file_start_or_b_file_size
  ld bc,0x0007
  ldir
  call sub_1cb0h
  push af
  push bc
  ld a,(trdos_variable.current_temporary_drive)
  ld (trdos_variable.cat_drive_or_read_verify_flag),a
  ld a,(trdos_variable.two_files_drive)
  call init_disk_drive_in_A_register
  call read_system_track
  ld a,(trdos_variable.cat_drive_or_read_verify_flag)
  call init_disk_drive_in_A_register
  call read_system_track
  pop bc
  pop af
  jr nz,l06f3h
  call sub_05b4h
  jr nz,l06f3h
  call sub_0581h
  jp nz,print_OK_and_exit_DOS
l06f3h:
  call sub_063ch
  call sub_1e43h
  ld a,(trdos_variable.file_type)
  cp "#" ; data file type?
  jp nz,print_OK_and_exit_DOS
  ld a,0x0A
  ld (trdos_variable.bytes_compared_by_find_command),a
  ld hl,trdos_variable.file_start
  inc (hl)
  ld a,(trdos_variable.two_files_drive)
  call init_disk_drive_in_A_register
  call sub_05b4h
  jp nz,print_OK_and_exit_DOS
  call read_file_descriptor_C
  ld hl,trdos_variable.file_start
  ld de,trdos_variable.c_file_start_or_b_file_size
  ld bc,0x0007
  ldir
  ld a,(trdos_variable.cat_drive_or_read_verify_flag)
  call init_disk_drive_in_A_register
  call read_system_track
  jr l06f3h
sub_072fh:
  ld a,(trdos_variable.file_2_length_in_sectors)
  or a
  ret z
  push hl
  ld hl,trdos_variable.move_command_blocks
  sub (hl)
  pop hl
  jr nc,l0775h
  ld a,(trdos_variable.file_2_length_in_sectors)
  ld b,a
  xor a
  ld (trdos_variable.file_2_length_in_sectors),a
l0744h:
  push bc
  ld a,(trdos_variable.two_files_drive)
  call init_disk_drive_in_A_register
  pop bc
  push bc
  ld hl,(trdos_variable.stack_sp_copy)
  push hl
  ld de,(trdos_variable.file_2_first_sector)
  call read_sectors
  ld hl,(trdos_variable.current_sector)
  ld (trdos_variable.file_2_first_sector),hl
  ld a,(trdos_variable.cat_drive_or_read_verify_flag)
  call init_disk_drive_in_A_register
  pop hl
  pop bc
  ld de,(trdos_variable.file_first_sector)
  call write_sectors
  ld hl,(trdos_variable.current_sector)
  ld (trdos_variable.file_first_sector),hl
  jr sub_072fh
l0775h:
  ld (trdos_variable.file_2_length_in_sectors),a
  push hl
  ld hl,trdos_variable.move_command_blocks
  ld b,(hl)
  pop hl
  xor a
  jr l0744h
sub_0781h:
  xor a
  ld (trdos_variable.deleted_files),a
  jr l07a0h
cli_command.erase:
  call parse_expression
  call pop_hl_if_checking_syntax
  call sub_102eh
  xor a
  ld (trdos_variable.deleted_files),a
  call sub_292fh
  call l07a0h
  jp nz,check_deleted_files_and_exit_DOS
  jp print_OK_and_exit_DOS
l07a0h:
  ld a,(trdos_variable.filename)
  ld (trdos_variable.filename_first_char),a
  ret nz
  ld hl,trdos_variable.deleted_files
  inc (hl)
  push bc
  call read_sector_8_of_track_0
  ld a,(trdos_sector_buffer.files)
  pop bc
  inc c
  cp c
  jr nz,l07bch
  dec a
  ld (trdos_sector_buffer.files),a
  xor a
l07bch:
  push af
  jr z,l07c3h
  ld hl,trdos_sector_buffer.deleted_files
  inc (hl)
l07c3h:
  push bc
  call sub_1e43h
  pop bc
  dec c
  call read_file_descriptor_C
  pop af
  jp z,l07d2h
  ld a,0x01
l07d2h:
  ld (trdos_variable.filename),a
  push af
  call l1e40h
  ld a,(trdos_variable.filename_first_char)
  ld (trdos_variable.filename),a
  pop af
  jr z,l07e7h
  call sub_1cb3h
  jr l07a0h
l07e7h:
  call read_sector_8_of_track_0
  ld hl,(trdos_variable.file_first_sector)
  ld (trdos_sector_buffer.next_free_sector),hl
  ld de,(trdos_variable.file_length_in_sectors)
  ld hl,(trdos_sector_buffer.free_sectors)
  ld d,0x00
  add hl,de
  ld (trdos_sector_buffer.free_sectors),hl
  jp sub_1e43h
l0800h:
  and 0xFC
  jp l3d9ah

; BLOCK 'not_used_0805' (start 0x0805 end 0x1000)
not_used_0805_start:
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF

; BLOCK 'message_interface_1' (start 0x1000 end 0x1018)
message_interface_1_start:
  defb 0x16
  defb 0x09
  defb 0x05
  defb "I"
  defb "n"
  defb "t"
  defb "e"
  defb "r"
  defb "f"
  defb "a"
  defb "c"
  defb "e"
  defb " "
  defb "o"
  defb "n"
  defb "e"
  defb " "
  defb "f"
  defb "i"
  defb "t"
  defb "t"
  defb "e"
  defb "d"
  defb 0x00
cli_command.asterisk:
  call parse_expression
  call pop_hl_if_checking_syntax
  call fetch_drive_letter_from_the_calculator_stack
  ld a,(de)
  call check_drive_letter_and_convert_to_number
  ld (trdos_variable.default_drive),a
  call init_disk_drive_in_A_register
  jp print_OK_and_exit_DOS
sub_102eh:
  ld b,"C" ; file type: code
  ld a,(trdos_variable.bad_sectors_or_move_command_first_track)
  or a
  jr nz,set_file_type_stored_in_B
  call get_char
  cp token.code ; `CODE`?
  ld b,"C" ; file type: code
  jr z,set_file_type_stored_in_B
  cp token.data ; `DATA`?
  ld b,"D" ; file type: array
  jr z,set_file_type_stored_in_B
  cp "#" ; data file type?
  ld b,"#" ; file type: data
  jr z,set_file_type_stored_in_B
  ld b,"B" ; file type: BASIC
set_file_type_stored_in_B:
  ld hl,trdos_variable.file_type
  ld (hl),b
  ret
get_uppercase_pressed_key:
  di
  push hl
  push bc
  push de
wait_for_key:
  rst 0x20
  defw rom_key_scan
  ld c,0x00
  jr nz,wait_for_key
  rst 0x20
  defw rom_k_test
  jr nc,wait_for_key
  dec d
color_the_trdos_title:
  ld e,a
  rst 0x20
  defw rom_k_decode
  pop de
  pop bc
  pop hl
  and 0xDF
  ei
  ret
sub_106eh:
  ld hl,0x58E5
  ld b,0x0A
l1073h:
  ld (hl),0x07
  inc hl
  djnz l1073h
  ld (hl),0x02
  inc hl
  ld (hl),0x16
  inc hl
  ld (hl),0x34
  inc hl
  defw 0x2536
  inc hl
  ld (hl),0x28
  inc hl
  ld (hl),0x07
  ld hl,0x40EE
  ld b,0x08
  xor a
l108fh:
  push bc
  scf
  rla
  push hl
  push af
  ld b,0x05
l1096h:
  inc hl
  ld (hl),a
  djnz l1096h
  pop af
  pop hl
  pop bc
  ld de,0x0100
  add hl,de
  djnz l108fh
  ret
  ret
message.del_files:
  defb " "
  defb "D"
  defb "e"
  defb "l"
  defb "."
message.files:
  defb " "
  defb "F"
  defb "i"
  defb "l"
  defb "e"
  defb "("
  defb "s"
  defb ")"
  defb 0x00
message.title:
  defb "T"
  defb "i"
  defb "t"
  defb "l"
  defb "e"
  defb ":"
  defb 0xA0
message.disk_drive:
  defb 0x17
  defb 0x11
  defb " "
  defb "D"
  defb "i"
  defb "s"
  defb "k"
  defb " "
  defb "D"
  defb "r"
  defb "i"
  defb "v"
  defb "e"
  defb ":"
  defb " "
  defb 0x00
message.tab_0x10_space:
  defb 0x17
  defb 0x10
  defb " "
  defb 0x00
message.40_track_single_side:
  defb 0x17
  defb 0x10
  defb " "
  defb "4"
  defb "0"
  defb " "
  defb "T"
  defb "r"
  defb "a"
  defb "c"
  defb "k"
  defb " "
  defb "S"
  defb "."
  defb " "
  defb "S"
  defb "i"
  defb "d"
  defb "e"
  defb 0x00
message.80_track_single_side:
  defb 0x17
  defb 0x10
  defb " "
  defb "8"
  defb "0"
  defb " "
  defb "T"
  defb "r"
  defb "a"
  defb "c"
  defb "k"
  defb " "
  defb "S"
  defb "."
  defb " "
  defb "S"
  defb "i"
  defb "d"
  defb "e"
  defb 0x00
message.40_track_double_side:
  defb 0x17
  defb 0x10
  defb " "
  defb "4"
  defb "0"
  defb " "
  defb "T"
  defb "r"
  defb "a"
  defb "c"
  defb "k"
  defb " "
  defb "D"
  defb "."
  defb " "
  defb "S"
  defb "i"
  defb "d"
  defb "e"
  defb 0x00
message.80_track_double_side:
  defb 0x17
  defb 0x10
  defb " "
  defb "8"
  defb "0"
  defb " "
  defb "T"
  defb "r"
  defb "a"
  defb "c"
  defb "k"
  defb " "
  defb "D"
  defb "."
  defb " "
  defb "S"
  defb "i"
  defb "d"
  defb "e"
  defb 0x00
message.free_sector:
  defb 0x17
  defb 0x10
  defb " "
  defb "F"
  defb "r"
  defb "e"
  defb "e"
  defb " "
  defb "S"
  defb "e"
  defb "c"
  defb "t"
  defb "o"
  defb "r"
  defb " "
  defb 0x00
message.file_name_start_length_line:
  defb 0x0D
  defb 0x0D
  defb " "
  defb " "
  defb "F"
  defb "i"
  defb "l"
  defb "e"
  defb " "
  defb "N"
  defb "a"
  defb "m"
  defb "e"
  defb " "
  defb " "
  defb " "
  defb " "
  defb "S"
  defb "t"
  defb "a"
  defb "r"
  defb "t"
  defb " "
  defb "L"
  defb "e"
  defb "n"
  defb "g"
  defb "t"
  defb "h"
  defb " "
  defb "L"
  defb "i"
  defb "n"
  defb "e"
  defb 0x00
sub_1151h:
  defb "*"
  ld h,c
  ld e,h
  ld (trdos_variable.stack_sp_copy),hl
  ld bc,0x0222
  jp bc_spaces
print_number_in_HL_right_justified_with_zeroes:
  xor a
  ld de,0x2710
l1161h:
  sbc hl,de
  jr c,l1168h
  inc a
  jr l1161h
l1168h:
  add a,"0"
  call print_character_A_preserving_DEHL
  add hl,de
  xor a
  ld de,read_cat_sector_0_into_tmp_buffer
l1172h:
  sbc hl,de
  jr c,l1179h
  inc a
  jr l1172h
l1179h:
  add a,"0"
  call print_character_A_preserving_DEHL
  add hl,de
  xor a
  ld de,0x0064
l1183h:
  sbc hl,de
  jr c,l118ah
  inc a
  jr l1183h
l118ah:
  add a,"0"
  call print_character_A_preserving_DEHL
  add hl,de
  xor a
  ld de,0x0009+1
l1194h:
  sbc hl,de
  jr c,l119bh
  inc a
  jr l1194h
l119bh:
  add a,"0"
  call print_character_A_preserving_DEHL
  add hl,de
  ld a,l
  add a,"0"
  call print_character_A_preserving_DEHL
  ret
print_character_A_preserving_DEHL:
  push hl
  push de
  call print_character_A
  pop de
  pop hl
  ret
l11b0h:
  push hl
  push bc
  ld a,(trdos_variable.cat_drive_or_read_verify_flag)
  ld hl,trdos_variable.current_temporary_drive
  cp (hl)
  call nz,init_disk_drive_in_A_register
  pop bc
  pop hl
  call sub_050ch
  ld a,(hl)
  or a
  jp z,exit_dos
  cp 0x01
  call z,sub_0507h
  ret nz
  jr l11b0h
cli_command.list:
  call z_if_next_command_line_char_is_carriage_return
  ld bc,0x0001+1
  ld (trdos_variable.stream_or_file_size),bc
  jr z,print_detailed_catalogue_of_current_temporary_drive
  cp "#" ; stream speficied?
  jr nz,l11f8h
  ld (sys_ch_add),hl
  call sub_1e0bh
  call get_char
  cp char.carriage_return
  jr z,print_detailed_catalogue_of_current_temporary_drive
  cp 0x2C
  jp nz,error.wrong_drive_letter
  call next_char
  call expt_exp
  jr l11fbh
l11f8h:
  call parse_expression
l11fbh:
  call pop_hl_if_checking_syntax
  call stk_fetch
  ex de,hl
  call parse_possible_drive_letter_in_filename_at_HL
print_detailed_catalogue_of_current_temporary_drive:
  call pop_hl_if_checking_syntax
  ld a,(trdos_variable.current_temporary_drive)
  ld (trdos_variable.cat_drive_or_read_verify_flag),a
  call read_system_track
  ld a,(trdos_variable.stream_or_file_size)
  cp 0x02
  push af
  call z,cls
  pop af
  cp 0x11
  jp nc,error.wrong_drive_letter
  call chan_open
  ld a,0xFF
  ld (trdos_variable.two_files_drive),a
  call sub_1151h
  ld hl,trdos_sector_buffer.next_free_sector
  ld de,(trdos_variable.stack_sp_copy)
  ld bc,0x20
  ldir
  call read_cat_sector_0_into_tmp_buffer
  ld hl,trdos_variable.tmp_buffer_address
  push hl
print_detailed_catalogue.page:
  ld hl,message.title
  push bc
  rst 0x18
  ld hl,(trdos_variable.stack_sp_copy)
  ld bc,0x0013+1
  add hl,bc
  rst 0x18
  ld hl,message.disk_drive
  rst 0x18
  ld a,(trdos_variable.current_temporary_drive)
  add a,"A" ; first drive letter
  call print_character_A
  call print_carriage_return
  ld hl,(trdos_variable.stack_sp_copy)
  ld bc,0x0001+2
  add hl,bc
  ld a,(hl)
  ld hl,(trdos_variable.stack_sp_copy)
  ld bc,0x0013
  add hl,bc
  sub (hl)
  push hl
  call print_number_in_A
  ld hl,message.files
  rst 0x18
  ld hl,(trdos_variable.stack_sp_copy)
  ld bc,0x0001+1
  add hl,bc
  ld a,(hl)
  ld hl,message.40_track_single_side
  cp 0x19
  jr z,l1292h
  ld hl,message.80_track_single_side
  cp 0x18
  jr z,l1292h
  ld hl,message.40_track_double_side
l128bh:
  cp 0x17
  jr z,l1292h
  ld hl,message.80_track_double_side
l1292h:
  rst 0x18
  pop hl
  ld c,(hl)
  call print_number_in_C
  ld hl,message.del_files
  rst 0x18
  ld hl,message.free_sector
  rst 0x18
  ld hl,(trdos_variable.stack_sp_copy)
  ld bc,0x0004
  add hl,bc
  ld c,(hl)
  inc hl
  ld b,(hl)
  call print_number_in_BC
  ld hl,message.file_name_start_length_line
  rst 0x18
  pop bc
  pop hl
  ld b,0x10
print_detailed_catalogue.file:
  call l11b0h
  call print_carriage_return
  push bc
  push hl
  call print_filename_at_HL
  ld bc,0x000D
  pop hl
  push hl
  add hl,bc
  ld c,(hl)
  push bc
  ld a,c
  ld b,0x02
  cp 0x0A
  jr c,sectors_number_is_smaller_than_10
  dec b
sectors_number_is_smaller_than_10:
  cp 0x64
  jr nc,print_sectors_number_in_TOS
print_B_spaces_before_sectors_number:
  ld a," "
  rst 0x10
  djnz print_B_spaces_before_sectors_number
print_sectors_number_in_TOS:
  pop bc
  call print_number_in_BC
  ld hl,message.tab_0x10_space
  rst 0x18
  pop hl
  push hl
  ld bc,0x0009
  add hl,bc
  ld e,(hl)
  inc hl
  ld d,(hl)
  push hl
  ex de,hl
  call print_number_in_HL_right_justified_with_zeroes
  ld a," "
  call print_character_A
  pop hl
  inc hl
  ld e,(hl)
  inc hl
  ld d,(hl)
  ex de,hl
  call print_number_in_HL_right_justified_with_zeroes
  pop hl
  push hl
  ld bc,0x0008
  add hl,bc
l1303h:
  ld a,(hl)
  cp "B" ; BASIC program?
  call z,print_detailed_catalogue.file.basic_program
  pop hl
  pop bc
  ld de,0x10 ; Length of a file descriptor
  add hl,de ; Point to the next entry
  djnz print_detailed_catalogue.file
  push hl
  call print_carriage_return
  call print_carriage_return
  jp print_detailed_catalogue.page
print_detailed_catalogue.file.basic_program:
  ld bc,0x0004+1
  add hl,bc
  ld b,(hl)
  inc hl
  ld e,(hl)
  inc hl
  ld d,(hl)
  dec b
  jr z,l1335h
  dec b
  jr z,l1335h
  ld a,0x10
l132ch:
  inc e
  cp e
  jr nz,l1333h
  ld e,0x00
  inc d
l1333h:
  djnz l132ch
l1335h:
  ld hl,(trdos_variable.stack_sp_copy)
  ld bc,0x20+1
  add hl,bc
  ld b,0x02
  push hl
  call read_sectors
  ld a,0x80
  pop hl
  ld bc,0x0200
  cpir
  ld a,(hl)
  cp 0xAA
  ret nz
  inc hl
  ld c,(hl)
  inc hl
  ld b,(hl)
  ld a,b
  or c
  ret z
  push bc
  ld a," "
  call print_character_A
  pop bc
  call print_number_in_BC
  ret
cli_command.copy_s:
  call point_interpreter_to_command_line
  call next_char
  call expt_exp
  call pop_hl_if_checking_syntax
  call sub_166fh
  ld hl,message.insert_source_disk
  call print_message_at_HL
l1375h:
  call get_uppercase_pressed_key
  cp "Y" ; Yes?
  jr nz,l1375h
  call cls_lower
  call sub_102eh
  call sub_1cb0h
  jp nz,print_no_files_error_and_exit_DOS
  call sub_13b7h
  ld a,(trdos_variable.file_type)
  cp "#" ; data file type?
  jp nz,print_OK_and_exit_DOS
l1393h:
  ld a,0x0A
  ld (trdos_variable.bytes_compared_by_find_command),a
  ld hl,trdos_variable.file_start
  inc (hl)
  call cls
  ld hl,message.insert_source_disk
  call print_message_at_HL
l13a5h:
  call get_uppercase_pressed_key
  cp "Y" ; Yes?
  jr nz,l13a5h
  call sub_1cb3h
  jp nz,print_OK_and_exit_DOS
  call sub_13b7h
  jr l1393h
sub_13b7h:
  call read_file_descriptor_C
  ld hl,trdos_variable.file_start
  ld de,trdos_variable.c_file_start_or_b_file_size
  ld bc,0x0007
  ldir
  ld a,(trdos_variable.file_2_length_in_sectors)
  ld (trdos_variable.error_code_msb),a
  call read_system_track
  call sub_3e11h
  ld (trdos_variable.ch_add),a
  ld a,0xFF
  ld (trdos_variable.entered_line_3_first_chars.1),a
  call sub_1451h
  ld hl,(trdos_variable.unknown_23839)
  ld (trdos_variable.file_first_sector),hl
  ld hl,(trdos_variable.current_sector)
  ld (trdos_sector_buffer.next_free_sector),hl
  ld hl,trdos_sector_buffer.files
  inc (hl)
  ld c,(hl)
  dec c
  ld b,0x00
  push bc
  ld de,0x0009
  ld (trdos_variable.current_sector),de
  call sub_1e43h
  pop bc
  call write_file_descriptor_C
  call sub_1e43h
  ret
sub_1403h:
  xor a
  ld (trdos_variable.entered_line_3_first_chars.1),a
  call read_system_track
  call sub_3e11h
  ld (trdos_variable.ch_add.high),a
  call sub_1cb3h
  jp z,error.file_exists
  call read_sector_8_of_track_0
  ld a,(trdos_sector_buffer.files)
  cp 0x80
  jp z,error.directory_full
  ld hl,trdos_variable.c_file_start_or_b_file_size
  ld de,trdos_variable.file_start
  ld bc,0x0007
  ldir
  call read_sector_8_of_track_0
  ld a,(trdos_variable.error_code_msb)
  ld (trdos_variable.file_length_in_sectors),a
  ld de,(trdos_variable.file_length_in_sectors)
  ld d,0x00
  or a
  ld hl,(trdos_sector_buffer.free_sectors)
  sbc hl,de
  jp c,error.no_space
  ld (trdos_sector_buffer.free_sectors),hl
  ld hl,(trdos_sector_buffer.next_free_sector)
  ld (trdos_variable.file_first_sector),hl
  ld (trdos_variable.unknown_23839),hl
  ret
sub_1451h:
  ld a,(trdos_variable.file_2_length_in_sectors)
  or a
  ret z
  ld a,(trdos_variable.entered_line_3_first_chars.1)
  or a
  jr nz,l146fh
  call cls
  ld hl,message.insert_source_disk
  call print_message_at_HL
l1465h:
  call get_uppercase_pressed_key
  cp "Y" ; Yes?
  jr nz,l1465h
  call cls_lower
l146fh:
  ld a,(trdos_variable.file_2_length_in_sectors)
  or a
  ret z
  push hl
  ld hl,trdos_variable.move_command_blocks
  sub (hl)
  pop hl
  jr nc,l14cbh
  ld a,(trdos_variable.file_2_length_in_sectors)
  ld b,a
  xor a
  ld (trdos_variable.file_2_length_in_sectors),a
l1484h:
  push bc
  ld (trdos_variable.sector_rw_flag),a
  ld hl,(trdos_variable.stack_sp_copy)
  push hl
  ld de,(trdos_variable.file_2_first_sector)
  call sub_14d8h
  call read_sectors
  ld hl,(trdos_variable.current_sector)
  ld (trdos_variable.file_2_first_sector),hl
  call cls
  ld hl,l2785h
  call print_message_at_HL
l14a5h:
  call get_uppercase_pressed_key
  cp "Y" ; Yes?
  jr nz,l14a5h
  call cls_lower
  ld a,(trdos_variable.entered_line_3_first_chars.1)
  or a
  call nz,sub_1403h
  pop hl
  pop bc
  ld de,(trdos_variable.file_first_sector)
  call sub_14e4h
  call write_sectors
  ld hl,(trdos_variable.current_sector)
  ld (trdos_variable.file_first_sector),hl
  jp sub_1451h
l14cbh:
  ld (trdos_variable.file_2_length_in_sectors),a
  push hl
  ld hl,trdos_variable.move_command_blocks
  ld b,(hl)
  pop hl
  xor a
  jp l1484h
sub_14d8h:
  push hl
  push de
  call sub_3e11h
  ld a,(trdos_variable.ch_add)
  ld (hl),a
  pop de
  pop hl
  ret
sub_14e4h:
  push hl
  push de
  call sub_3e11h
  ld a,(trdos_variable.ch_add.high)
  ld (hl),a
  pop de
  pop hl
  ret
sub_14f0h:
  xor a
  ld (trdos_variable.entered_line_3_first_chars.1),a
  call read_system_track
  call sub_3e11h
  ld (trdos_variable.ch_add.high),a
  ld a,(trdos_sector_buffer.disk_type)
  ld (trdos_variable.file_start.high),a
  ld hl,0x0280
  cp 0x19
  jr z,l151fh
  ld hl,0x0500
  cp 0x18
  jr z,l151fh
  cp 0x17
  jr z,l151fh
  ld hl,0x0A00
  cp 0x16
  jr z,l151fh
  jp error.wrong_drive_letter
l151fh:
  ld (trdos_variable.filename),hl
  ld bc,(trdos_variable.filename.2)
  sbc hl,bc
  jp c,error.no_space
  ret
cli_command.copy_b:
  call pop_hl_if_checking_syntax
  call sub_166fh
  ld hl,message.backup_disk
  call print_message_at_HL
  ld hl,message.insert_source_disk
  call print_message_at_HL
l153eh:
  call get_uppercase_pressed_key
  cp "Y" ; Yes?
  jr nz,l153eh
  call cls_lower
  ld a,0xFF
  ld (trdos_variable.entered_line_3_first_chars.1),a
  call read_system_track
  call sub_3e11h
  ld (trdos_variable.ch_add),a
  ld a,(trdos_sector_buffer.disk_type)
  cp 0x19
  ld hl,0x0280
  jr z,l1575h
  ld hl,0x0500
  cp 0x18
  jr z,l1575h
  cp 0x17
  jr z,l1575h
  ld hl,0x0A00
  cp 0x16
  jr z,l1575h
  jp error.wrong_drive_letter
l1575h:
  ld bc,(trdos_sector_buffer.free_sectors)
  sbc hl,bc
  ld (trdos_variable.file_type),hl
  ld (trdos_variable.filename.2),hl
  ld hl,0x0000
  ld (trdos_variable.filename.4),hl
  ld (trdos_variable.filename.6),hl
  call sub_15b8h
  call read_system_track
  ld a,(trdos_variable.file_start.high)
  ld (trdos_sector_buffer.disk_type),a
  ld hl,(trdos_variable.filename)
  ld bc,(trdos_variable.filename.2)
  sbc hl,bc
  ld (trdos_sector_buffer.free_sectors),hl
  call sub_14e4h
  ld de,0x0009
  ld (trdos_variable.current_sector),de
  call sub_1e43h
l15afh:
  jp print_OK_and_exit_DOS
sub_15b2h:
  ld hl,(trdos_variable.file_type)
  ld a,h
  or l
  ret
sub_15b8h:
  call sub_15b2h
  ret z
  ld a,(trdos_variable.entered_line_3_first_chars.1)
  or a
  jr nz,l15dbh
  call cls
  ld hl,message.backup_disk
  call print_message_at_HL
  ld hl,message.insert_source_disk
  call print_message_at_HL
l15d1h:
  call get_uppercase_pressed_key
  cp "Y" ; Yes?
  jr nz,l15d1h
  call cls_lower
l15dbh:
  call sub_15b2h
  ret z
  push bc
  push hl
  ld hl,trdos_variable.move_command_blocks
  ld c,(hl)
  ld b,0x00
  pop hl
  sbc hl,bc
  pop bc
  jp nc,l1644h
  ld bc,(trdos_variable.file_type)
  ld hl,0x0000
  ld (trdos_variable.file_type),hl
l15f8h:
  push bc
  ld hl,(trdos_variable.stack_sp_copy)
  push hl
  call sub_14d8h
  ld de,(trdos_variable.filename.4)
  ld b,c
  call read_sectors
  ld hl,(trdos_variable.current_sector)
  ld (trdos_variable.filename.4),hl
  call cls
  ld hl,message.backup_disk
  call print_message_at_HL
  ld hl,l2785h
  call print_message_at_HL
l161dh:
  call get_uppercase_pressed_key
  cp "Y" ; Yes?
  jr nz,l161dh
  call cls_lower
  ld a,(trdos_variable.entered_line_3_first_chars.1)
  or a
  call nz,sub_14f0h
  pop hl
  pop bc
  ld de,(trdos_variable.filename.6)
  ld b,c
  call sub_14e4h
  call write_sectors
  ld hl,(trdos_variable.current_sector)
  ld (trdos_variable.filename.6),hl
  jp sub_15b8h
l1644h:
  ld (trdos_variable.file_type),hl
  push hl
  ld hl,trdos_variable.move_command_blocks
  ld c,(hl)
  ld b,0x00
  pop hl
  xor a
  jp l15f8h
set_z_if_file_descriptor_C_is_a_deleted_file:
  call read_file_descriptor_C
  ld a,(trdos_variable.filename)
  cp 0x01
  ret
read_file_descriptor:
  ld c,a
read_file_descriptor_C:
  xor a
transfer_file_descriptor_C:
  push bc
  call sub_17e9h
  pop bc
  ret
write_file_descriptor:
  ld c,a
  call write_file_descriptor_C
  jp sub_1e43h
write_file_descriptor_C:
  ld a,0xFF
  jr transfer_file_descriptor_C
sub_166fh:
  ld a,0xFF
  ld (trdos_variable.command_mode),a
  call sub_1680h
  ld hl,(sys_worksp)
  ld (trdos_variable.stack_sp_copy),hl
  jp bc_spaces
sub_1680h:
  rst 0x20
  defw rom_free_mem
  ld hl,0xFFFF
  sbc hl,bc
  ld a,h
  cp 0x10
  jr nc,l168fh
  ld a,0x11
l168fh:
  dec a
  ld (trdos_variable.move_command_blocks),a
  ld b,a
  ld c,0x00
  ret
sub_1697h:
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  ld (trdos_variable.stream_or_file_size),hl
  ld de,(trdos_variable.file_length_in_sectors)
  ld hl,(trdos_variable.ch_add)
  ld d,0x00
  add hl,de
  ld (trdos_variable.ch_add),hl
  ret
cli_command.move:
  call pop_hl_if_checking_syntax
  call sub_166fh
  call read_system_track
  ld a,(trdos_sector_buffer.deleted_files)
  or a
  jp z,print_OK_and_exit_DOS
  ld hl,0x0000
  ld (trdos_variable.ch_add),hl
  ld c,0xFF
l16c3h:
  inc c
  call set_z_if_file_descriptor_C_is_a_deleted_file
  jr nz,l16c3h
  ld a,c
  ld (trdos_variable.move_command_deleted_files),a
  ld hl,(trdos_variable.file_first_sector)
  ld (trdos_variable.move_command_first_sector),hl
  call sub_1697h
l16d6h:
  inc c
  call set_z_if_file_descriptor_C_is_a_deleted_file
  jr z,l16d6h
  cp 0x00
  jp nz,l1710h
  ld a,(trdos_variable.move_command_deleted_files)
  ld c,a
l16e5h:
  inc c
  call set_z_if_file_descriptor_C_is_a_deleted_file
  cp 0x00
  jr z,l176ch
  xor a
  ld (trdos_variable.filename),a
  call l1e40h
  call sub_1697h
  jr l16e5h

; BLOCK 'not_used_16F9' (start 0x16f9 end 0x1710)
not_used_16F9_start:
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
l1710h:
  ld a,(trdos_variable.file_length_in_sectors)
  ld (trdos_variable.unknown_23763),a
  ld (trdos_variable.basic_program_autostart),a
  ld hl,(trdos_variable.file_first_sector)
  ld (trdos_variable.move_command_first_sector),hl
  push bc
  call sub_17a5h
  pop bc
  ld hl,(trdos_variable.current_sector)
  ld (trdos_variable.move_command_first_sector),hl
  ld (trdos_variable.file_first_sector),hl
  xor a
  ld (trdos_variable.file_length_in_sectors),a
  ld a,(trdos_variable.filename)
  push af
  ld a,0x01
  ld (trdos_variable.filename),a
  call l1e40h
  pop af
  ld (trdos_variable.filename),a
  ld a,(trdos_variable.move_command_deleted_files)
  ld c,a
  ld hl,(trdos_variable.stream_or_file_size)
  ld (trdos_variable.file_first_sector),hl
  ld a,(trdos_variable.basic_program_autostart)
  ld (trdos_variable.file_length_in_sectors),a
  call l1e40h
  ld a,(trdos_variable.move_command_deleted_files)
  inc a
  ld c,a
  call read_file_descriptor_C
  ld hl,(trdos_variable.move_command_first_sector)
  ld (trdos_variable.file_first_sector),hl
  call l1e40h
  ld a,(trdos_variable.move_command_deleted_files)
  ld c,a
  jp l16c3h
l176ch:
  ld hl,(trdos_variable.stack_sp_copy)
  ld bc,message_interface_1_start
  call reclaim_2
  call read_sector_8_of_track_0
  ld hl,(trdos_sector_buffer.free_sectors)
  ld de,(trdos_variable.ch_add)
  add hl,de
  ld (trdos_sector_buffer.free_sectors),hl
  ld a,(trdos_sector_buffer.files)
  ld hl,trdos_sector_buffer.deleted_files
  sub (hl)
  ld (trdos_sector_buffer.files),a
  ld (hl),0x00
  ld hl,(trdos_variable.move_command_first_sector)
  ld (trdos_sector_buffer.next_free_sector),hl
  push af
  call sub_1e43h
  pop af
  ld c,a
  call read_file_descriptor_C
  xor a
  ld (trdos_variable.filename),a
  jp l0569h
sub_17a5h:
  ld a,(trdos_variable.unknown_23763)
  or a
  ret z
  push hl
  ld hl,trdos_variable.move_command_blocks
  sub (hl)
  pop hl
  jr nc,l17ddh
  ld a,(trdos_variable.unknown_23763)
  ld b,a
  xor a
  ld (trdos_variable.unknown_23763),a
l17bah:
  push bc
  ld hl,(trdos_variable.stack_sp_copy)
  push hl
  ld de,(trdos_variable.move_command_first_sector)
  call read_sectors
  ld hl,(trdos_variable.current_sector)
  ld (trdos_variable.move_command_first_sector),hl
  pop hl
  pop bc
  ld de,(trdos_variable.start_file_parameter_or_number_of_tracks)
  call write_sectors
  ld hl,(trdos_variable.current_sector)
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  jr sub_17a5h
l17ddh:
  ld (trdos_variable.unknown_23763),a
  push hl
  ld hl,trdos_variable.move_command_blocks
  ld b,(hl)
  pop hl
  xor a
  jr l17bah
sub_17e9h:
  push af
  ld hl,trdos_variable.cat_current_sector
  ld (hl),0x00
  ld a,c
l17f0h:
  sub 0x10
  jr c,l17f7h
  inc (hl)
  jr l17f0h
l17f7h:
  add a,0x10
  ld c,a
  push bc
  call read_current_cat_sector_into_tmp_buffer
  pop bc
  pop af
  call convert_dir_entry_C_to_its_address_HL_in_sector_buffer
  ld de,trdos_variable.filename
  ld bc,0x10
  or a
  jr z,l180dh
  ex de,hl
l180dh:
  ldir
  ret
cli_command.verify:
  ld a,0xFF
  ld (trdos_variable.cat_drive_or_read_verify_flag),a
cli_command.gosub:
  call sub_1852h
l1818h:
  call sub_1836h
  call pop_hl_if_checking_syntax
  ld a,0xFF
  ld (trdos_variable.error_code_msb),a
  ld a,(trdos_variable.cat_drive_or_read_verify_flag)
  cp 0xFF
  jp z,print_OK_and_exit_DOS
  ld a,(trdos_variable.file_type)
  cp "B" ; BASIC file type?
  jp z,l012ah
  jp print_OK_and_exit_DOS
sub_1836h:
  call sub_187ah
  call pop_hl_if_checking_syntax
  call sub_18abh
  jp l1921h
sub_1842h:
  ld hl,(sys_ch_add)
  inc hl
  ld a,(hl)
  cp char.carriage_return
  ret z
  ld a,0x01
  ld (trdos_variable.bad_sectors_or_move_command_first_track),a
  call sub_1debh
sub_1852h:
  xor a
  ld (trdos_variable.error_code_msb),a
  ret
sub_1857h:
  call sub_102eh
  ld a,0x42
  cp b
  jr nz,l1866h
  ld hl,(sys_ch_add)
  dec hl
  ld (sys_ch_add),hl
l1866h:
  call sub_1debh
  ld hl,(trdos_variable.ch_add)
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  ld hl,(trdos_variable.stream_or_file_size)
  ld (trdos_variable.ch_add),hl
  xor a
  ld (trdos_variable.bad_sectors_or_move_command_first_track),a
  ret
sub_187ah:
  call z_if_next_command_line_char_is_carriage_return
  jp z,l027bh
  call parse_expression
  call check_error_code_msb
  call nz,sub_1857h
  call get_char
  cp token.code ; `CODE`?
  call z,sub_1842h
  cp token.data ; `DATA`?
  push af
  call check_error_code_msb
  call z,sub_102eh
  pop af
  call z,sub_1bfch
  call pop_hl_if_checking_syntax
  call sub_292fh
sub_18a4h:
  jp nz,print_no_files_error_and_exit_DOS
  call read_file_descriptor_C
  ret
sub_18abh:
  ld a,(trdos_variable.bad_sectors_or_move_command_first_track)
  or a
  ld hl,(trdos_variable.file_start)
  jr z,l18b7h
  ld hl,(trdos_variable.ch_add)
l18b7h:
  ld de,(trdos_variable.file_first_sector)
  cp 0x03
  ld a,(trdos_variable.file_length_in_sectors)
  push de
  ld de,(trdos_variable.file_length_in_bytes)
  jr nz,l18cbh
  ld de,(trdos_variable.stream_or_file_size)
l18cbh:
  ld b,a
  ld (trdos_variable.stream_or_file_size),de
  ld a,(trdos_variable.file_type)
  cp "C" ; code file type?
  ld a,b
  jr nz,l18fdh
  ld a,b
  cp d
  jr z,l18f6h
  dec a
  cp d
  ld a,b
  jr z,l18f6h
  ld a,(trdos_variable.bad_sectors_or_move_command_first_track)
  cp 0x03
  ld a,b
  jr z,l18f6h
  xor a
  ld (trdos_variable.bad_sectors_or_move_command_first_track),a
  ld d,b
  ld e,0x00
  ld (trdos_variable.stream_or_file_size),de
  jr l18f9h
l18f6h:
  call sub_191bh
l18f9h:
  ld a,b
  call sub_1each
l18fdh:
  ld b,a
  ld a,(trdos_variable.file_type)
  cp "C" ; code file type?
  pop de
  ret z
  push de
  cp "B" ; BASIC file type?
  push af
  call z,sub_1a01h
  pop af
  cp "D" ; array file type?
  call z,sub_1a4ch
  call sub_191bh
  ld a,(trdos_variable.file_size.high)
  ld b,a
  pop de
  ret
sub_191bh:
  ld a,0x03
  ld (trdos_variable.bad_sectors_or_move_command_first_track),a
  ret
l1921h:
  call check_error_code_msb
  jr z,l192dh
  push af
  call sub_1a94h
  pop af
  cp 0xFF
l192dh:
  push af
  call z,sub_195ch
  pop af
  jr z,l1937h
  jp write_sectors
l1937h:
  ld a,(trdos_variable.bad_sectors_or_move_command_first_track)
  cp 0x03
  call z,sub_1946h
  ld hl,(sys_e_line)
  dec hl
  ld (hl),0x80
  ret
sub_1946h:
  ld a,(trdos_variable.cat_drive_or_read_verify_flag)
  cp 0xFF
  jp nz,l2fdeh
  ld a,(trdos_variable.stream_or_file_size)
  or a
  ret z
  ld c,a
  ld b,0x01
  ld de,(trdos_variable.current_sector)
  jr l196ah
sub_195ch:
  ld a,(trdos_variable.cat_drive_or_read_verify_flag)
  cp 0xFF
  jp nz,read_sectors
  ld (trdos_variable.current_sector),de
  ld c,0x00
l196ah:
  ld a,b
  or a
  ret z
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  ld (trdos_variable.ch_add),de
l1974h:
  push bc
  ld b,0x01
  ld de,(trdos_variable.ch_add)
  ld hl,trdos_variable.tmp_buffer_address
  call read_sectors
  ld hl,(trdos_variable.current_sector)
  ld (trdos_variable.ch_add),hl
  pop bc
  ld hl,(trdos_variable.start_file_parameter_or_number_of_tracks)
  ld de,trdos_variable.tmp_buffer_address
l198eh:
  ld a,(de)
  cp (hl)
  jr nz,l199dh
  inc hl
  inc de
  dec c
  jr nz,l198eh
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  djnz l1974h
  ret
l199dh:
  ld hl,message.verify_error
  ld a,0x0D
  jp print_error_number_A_whose_message_is_at_HL_and_exit_DOS
cli_command.peek:
  ld a,0xFF
  jr l19abh
cli_command.poke:
  ld a,0xEE
l19abh:
  ld (trdos_variable.error_code_msb),a
  jp l1818h
cli_command.merge:
  ld a,0xFF
  ld (trdos_variable.unknown_23839),a
  call sub_187ah
  call pop_hl_if_checking_syntax
  ld a,(trdos_variable.file_type)
  cp "B" ; BASIC file type?
  jp nz,error.wrong_drive_letter
  ld bc,(trdos_variable.file_start)
  ld (trdos_variable.stream_or_file_size),bc
  push bc
  inc bc
  rst 0x20
  defw rom_bc_spaces
  defb "6"
  defw 0xEB80
  pop de
  push hl
  ld de,(trdos_variable.file_first_sector)
  call sub_191bh
  ld a,(trdos_variable.file_size.high)
  ld b,a
  call sub_1852h
  call l1921h
  pop hl
  ld de,(sys_prog)
  rst 0x20
  defw rom_me_new_lp
  jp print_OK_and_exit_DOS
sub_19f2h:
  ex de,hl
  scf
  sbc hl,de
  ret c
  ld de,0x0009+1
  add hl,de
  ld b,h
  ld c,l
test_room:
  rst 0x20
  defw rom_test_room
  ret
sub_1a01h:
  ld de,(sys_prog)
  ld hl,(sys_e_line)
  dec hl
  push hl
  push de
  sbc hl,de
  ld de,(trdos_variable.file_start)
  push de
  push hl
  ld hl,0x0000
  ld a,(trdos_variable.cat_drive_or_read_verify_flag)
  cp 0xFF
  jr z,l1a20h
  ld hl,0x0004+1
l1a20h:
  add hl,de
  ld (trdos_variable.stream_or_file_size),hl
  pop hl
  ld a,(trdos_variable.cat_drive_or_read_verify_flag)
  cp 0xFF
  jr nz,l1a31h
  pop de
  pop de
  pop hl
  jr l1a48h
l1a31h:
  call sub_19f2h
  pop bc
  pop de
  pop hl
  push bc
  rst 0x20
  defw rom_reclaim_1
  pop bc
  call make_room
  inc hl
  ld bc,(trdos_variable.file_length_in_bytes)
  add hl,bc
  ld (sys_vars),hl
l1a48h:
  ld hl,(sys_prog)
  ret
sub_1a4ch:
  ld de,(trdos_variable.file_length_in_bytes)
  ld (trdos_variable.stream_or_file_size),de
  ld hl,(trdos_variable.start_file_parameter_or_number_of_tracks)
  ld a,(trdos_variable.cat_drive_or_read_verify_flag)
  cp 0xFF
  ret z
  ld hl,(trdos_variable.ch_add)
  push hl
  call sub_19f2h
  pop hl
  ld a,h
  or l
  jr z,l1a79h
  ld hl,(trdos_variable.start_file_parameter_or_number_of_tracks)
  dec hl
  dec hl
  dec hl
  ld bc,(trdos_variable.ch_add)
  inc bc
  inc bc
  inc bc
  call reclaim_2
l1a79h:
  ld hl,(sys_e_line)
  dec hl
  ld bc,(trdos_variable.file_length_in_bytes)
  push bc
  inc bc
  inc bc
  inc bc
  call make_room
  inc hl
  ld a,(trdos_variable.basic_program_autostart.high)
  ld (hl),a
  inc hl
  pop de
  ld (hl),e
  inc hl
  ld (hl),d
  inc hl
  ret
sub_1a94h:
  ld a,(trdos_variable.ch_add)
  ld c,b
  ld b,a
  ld a,c
  cp b
  jr c,l1ab6h
  ld a,b
  or a
  jp z,error.wrong_drive_letter
  dec b
  jr z,l1ab0h
  ld a,0x10
l1aa7h:
  inc e
  cp e
  jr nz,l1aaeh
  ld e,0x00
  inc d
l1aaeh:
  djnz l1aa7h
l1ab0h:
  ld b,0x01
  ld hl,(trdos_variable.start_file_parameter_or_number_of_tracks)
  ret
l1ab6h:
  ld hl,message.cr.rec_overflow
  ld a,0x05
  jp print_error_number_A_whose_message_is_at_HL_and_exit_DOS
sub_1abeh:
  call sub_292fh
  jp z,error.file_exists
sub_1ac4h:
  call read_sector_8_of_track_0
  ld a,(trdos_sector_buffer.files)
  cp 0x80
  jp z,error.directory_full
  ret
cli_command.save:
  call sub_1852h
  ld hl,0x0000
  ld (trdos_variable.basic_program_autostart),hl
  call parse_expression
  call get_char
  cp token.code ; `CODE`?
  jr z,l1b39h
  cp token.line ; `LINE`?
  jr nz,l1af8h
  call sub_1e0bh
  call pop_hl_if_checking_syntax
  ld hl,(trdos_variable.stream_or_file_size)
  ld (trdos_variable.basic_program_autostart),hl
  ld hl,trdos_variable.file_type
  jr l1b1fh
l1af8h:
  cp token.screen_dollar ; `SCREEN$`?
  jr nz,l1b0dh
  ld hl,0x4000
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  ld hl,0x1B00
  ld (trdos_variable.ch_add),hl
  ld (trdos_variable.stream_or_file_size),hl
  jr l1b48h
l1b0dh:
  call pop_hl_if_checking_syntax
  call get_char
  ld hl,trdos_variable.file_type
  cp token.data ; `DATA`?
  jr z,l1b2ch
  cp char.carriage_return
  jp nz,error.wrong_drive_letter
l1b1fh:
  ld (hl),"B" ; file type: BASIC
  call sub_1abeh
  call remove_fp
l1b27h:
  call sub_1bdeh
  jr l1b53h
l1b2ch:
  ld (hl),"D" ; file type: array
  call sub_1abeh
  call sub_1c0fh
  jr nc,l1b53h
  jp c,error.wrong_drive_letter
l1b39h:
  call sub_1de5h
  ld hl,(trdos_variable.ch_add)
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  ld hl,(trdos_variable.stream_or_file_size)
  ld (trdos_variable.ch_add),hl
l1b48h:
  call pop_hl_if_checking_syntax
  ld a,"C" ; file type: code
  ld (trdos_variable.file_type),a
  call sub_1abeh
l1b53h:
  call sub_1b59h
  jp l0569h
sub_1b59h:
  ld hl,(trdos_variable.start_file_parameter_or_number_of_tracks)
  ld (trdos_variable.file_start),hl
  ex de,hl
  ld hl,(trdos_variable.ch_add)
  ld a,l
  or h
  jp z,error.wrong_drive_letter
  ld a,l
  or a
  jr z,l1b6dh
  inc h
l1b6dh:
  ld a,h
  ld (trdos_variable.file_length_in_sectors),a
  ld e,a
  ld d,0x00
  ld hl,(trdos_sector_buffer.free_sectors)
  sbc hl,de
  jp c,error.no_space
  push hl
  ld hl,(sys_e_line)
  ld (hl),0xAA
  inc hl
  ld de,(trdos_variable.basic_program_autostart)
  ld (hl),e
  inc hl
  ld (hl),d
  ld hl,(trdos_variable.stream_or_file_size)
  ld (trdos_variable.file_length_in_bytes),hl
  ld hl,(trdos_sector_buffer.next_free_sector)
  ld (trdos_variable.file_first_sector),hl
  ex de,hl
  ld hl,(trdos_variable.file_start)
  ld a,(trdos_variable.file_length_in_sectors)
  ld b,a
  call write_sectors
  ld hl,(trdos_variable.current_sector)
  push hl
  call read_sector_8_of_track_0
  pop hl
  ld (trdos_sector_buffer.next_free_sector),hl
  pop hl
  ld (trdos_sector_buffer.free_sectors),hl
  ld hl,trdos_sector_buffer.files
  ld a,(hl)
  ld (trdos_variable.find_command_found_file),a
  inc (hl)
  push hl
  call sub_1e43h
  pop hl
  ld c,(hl)
  dec c
  ld a,(trdos_variable.file_type)
  cp "B" ; BASIC file type?
  call z,sub_1bc8h
  ret
sub_1bc8h:
  ld hl,(sys_e_line)
  ld de,(sys_prog)
  scf
  sbc hl,de
  ld (trdos_variable.file_start),hl
  ld hl,(sys_vars)
  sbc hl,de
  ld (trdos_variable.file_length_in_bytes),hl
  ret
sub_1bdeh:
  ld hl,(sys_vars)
  ld de,(sys_prog)
  sbc hl,de
  ld (trdos_variable.stream_or_file_size),hl
  ld hl,(sys_prog)
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  ld hl,(sys_e_line)
  inc hl
  inc hl
  inc hl
  sbc hl,de
  ld (trdos_variable.ch_add),hl
  ret
sub_1bfch:
  call sub_1c1bh
  ret nc
  ld hl,0x0000
  ld (trdos_variable.ch_add),hl
  ld a,(trdos_variable.cat_drive_or_read_verify_flag)
  cp 0xFF
  ret nz
  jp l1c13h
sub_1c0fh:
  call sub_1c1bh
  ret nc
l1c13h:
  ld a,0x0E
  ld hl,message.array_not_found
  jp print_error_number_A_whose_message_is_at_HL_and_exit_DOS
sub_1c1bh:
  call next_char
  call look_vars
  set 7,c
  ld a,c
  ld (trdos_variable.basic_program_autostart.high),a
  jr nc,l1c2bh
l1c29h:
  scf
  ret
l1c2bh:
  jr nz,l1c29h
  inc hl
  ld e,(hl)
  inc hl
  ld d,(hl)
  inc hl
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  ld (trdos_variable.stream_or_file_size),de
  ld (trdos_variable.ch_add),de
  call next_char
  cp 0x29
  jr nz,l1c2bh
  ret
error.no_space:
  ld hl,message.cr.no_space
  ld a,0x03
print_error_number_A_whose_message_is_at_HL_and_exit_DOS:
  call print_error_number_A_whose_message_is_at_HL
  jp exit_dos
error.file_exists:
  ld hl,message.cr.file_exists
  ld a,0x02
  jr print_error_number_A_whose_message_is_at_HL_and_exit_DOS
sub_1c57h:
  ld hl,trdos_variable.filename
  ld b,0x08
l1c5ch:
  ld (hl),0x20
  inc hl
  djnz l1c5ch
  call fetch_drive_letter_from_the_calculator_stack
  ex de,hl
  call parse_possible_drive_letter_in_filename_at_HL
  ld a,c
  or a
  jp z,error.wrong_drive_letter
  cp 0x09
  jr c,l1c73h
  ld c,0x08
l1c73h:
  ld a,(hl)
  cp 0x20
  jp c,error.wrong_drive_letter
  ld de,trdos_variable.filename
  push bc
  ldir
  pop bc
  ret
parse_possible_drive_letter_in_filename_at_HL:
  inc hl
  ld a,(hl)
  cp ":"
  jr nz,no_drive_letter_in_filename_at_HL_so_use_default_drive
  dec hl
  ld a,(hl)
  call check_drive_letter_and_convert_to_number
  push bc
  push hl
  call init_disk_drive_in_A_register
  pop hl
  pop bc
  dec bc
  dec bc
  inc hl
  inc hl
  ret
no_drive_letter_in_filename_at_HL_so_use_default_drive:
  dec hl
  ld a,(trdos_variable.default_drive)
  push bc
  push hl
  call init_disk_drive_in_A_register
  pop hl
  pop bc
  ret
convert_dir_entry_C_to_its_address_HL_in_sector_buffer:
  ld l,c
  ld h,0x00
  add hl,hl
  add hl,hl
  add hl,hl
  add hl,hl
  ld bc,trdos_variable.tmp_buffer_address
  add hl,bc
  ret
sub_1cb0h:
  call sub_1c57h
sub_1cb3h:
  call read_cat_sector_0_into_tmp_buffer
  ld b,0x80
  ld c,0x00
l1cbah:
  push bc
  call convert_dir_entry_C_to_its_address_HL_in_sector_buffer
  call sub_050ch
  pop bc
  push bc
  ld a,c
  cp 0x10
  jr nz,l1ccdh
  pop bc
  ld c,0x00
  jr l1cbah
l1ccdh:
  ld de,trdos_variable.filename
  ld a,(trdos_variable.bytes_compared_by_find_command)
  ld b,a
  xor a
  cp (hl)
  jr nz,l1cdbh
  pop bc
  jr l1ce4h
l1cdbh:
  call sub_2713h
  pop bc
  jr z,l1ce7h
  inc c
  djnz l1cbah
l1ce4h:
  or 0xFF
  ret
l1ce7h:
  ld a,0x80
  sub b
  ld c,a
  ld (trdos_variable.find_command_found_file),a
  xor a
  ret z
find_file:
  call sub_1cb3h
  ld hl,trdos_variable.error_code
  ld (hl),c
  ret z
  ld (hl),0xFF
  ret
cli_command.return:
  call pop_hl_if_checking_syntax
  call sub_20e5h
  res 3,(iy+0x01)
  call sub_0232h
  ld sp,(trdos_variable.sp_copy)
  ld hl,(sys_err_sp)
  dec hl
  ld a,0x12
  cp (hl)
  jr nz,l1d19h
  dec hl
  ld (sys_err_sp),hl
l1d19h:
  ret
error.wrong_drive_letter:
  bit 7,(iy+0x00)
  jr z,l1d25h
  ld a,0x0B ; Error report code: "Nonsense in BASIC"
  ld (sys_err_nr),a
l1d25h:
  inc a
  ld hl,message.error
l1d29h:
  call print_error_number_A_whose_message_is_at_HL
  jp exit_dos
l1d2fh:
  ld a,(sys_err_nr)
  ld hl,message.break
  cp 0x14
  jr z,l1d29h
  cp 0x0C
  jr z,l1d29h
  ld hl,message.out_of_ram
  cp 0x03
  jr z,l1d29h
  ld hl,message.array_not_found
  cp 0x01
  jr z,l1d29h
  jr error.wrong_drive_letter
cli_command.run:
  call sub_1852h
  call sub_1836h
  call pop_hl_if_checking_syntax
  ld hl,(trdos_variable.file_start)
  ld a,(trdos_variable.file_type)
  cp "B" ; BASIC file type?
  jp z,l012ah
  push hl
  ret
sub_1d63h:
  ld hl,trdos_variable.command_mode
  ld a,(hl)
  cp 0xFF
  ld (hl),0x00
  ret nz
  rst 0x20
  defw rom_set_work
  ret
z_if_checking_syntax:
  bit 7,(iy+0x01)
  ret
pop_hl_if_checking_syntax:
  call z_if_checking_syntax
  ret nz
  pop hl
  ret
sub_1d7bh:
  call next_char
  call expt_1num
  jr z_if_checking_syntax
open_channel_0:
  xor a
chan_open:
  rst 0x20
  defw rom_chan_open
  ret
open_channel_2:
  ld a,0x02
  jr chan_open
get_char:
  rst 0x20
  defw rom_get_char
  ret
editor:
  call open_channel_0
  rst 0x20
  defw rom_editor
  ret
cls:
  rst 0x20
  defw rom_cls
  ret
look_vars:
  rst 0x20
  defw rom_look_vars
  ret
cls_lower:
  rst 0x20
  defw rom_cls_lower
  ret
print_number_in_A:
  ld c,a
print_number_in_C:
  ld b,0x00
  jp print_number_in_BC
print_number_in_BC:
  push bc
  call sub_20f1h
  pop bc
  rst 0x20
  defw rom_out_num_1
  call sub_20f1h
  ret
stk_fetch:
  rst 0x20
  defw rom_stk_fetch
  ret
find_int2:
  rst 0x20
  defw rom_find_int2
  ret
expt_exp:
  rst 0x20
  defw rom_expt_exp
  ret
expt_1num:
  rst 0x20
  defw rom_expt_1num
  ret
point_interpreter_to_command_line:
  ld hl,(trdos_variable.command_line_address)
  inc hl
  ld (sys_ch_add),hl
  ret
sub_1dcdh:
  call parse_expression
sub_1dd0h:
  call get_char
  cp 0x2C
  jp nz,error.wrong_drive_letter
  call next_char
  call expt_exp
  ret
parse_expression:
  call point_interpreter_to_command_line
  jp expt_exp
sub_1de5h:
  call get_char
  cp token.code ; `CODE`?
  ret nz
sub_1debh:
  call sub_1d7bh
  jr z,l1dfbh
  call find_int2
  ld (trdos_variable.ch_add),bc
  ld (trdos_variable.stream_or_file_size),bc
l1dfbh:
  call get_char
  cp ","
  jr z,sub_1e0bh
  cp char.carriage_return
  jp nz,error.wrong_drive_letter
  call pop_hl_if_checking_syntax
  ret
sub_1e0bh:
  call sub_1d7bh
  ret z
  call find_int2
  ld (trdos_variable.stream_or_file_size),bc
  ld a,0x03
  ld (trdos_variable.bad_sectors_or_move_command_first_track),a
  ret
remove_fp:
  ld hl,(trdos_variable.command_line_address)
  rst 0x20
  defw rom_remove_fp
  ret
bc_spaces:
  ld hl,(sys_worksp)
  rst 0x20
  defw rom_bc_spaces
  ret
next_char:
  rst 0x20
  defw rom_next_char
  ret
reclaim_2:
  rst 0x20
  defw rom_reclaim_2
  ret
make_room:
  rst 0x20
  defw rom_make_room
  ret
sub_1e36h:
  call sub_3eb2h
  ld a,h
  out (0x3F),a
  ret
read_sectors:
  xor a
  jr transfer_B_sectors_first_sector_DE_address_HL
l1e40h:
  call write_file_descriptor_C
sub_1e43h:
  ld de,(trdos_variable.current_sector)
  dec de
  ld b,0x01
  ld hl,trdos_variable.tmp_buffer_address
write_sectors:
  push hl
  push de
  call sub_3e11h
  bit 7,(hl)
  jr z,l1e60h
  bit 0,(hl)
  jr nz,l1e60h
  ld hl,message.cr.read_only
  jp l1d29h
l1e60h:
  pop de
  pop hl
write_B_sectors_to_sector_DE_from_address_HL:
  ld a,0xFF
transfer_B_sectors_first_sector_DE_address_HL:
  ld (trdos_variable.sector_rw_flag),a
l1e67h:
  ld (trdos_variable.current_sector),de
  push bc
  push hl
  call sub_1e36h
  pop hl
  pop bc
  xor a
  or b
  ret z
l1e75h:
  push bc
  push hl
  call define_buffer_address
  ld a,(trdos_variable.current_sector)
  call set_sector_number
  ld a,(trdos_variable.current_track)
  call seek_track
  ld a,(trdos_variable.sector_rw_flag)
  or a
  push af
  call z,sub_3f0eh
  pop af
  call nz,sub_3f0ah
  pop hl
  ld de,0x0100
  add hl,de
  push hl
  ld a,0x10
  ld hl,trdos_variable.current_sector
  inc (hl)
  cp (hl)
  jr nz,l1ea7h
  ld (hl),0x00
  ld hl,trdos_variable.current_track
  inc (hl)
l1ea7h:
  pop hl
  pop bc
  djnz l1e75h
  ret
sub_1each:
  push hl
  ld h,a
  ld l,0x00
  push hl
  sbc hl,de
  call c,sub_1ebch
  pop hl
  ld a,h
  pop hl
  ret c
  ld a,d
  ret
sub_1ebch:
  xor a
  ld (trdos_variable.bad_sectors_or_move_command_first_track),a
  scf
  ret
cli_command.format:
  ld hl,0xFFFF
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  ld (trdos_variable.ch_add),hl
  ld (trdos_variable.basic_program_autostart),hl
  call z_if_next_command_line_char_is_carriage_return
  jp z,error.wrong_drive_letter
  call parse_expression
  call pop_hl_if_checking_syntax
  call sub_1c57h
  call sub_3e11h
  and 0x80
  ld a,0x28
  jr z,l1ee8h
  ld a,0x50
l1ee8h:
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),a
  call init_interface
  call select_top_side
  call sub_3ea0h
  ld e,0x01
  call sub_1ffdh
  call select_bottom_side
  ld e,0x00
  call sub_1ffdh
  ld a,(trdos_variable.filename)
  cp 0x24
  jr z,l1f1bh
  call select_top_side
  call sub_3ea0h
  call sub_3eb5h
  ld a,h
  cp 0x01
  jr nz,l1f1bh
  ld a,0x80
  ld (trdos_variable.ch_add.high),a
l1f1bh:
  call sub_20bdh
  ld hl,trdos_variable.tmp_buffer_address
  ld (hl),0x00
  ld de,trdos_variable.k_channel.output
  ld bc,0x00FF
  ldir
  ld bc,trdos_variable.start_file_parameter_or_number_of_tracks
  ld de,trdos_variable.ch_add.high
  ld a,(bc)
  cp 0x50
  jr z,l1f49h
  ld a,(de)
  cp 0x80
  jr z,l1f42h
  ld a,0x19
  ld hl,0x0270
  jr l1f55h
l1f42h:
  ld a,0x17
l1f44h:
  ld hl,0x04F0
  jr l1f55h
l1f49h:
  ld a,(de)
  cp 0x80
  ld a,0x18
  jr nz,l1f44h
  ld a,0x16
  ld hl,0x09F0
l1f55h:
  ld (trdos_sector_buffer.disk_type),a
  ld (trdos_sector_buffer.free_sectors),hl
  ld a,0x01
  ld (trdos_sector_buffer.next_free_track),a
  ld a,0x10
  ld (trdos_sector_buffer.sectors_per_track),a
  ld hl,0x5E0F
  ld de,0x5E10
  ld bc,0x0008
  ld (hl),0x20
  ldir
  ld hl,trdos_variable.filename
  ld de,trdos_sector_buffer.disc_title
  ld bc,0x0008
  ldir
  call select_bottom_side
  ld b,0x01
  ld de,0x0008
  ld hl,trdos_variable.tmp_buffer_address
  call write_B_sectors_to_sector_DE_from_address_HL
  ld a,(trdos_variable.bad_sectors_or_move_command_first_track)
  push af
  xor a
  ld (trdos_variable.file_type),a
  ld hl,(trdos_sector_buffer.free_sectors)
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  ld hl,trdos_variable.filename
  rst 0x18
  ld a,0x0D
  rst 0x10
  ld hl,(trdos_variable.start_file_parameter_or_number_of_tracks)
  pop af
  push hl
  ld d,0x00
  ld e,a
  sbc hl,de
  ld b,h
  ld c,l
  call print_number_in_BC
  ld a,0x2F
  rst 0x10
  pop bc
  call print_number_in_BC
  jp exit_dos

; BLOCK 'unknown_table_1FB9' (start 0x1fb9 end 0x1fca)
unknown_table_1FB9_start:
  defb 0x01
l1fbah:
  defb 0x09
  defb 0x02
  defb 0x0A
  defb 0x03
  defb 0x0B
  defb 0x04
  defb 0x0C
  defb 0x05
  defb 0x0D
  defb 0x06
  defb 0x0E
  defb 0x07
  defb 0x0F
  defb 0x08
  defb 0x10
  defb 0x01
sub_1fcah:
  call point_HL_to_step_rate_of_temporary_drive
  or 0x11
  ld b,a
  ld a,0x32
  call vg93_command_B_with_data_A
  ld a,0x02
  call vg93_command_B_with_data_A
  call sub_3dfdh
  in a,(0x1F)
  and 0x04
  ld a,0x50
  jr z,l1fe7h
  ld a,0x28
l1fe7h:
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),a
  ret
select_bottom_side:
  ld a,(trdos_variable.system_register)
  or 0x3C
l1ff0h:
  ld (trdos_variable.system_register),a
  out (0xFF),a
  ret
select_top_side:
  ld a,(trdos_variable.system_register)
  and 0x6F
  jr l1ff0h
sub_1ffdh:
  di
  ld a,0xF4
  out (0x1F),a
  ld hl,unknown_table_1FB9_start
  ld c,0x7F
l2007h:
  ld b,0x0A
  ld d,0x4E
  call sub_20b1h
  ld b,0x0C
  ld d,0x00
  call sub_20b1h
  ld b,0x03
  ld d,0xF5
  call sub_20b1h
  ld d,0xFE
  call sub_20afh
  ld d,e
  call sub_20afh
  ld d,0x00
  call sub_20afh
  ld d,(hl)
  call sub_20afh
  ld d,0x01
  call sub_20afh
  ld d,0xF7
  call sub_20afh
  ld b,0x16
  ld d,0x4E
  call sub_20b1h
  ld b,0x0C
  ld d,0x00
  call sub_20b1h
  ld b,0x03
  ld d,0xF5
  call sub_20b1h
  ld d,0xFB
  call sub_20afh
  ld b,0x00
  ld d,0x00
  call sub_20b1h
  ld d,0xF7
  call sub_20afh
  ld b,0x32
  ld d,0x4E
  call sub_20b1h
  ld a,(hl)
  inc hl
  cp 0x10
  jr nz,l2007h
  ld b,0x00
  call sub_20b1h
  jp m,l2076h
  call sub_20b1h
l2076h:
  in a,(0x1F)
  and 0x40
  jp nz,l3f39h
sub_207dh:
  ld a,(trdos_variable.start_file_parameter.high)
  or a
  ret nz
  ld c,0x7F
  ld a,e
  out (0x3F),a
  ld hl,l1fbah
l208ah:
  ld b,0x03
  ld a,(hl)
  out (0x5F),a
  push hl
l2090h:
  di
  ld a,0x80
  out (0x1F),a
  push bc
  call sub_3fe5h
  in a,(0x1F)
  and 0x7F
  pop bc
  jr z,l20a6h
  djnz l2090h
  ld hl,trdos_variable.bad_sectors_or_move_command_first_track
  inc (hl)
l20a6h:
  pop hl
  ld a,(hl)
  inc hl
  cp 0x01
  jr nz,l208ah
  ei
  ret
sub_20afh:
  ld b,0x01
sub_20b1h:
  in a,(0xFF)
  and 0xC0
  jr z,sub_20b1h
  ret m
  out (c),d
  djnz sub_20b1h
  ret
sub_20bdh:
  ld hl,trdos_variable.start_file_parameter_or_number_of_tracks
  ld b,(hl)
  xor a
  inc hl
  ld (hl),a
  ld e,0xFF
l20c6h:
  push bc
  inc e
  ld a,e
  ld b,0x1B
  call vg93_command_B_with_data_A
  call select_bottom_side
  call sub_1ffdh
  ld a,(trdos_variable.ch_add.high)
  cp 0x80
  jr nz,l20e1h
  call select_top_side
  call sub_1ffdh
l20e1h:
  pop bc
  djnz l20c6h
  ret
sub_20e5h:
  push af
  ld a,(trdos_variable.two_files_drive)
  cp 0xFF
  jr z,l211ch
  pop af
  call sub_2970h
sub_20f1h:
  push af
  ld a,(trdos_variable.interface_1_zone)
  cp 0xF4
  jr z,l211ch
  xor a
  ld hl,trdos_variable.interface_1_flag
  or (hl)
  ld (hl),0xFF
  jr z,l211ch
  ld a,(trdos_variable.buffer_flag)
  or a
  ld hl,trdos_variable.unknown_23747
  ld de,trdos_variable.r_channel.input.high
  jr nz,l2111h
  ld de,0x5E34
l2111h:
  ld b,0x2D
l2113h:
  ld c,(hl)
  ld a,(de)
  ld (hl),a
  ld a,c
  ld (de),a
  inc hl
  inc de
  djnz l2113h
l211ch:
  pop af
  ret
sub_211eh:
  ld hl,(sys_e_line)
  ld (hl),0x0D
  ld (sys_k_cur),hl
  inc hl
  ld (hl),0x80
  ret
sub_212ah:
  ld de,(sys_e_line)
  ld hl,trdos_variable.entered_line_3_first_chars
  call sub_02b0h
  ret
sub_2135h:
  ld a,(trdos_variable.error_code)
  or a
  push af
  call nz,sub_212ah
  pop af
  call z,sub_211eh
  ld hl,(sys_e_line)
  call print_carriage_return
  ld a,(trdos_variable.default_drive)
  add a,"A" ; first drive letter
  rst 0x10
  ld a,0x3E
  rst 0x10
  ld hl,sys_err_nr
  ld (hl),0xFF
  jp editor
sub_2158h:
  call next_char
  call get_char
  cp ","
  jp nz,error.wrong_drive_letter
  ld hl,(trdos_variable.stream_or_file_size)
  ld (trdos_variable.ch_add),hl
  call sub_1e0bh
  call pop_hl_if_checking_syntax
  ld hl,(trdos_variable.stream_or_file_size)
  ld a,h
  or a
  jp nz,error.wrong_drive_letter
  inc hl
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  ld hl,(trdos_variable.ch_add)
  ld (trdos_variable.stream_or_file_size),hl
  ret
cli_command.open_channel:
  ld hl,(trdos_variable.command_line_address)
  ld (sys_ch_add),hl
  call sub_1e0bh
  call sub_1dd0h
l218eh:
  call get_char
  cp "A"
  jr nc,l219ah
  call next_char
  jr l218eh
l219ah:
  cp token.rnd ; random access data file type?
  push af
  call z,sub_2158h
  pop af
  jr z,l21aeh
  and 0xDF
  cp "R" ; sequential access data file type read mode?
  jr z,l21aeh
  cp "W" ; sequential access data file type write mode?
  jp nz,error.wrong_drive_letter
l21aeh:
  ld (trdos_variable.data_file_mode),a
  call pop_hl_if_checking_syntax
  ld a,"#" ; data file type?
  ld (trdos_variable.file_type),a
  ld a,0x00
  ld (trdos_variable.file_start),a
  call sub_21dbh
  push af
  call open_stream_in_variable_stream_or_file_size
  pop af
  push af
  call nz,sub_2288h
  pop af
  call sub_2242h
  ld hl,(trdos_variable.command_line_address)
  ld bc,0x0124
  add hl,bc
  ld (trdos_variable.command_line_address),hl
  jp exit_dos
sub_21dbh:
  ld a,0x0A
  ld (trdos_variable.bytes_compared_by_find_command),a
  call sub_1cb0h
  push af
  call read_system_track
  pop af
  jr nz,l2206h
  ld a,(trdos_variable.data_file_mode)
  cp "R" ; sequential access data file type read mode?
  jr z,l2201h
l21f1h:
  ld hl,trdos_variable.file_start
  inc (hl)
  call sub_1cb3h
  jr z,l21f1h
  ld hl,trdos_variable.file_start
  dec (hl)
  call sub_1cb3h
l2201h:
  call read_file_descriptor_C
  xor a
  ret
l2206h:
  ld a,(trdos_variable.data_file_mode)
  cp "R" ; sequential access data file type read mode?
  ret nz
  jp print_no_files_error_and_exit_DOS
open_stream_in_variable_stream_or_file_size:
  ld a,(trdos_variable.stream_or_file_size)
  rst 0x20
  defw rom_str_data1
  ld a,b
  or c
  jp nz,error.stream_opened
  ret
error.stream_opened:
  ld a,0x19 ; Error report code: "Parameter error" (should be 0x17, invalid stream)
  ld (sys_err_nr),a
  ld hl,message.stream_opened
  ld a,0x0A
jp_print_error_number_A_whose_message_is_at_HL:
  jp print_error_number_A_whose_message_is_at_HL_and_exit_DOS
l2228h:
  ld a,0x0B
  ld hl,message.not_disk_file
  jr jp_print_error_number_A_whose_message_is_at_HL
sub_222fh:
  push hl
  ld c,0x20
  rst 0x28
  ld a,(trdos_variable.start_file_parameter_or_number_of_tracks)
  ld (hl),a
  inc hl
  xor a
  ld (hl),a
  inc hl
  ld (hl),a
  inc hl
  ld (hl),a
  ld a,0x7F
  pop hl
  ret
sub_2242h:
  push af
  call open_stream_in_variable_stream_or_file_size
  ex de,hl
  ld hl,(sys_prog)
  ld bc,(sys_chans)
  sbc hl,bc
  ex de,hl
  ld (hl),e
  inc hl
  ld (hl),d
  call sub_22b2h
  ld a,(trdos_variable.data_file_mode)
  cp token.rnd ; random access data file type?
  call z,sub_222fh
  jr z,l226bh
  ld a,(trdos_variable.data_file_mode)
  cp "R" ; sequential access data file type read mode?
  ld a,0xFF
  jr nz,l226bh
  xor a
l226bh:
  ld (hl),a
  pop af
  jp l2270h
l2270h:
  push af
  ld bc,0x0013+1
  add hl,bc
  push hl
  call sub_2423h
  pop hl
  inc hl
  ld b,0x01
  pop af
  or a
  push af
  call nz,write_sectors
  pop af
  call z,read_sectors
  ret
sub_2288h:
  ld hl,(trdos_variable.start_file_parameter_or_number_of_tracks)
  push hl
  ld hl,0x2000
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  call sub_229ah
  pop hl
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  ret
sub_229ah:
  ld hl,message_interface_1_start
  ld (trdos_variable.ch_add),hl
  call sub_1ac4h
  call sub_1b59h
  ld hl,0x0000
  ld (trdos_variable.file_length_in_bytes),hl
  call write_file_descriptor_C
  jp sub_1e43h
sub_22b2h:
  ld hl,(sys_prog)
  dec hl
  ld (sys_curchl),hl
  push hl
  ld bc,0x0124
  call make_room
  ld a,0x00
  ld b,0x00
l22c4h:
  ld (de),a
  dec de
  djnz l22c4h
  pop hl
  push hl
  ld de,l3d0eh
  ld (hl),e
  inc hl
  ld (hl),d
  inc hl
  ld de,l3d06h
  ld (hl),e
  inc hl
  ld (hl),d
  inc hl
  ld (hl),"D" ; file type: array -- XXX TODO -- confirm
  inc hl
  inc hl
  inc hl
  inc hl
  inc hl
  ld (hl),0x24
  inc hl
  ld (hl),0x01
  inc hl
  ld a,(trdos_variable.current_temporary_drive)
  ld (hl),a
  inc hl
  ld a,(trdos_variable.find_command_found_file)
  ld (hl),a
  inc hl
  ld a,(trdos_variable.data_file_mode)
  cp "R" ; sequential access data file type read mode?
  ld (hl),0x00
  jr z,l22fch
  ld a,(trdos_variable.file_length_in_bytes)
  ld (hl),a
l22fch:
  inc hl
  ld (hl),b
  jr z,l2304h
  ld a,(trdos_variable.file_length_in_bytes.high)
  ld (hl),a
l2304h:
  inc hl
  ex de,hl
  pop hl
  push de
  ld de,0x10
  add hl,de
  ex de,hl
  ld hl,trdos_variable.filename
  ld bc,0x10
  ldir
  pop hl
  ret
sub_2317h:
  ld c,0x0D
  rst 0x28
  ld c,(hl)
  rst 0x28
  ld bc,0x0024
  add hl,bc
  ret
sub_2321h:
  ld c,0x24
l2323h:
  ld b,0x00
  ld hl,(sys_curchl)
  add hl,bc
  ret
l232ah:
  ld c,0x0D
  rst 0x28
  inc (hl)
  ret nz
  push hl
  call sub_2443h
  call sub_23fch
  pop hl
  inc hl
  inc (hl)
  push hl
  call sub_2418h
  pop hl
  ld a,0x10
  cp (hl)
  ret nz
  push hl
  ld c,0x0F
  rst 0x28
  ld a,(hl)
  cp 0x7F
  pop hl
  jr z,l2358h
  ld hl,(sys_curchl)
  call sub_26e1h
  ld c,0x0E
  rst 0x28
  jp l2379h
l2358h:
  call sub_23cfh
  push af
  call z,sub_2418h
  ld c,0x0E
  rst 0x28
  pop af
  call nz,sub_2370h
  ret
sub_2367h:
  ld (hl),0x00
  ld c,0x19
  rst 0x28
  ld d,0x20
  ld e,(hl)
  ret
sub_2370h:
  call sub_2367h
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),de
  jr l2381h
l2379h:
  call sub_2367h
  inc e
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),de
l2381h:
  call sub_229ah
  call sub_2970h
  ld c,0x10
  rst 0x28
  ex de,hl
  ld hl,trdos_variable.filename
  ld bc,0x10
  ldir
  ld c,0x0C
  rst 0x28
  ld a,(trdos_variable.find_command_found_file)
  ld (hl),a
  ret
sub_239bh:
  ld c,0x0D
  rst 0x28
  inc (hl)
  ret nz
  inc hl
  inc (hl)
  push hl
  call sub_2443h
  ld c,0x23
  rst 0x28
  ld a,(hl)
  or a
  jr z,l23b6h
  pop hl
  push hl
  dec (hl)
  call sub_23fch
  pop hl
  push hl
  inc (hl)
l23b6h:
  call sub_2418h
  pop hl
  ld a,0x10
  cp (hl)
  call z,sub_23c1h
  ret
sub_23c1h:
  call sub_23cfh
  push af
  call sub_2970h
  pop af
  jp nz,l2492h
  jp sub_2418h
sub_23cfh:
  ld (hl),0x00
  ld c,0x19
  rst 0x28
  inc (hl)
  ld c,0x10
  rst 0x28
  ld de,trdos_variable.filename
  ld bc,0x10
  ldir
  call sub_1cb3h
  ret nz
sub_23e4h:
  call read_file_descriptor_C
  ld c,0x10
  rst 0x28
  ex de,hl
  ld hl,trdos_variable.filename
  ld bc,0x10
  ldir
  ld c,0x0C
  rst 0x28
  ld a,(trdos_variable.find_command_found_file)
  ld (hl),a
  xor a
  ret
sub_23fch:
  call sub_2423h
  call sub_2321h
  ld b,0x01
  call write_sectors
  ld c,0x0F
  rst 0x28
  ld a,(hl)
  cp 0x7F
  ret z
  call sub_2321h
  xor a
  ld b,a
l2413h:
  ld (hl),a
  inc hl
  djnz l2413h
  ret
sub_2418h:
  call sub_2423h
  call sub_2321h
  ld b,0x01
  jp read_sectors
sub_2423h:
  ld hl,(sys_curchl)
  ld bc,0x001E
  add hl,bc
  ld e,(hl)
  inc hl
  ld d,(hl)
  ld c,0x0E
  rst 0x28
  ld b,(hl)
  dec b
  inc b
  push af
  ld a,0x10
  jr z,l2441h
l2438h:
  inc e
  cp e
  jr nz,l243fh
  ld e,0x00
  inc d
l243fh:
  djnz l2438h
l2441h:
  pop af
  ret
sub_2443h:
  ld c,0x0B
  rst 0x28
  ld a,(hl)
  jp init_disk_drive_in_A_register
l244ah:
  ld hl,trdos_variable.0xc9
  push hl
  ld hl,sub_20f1h
  push hl
  push af
  call sub_20f1h
  ld a,0x0A
  ld (trdos_variable.bytes_compared_by_find_command),a
  pop af
  call sub_24a2h
  push af
  call sub_2650h
  jp z,l249eh
  pop af
  call sub_2317h
  ld (hl),a
  jp l232ah
sub_246eh:
  ld c,0x0D
  rst 0x28
  ld a,(hl)
  ld bc,0x000D+1
  add hl,bc
  cp (hl)
  ret nz
  ld c,0x0E
  rst 0x28
  ld a,(hl)
  ld bc,0x000D+1
  add hl,bc
  cp (hl)
  ret nz
  ld hl,trdos_variable.interface_1_zone
  ld a,(hl)
  cp 0xF4
  jr z,l2492h
  bit 4,(hl)
  jr z,l2492h
  or 0x01
  pop hl
  ret
l2492h:
  ld a,0x07 ; Error report code: "End of file"
l2494h:
  ld (sys_err_nr),a
  call sub_20e5h
  rst 0x20
  defw rom_0058
  ret
l249eh:
  ld a,0x17
  jr l2494h
sub_24a2h:
  ld d,a
  ld c,0x0F
  rst 0x28
  ld a,(hl)
  cp 0x7F
  ld a,d
  ret nz
  ld bc,0x0013
  add hl,bc
  ld a,(hl)
  or a
  ld a,d
  jr nz,l24d5h
  dec hl
  ld a,(hl)
  or a
  jr nz,l24c2h
  push bc
  push hl
  push de
  call sub_24f8h
  pop de
  pop hl
  pop bc
l24c2h:
  ld c,(hl)
  ld a,d
  ex de,hl
  ld hl,(trdos_variable.stack_sp_copy)
  add hl,bc
  cp 0x06
  ld (hl),a
  call z,sub_2504h
  ld c,0x21
  rst 0x28
  inc (hl)
  pop hl
  ret
l24d5h:
  dec hl
  ld a,(hl)
  dec hl
  inc a
  cp (hl)
  inc hl
  inc (hl)
  push hl
  push af
  ld c,0x23
  rst 0x28
  ld (hl),0xFF
  pop af
  pop hl
  jr c,l24eeh
  ld a,d
  cp 0x0D
  jr z,l24f2h
  pop bc
  ret
l24eeh:
  ld a,d
  cp 0x0D
  ret nz
l24f2h:
  xor a
  ld (hl),a
  inc hl
  ld (hl),a
  ld a,d
  ret
sub_24f8h:
  ld hl,(sys_worksp)
  ld (trdos_variable.stack_sp_copy),hl
  ld bc,0x10
  jp bc_spaces
sub_2504h:
  ld (hl),0x0D
  ld hl,(sys_ch_add)
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  ld hl,(trdos_variable.stack_sp_copy)
  ld (sys_ch_add),hl
  ld hl,sys_flags
  res 7,(hl)
  call expt_1num
  ld hl,sys_flags
  set 7,(hl)
  ld hl,(trdos_variable.stack_sp_copy)
  ld (sys_ch_add),hl
  call expt_1num
  call find_int2
  push bc
  pop de
  ld c,0x20
  rst 0x28
  ld b,(hl)
  xor a
  ld hl,0x0000
  ld (trdos_variable.stream_or_file_size),hl
l2538h:
  add hl,de
  jr nc,l2544h
  push hl
  ld hl,(trdos_variable.stream_or_file_size)
  inc hl
  ld (trdos_variable.stream_or_file_size),hl
  pop hl
l2544h:
  djnz l2538h
  ld (trdos_variable.ch_add),hl
  ld a,(trdos_variable.stream_or_file_size)
  ld hl,trdos_variable.ch_add.high
  rrd
  and 0x0F
  ld (trdos_variable.stream_or_file_size),a
  ld hl,(trdos_variable.start_file_parameter_or_number_of_tracks)
  ld (sys_ch_add),hl
  call sub_2568h
  ld c,0x21
  rst 0x28
  ld a,0xFF
  ld (hl),a
  inc hl
  ld (hl),a
  ret
sub_2568h:
  ld c,0x19
  rst 0x28
  ld a,(trdos_variable.ch_add.high)
  cp (hl)
  jp nz,l2584h
  ld c,0x0E
  rst 0x28
  ld a,(trdos_variable.stream_or_file_size)
  cp (hl)
  jp nz,l25a7h
l257ch:
  ld c,0x0D
  rst 0x28
  ld a,(trdos_variable.ch_add)
  ld (hl),a
  ret
l2584h:
  call sub_25cah
  call nz,sub_25c3h
  ld a,(trdos_variable.ch_add.high)
  ld c,0x19
  rst 0x28
  ld (hl),a
  ld c,0x10
  rst 0x28
  ld de,trdos_variable.filename
  ld bc,0x10
  ldir
  call sub_1cb3h
  jp nz,l25d2h
  call sub_23e4h
  jr l25adh
l25a7h:
  call sub_25cah
  call nz,sub_25c3h
l25adh:
  ld a,(trdos_variable.stream_or_file_size)
  ld c,0x0E
  rst 0x28
  ld (hl),a
  push hl
  call sub_2443h
  call sub_2418h
  pop hl
  dec hl
  ld a,(trdos_variable.ch_add)
  ld (hl),a
  jr l257ch
sub_25c3h:
  call sub_2443h
  call sub_23fch
  ret
sub_25cah:
  ld c,0x23
  rst 0x28
  ld a,(hl)
  or a
  ld (hl),0x00
  ret
l25d2h:
  ld hl,(trdos_variable.ch_add.high)
  ld h,0x20
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  ld hl,(trdos_variable.ch_add)
  push hl
  ld hl,(trdos_variable.stream_or_file_size)
  push hl
  call l2381h
  pop hl
  ld (trdos_variable.stream_or_file_size),hl
  pop hl
  ld (trdos_variable.ch_add),hl
  jr l25adh
l25efh:
  call sub_20f1h
  ld hl,sys_tv_flag
  res 3,(hl)
  ld hl,(sys_err_sp)
  ld e,(hl)
  inc hl
  ld d,(hl)
  or a
  ld hl,0x107F
  sbc hl,de
  jr nz,l2626h
  ld sp,(sys_err_sp)
  pop de
  pop de
  ld (sys_err_sp),de
l260fh:
  call sub_262bh
  jr c,l261dh
l2614h:
  ld hl,trdos_variable.0xc9
  push hl
  ld hl,sub_20e5h
  push hl
  ret
l261dh:
  cp char.carriage_return
  jr z,l2614h
  rst 0x20
  defw rom_add_char_keeping_current_mode
  jr l260fh
l2626h:
  call sub_262bh
  jr l2614h
sub_262bh:
  ld a,0x0A
  ld (trdos_variable.bytes_compared_by_find_command),a
  call sub_2650h
  jr z,l2642h
  cp 0x7F
  jp nz,l249eh
  ld bc,0x0013
  add hl,bc
  ld (hl),0x00
  jr l2645h
l2642h:
  call sub_246eh
l2645h:
  call sub_2317h
  ld a,(hl)
  push af
  call sub_239bh
  pop af
  scf
  ret
sub_2650h:
  ld c,0x0F
  rst 0x28
  ld a,(hl)
  or a
  ret
cli_command.close_channel:
  ld hl,(trdos_variable.command_line_address)
  ld (sys_ch_add),hl
  call sub_1e0bh
  call pop_hl_if_checking_syntax
  ld a,(trdos_variable.stream_or_file_size)
  rst 0x20
  defw rom_str_data1
  ld a,b
  or c
  jp z,exit_dos
  push hl
  ld hl,(sys_chans)
  add hl,bc
  ld a,(hl)
  ld hl,l3d0eh
  cp h
  pop hl
  jp nz,l2228h
  ld (hl),0x00
  inc hl
  ld (hl),0x00
  ld (trdos_variable.ch_add),bc
  ld hl,(sys_chans)
  add hl,bc
  dec hl
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  call sub_26ceh
  ld hl,(trdos_variable.start_file_parameter_or_number_of_tracks)
  ld bc,0x0124
  call reclaim_2
  ld hl,sys_strms
  ld b,0x10
l269dh:
  push bc
  ld bc,(trdos_variable.ch_add)
  ld e,(hl)
  inc hl
  ld d,(hl)
  ex de,hl
  sbc hl,bc
  ex de,hl
  jr c,l26bch
  ld d,(hl)
  dec hl
  ld e,(hl)
  inc hl
  push hl
  ex de,hl
  ld bc,0x0124
  sbc hl,bc
  ex de,hl
  pop hl
  ld (hl),d
  dec hl
  ld (hl),e
  inc hl
l26bch:
  inc hl
  pop bc
  djnz l269dh
  ld hl,(trdos_variable.command_line_address)
  ld bc,0x0124
  sbc hl,bc
  ld (trdos_variable.command_line_address),hl
  jp exit_dos
sub_26ceh:
  ld bc,0x000F
  add hl,bc
  ld a,(hl)
  or a
  ret z
  ld hl,(trdos_variable.start_file_parameter_or_number_of_tracks)
  ld (sys_curchl),hl
  call sub_26e1h
  jp sub_23fch
sub_26e1h:
  ld bc,0x000D
  add hl,bc
  ld e,(hl)
  inc hl
  ld d,(hl)
  ld bc,0x000D
  add hl,bc
  ld (hl),e
  inc hl
  ld (hl),d
  ld c,0x10
  rst 0x28
  ld de,trdos_variable.filename
  ld bc,0x10
  ldir
  call sub_2443h
  ld c,0x0C
  rst 0x28
  ld c,(hl)
  call write_file_descriptor_C
  jp sub_1e43h
print_message_at_HL:
  ld a,(hl)
  or a
  ret z
  and 0x7F
  rst 0x10
  bit 7,(hl)
  ret nz
  inc hl
  jr print_message_at_HL
sub_2713h:
  ld a,(de)
  cp (hl)
  ret nz
  inc de
  inc hl
  djnz sub_2713h
  ret
error.no_disk:
  ld hl,message.no_disk
  ld a,0x06
  jp print_error_number_A_whose_message_is_at_HL_and_exit_DOS
error.directory_full:
  ld hl,message.directory_full
  ld a,0x04
error.not_disk_file:
  jp print_error_number_A_whose_message_is_at_HL_and_exit_DOS
set_sys_err_nr_to_tape_loading_error:
  ld a,0x1A ; Error report code: "Tape loading error"
  jr set_sys_err_nr
  ld a,0x12 ; Error report code: "Invalid I/O device"
set_sys_err_nr:
  ld (sys_err_nr),a
  ret
  ld a,0x03 ; Error report code: "Out of memory"
  jr set_sys_err_nr
test_track:
  xor a
  ld (trdos_variable.start_file_parameter.high),a
  ld (trdos_variable.bad_sectors_or_move_command_first_track),a
  in a,(0x1F)
  ld (trdos_variable.disk_drive_ready),a
  ld e,d
  push de
  ld a,e
  out (0x7F),a
  ld a,0x18
  call l3d9ah
  ld a,(trdos_variable.disk_drive_ready)
  and 0x80
  call nz,sub_3ea0h
  pop de
  call sub_207dh
  ld a,(trdos_variable.bad_sectors_or_move_command_first_track)
  or a
  ret z
  ld a,0x07 ; Error code: "Disk errors"
  ld (trdos_variable.error_code),a
  ret
message.ok:
  defb "O"
  defb "."
  defb "K"
  defb "."
  defb 0x00
message.verify_error:
  defb "V"
  defb "e"
  defb "r"
  defb "i"
  defb "f"
  defb "y"
  defb " "
  defb "E"
  defb "r"
  defb "r"
  defb "o"
  defb "r"
  defb "."
  defb 0x8D
message.backup_disk:
  defb "B"
  defb "A"
  defb "C"
  defb "K"
  defb "U"
  defb "P"
  defb " "
  defb "D"
  defb "I"
  defb "S"
  defb "K"
  defb 0x8D
l2785h:
  defb "I"
  defb "n"
  defb "s"
  defb "e"
  defb "r"
  defb "t"
  defb " "
  defb "D"
  defb "e"
  defb "s"
  defb "t"
  defb "i"
  defb "n"
  defb "a"
  defb "t"
  defb "i"
  defb "o"
  defb "n"
  defb " "
  defb "d"
  defb "i"
  defb "s"
  defb "k"
  defb 0x0D
  defb "t"
  defb "h"
  defb "e"
  defb "n"
  defb " "
  defb "p"
  defb "r"
  defb "e"
  defb "s"
  defb "s"
  defb " "
  defb "Y"
  defb 0x00
message.insert_source_disk:
  defb "I"
  defb "n"
  defb "s"
  defb "e"
  defb "r"
  defb "t"
  defb " "
  defb "S"
  defb "o"
  defb "u"
  defb "r"
  defb "c"
  defb "e"
  defb " "
  defb "d"
  defb "i"
  defb "s"
  defb "k"
  defb " "
  defb "t"
  defb "h"
  defb "e"
  defb "n"
  defb " "
  defb "p"
  defb "r"
  defb "e"
  defb "s"
  defb "s"
  defb " "
  defb "Y"
  defb 0x00
message.break:
  defb "*"
  defb "B"
  defb "R"
  defb "E"
  defb "A"
  defb "K"
  defb "*"
  defb 0x8D
message.out_of_ram:
  defb "O"
  defb "u"
  defb "t"
  defb " "
  defb "o"
  defb "f"
  defb " "
  defb "R"
  defb "A"
  defb "M"
  defb 0x8D
message.array_not_found:
  defb "A"
  defb "r"
  defb "r"
  defb "a"
  defb "y"
  defb " "
  defb "n"
  defb "o"
  defb "t"
  defb " "
  defb "f"
  defb "o"
  defb "u"
  defb "n"
  defb "d"
  defb 0x8D
message.directory_full:
  defb "D"
  defb "i"
  defb "r"
  defb "e"
  defb "c"
  defb "t"
  defb "o"
  defb "r"
  defb "y"
  defb " "
  defb "f"
  defb "u"
  defb "l"
  defb "l"
  defb 0x8D
message.no_disk:
  defb "N"
  defb "o"
  defb " "
  defb "d"
  defb "i"
  defb "s"
  defb "k"
  defb 0x8D
message.stream_opened:
  defb "S"
  defb "t"
  defb "r"
  defb "e"
  defb "a"
  defb "m"
  defb " "
  defb "o"
  defb "p"
  defb "e"
  defb "n"
  defb "e"
  defb "d"
  defb 0x8D
message.not_disk_file:
  defb "N"
  defb "o"
  defb "t"
  defb " "
  defb "d"
  defb "i"
  defb "s"
  defb "k"
  defb " "
  defb "f"
  defb "i"
  defb "l"
  defb "e"
  defb 0x8D
message.file_exists:
  defb "F"
  defb "i"
  defb "l"
  defb "e"
  defb " "
  defb "e"
  defb "x"
  defb "i"
  defb "s"
  defb "t"
  defb "s"
  defb 0x0D
  defb "O"
  defb "v"
  defb "e"
  defb "r"
  defb " "
  defb "w"
  defb "r"
  defb "i"
  defb "t"
  defb "e"
  defb "?"
  defb "("
  defb "Y"
  defb "/"
  defb "N"
  defb 0xA9
execute_command_C:
  push af
  push bc
  ld (trdos_variable.de_copy),de
  ld (trdos_variable.hl_copy),hl
  call sub_20f1h
  ld a,0xFF
  ld (trdos_variable.show_screen_flag),a
  ld (trdos_variable.unknown_23839),a
  pop bc
  pop af
  ld hl,finishing_proc_0x0201
  ld (trdos_variable.internal_address_of_finishing_proc),hl
  ld hl,0x0000
  add hl,sp
  ld (trdos_variable.sp_copy),hl
  dec hl
  dec hl
  ld sp,hl
  push af
  call sub_021dh
  ld hl,table.c_commands
l2869h:
  ld a,(hl)
  cp c
  jr nz,l287fh
  pop af
  inc hl
  ld e,(hl)
  inc hl
  ld d,(hl)
  ld hl,exit_dos
  push hl
  push de
  ld hl,(trdos_variable.hl_copy)
  ld de,(trdos_variable.de_copy)
  ret
l287fh:
  cp 0xFF
  jr nz,l2887h
  pop af
  jp exit_dos
l2887h:
  inc hl
  inc hl
  inc hl
  jr l2869h
table.c_commands:
  defb 0x00
  defw init_interface
  defb 0x01
  defw init_disk_drive_in_A_register
  defb 0x02
  defw seek_track
  defb 0x03
  defw set_sector_number
  defb 0x04
  defw define_buffer_address
  defb 0x05
  defw read_sectors
  defb 0x06
  defw write_sectors
  defb 0x07
  defw print_catalogue
  defb 0x08
  defw read_file_descriptor
  defb 0x09
  defw write_file_descriptor
  defb 0x0A
  defw find_file
  defb 0x0B
  defw save_code_file
  defb 0x0C
  defw save_basic_program
  defb 0x0D
  defw exit_dos
  defb 0x0E
  defw load_or_verify
  defb 0x0F
  defw exit_dos
  defb 0x10
  defw exit_dos
  defb 0x11
  defw exit_dos
  defb 0x12
  defw delete_file
  defb 0x13
  defw copy_from_hl_to_file_descriptor
  defb 0x14
  defw copy_from_file_descriptor_to_HL
  defb 0x15
  defw test_track
  defb 0x16
  defw select_bottom_side
  defb 0x17
  defw select_top_side
  defb 0x18
  defw read_system_track
  defb 0xFF
print_catalogue:
  push af
  call read_system_track
  pop af
  jp print_catalogue_to_stream_A
copy_from_hl_to_file_descriptor:
  xor a
  jr l28e5h
copy_from_file_descriptor_to_HL:
  ld a,0xFF
l28e5h:
  ld de,trdos_variable.filename
  ld bc,0x10
  or a
  jr z,l28efh
  ex de,hl
l28efh:
  ldir
  ret
save_basic_program:
  call read_system_track
  call sub_1ac4h
  jp l1b27h
save_code_file:
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  ld (trdos_variable.ch_add),de
  ld (trdos_variable.stream_or_file_size),de
  call read_system_track
  call sub_1ac4h
  jp l1b53h
load_or_verify:
  or a
  ld (trdos_variable.bad_sectors_or_move_command_first_track),a
  ld (trdos_variable.ch_add),hl
  ld (trdos_variable.stream_or_file_size),de
  call sub_1cb3h
  call sub_18a4h
  call sub_18abh
  jp l1921h
delete_file:
  call read_system_track
  call sub_1cb3h
  jp l07a0h
sub_292fh:
  call sub_1c57h
  call read_system_track
  jp sub_1cb3h
print_filename_at_HL:
  push bc
  ld b,0x08
print_filename_letter_at_HL:
  ld a,(hl)
  rst 0x10
  inc hl
  djnz print_filename_letter_at_HL
  ld a,"<"
  rst 0x10
  ld a,(hl)
  rst 0x10
  ld a,">"
  rst 0x10
  pop bc
  ret
allocate_the_sector_buffer:
  push hl
  push de
  push bc
  push af
  ld hl,trdos_variable.buffer_flag
  ld a,(hl)
  or a
  jr z,allocate_the_sector_buffer.exit
  push hl
  ld bc,0x0100+1
  push bc
  call test_room
  pop bc
  pop hl
  ld (hl),0x00
  ld hl,trdos_variable.tmp_buffer_address
  call make_room
  ld hl,(trdos_variable.command_line_address)
  ld bc,0x0100+1
  add hl,bc
  jr l298fh
sub_2970h:
  push hl
  push de
  push bc
  push af
  ld hl,trdos_variable.buffer_flag
  ld a,(hl)
  or a
  jr nz,allocate_the_sector_buffer.exit
  ld (hl),0xFF
  ld hl,trdos_variable.tmp_buffer_address
  ld bc,0x0100+1
  call reclaim_2
  or a
  ld bc,0x0100+1
  ld hl,(trdos_variable.command_line_address)
  sbc hl,bc
l298fh:
  ld (trdos_variable.command_line_address),hl
allocate_the_sector_buffer.exit:
  pop af
  pop bc
  pop de
  pop hl
  ret
cli_command.4:
  xor a
set_number_of_tracks:
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),a
  call pop_hl_if_checking_syntax
  call z_if_next_command_line_char_is_carriage_return
  jp z,error.wrong_drive_letter
  call sub_3e11h
  ld a,(trdos_variable.start_file_parameter_or_number_of_tracks)
  ld (hl),a
  jp print_OK_and_exit_DOS
cli_command.8:
  ld a,0x80
  jr set_number_of_tracks
message.error:
  defb 0x0D
  defb "*"
  defb "E"
  defb "R"
  defb "R"
  defb "O"
  defb "R"
  defb "*"
  defb 0x8D
message.cr.no_space:
  defb 0x0D
  defb "N"
  defb "o"
  defb " "
  defb "s"
  defb "p"
  defb "a"
  defb "c"
  defb "e"
  defb 0x8D
message.cr.file_exists:
  defb 0x0D
  defb "F"
  defb "i"
  defb "l"
  defb "e"
  defb " "
  defb "e"
  defb "x"
  defb "i"
  defb "s"
  defb "t"
  defb "s"
  defb 0x8D
message.cr.free:
  defb " "
  defb "F"
  defb "r"
  defb "e"
  defb "e"
  defb 0x8D
message.cr.read_only:
  defb 0x0D
  defb "R"
  defb "e"
  defb "a"
  defb "d"
  defb " "
  defb "O"
  defb "n"
  defb "l"
  defb 0xF9
message.cr.disc_error:
  defb 0x0D
  defb "D"
  defb "i"
  defb "s"
  defb "c"
  defb " "
  defb "E"
  defb "r"
  defb "r"
  defb "o"
  defb 0xF2
message.cr.rec_overflow:
  defb 0x0D
  defb "R"
  defb "e"
  defb "c"
  defb "."
  defb " "
  defb " "
  defb "O"
  defb "/"
  defb 0xC6
message.cr.title:
  defb "T"
  defb "i"
  defb "t"
  defb "l"
  defb "e"
  defb ":"
  defb 0xA0
message.cr.retry_abort_ignore:
  defb 0x0D
  defb "R"
  defb "e"
  defb "t"
  defb "r"
  defb "y"
  defb ","
  defb "A"
  defb "b"
  defb "o"
  defb "r"
  defb "t"
  defb ","
  defb "I"
  defb "g"
  defb "n"
  defb "o"
  defb "r"
  defb "e"
  defb "?"
  defb 0x00
message.cr.trk:
  defb 0x0D
  defb "T"
  defb "r"
  defb "k"
  defb 0xA0
message.sec:
  defb " "
  defb "s"
  defb "e"
  defb "c"
  defb 0xA0
message.del_file:
  defb " "
  defb "D"
  defb "e"
  defb "l"
  defb "."
  defb " "
  defb "F"
  defb "i"
  defb "l"
  defb "e"
  defb 0x8D
message.cr.no_files:
  defb 0x0D
  defb "N"
  defb "o"
message.files.cr:
  defb " "
  defb "F"
  defb "i"
  defb "l"
  defb "e"
  defb "("
  defb "s"
  defb ")"
  defb 0x8D
  defb 0x00
sub_2a35h:
  ld hl,l2a41h
  ld de,0x4080
  ld bc,0x20
  ldir
  ret
l2a41h:
  ld a,(l03b5h)
  cp 0xF3
  ld a,0x10
  jr z,l2a4bh
  xor a
l2a4bh:
  ld (sys_kstate.1),a
  ld bc,0x7FFD
  ld a,0x10
  out (c),a
  ret
magic_button_nmi_request:
  push af
  push bc
  push de
  push hl
  push ix
  push iy
  exx
  push bc
  push de
  push hl
  ex af,af'
  push af
  ld a,i
  push af
  ld a,r
  push af
  ld hl,0x0000
  add hl,sp
  push hl
  ld a,0x3C
  out (0xFF),a
  ld a,0x3F
  ld i,a
  in a,(0x1F)
  and 0x80
  rrca
  rrca
  rrca
  ld (sys_kstate.1),a
  call sub_2f65h
  call sub_3ea0h
  call sub_3ea0h
  ld de,0x0009+1
  ld hl,0x4000
  push hl
  call sub_2d73h
  ld hl,0x4100
  ld de,0x000B
  call sub_2d73h
  pop hl
  push hl
  ld de,0x0008
  call sub_2f1bh
  ld hl,0x40E3
  ld a,(hl)
  ld (sys_kstate),a
  inc hl
  ld a,(hl)
  inc (hl)
  inc hl
  ld e,(hl)
  inc hl
  ld d,(hl)
  or a
  ex de,hl
  ld de,0x00C0
  sbc hl,de
  ld (0x40E5),hl
  ld hl,0x4000
  ld de,0x0008
  call sub_2d73h
  pop hl
  ld de,(0x40E1)
  push de
  ld de,0x0009+1
  call sub_2f1bh
  pop de
  call sub_2d4ch
  push de
  ld a,0x3C
  out (0xFF),a
  call sub_2f65h
  ld hl,0x4000
  ld de,0x0008
  ld b,0x01
  call sub_2f1bh
  pop de
  ld hl,(0x40E1)
  ld (0x40E1),de
  push hl
  ld hl,0x4000
  ld de,0x0008
  ld b,0x01
  call sub_2d73h
  ld a,(0x40E4)
  dec a
  call sub_2ce5h
  ld (hl),0x40
  inc hl
  ld b,0x07
l2b09h:
  ld (hl),0x20
  inc hl
  djnz l2b09h
  ld (hl),0x43
  pop de
  pop bc
  inc hl
  ld (hl),c
  inc hl
  ld (hl),b
  inc hl
  inc hl
  inc hl
  ld (hl),0xC0
  inc hl
  ld (hl),e
  inc hl
  ld (hl),d
  ld hl,0x4000
  ld de,0x0000
  in a,(0x5F)
  dec a
  ld e,a
  ld b,0x01
  call sub_2d73h
  ld hl,0x0000
  add hl,sp
  ld (0x4140),hl
  ld sp,0x41FF
  call sub_2a35h
  ld hl,0xC000
  xor a
l2b3fh:
  add a,(hl)
  inc hl
  ld b,a
  ld a,h
  or a
  ld a,b
  jr nz,l2b3fh
  ld hl,0x4100
  ld (hl),a
  push hl
  ld hl,l2b58h
  push hl
  ld hl,trdos_entry.call_pushed_address
  push hl
  di
  jp 0x4080
l2b58h:
  pop hl
  ld bc,0x7FFD
  ld a,0xAA
  ld (0x4130),a
  ld d,0x05
  ld a,(sys_kstate.1)
  or d
  ld d,a
  out (c),d
  ld a,(0xC130)
  cp 0xAA
  jp nz,l2c1bh
  ld a,d
  and 0xF8
  ld d,a
  inc hl
  ld b,0x08
l2b79h:
  ld (hl),d
  out (c),d
  xor a
  ld hl,0xC000
l2b80h:
  add a,(hl)
  inc hl
  ld e,a
  ld a,h
  or a
  ld a,e
  jr nz,l2b80h
  ld hl,0x4100
  cp (hl)
  inc hl
  jr z,l2b93h
  inc d
  djnz l2b79h
  dec d
l2b93h:
  ld b,0x08
l2b95h:
  push bc
  call sub_2c37h
  pop bc
  djnz l2b95h
  ld c,0x00
  call sub_2f3ah
  call sub_2d2ah
  ld a,(0x40E4)
  ld (0x4102),a
  inc a
  ld (0x40E4),a
  ld hl,(0x40E5)
  ld de,0x0001
  sbc hl,de
  ld (0x40E5),hl
  ret c
  ld hl,(0x40E1)
  ld (0x411E),hl
  call sub_2d1eh
  ld a,0x38
  ld (0x4111),a
  ld a,0x01
  ld (0x411D),a
  ld hl,0x4100
  ld (0x4119),hl
  ld hl,0x0100
  ld (0x411B),hl
  ld de,(0x40E1)
  call sub_2f65h
  ld c,d
  call sub_2f3ah
  ld hl,0x4100
  ld b,0x01
  call sub_2d58h
  ld (0x40E1),de
  ld c,0x00
  call sub_2f3ah
  call sub_2d34h
  ld a,(0x4102)
  call sub_2ce5h
  ld de,0x4110
  ld bc,0x10
  ex de,hl
  ldir
  in a,(0x5F)
  dec a
  ld e,a
  ld d,0x00
  ld hl,0x4000
  call sub_2d73h
  ld bc,0x7FFD
  ld a,(0x4101)
  out (c),a
l2c1bh:
  ld hl,(0x4140)
  ld sp,hl
  ld hl,0x4000
  ld de,0x0009+1
  call sub_2f1bh
  ld hl,0x4100
  ld de,0x000B
  call sub_2f1bh
  ld a,0x3C
  push af
  jp l2ebch
sub_2c37h:
  ld a,b
  dec a
  ld (0x4103),a
  ld b,a
  ld a,(0x4101)
  and 0x07
  cp b
  ret z
  ld a,0x02
  cp b
  ret z
  ld a,(0x4101)
  and 0x08
  jr z,l2c55h
  ld a,b
  cp 0x07
  ret z
  jr l2c59h
l2c55h:
  ld a,b
  cp 0x05
  ret z
l2c59h:
  call sub_2c5dh
  ret
sub_2c5dh:
  ld hl,0x4101
  ld a,(hl)
  and 0xF8
  ld c,a
  ld a,b
  or c
  push bc
  ld bc,0x7FFD
  out (c),a
  pop bc
  ld hl,0xC000
l2c70h:
  ld a,(hl)
  or a
  jr nz,l2c7ah
  inc hl
  ld a,h
  or a
  jr nz,l2c70h
  ret
l2c7ah:
  call sub_2c7eh
  ret
sub_2c7eh:
  ld c,0x00
  call sub_2f3ah
  call sub_2d2ah
  ld a,(0x40E4)
  ld (0x4102),a
  inc a
  ld (0x40E4),a
  ld hl,(0x40E5)
  ld de,0x0040
  sbc hl,de
  ld (0x40E5),hl
  ret c
  ld hl,(0x40E1)
  ld (0x411E),hl
  call sub_2d1eh
  ld a,0x40
  ld (0x411D),a
  ld hl,0xC000
  ld (0x4119),hl
  ld hl,0x4000
  ld (0x411B),hl
  ld de,(0x40E1)
  call sub_2d3eh
  ld (0x40E1),de
  ld c,0x00
  call sub_2f3ah
  call sub_2d34h
  ld a,(0x4102)
  call sub_2ce5h
  ld de,0x4110
  ld bc,0x10
  ex de,hl
  ldir
  in a,(0x5F)
  dec a
  ld e,a
  ld d,0x00
  ld hl,0x4000
  call sub_2d73h
  ret
sub_2ce5h:
  ld c,a
  and 0xF0
  rrca
  rrca
  rrca
  rrca
  ld b,a
  push bc
  ld e,b
  ld d,0x00
  ld hl,0x4000
  push de
  call sub_2f1bh
  pop de
  pop bc
  ld b,0x00
  ld a,c
  and 0x0F
  rlca
  rlca
  rlca
  rlca
  ld hl,0x4000
  add a,l
  ld l,a
  ret
sub_2d09h:
  ld hl,0x4110
  ld b,0x09
l2d0eh:
  ld (hl),0x20
  inc hl
  djnz l2d0eh
  ld a,0x40
  ld (0x4110),a
  ld a,0x43
  ld (0x4118),a
  ret
sub_2d1eh:
  call sub_2d09h
  ld a,(0x4103)
  add a,0x30
  ld (0x4111),a
  ret
sub_2d2ah:
  ld hl,0x4000
  ld de,0x0008
  call sub_2f1bh
  ret
sub_2d34h:
  ld hl,0x4000
  ld de,0x0008
  call sub_2d73h
  ret
sub_2d3eh:
  call sub_2f65h
  ld c,d
  call sub_2f3ah
  ld hl,0xC000
  ld b,0x40
  jr sub_2d58h
sub_2d4ch:
  call sub_2f65h
  ld c,d
  call sub_2f3ah
  ld hl,0x4000
  ld b,0xC0
sub_2d58h:
  push bc
  push de
  call sub_2d73h
  ld de,0x0100
  add hl,de
  pop de
  inc e
  ld a,e
  cp 0x10
  jr nz,l2d6fh
  ld e,0x00
  inc d
  ld c,d
  call sub_2f3ah
l2d6fh:
  pop bc
  djnz sub_2d58h
  ret
sub_2d73h:
  ld a,e
  inc a
  out (0x5F),a
  push hl
  ld d,0x14
  push de
l2d7bh:
  di
  ld c,0x7F
  ld a,0xA0
  out (0x1F),a
  call sub_3fcah
  pop de
  pop hl
  in a,(0x1F)
  and 0x7F
  ret z
  dec d
  push hl
  push de
  jr nz,l2d7bh
  halt
read_all_file_sectors:
  ld hl,(trdos_variable.file_start)
  ld de,(trdos_variable.file_first_sector)
  ld a,(trdos_variable.file_length_in_sectors)
  ld b,a
  call read_sectors
  ret
cli_command.go_to:
  call parse_expression
  call pop_hl_if_checking_syntax
  call sub_102eh
  call sub_1c57h
  ld (0x4020),bc
  ld a,c
  cp 0x08
  jr nc,l2dd2h
  call read_system_track
  call sub_1cb3h
  jp nz,print_no_files_error_and_exit_DOS
  ld hl,trdos_variable.filename
  ld bc,(0x4020)
  add hl,bc
  ld (hl),0x38
  push hl
  call sub_1cb3h
  pop hl
  jr z,l2dd8h
  ld (hl),0x20
l2dd2h:
  call sub_1cb3h
  jp l2e33h
l2dd8h:
  call read_file_descriptor_C
  call read_all_file_sectors
  ld sp,0x40FF
  ld b,0x08
l2de3h:
  push bc
  ld a,b
  ld bc,0x7FFD
  dec a
  push af
  or 0x10
  out (c),a
  pop af
  add a,0x30
  ld hl,trdos_variable.filename
  ld bc,(0x4020)
  add hl,bc
  ld (hl),a
  call sub_1cb3h
  jr nz,l2e05h
  call read_file_descriptor_C
  call read_all_file_sectors
l2e05h:
  pop bc
  djnz l2de3h
  ld a,0x20
  ld hl,trdos_variable.filename
  ld bc,(0x4020)
  add hl,bc
  ld (hl),a
  call sub_1cb3h
  call read_file_descriptor_C
  push bc
  push af
  ld bc,0x7FFD
  ld a,(0x4101)
  out (c),a
  pop af
  pop bc
  jr l2e39h
  call parse_expression
  call pop_hl_if_checking_syntax
  call sub_102eh
  call sub_292fh
l2e33h:
  jp nz,print_no_files_error_and_exit_DOS
  call read_file_descriptor_C
l2e39h:
  ld a,(trdos_variable.filename)
  cp 0x24
  di
  jr nz,l2e43h
  im 2
l2e43h:
  ld sp,0x40F0
  call sub_3e11h
  ld (0x4010),a
  ld a,(trdos_variable.system_register)
  ld (0x4011),a
  ld hl,(trdos_variable.file_start)
  push hl
  ld de,(trdos_variable.file_first_sector)
  push de
  inc e
  ld a,e
  cp 0x10
  jr nz,l2e64h
  ld e,0x00
  inc d
l2e64h:
  ld c,d
  call sub_2f07h
  ld a,(0x4010)
  and 0x02
  call nz,sub_2f0fh
  ld a,c
  call sub_2f50h
  ld hl,0x4100
  ld b,0xBF
l2e79h:
  push bc
  push de
  call sub_2f1bh
  ld de,0x0100
  add hl,de
  pop de
  inc e
  ld a,e
  cp 0x10
  jr nz,l2e9ch
  ld e,0x00
  inc d
  ld c,d
  call sub_2f07h
  ld a,(0x4010)
  and 0x02
  call nz,sub_2f0fh
  ld a,c
  call sub_2f50h
l2e9ch:
  pop bc
  djnz l2e79h
  pop de
  pop hl
  ld sp,hl
  ld a,(0x4011)
  push af
  ld c,d
  call sub_2f07h
  ld a,(0x4010)
  and 0x02
  call nz,sub_2f0fh
  ld a,c
  call sub_2f50h
  ld hl,0x4000
  call sub_2f1bh
l2ebch:
  pop af
  ex af,af'
  pop af
  ld r,a
  pop af
  ld i,a
  di
  ld a,0xFF
  jp po,l2ecch
  ld a,0x00
l2ecch:
  ld (sys_kstate),a
  pop af
  pop hl
  pop de
  pop bc
  exx
  ex af,af'
  pop iy
  pop ix
  pop hl
  pop de
  pop bc
  ld a,(sys_bordcr)
  and 0x38
  rrca
  rrca
  rrca
  out (0xFE),a
  ld a,(0x5B08)
  cp 0xEE
  jr nz,l2ef7h
  push bc
  ld bc,0x7FFD
  ld a,(sys_bankm)
  out (c),a
  pop bc
l2ef7h:
  ld a,(sys_kstate)
  or a
  ld a,0xC9
  ld (sys_kstate),a
  jr nz,l2f03h
  ei
l2f03h:
  pop af
  jp sys_kstate
sub_2f07h:
  ld a,(0x4011)
  or 0x3C
l2f0ch:
  out (0xFF),a
  ret
sub_2f0fh:
  ld a,c
  or a
  rra
  ld c,a
  ret nc
  ld a,(0x4011)
  and 0x6F
  jr l2f0ch
sub_2f1bh:
  ld a,e
  inc a
  out (0x5F),a
  push hl
  ld d,0x14
  push de
l2f23h:
  di
  ld c,0x7F
  ld a,0x80
  out (0x1F),a
  call sub_3fe5h
  pop de
  pop hl
  in a,(0x1F)
  and 0x7F
  ret z
  dec d
  push hl
  push de
  jr nz,l2f23h
  halt
sub_2f3ah:
  ld a,0x3C
  out (0xFF),a
  ld a,(sys_kstate)
  and 0x08
  jr nz,l2f4fh
  ld a,c
  or a
  rra
  ld c,a
  jr nc,l2f4fh
  ld a,0x2C
  out (0xFF),a
l2f4fh:
  ld a,c
sub_2f50h:
  out (0x7F),a
  call sub_3dfdh
  ld a,0x18
l2f57h:
  out (0x1F),a
l2f59h:
  in a,(0xFF)
  and 0x80
  jr z,l2f59h
  push bc
  call sub_3dfdh
  pop bc
  ret
sub_2f65h:
  ld a,0x08
  jr l2f57h
l2f69h:
  ld hl,(trdos_variable.sp_copy)
  dec hl
  dec hl
  ld sp,hl
  jp l1d2fh
call_zx_spectrum_rom_routine_stored_at_the_address_on_tos:
  ld (trdos_variable.hl_copy),hl
  ld (trdos_variable.de_copy),de
  pop hl
  ld e,(hl)
  inc hl
  ld d,(hl)
  inc hl
  push hl
  ld hl,trdos_entry.call_pushed_address
  push hl
  push de
  ld hl,trdos_variable.0xc9
  push hl
  ld hl,(trdos_variable.hl_copy)
  ld de,(trdos_variable.de_copy)
  ret
init_main_variables:
  ld hl,0xFFFF
  ld (trdos_variable.drive_a_step_rate),hl
  ld (trdos_variable.drive_c_step_rate),hl
  ld (trdos_variable.drive_a_mode),hl
  ld (trdos_variable.drive_c_mode),hl
  xor a
  ld (trdos_variable.show_title_flag),a
  ld (trdos_variable.default_drive),a
  ld (trdos_variable.interface_1_flag),a
  ld (trdos_variable.error_code),a
  ld (trdos_variable.unknown_23839),a
  ld a,0xFF
  out (0xFF),a
  ld (sys_err_nr),a
  ld (trdos_variable.system_register),a
  ld (trdos_variable.buffer_flag),a
  ld a,0xC9
  ld (trdos_variable.0xc9),a
  ld a,0xD0
  out (0x1F),a
  ret
l2fc6h:
  call sub_050ch
  ld a,(hl)
  or a
  jp z,l03ach
  cp 0x01
  call z,sub_0507h
  ret nz
  jr l2fc6h
sub_2fd6h:
  ld b,0x01
  ld hl,trdos_variable.tmp_buffer_address
  jp l1e67h
l2fdeh:
  push hl
  ld de,(trdos_variable.current_sector)
  call sub_2fd6h
  ld a,(trdos_variable.stream_or_file_size)
  pop de
  or a
  ret z
  ld c,a
  ld hl,trdos_variable.tmp_buffer_address
  defb 0xED,0xB0,0xC9

; BLOCK 'token_commands_table' (start 0x2ff3 end 0x3008)
token_commands_table_start:
  defb token.cat
  defb "*" ; set current drive
  defb token.format
  defb token.move
  defb token.new
  defb token.erase
  defb token.load
  defb token.save
  defb token.return
  defb token.peek
  defb token.poke
  defb token.merge
  defb token.run
  defb token.open_channel
  defb token.close_channel
  defb token.copy
  defb "4" ; set 40 tracks
  defb token.go_to
  defb "8" ; set 80 tracks
  defb token.list
  defb token.verify

; BLOCK 'token_commands_addresses_table' (start 0x3008 end 0x3032)
token_commands_addresses_table_start:
  defw cli_command.cat
  defw cli_command.asterisk
  defw cli_command.format
  defw cli_command.move
  defw cli_command.new
  defw cli_command.erase
  defw cli_command.gosub
  defw cli_command.save
  defw cli_command.return
  defw cli_command.peek
  defw cli_command.poke
  defw cli_command.merge
  defw cli_command.run
  defw cli_command.open_channel
  defw cli_command.close_channel
  defw cli_command.copy
  defw cli_command.4
  defw cli_command.go_to
  defw cli_command.8
  defw cli_command.list
  defw cli_command.verify
sub_3032h:
  ld hl,(sys_e_line)
  ld (trdos_variable.command_line_address),hl
  ld (trdos_variable.ch_add),hl
  ld a,0xFF
  ld (trdos_variable.bad_sectors_or_move_command_first_track),a
  ld hl,trdos_variable.stream_or_file_size
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
  jr l3057h
sub_3048h:
  ld (trdos_variable.command_line_address),hl
  ld (trdos_variable.ch_add),hl
  call sub_30e1h
  ret nz
  inc hl
  inc hl
  ld (trdos_variable.start_file_parameter_or_number_of_tracks),hl
l3057h:
  call sub_30a9h
  jr nz,l3087h
  ex de,hl
  inc de
  ld b,0x00
  ld hl,unknown_table_31C8_start
  add hl,bc
  ld a,(hl)
  ld hl,(trdos_variable.ch_add)
  ld (hl),a
  inc hl
  ex de,hl
  rst 0x20
  defw rom_differ
  push bc
  rst 0x20
  defw rom_reclaim_2
  pop bc
  ld a,(trdos_variable.bad_sectors_or_move_command_first_track)
  or a
  jr nz,l3087h
  ld hl,(trdos_variable.start_file_parameter_or_number_of_tracks)
  ld e,(hl)
  inc hl
  ld d,(hl)
  ex de,hl
  or a
  sbc hl,bc
  ex de,hl
  ld (hl),d
  dec hl
  ld (hl),e
l3087h:
  ld hl,(trdos_variable.ch_add)
  ld a,(hl)
  cp char.carriage_return
  ret z
  inc hl
  ld (trdos_variable.ch_add),hl
  ld a,(hl)
  cp char.carriage_return
  ret z
  cp '"'
  jr nz,l3057h
l309ah:
  inc hl
  ld a,(hl)
  cp char.carriage_return
  ret z
  cp '"'
  jr nz,l309ah
  inc hl
  ld (trdos_variable.ch_add),hl
  jr l3057h
sub_30a9h:
  ld hl,(trdos_variable.ch_add)
  ld de,tokens_start
  ld c,0x00
l30b1h:
  ld a,(hl)
  and 0xDF
  ld b,a
  or a
  jr nz,l30bbh
  inc hl
  jr l30b1h
l30bbh:
  ld a,(de)
  and 0x80
  jr nz,l30c8h
  ld a,(de)
  cp b
  jr nz,l30d9h
  inc hl
  inc de
  jr l30b1h
l30c8h:
  ld a,(de)
  and 0x7F
  cp b
  ret z
l30cdh:
  inc c
  ld hl,(trdos_variable.ch_add)
  inc de
  ld a,(de)
  cp 0xFF
  jr nz,l30b1h
  or a
  ret
l30d9h:
  inc de
  ld a,(de)
  and 0x80
  jr z,l30d9h
  jr l30cdh
sub_30e1h:
  ld hl,(sys_ppc)
  inc hl
  inc hl
  ld a,h
  or l
  jr z,l30f4h
  xor a
  ld (trdos_variable.bad_sectors_or_move_command_first_track),a
  dec hl
  dec hl
  rst 0x20
  defw rom_line_addr
  ret
l30f4h:
  ld a,0xFF
  ld (trdos_variable.bad_sectors_or_move_command_first_track),a
  ld hl,(sys_e_line)
  ret

; BLOCK 'tokens' (start 0x30fd end 0x31c7)
tokens_start:
  defb "S"
  defb "A"
  defb "V"
  defb "E"
  defb 0x80
  defb "S"
  defb "A"
  defb "V"
  defb 0xC5
  defb "L"
  defb "O"
  defb "A"
  defb "D"
  defb 0x80
  defb "L"
  defb "O"
  defb "A"
  defb 0xC4
  defb "R"
  defb "U"
  defb "N"
  defb 0x80
  defb "R"
  defb "U"
  defb 0xCE
  defb "C"
  defb "A"
  defb "T"
  defb 0x80
  defb "C"
  defb "A"
  defb 0xD4
  defb "E"
  defb "R"
  defb "A"
  defb "S"
  defb "E"
  defb 0x80
  defb "E"
  defb "R"
  defb "A"
  defb "S"
  defb 0xC5
  defb "N"
  defb "E"
  defb "W"
  defb 0x80
  defb "N"
  defb "E"
  defb 0xD7
  defb "M"
  defb "O"
  defb "V"
  defb "E"
  defb 0x80
  defb "M"
  defb "O"
  defb "V"
  defb 0xC5
  defb "M"
  defb "E"
  defb "R"
  defb "G"
  defb "E"
  defb 0x80
  defb "M"
  defb "E"
  defb "R"
  defb "G"
  defb 0xC5
  defb "P"
  defb "E"
  defb "E"
  defb "K"
  defb 0x80
  defb "P"
  defb "E"
  defb "E"
  defb 0xCB
  defb "P"
  defb "O"
  defb "K"
  defb "E"
  defb 0x80
  defb "P"
  defb "O"
  defb "K"
  defb 0xC5
  defb "O"
  defb "P"
  defb "E"
  defb "N"
  defb 0x83
  defb "C"
  defb "L"
  defb "O"
  defb "S"
  defb "E"
  defb 0x83
  defb "C"
  defb "O"
  defb "D"
  defb "E"
  defb 0x80
  defb "C"
  defb "O"
  defb "D"
  defb 0xC5
  defb "R"
  defb "N"
  defb "D"
  defb 0x80
  defb "R"
  defb "N"
  defb 0xC4
  defb "D"
  defb "A"
  defb "T"
  defb "A"
  defb 0x80
  defb "D"
  defb "A"
  defb "T"
  defb 0xC1
  defb "S"
  defb "C"
  defb "R"
  defb "E"
  defb "E"
  defb "N"
  defb 0x04
  defb 0x84
  defb "S"
  defb "C"
  defb "R"
  defb "E"
  defb "E"
  defb "N"
  defb 0x84
  defb "C"
  defb "O"
  defb "P"
  defb "Y"
  defb 0x80
  defb "C"
  defb "O"
  defb "P"
  defb 0xD9
  defb "F"
  defb "O"
  defb "R"
  defb "M"
  defb "A"
  defb "T"
  defb 0x80
  defb "F"
  defb "O"
  defb "R"
  defb "M"
  defb "A"
  defb 0xD4
  defb "G"
  defb "O"
  defb "T"
  defb "O"
  defb 0x80
  defb "G"
  defb "O"
  defb "T"
  defb 0xCF
  defb "L"
  defb "I"
  defb "S"
  defb "T"
  defb 0x80
  defb "L"
  defb "I"
  defb "S"
  defb 0xD4
  defb "L"
  defb "I"
  defb "N"
  defb "E"
  defb 0x80
  defb "L"
  defb "I"
  defb "N"
  defb 0xC5
  defb "V"
  defb "E"
  defb "R"
  defb "I"
  defb "F"
  defb "Y"
  defb 0x80
  defb "V"
  defb "E"
  defb "R"
  defb "I"
  defb "F"
  defb 0xD9
  defb 0xFF

; BLOCK 'not_used_31C7' (start 0x31c7 end 0x31c8)
not_used_31C7_start:
  defb 0xFF

; BLOCK 'unknown_table_31C8' (start 0x31c8 end 0x31f3)
unknown_table_31C8_start:
  defb 0xF8
  defb 0xF8
  defb 0xEF
  defb 0xEF
  defb 0xF7
  defb 0xF7
  defb 0xCF
  defb 0xCF
  defb 0xD2
  defb 0xD2
  defb 0xE6
  defb 0xE6
  defb 0xD1
  defb 0xD1
  defb 0xD5
  defb 0xD5
  defb 0xBE
  defb 0xBE
  defb 0xF4
  defb 0xF4
  defb 0xD3
  defb 0xD4
  defb 0xAF
  defb 0xAF
  defb 0xA5
  defb 0xA5
  defb 0xE4
  defb 0xE4
  defb 0xAA
  defb 0xAA
  defb 0xFF
  defb 0xFF
  defb 0xD0
  defb 0xD0
  defb 0xEC
  defb 0xEC
  defb 0xF0
  defb 0xF0
  defb 0xCA
  defb 0xCA
  defb 0xD6
  defb 0xD6
  defb 0x00
sub_31f3h:
  ld hl,(sys_chans)
  or a
  ld bc,trdos_variable.tmp_buffer_address

  sbc hl,bc
  ret

; BLOCK 'not_used_31FD' (start 0x31fd end 0x3cfa)
not_used_31FD_start:
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0x18
  defb 0x03
  defb 0xFF
  defb 0x18
  defb 0x03
  defb 0xC3
  defb 0x00
  defb "="
  defb 0xC3
  defb 0x03
  defb "="
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
l3cfah:
  jp sub_20f1h
jp_execute_command_C:
  jp execute_command_C
trdos_entry.enter_trdos_from_basic:
  nop
  jr enter_trdos_from_basic
trdos_entry.interpret_rem_command:
  nop
  jr interpret_rem_command_in_basic
l3d06h:
  nop
  jp l25efh
l3d0ah:
  jp l244ah
  nop
l3d0eh:
  jr l3d0ah
  nop
  jr l3cfah
trdos_entry.execute_command_in_c:
  nop
  jr jp_execute_command_C
l3d16h:
  nop
  jp l2f69h
interpret_rem_command_in_basic:
  call entry.init_dos
  push hl
  jp do_interpret_rem_command_in_basic
entry.init_dos:
  call sub_31f3h
  nop
  nop
  call c,sub_3d4ch
  ld hl,trdos_variable.0xc9
  ret
  nop
  nop
trdos_entry.call_pushed_address:
  nop
  ret
enter_trdos_from_basic:
  call entry.init_dos
  push hl
  jp l0239h
l3d38h:
  xor a
  out (0xF7),a
  in a,(0xF7)
  cp 0x1E
  jr z,l3d44h
  cp 0x1F
  ret nz
l3d44h:
  rst 8
  ld sp,0x013E
  ld (trdos_variable.file_2_length_in_bytes),a
  ret
sub_3d4ch:
  xor a
  out (0xFF),a
  in a,(0xF6)
  ld hl,l3d38h
  ld de,sys_membot
  ld bc,0x0013+1
  ldir
  ld hl,l3d67h
  push hl
  ld hl,trdos_entry.call_pushed_address
  push hl
  jp sys_membot
l3d67h:
  ld hl,init_main_variables
  push hl
  ld hl,trdos_entry.call_pushed_address
  push hl
  ld hl,set_z_if_file_descriptor_C_is_a_deleted_file+2
  push hl
  ld hl,sys_tstack
  push hl
  ld (hl),0xC9
  ld hl,sys_p_ramt.high
  ld bc,0x0070
  ret
print_carriage_return:
  ld a,0x0D
print_character_A:
  push hl
  push bc
  push de
  push af
  call sub_20f1h
  pop af
  call print_a
  call sub_20f1h
  pop de
  pop bc
  pop hl
  ret
print_a:
  rst 0x20
  defw rom_print_a
  ret
init_interface:
  ld a,0x08
l3d9ah:
  out (0x1F),a
l3d9ch:
  push hl
  rst 0x20
  defw rom_break_key
  jr c,break_key_not_pressedt
  rst 0x20
  defw rom_report_l ; Error report: "BREAK into program"
break_key_not_pressedt:
  pop hl
  in a,(0xFF)
  and 0x80
  jr z,l3d9ch
  ret
sub_3dadh:
  ld a,0x08
  call l3d9ah
  ld de,0x0000
  in a,(0x1F)
  and 0x02
  ld b,a
l3dbah:
  in a,(0x1F)
  and 0x02
  cp b
  ret nz
  inc de
  ld a,e
  or d
  jr nz,l3dbah
  jp report_tape_loading_error_and_issue_no_disk_error
init_default_disk_drive:
  ld a,(trdos_variable.default_drive)
init_disk_drive_in_A_register:
  ld (trdos_variable.current_temporary_drive),a
  ld hl,trdos_variable.system_register
  ld c,a
  ld a,0x3C
  or c
  out (0xFF),a
  ld (hl),a
  call point_HL_to_step_rate_of_temporary_drive
  and 0x80
  jr z,l3dfah
  call sub_3dadh
  call calculate_step_rate_of_temporary_drive
  call sub_3e11h
  cp 0xFF
  jr nz,l3dfah
  push hl
  call sub_1fcah
  pop hl
  cp 0x50
  ld a,0x00
  jr nz,l3df9h
  ld a,0x80
l3df9h:
  ld (hl),a
l3dfah:
  call sub_1e36h
sub_3dfdh:
  ld a,0x50
sub_3dffh:
  ld c,0xFF
l3e01h:
  dec c
  jr nz,l3e01h
  dec a
  jr nz,sub_3dffh
  ret
point_HL_to_step_rate_of_temporary_drive:
  ld de,trdos_variable.drive_a_step_rate
l3e0bh:
  ld hl,(trdos_variable.current_temporary_drive)
  add hl,de
  ld a,(hl)
  ret
sub_3e11h:
  ld de,trdos_variable.drive_a_mode
  jr l3e0bh
calculate_step_rate_of_temporary_drive:
  call point_HL_to_step_rate_of_temporary_drive
  ld b,0x08 ; initial step rate
  ld c,0x04 ; number of tries
try_B_as_step_rate_of_temporary_drive:
  ld (hl),b
  ld a,0x08
  call l3d9ah
  ld a,0x20
  ld b,0x0B
  call vg93_command_B_with_data_A
  ld b,(hl)
  ld a,0x01
  call vg93_command_B_with_data_A
  in a,(0x1F)
  and 0x04
  jr nz,next_try_of_step_rate_of_temporary_drive
  xor a
  call vg93_command_B_with_data_A
  in a,(0x1F)
  and 0x04
  ret nz
next_try_of_step_rate_of_temporary_drive:
  inc b ; try the next step rate
  dec c ; more tries?
  ret z
  jr try_B_as_step_rate_of_temporary_drive
vg93_command_B_with_data_A:
  out (0x7F),a
  ld a,b
  or 0x18
  jp l0800h
sub_3e4ch:
  out (0x7F),a
  push bc
  ld b,a
  in a,(0x3F)
  cp b
  pop bc
  push af
  ld a,b
  or 0x18
  call l0800h
  pop af
  ret z
  push bc
  call sub_3dfdh
  pop bc
  ret
seek_track:
  ld c,a
  call select_bottom_side
  call sub_3e11h
  and 0x02
  call nz,sub_3eaah
  push bc
  bit 7,(hl)
  jr z,l3e83h
  bit 0,(hl)
  jr nz,l3e83h
  in a,(0x3F)
  cp c
  jr z,l3e82h
  rlca
  out (0x3F),a
  ld a,c
  rlca
l3e82h:
  ld c,a
l3e83h:
  call point_HL_to_step_rate_of_temporary_drive
  ld b,a
  in a,(0x3F)
  cp c
  push bc
  call nz,sub_3dfdh
  pop bc
  ld a,c
  call sub_3e4ch
  pop bc
  ld a,c
  out (0x3F),a
  ld a,(trdos_variable.disk_drive_ready)
  or a
  ret z
  xor a
  ld (trdos_variable.disk_drive_ready),a
sub_3ea0h:
  ld b,0x03
l3ea2h:
  ld a,0xFF
  call sub_3dffh
  djnz l3ea2h
  ret
sub_3eaah:
  ld a,c
  or a
  rra
  ld c,a
  ret nc
l3eafh:
  jp select_top_side
sub_3eb2h:
  call select_bottom_side
sub_3eb5h:
  in a,(0x1F)
  and 0x80
  ld (trdos_variable.disk_drive_ready),a
  in a,(0x3F)
  ld h,a
  call vg93_command_B_with_data_A
  ld c,0x7F
  ld d,0x01
  di
  ld a,0xC0
  out (0x1F),a
  push bc
  ld b,0x03
l3eceh:
  in a,(0xFF)
  and 0xC0
  jr nz,l3ef2h
  inc de
  ld a,e
  or d
  jr nz,l3eceh
  djnz l3eceh
  pop bc
  ei
  ld a,0xD0
  out (0x1F),a
  ld a,(trdos_variable.basic_program_autostart)
  cp 0xFF
  ret z
report_tape_loading_error_and_issue_no_disk_error:
  call set_sys_err_nr_to_tape_loading_error
  ld a,0xFF
  ld (trdos_variable.show_title_flag),a
  jp error.no_disk
l3ef2h:
  pop bc
  in h,(c)
l3ef5h:
  in a,(0xFF)
  and 0xC0
  jr z,l3ef5h
  ei
  ret m
  di
  in a,(0x7F)
  jr l3ef5h
set_sector_number:
  ld (trdos_variable.sector),a
  ret
define_buffer_address:
  ld (trdos_variable.buffer),hl
  ret
sub_3f0ah:
  ld a,0xA0
  jr l3f10h
sub_3f0eh:
  ld a,0x80
l3f10h:
  ld (trdos_variable.last_fdc_command),a
l3f13h:
  ld d,0x0A
l3f15h:
  push de
  di
  ld a,(trdos_variable.sector)
  inc a
  out (0x5F),a
  ld hl,(trdos_variable.buffer)
  ld c,0x7F
  ld a,(trdos_variable.last_fdc_command)
  out (0x1F),a
  cp 0xA0
  push af
  call z,sub_3fbah
  pop af
  call nz,sub_3fd5h
  pop de
  ei
  in a,(0x1F)
  ld b,a
  and 0x7F
  ret z
l3f39h:
  ld hl,message.cr.read_only
  and 0x40
  jr nz,l3f4bh
  ld a,b
  and 0x04
  jr z,l3fa0h
  dec d
  jr nz,l3f15h
l3f48h:
  ld hl,message.cr.disc_error
l3f4bh:
  ld a,0xD0
  out (0x1F),a
  ld a,b
  and 0x01
  jp nz,report_tape_loading_error_and_issue_no_disk_error
  in a,(0x3F)
  or a
  jr nz,l3f5fh
  in a,(0x5F)
  cp 0x0A
  ret z
l3f5fh:
  push hl
  call cls
  pop hl
  rst 0x18
  ld hl,message.cr.trk
  rst 0x18
  in a,(0x3F)
  call print_number_in_A
  ld hl,message.sec
  rst 0x18
  in a,(0x5F)
  call print_number_in_A
  ld hl,message.cr.retry_abort_ignore
  rst 0x18
ignore_retry_or_abort:
  call get_uppercase_pressed_key
  cp "I" ; Ignore?
  ret z
  cp "R" ; Retry?
  jr z,l3f94h
  cp "A" ; Abort?
  jr nz,ignore_retry_or_abort
  call set_sys_err_nr_to_tape_loading_error
  ld a,0x07
  ld (trdos_variable.error_code),a
  jp exit_dos
l3f94h:
  ld a,(trdos_variable.current_track)
  call seek_track
  call sub_3ea0h
  jp l3f13h
l3fa0h:
  dec d
  jp z,l3f48h
  push de
  call point_HL_to_step_rate_of_temporary_drive
  and 0x02
  jr nz,l3fadh
  inc (hl) ; increase the step rate
l3fadh:
  call init_interface
  ld a,(trdos_variable.current_track)
  call seek_track
  pop de
  jp l3f15h
sub_3fbah:
  ld b,0x04
l3fbch:
  in a,(0xFF)
  and 0xC0
  jr nz,l3fd1h
  inc de
  ld a,e
  or d
  jr nz,l3fbch
  djnz l3fbch
  ret
sub_3fcah:
  in a,(0xFF)
  and 0xC0
  jr z,sub_3fcah
  ret m
l3fd1h:
  outi
  jr sub_3fcah
sub_3fd5h:
  ld b,0x04
l3fd7h:
  in a,(0xFF)
  and 0xC0
  jr nz,l3fech
  inc de
  ld a,e
  or d
  jr nz,l3fd7h
  djnz l3fd7h
  ret
sub_3fe5h:
  in a,(0xFF)
  and 0xC0
  jr z,sub_3fe5h
  ret m
l3fech:
  ini
  jr sub_3fe5h

; BLOCK 'not_used_3FF0' (start 0x3ff0 end 0x4000)
not_used_3FF0_start:
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  defb 0xFF
  