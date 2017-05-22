/* ex: set ro ft=c: -*- buffer-read-only: t -*-
 *
 *    keywords.h
 *
 *    Copyright (C) 1994, 1995, 1996, 1997, 1999, 2000, 2001, 2002, 2005,
 *    2006, 2007 by Larry Wall and others
 *
 *    You may distribute under the terms of either the GNU General Public
 *    License or the Artistic License, as specified in the README file.
 *
 * !!!!!!!   DO NOT EDIT THIS FILE   !!!!!!!
 * This file is built by regen/keywords.pl from its data.
 * Any changes made here will be lost!
 */

#define KEY_NULL		0
#define KEY___FILE__		1
#define KEY___LINE__		2
#define KEY___PACKAGE__		3
#define KEY___DATA__		4
#define KEY___END__		5
#define KEY___SUB__		6
#define KEY_AUTOLOAD		7
#define KEY_BEGIN		8
#define KEY_UNITCHECK		9
#define KEY_DESTROY		10
#define KEY_END			11
#define KEY_INIT		12
#define KEY_CHECK		13
#define KEY_abs			14
#define KEY_accept		15
#define KEY_alarm		16
#define KEY_and			17
#define KEY_atan2		18
#define KEY_bind		19
#define KEY_binmode		20
#define KEY_bless		21
#define KEY_break		22
#define KEY_caller		23
#define KEY_chdir		24
#define KEY_chmod		25
#define KEY_chomp		26
#define KEY_chop		27
#define KEY_chown		28
#define KEY_chr			29
#define KEY_chroot		30
#define KEY_close		31
#define KEY_closedir		32
#define KEY_cmp			33
#define KEY_connect		34
#define KEY_continue		35
#define KEY_cos			36
#define KEY_crypt		37
#define KEY_dbmclose		38
#define KEY_dbmopen		39
#define KEY_default		40
#define KEY_defined		41
#define KEY_delete		42
#define KEY_die			43
#define KEY_do			44
#define KEY_dump		45
#define KEY_each		46
#define KEY_else		47
#define KEY_elsif		48
#define KEY_endgrent		49
#define KEY_endhostent		50
#define KEY_endnetent		51
#define KEY_endprotoent		52
#define KEY_endpwent		53
#define KEY_endservent		54
#define KEY_eof			55
#define KEY_eq			56
#define KEY_eval		57
#define KEY_evalbytes		58
#define KEY_exec		59
#define KEY_exists		60
#define KEY_exit		61
#define KEY_exp			62
#define KEY_extern		63
#define KEY_fc			64
#define KEY_fcntl		65
#define KEY_fileno		66
#define KEY_flock		67
#define KEY_for			68
#define KEY_foreach		69
#define KEY_fork		70
#define KEY_format		71
#define KEY_formline		72
#define KEY_ge			73
#define KEY_getc		74
#define KEY_getgrent		75
#define KEY_getgrgid		76
#define KEY_getgrnam		77
#define KEY_gethostbyaddr	78
#define KEY_gethostbyname	79
#define KEY_gethostent		80
#define KEY_getlogin		81
#define KEY_getnetbyaddr	82
#define KEY_getnetbyname	83
#define KEY_getnetent		84
#define KEY_getpeername		85
#define KEY_getpgrp		86
#define KEY_getppid		87
#define KEY_getpriority		88
#define KEY_getprotobyname	89
#define KEY_getprotobynumber	90
#define KEY_getprotoent		91
#define KEY_getpwent		92
#define KEY_getpwnam		93
#define KEY_getpwuid		94
#define KEY_getservbyname	95
#define KEY_getservbyport	96
#define KEY_getservent		97
#define KEY_getsockname		98
#define KEY_getsockopt		99
#define KEY_given		100
#define KEY_glob		101
#define KEY_gmtime		102
#define KEY_goto		103
#define KEY_grep		104
#define KEY_gt			105
#define KEY_hex			106
#define KEY_if			107
#define KEY_index		108
#define KEY_int			109
#define KEY_ioctl		110
#define KEY_join		111
#define KEY_keys		112
#define KEY_kill		113
#define KEY_last		114
#define KEY_lc			115
#define KEY_lcfirst		116
#define KEY_le			117
#define KEY_length		118
#define KEY_link		119
#define KEY_listen		120
#define KEY_local		121
#define KEY_localtime		122
#define KEY_lock		123
#define KEY_log			124
#define KEY_lstat		125
#define KEY_lt			126
#define KEY_m			127
#define KEY_map			128
#define KEY_mkdir		129
#define KEY_msgctl		130
#define KEY_msgget		131
#define KEY_msgrcv		132
#define KEY_msgsnd		133
#define KEY_my			134
#define KEY_ne			135
#define KEY_next		136
#define KEY_no			137
#define KEY_not			138
#define KEY_oct			139
#define KEY_open		140
#define KEY_opendir		141
#define KEY_or			142
#define KEY_ord			143
#define KEY_our			144
#define KEY_pack		145
#define KEY_package		146
#define KEY_pipe		147
#define KEY_pop			148
#define KEY_pos			149
#define KEY_print		150
#define KEY_printf		151
#define KEY_prototype		152
#define KEY_push		153
#define KEY_q			154
#define KEY_qq			155
#define KEY_qr			156
#define KEY_quotemeta		157
#define KEY_qw			158
#define KEY_qx			159
#define KEY_rand		160
#define KEY_read		161
#define KEY_readdir		162
#define KEY_readline		163
#define KEY_readlink		164
#define KEY_readpipe		165
#define KEY_recv		166
#define KEY_redo		167
#define KEY_ref			168
#define KEY_rename		169
#define KEY_require		170
#define KEY_reset		171
#define KEY_return		172
#define KEY_reverse		173
#define KEY_rewinddir		174
#define KEY_rindex		175
#define KEY_rmdir		176
#define KEY_s			177
#define KEY_say			178
#define KEY_scalar		179
#define KEY_seek		180
#define KEY_seekdir		181
#define KEY_select		182
#define KEY_semctl		183
#define KEY_semget		184
#define KEY_semop		185
#define KEY_send		186
#define KEY_setgrent		187
#define KEY_sethostent		188
#define KEY_setnetent		189
#define KEY_setpgrp		190
#define KEY_setpriority		191
#define KEY_setprotoent		192
#define KEY_setpwent		193
#define KEY_setservent		194
#define KEY_setsockopt		195
#define KEY_shift		196
#define KEY_shmctl		197
#define KEY_shmget		198
#define KEY_shmread		199
#define KEY_shmwrite		200
#define KEY_shutdown		201
#define KEY_sin			202
#define KEY_sleep		203
#define KEY_socket		204
#define KEY_socketpair		205
#define KEY_sort		206
#define KEY_splice		207
#define KEY_split		208
#define KEY_sprintf		209
#define KEY_sqrt		210
#define KEY_srand		211
#define KEY_stat		212
#define KEY_state		213
#define KEY_study		214
#define KEY_sub			215
#define KEY_substr		216
#define KEY_symlink		217
#define KEY_syscall		218
#define KEY_sysopen		219
#define KEY_sysread		220
#define KEY_sysseek		221
#define KEY_system		222
#define KEY_syswrite		223
#define KEY_tell		224
#define KEY_telldir		225
#define KEY_tie			226
#define KEY_tied		227
#define KEY_time		228
#define KEY_times		229
#define KEY_tr			230
#define KEY_truncate		231
#define KEY_uc			232
#define KEY_ucfirst		233
#define KEY_umask		234
#define KEY_undef		235
#define KEY_unless		236
#define KEY_unlink		237
#define KEY_unpack		238
#define KEY_unshift		239
#define KEY_untie		240
#define KEY_until		241
#define KEY_use			242
#define KEY_utime		243
#define KEY_values		244
#define KEY_vec			245
#define KEY_wait		246
#define KEY_waitpid		247
#define KEY_wantarray		248
#define KEY_warn		249
#define KEY_when		250
#define KEY_while		251
#define KEY_write		252
#define KEY_x			253
#define KEY_xor			254
#define KEY_y			255

/* Generated from:
 * a05a269d8b338c16c3837dddf59d662a73d006ed60ed571e65e83147d0ba67ad regen/keywords.pl
 * ex: set ro: */
