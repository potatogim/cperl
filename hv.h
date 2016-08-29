/*    hv.h
 *
 *    Copyright (C) 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000,
 *    2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008 by Larry Wall and others
 *    Copyright (C) 2016 cPanel Inc.
 *    Copyright (C) 2017 Reini Urban
 *
 *    You may distribute under the terms of either the GNU General Public
 *    License or the Artistic License, as specified in the README file.
 *
 *    Now a proper open addressing hash table, with linear probing and each entry
 *    contains a HE* ptr and the HASH value for faster inline comparison.
 *    The AUX struct is allocated seperately, at [HvMAX].
 */

#define PERL_HASH_ITER_BUCKET(iter)      ((iter)->xhv_riter)

/* open addressing with linear or robin-hood probing, or ... */
#define HASH_OPEN_LINEAR
/* #define HASH_OPEN_QUADRATIC */
/* #define HASH_OPEN_DOUBLE    */
/* #define HASH_OPEN_ROBINHOOD */
/* #define HASH_OPEN_HOPSCOTCH */
/* #define HASH_CHAINED_LIST   */

#undef PERL_HASH_RANDOMIZE_KEYS
#define PL_HASH_RAND_BITS_ENABLED    0

#define PERL_INLINE_HASH

/* inlined entry in hash array, 1-2 words */
struct array_he {
    HE		*hent_he;	/* ptr to full hash entry */
#ifdef PERL_INLINE_HASH
    U32		 hent_hash;	/* hash to catch 99% fails */
#endif
};

/* hash entry with key + value */
struct he {
    /*SV		*hent_val;*/	/* scalar value that was hashed */
    union {
	SV	*hent_val;	/* scalar value that was hashed */
	Size_t	hent_refcount;	/* references for this shared hash key */
    } he_valu;
    U32		hent_hash;	/* hash of key */
    I32		hent_len;	/* length of hash key, with utf8 bit as MSB */
#ifdef PERL_GCC_BRACE_GROUPS_FORBIDDEN
    char	hent_key[1];	/* variable-length hash key + flag */
#else
    char	hent_key[];      /* for easier debugging */
#endif
};

/* hash key -- defined separately for use as shared pointer.
   Not needed anymore.
 */
struct hek {
    U32		hek_hash;	/* hash of key */
    I32		hek_len;	/* length of hash key */
#if defined(PERL_GCC_BRACE_GROUPS_FORBIDDEN) || defined(__SUNPRO_C)
    char	hek_key[1];	/* variable-length hash key */
#else
    char	hek_key[];      /* for easier debugging */
#endif
    /* The hash-key is \0-terminated */
    /* after the \0 there is a byte for flags, such as whether the key
       is UTF-8 or WASUTF8 */
};

struct shared_he {
    struct he shared_he_he;
    struct hek shared_he_hek;
};

/* Subject to change.
   Don't access this directly.
*/

union _xhvnameu {
    HEK *xhvnameu_name;		/* When xhv_name_count is 0 */
    HEK **xhvnameu_names;	/* When xhv_name_count is non-0 */
};

struct xpvhv_aux {
    union _xhvnameu xhv_name_u;	/* name, if a symbol table */
    AV		*xhv_backreferences; /* back references for weak references */
    HE		*xhv_eiter;	/* current entry of iterator: todo: move to loop context */
    U32		xhv_riter;	/* current root of iterator: todo: move to loop context */
    /* Concerning xhv_name_count: When non-zero, xhv_name_u contains a pointer 
     * to an array of HEK pointers, this being the length. The first element is
     * the name of the stash, which may be NULL. If xhv_name_count is positive,
     * then *xhv_name is one of the effective names. If xhv_name_count is negative
     * then xhv_name_u.xhvnameu_names[1] is the first effective name.
     */
    I32		xhv_name_count;
    struct mro_meta *xhv_mro_meta;
#ifdef PERL_HASH_RANDOMIZE_KEYS
    U32         xhv_rand;       /* random value for hash traversal */
    U32         xhv_last_rand;  /* last random value for hash traversal,
                                   used to detect each() after insert for warnings */
#endif
    U32         xhv_aux_flags;  /* assorted extra flags */
};

#define HvAUXf_SCAN_STASH   0x1   /* stash is being scanned by gv_check */
#define HvAUXf_NO_DEREF     0x2   /* @{}, %{} etc (and nomethod) not present */
#define HvAUXf_STATIC       0x8   /* HvARRAY and xpvhv_aux is statically allocated (embedders) */
#define HvAUXf_SMALL       0x10   /* Small hash, linear scan */
#define HvAUXf_ROLE        0x20   /* The class is a role */

/* hash structure: */
/* This structure must match the beginning of struct xpvmg in sv.h. */
struct xpvhv {
    HV*		xmg_stash;	/* class package */
    union _xmgu	xmg_u;
    U32     	xhv_keys;       /* total keys, including placeholders */
    U32     	xhv_max;        /* subscript of last element of xhv_array */
    struct xpvhv_aux* xhv_aux;
};

#define HV_NO_RITER (U32)U32_MAX

/*
=head1 Hash Manipulation Functions

=for apidoc AmU||HEf_SVKEY
This flag, used in the length slot of hash entries and magic structures,
specifies the structure contains an C<SV*> pointer where a C<char*> pointer
is to be expected.  (For information only--not to be used).

=head1 Handy Values

=for apidoc AmU||Nullhv
Null HV pointer.

(deprecated - use C<(HV *)NULL> instead)

=head1 Hash Manipulation Functions

=for apidoc Am|char*|HvNAME|HV* stash
Returns the package name of a stash, or C<NULL> if C<stash> isn't a stash.
See C<L</SvSTASH>>, C<L</CvSTASH>>.

=for apidoc Am|I32|HvNAMELEN|HV *stash
Returns the length of the stash's name.

=for apidoc Am|unsigned char|HvNAMEUTF8|HV *stash
Returns true if the name is in UTF-8 encoding.

=for apidoc Am|char*|HvENAME|HV* stash
Returns the effective name of a stash, or NULL if there is none.  The
effective name represents a location in the symbol table where this stash
resides.  It is updated automatically when packages are aliased or deleted.
A stash that is no longer in the symbol table has no effective name.  This
name is preferable to C<HvNAME> for use in MRO linearisations and isa
caches.

=for apidoc Am|I32|HvENAMELEN|HV *stash
Returns the length of the stash's effective name.

=for apidoc Am|unsigned char|HvENAMEUTF8|HV *stash
Returns true if the effective name is in UTF-8 encoding.

=for apidoc Am|void*|HeKEY|HE* he
Returns the actual pointer stored in the key slot of the hash entry.  The
pointer may be either C<char*> or C<SV*>, depending on the value of
C<HeKLEN()>.  Can be assigned to.  The C<HePV()> or C<HeSVKEY()> macros are
usually preferable for finding the value of a key.

=for apidoc Am|I32|HeKLEN|HE* he
If this is negative, and amounts to C<HEf_SVKEY>, it indicates the entry
holds an C<SV*> key.  Otherwise, holds the actual length of the key.  Can
be assigned to.  The C<HePV()> macro is usually preferable for finding key
lengths.

=for apidoc Am|bool|He_IS_SVKEY|HE* he
Returns true if the key is an C<SV*>, or false if the hash entry does not
contain an C<SV*> key. It checks if C<HeKLEN(he) == (U32)HEf_SVKEY>.

=for apidoc Am|SV*|HeVAL|HE* he
Returns the value slot (type C<SV*>)
stored in the hash entry.  Can be assigned
to.

  SV *foo= HeVAL(hv);
  HeVAL(hv)= sv;


=for apidoc Am|U32|HeHASH|HE* he
Returns the computed hash stored in the hash entry.

=for apidoc Am|char*|HePV|HE* he|STRLEN len

Returns the key slot of the hash entry as a C<char*> value, doing any
necessary dereferencing of possibly C<SV*> keys.  The length of the
string is placed in C<len> (this is a macro, so do I<not> use
C<&len>).  If you do not care about what the length of the key is, you
may use the global variable C<PL_na>, though this is rather less
efficient than using a local variable.  Remember though, that hash
keys in perl are free to contain embedded nulls, so using C<strlen()>
or similar is not a good way to find the length of hash keys.  This is
very similar to the C<SvPV()> macro described elsewhere in this
document.  See also C<L</HeUTF8>>.  Note also that the hash key
length cannot be longer than 31bit, even if it is a HEf_SVKEY.

If you are using C<HePV> to get values to pass to C<newSVpvn()> to create a
new SV, you should consider using C<newSVhek(HeKEY_hek(he))> as it is more
efficient.

=for apidoc Am|U32|HeUTF8|HE* he
Returns whether the C<char *> value returned by C<HePV> is encoded in UTF-8,
doing any necessary dereferencing of possibly C<SV*> keys.  The value returned
will be 0 or non-0, not necessarily 1 (or even a value with any low bits set),
so B<do not> blindly assign this to a C<bool> variable, as C<bool> may be a
typedef for C<char>.

=for apidoc Am|SV*|HeSVKEY|HE* he
Returns the key as an C<SV*>, or C<NULL> if the hash entry does not
contain an C<SV*> key.

=for apidoc Am|SV*|HeSVKEY_force|HE* he
Returns the key as an C<SV*>.  Will create and return a temporary mortal
C<SV*> if the hash entry contains only a C<char*> key.

=for apidoc Am|SV*|HeSVKEY_set|HE* he|SV* sv
Sets the key to a given C<SV*>, taking care to set the appropriate flags to
indicate the presence of an C<SV*> key, and returns the same
C<SV*>.

=cut
*/

#define PERL_HASH_DEFAULT_HvMAX 7

/* Small hash optimization. https://github.com/perl11/cperl/issues/102
   If max 7 keys just do a linear scan (and unnecessarily set HvAUXf_SMALL) */

#define PERL_HV_SMALL_MAX     7

/* During hsplit(), if HvMAX(hv)+1 (the new bucket count) is >= this value,
 * we preallocate the HvAUX() struct.
 * The assumption being that we are using so much space anyway we might
 * as well allocate the extra bytes and speed up later keys()
 * or each() operations. We don't do this to small hashes as we assume
 * that a) it will be easy/fast to resize them to add the iterator, and b) that
 * many of them will be objects which won't be traversed. Larger hashes however
 * will take longer to extend, and the size of the aux struct is swamped by the
 * overall length of the bucket array.
 * */
#define PERL_HV_ALLOC_AUX_SIZE (1 << 9)

/* 64bit arith is faster, even with the added mask */
#if LONGSIZE >= 8
#define HvHASH_INDEX(hash, max) \
    ((U64_CONST(0x7fffffff00000000) | hash) & (max))
#else
#define HvHASH_INDEX(hash, max) (hash & (max))
#endif

/* These hash entry flags ride on hent_klen (for use only in magic/tied HVs) */
#define HEf_SVKEY	-2	/* hent_key is an SV* */

#ifndef PERL_CORE
#  define Nullhv Null(HV*)
#endif
#define HvARRAY(hv)	((hv)->sv_u.svu_hash)
#define HvFILL(hv)	Perl_hv_fill(aTHX_ MUTABLE_HV(hv))
#define HvMAX(hv)	((XPVHV*)SvANY(hv))->xhv_max
#define HvAUX(hv)	((XPVHV*)SvANY(hv))->xhv_aux
#define HvRITER(hv)	(*Perl_hv_riter_p(aTHX_ MUTABLE_HV(hv)))
#define HvEITER(hv)	(*Perl_hv_eiter_p(aTHX_ MUTABLE_HV(hv)))
#define HvRITER_set(hv,r)	Perl_hv_riter_set(aTHX_ MUTABLE_HV(hv), r)
#define HvEITER_set(hv,e)	Perl_hv_eiter_set(aTHX_ MUTABLE_HV(hv), e)
#define HvRITER_get(hv)	(SvOOK(hv) ? HvAUX(hv)->xhv_riter : HV_NO_RITER)
#define HvEITER_get(hv)	(SvOOK(hv) ? HvAUX(hv)->xhv_eiter : NULL)
#define HvRAND_get(hv)	(SvOOK(hv) ? HvAUX(hv)->xhv_rand : 0)
#define HvLASTRAND_get(hv)	(SvOOK(hv) ? HvAUX(hv)->xhv_last_rand : 0)
/* HvSTATIC must be combined with SvREADONLY! */
#define HvFLAGS(hv)      (SvOOK(hv) ? HvAUX(hv)->xhv_aux_flags : 0)
#define HvSTATIC_get(hv) (SvOOK(hv) ? HvAUX(hv)->xhv_aux_flags & HvAUXf_STATIC : 0)
#define HvSTATIC(hv)     HvSTATIC_get(hv)

#define HvNAME(hv)	HvNAME_get(hv)
#define HvNAMELEN(hv)   HvNAMELEN_get(hv)
#define HvENAME(hv)	HvENAME_get(hv)
#define HvENAMELEN(hv)  HvENAMELEN_get(hv)

#define HvPKGTYPE(hv) (hv && HvCLASS(hv) \
                        ? HvROLE(stash) ? "role" : "class" \
                       : "package")
#define HvPKGTYPE_NN(hv) HvCLASS(hv) \
                           ? HvROLE(hv) ? "role" : "class" \
                           : "package"

/* Checking that hv is a valid package stash is the
   caller's responsibility */
#define HvMROMETA(hv) (HvAUX(hv)->xhv_mro_meta \
                       ? HvAUX(hv)->xhv_mro_meta \
                       : Perl_mro_meta_init(aTHX_ hv))

#define HvNAME_HEK_NN(hv)			  \
 (						  \
  HvAUX(hv)->xhv_name_count			  \
  ? *HvAUX(hv)->xhv_name_u.xhvnameu_names	  \
  : HvAUX(hv)->xhv_name_u.xhvnameu_name		  \
 )
/* This macro may go away without notice.  */
#define HvNAME_HEK(hv) \
	(SvOOK(hv) && HvAUX(hv)->xhv_name_u.xhvnameu_name ? HvNAME_HEK_NN(hv) : NULL)
#define HvNAME_get(hv) \
	((SvOOK(hv) && HvAUX(hv)->xhv_name_u.xhvnameu_name && HvNAME_HEK_NN(hv)) \
			 ? HEK_KEY(HvNAME_HEK_NN(hv)) : NULL)
#define HvNAMELEN_get(hv) \
	((SvOOK(hv) && HvAUX(hv)->xhv_name_u.xhvnameu_name && HvNAME_HEK_NN(hv)) \
				 ? HEK_LEN(HvNAME_HEK_NN(hv)) : 0)
#define HvNAMEUTF8(hv) \
	((SvOOK(hv) && HvAUX(hv)->xhv_name_u.xhvnameu_name && HvNAME_HEK_NN(hv)) \
				 ? HEK_UTF8(HvNAME_HEK_NN(hv)) : 0)
#define HvENAME_HEK_NN(hv)                                             \
 (                                                                      \
  HvAUX(hv)->xhv_name_count > 0   ? HvAUX(hv)->xhv_name_u.xhvnameu_names[0] : \
  HvAUX(hv)->xhv_name_count < -1  ? HvAUX(hv)->xhv_name_u.xhvnameu_names[1] : \
  HvAUX(hv)->xhv_name_count == -1 ? NULL                              : \
                                    HvAUX(hv)->xhv_name_u.xhvnameu_name \
 )
#define HvENAME_HEK(hv) \
	(SvOOK(hv) && HvAUX(hv)->xhv_name_u.xhvnameu_name ? HvENAME_HEK_NN(hv) : NULL)
#define HvENAME_get(hv) \
   ((SvOOK(hv) && HvAUX(hv)->xhv_name_u.xhvnameu_name && HvAUX(hv)->xhv_name_count != -1) \
			 ? HEK_KEY(HvENAME_HEK_NN(hv)) : NULL)
#define HvENAMELEN_get(hv) \
   ((SvOOK(hv) && HvAUX(hv)->xhv_name_u.xhvnameu_name && HvAUX(hv)->xhv_name_count != -1) \
				 ? HEK_LEN(HvENAME_HEK_NN(hv)) : 0)
#define HvENAMEUTF8(hv) \
   ((SvOOK(hv) && HvAUX(hv)->xhv_name_u.xhvnameu_name && HvAUX(hv)->xhv_name_count != -1) \
				 ? HEK_UTF8(HvENAME_HEK_NN(hv)) : 0)

/* the number of keys (including any placeholders) - NOT PART OF THE API */
#define XHvTOTALKEYS(xhv)	((xhv)->xhv_keys)

/*
 * HvKEYS gets the number of keys that actually exist(), and is provided
 * for backwards compatibility with old XS code. The core uses HvUSEDKEYS
 * (keys, excluding placeholders) and HvTOTALKEYS (including placeholders)
 */
#define HvKEYS(hv)		HvUSEDKEYS(hv)
#define HvUSEDKEYS(hv)		(HvTOTALKEYS(hv) - HvPLACEHOLDERS_get(hv))
#define HvTOTALKEYS(hv)		XHvTOTALKEYS((XPVHV*)SvANY(hv))
#define HvPLACEHOLDERS(hv)	(*Perl_hv_placeholders_p(aTHX_ MUTABLE_HV(hv)))
#define HvPLACEHOLDERS_get(hv)	(SvMAGIC(hv) ? Perl_hv_placeholders_get(aTHX_ (const HV *)hv) : 0)
#define HvPLACEHOLDERS_set(hv,p)	Perl_hv_placeholders_set(aTHX_ MUTABLE_HV(hv), p)

/* cperl only*/
#define HvSMALL(hv)		(HvTOTALKEYS(hv) <= PERL_HV_SMALL_MAX)
#define XHvSMALL(xhv)		(XHvTOTALKEYS(xhv) <= PERL_HV_SMALL_MAX)

#define HvSHAREKEYS(hv)		(SvFLAGS(hv) & SVphv_SHAREKEYS)
#define HvSHAREKEYS_on(hv)	(SvFLAGS(hv) |= SVphv_SHAREKEYS)
#define HvSHAREKEYS_off(hv)	(SvFLAGS(hv) &= ~SVphv_SHAREKEYS)

/* cperl only. basically if the class has a closed compile-time @ISA,
   and it's objects are copies of [@class::FIELDS]. */
#define HvCLASS(stash)          (SvFLAGS(stash) & SVphv_CLASS)
#define HvCLASS_on(stash)       (SvFLAGS(stash) |= SVphv_CLASS)
#define HvROLE(stash)           (HvFLAGS(stash) & HvAUXf_ROLE)
#define HvROLE_on(stash)        STMT_START {                            \
        if (!SvOOK(stash)) { hv_iterinit(stash); SvOOK_on(stash); }     \
        HvAUX(stash)->xhv_aux_flags |= HvAUXf_ROLE; } STMT_END

/* This is an optimisation flag. It won't be set if all hash keys have a 0
 * flag. Currently the only flags relate to utf8.
 * Hence it won't be set if all keys are 8 bit only. It will be set if any key
 * is utf8 (including 8 bit keys that were entered as utf8, and need upgrading
 * when retrieved during iteration. It may still be set when there are no longer
 * any utf8 keys.
 * See HVhek_ENABLEHVKFLAGS for the trigger.
 */
#define HvHASKFLAGS(hv)		(SvFLAGS(hv) & SVphv_HASKFLAGS)
#define HvHASKFLAGS_on(hv)	(SvFLAGS(hv) |= SVphv_HASKFLAGS)
#define HvHASKFLAGS_off(hv)	(SvFLAGS(hv) &= ~SVphv_HASKFLAGS)

#define HvLAZYDEL(hv)		(SvFLAGS(hv) & SVphv_LAZYDEL)
#define HvLAZYDEL_on(hv)	(SvFLAGS(hv) |= SVphv_LAZYDEL)
#define HvLAZYDEL_off(hv)	(SvFLAGS(hv) &= ~SVphv_LAZYDEL)

#ifndef PERL_CORE
#  define Nullhe Null(HE*)
#endif
#define AHe(ahe)		(ahe).hent_he
#ifdef PERL_INLINE_HASH
#  define AHeHASH_set(ahep, hash) (ahep)->hent_hash = hash
#else
#  define AHeHASH_set(ahep, hash)
#endif
#define HeNEXT(he)		(he+1)
#define HeKEY_hek(he)		(*(HEK**)&(he)->hent_hash)
#define HeKEY(he)		(he)->hent_key
#define HeKEY_sv(he)		(*(SV**)HeKEY(he))
#define HeKLEN(he)		(he)->hent_len
#define HeKUTF8(he)  		(HeKFLAGS(he) & HVhek_UTF8)
#define HeKWASUTF8(he)  	(HeKFLAGS(he) & HVhek_WASUTF8)
#define HeKLEN_UTF8(he)  	(HeKUTF8(he) ? -HeKLEN(he) : HeKLEN(he))
#define HeKFLAGS(he)  		(*((unsigned char *)(HeKEY(he))+HeKLEN(he)+1))
#define HeVAL(he)		(he)->he_valu.hent_val
#define HeHASH(he)		(he)->hent_hash
/* Here we require a STRLEN lp */
#define HePV(he,lp)		((He_IS_SVKEY(he)) ?		\
				 SvPV(HeKEY_sv(he),lp) :        \
				 ((lp = HeKLEN(he)), HeKEY(he)))
#define HeUTF8(he)		((He_IS_SVKEY(he)) ? SvUTF8(HeKEY_sv(he)) : (U32)HeKUTF8(he))
#define HeSTATIC(he)		(HEK_FLAGS(HeKEY_hek(he)) & HVhek_STATIC)

#define HeSVKEY(he)		((He_IS_SVKEY(he)) ? HeKEY_sv(he) : NULL)
#define HeSVKEY_force(he)	((He_IS_SVKEY(he)) ?		        \
				  HeKEY_sv(he) :			\
				  newSVpvn_flags(HeKEY(he), HeKLEN(he), SVs_TEMP | \
                                      ( HeKUTF8(he) ? SVf_UTF8 : 0 )))
#define HeSVKEY_set(he,sv)	((HeKLEN(he) = HEf_SVKEY), (HeKEY_sv(he) = sv))
#define He_IS_SVKEY(he) 	HeKLEN(he) == HEf_SVKEY
#define He_IS_PLACEHOLDER(he) 	HeVAL(he) == &PL_sv_placeholder
#define SV_IS_PLACEHOLDER(sv) 	sv == &PL_sv_placeholder

#ifndef PERL_CORE
#  define Nullhek Null(HEK*)
#endif
#define HEK_BASESIZE		STRUCT_OFFSET(HEK, hek_key[0])
#define HEK_HASH(hek)		(hek)->hek_hash
#define HEK_LEN(hek)		(hek)->hek_len
#define HEK_KEY(hek)		(hek)->hek_key
#define HEK_FLAGS(hek)	(*((unsigned char *)(HEK_KEY(hek))+HEK_LEN(hek)+1))
#define HEK_IS_SVKEY(hek) 	HEK_LEN(hek) == HEf_SVKEY

#define HVhek_UTF8	0x01 /* Key is utf8 encoded. */
#define HVhek_WASUTF8	0x02 /* Key is bytes here, but was supplied as utf8. */
#define HVhek_MASK	0x03

/* unmasked HEK_FLAGS, not relevant in hash-table comparisons */
#define HVhek_UNSHARED	0x08 /* This key isn't a shared hash key. */
#define HVhek_TAINTED	0x10 /* This key is tainted */
#define HVhek_STATIC	0x80 /* This key was statically allocated */

/* the following flags are options for functions, they are not stored in heks */
#define HVhek_FREEKEY	0x100 /* Internal flag to say key is Newx()ed.  */
#define HVhek_PLACEHOLD	0x200 /* Internal flag to create placeholder.
                               * (may change, but Storable is a core module) */
#define HVhek_KEYCANONICAL 0x400 /* Internal flag - key is in canonical form.
				    If the string is UTF-8, it cannot be
				    converted to bytes. */
/* not needed anymore: */
#define HVhek_ENABLEHVKFLAGS        (HVhek_MASK & ~(HVhek_UNSHARED))

#define HEK_UTF8(hek)		(HEK_FLAGS(hek) & HVhek_UTF8)
#define HEK_UTF8_on(hek)	(HEK_FLAGS(hek) |= HVhek_UTF8)
#define HEK_UTF8_off(hek)	(HEK_FLAGS(hek) &= ~HVhek_UTF8)
#define HEK_WASUTF8(hek)	(HEK_FLAGS(hek) & HVhek_WASUTF8)
#define HEK_WASUTF8_on(hek)	(HEK_FLAGS(hek) |= HVhek_WASUTF8)
#define HEK_WASUTF8_off(hek)	(HEK_FLAGS(hek) &= ~HVhek_WASUTF8)
#define HEK_UNSHARED(hek)	(HEK_FLAGS(hek) & HVhek_UNSHARED)
#define HEK_TAINTED(hek)	(HEK_FLAGS(hek) & HVhek_TAINTED)
#define HEK_TAINTED_on(hek)	(HEK_FLAGS(hek) |= HVhek_TAINTED)
#define HEK_STATIC(hek)		(HEK_FLAGS(hek) & HVhek_STATIC)

/* calculate HV array allocation */
#ifndef PERL_USE_LARGE_HV_ALLOC
/* Default to allocating the correct size - default to assuming that malloc()
   is not broken and is efficient at allocating blocks sized at powers-of-two.
*/   
#  define PERL_HV_ARRAY_ALLOC_BYTES(size) ((size) * sizeof(AHE))
#else
#  define MALLOC_OVERHEAD 16
#  define PERL_HV_ARRAY_ALLOC_BYTES(size) \
			(((size) < 64)					\
			 ? (size) * sizeof(AHE)				\
			 : (size) * sizeof(AHE) * 2 - MALLOC_OVERHEAD)
#endif

/* Flags for hv_iternext_flags.  */
#define HV_ITERNEXT_WANTPLACEHOLDERS	0x01	/* Don't skip placeholders.  */

#define hv_iternext(hv)	hv_iternext_flags(hv, 0)
#define hv_magic(hv, gv, how) sv_magic(MUTABLE_SV(hv), MUTABLE_SV(gv), how, NULL, 0)
#define hv_undef(hv) Perl_hv_undef_flags(aTHX_ hv, 0)

#define Perl_sharepvn(pv, len, hash) HEK_KEY(share_hek(pv, len, hash))
#define sharepvn(pv, len, hash)	     Perl_sharepvn(pv, len, hash)

#define share_hek_he(hek)						\
    ((struct shared_he *)(((char *)hek)                                 \
     - STRUCT_OFFSET(struct shared_he, shared_he_hek)))
#define share_hek_hek(hek)						\
    (UNLIKELY(HEK_STATIC(hek)) ? (hek) :                                \
     (++(share_hek_he(hek)->shared_he_he.he_valu.hent_refcount),        \
     hek))

#define hv_store_ent(hv, keysv, val, hash)				\
    ((HE *) hv_common((hv), (keysv), NULL, 0, 0, HV_FETCH_ISSTORE,	\
		      (val), (hash)))
#ifdef PERL_CORE
#define hv_store_ent_void(hv, keysv, val, hash)				\
    ((void)hv_common((hv), (keysv), NULL, 0, 0, HV_FETCH_ISSTORE,	\
		      (val), (hash)))
#endif

#define hv_exists_ent(hv, keysv, hash)					\
    cBOOL(hv_common((hv), (keysv), NULL, 0, 0, HV_FETCH_ISEXISTS, 0, (hash)))
#define hv_fetch_ent(hv, keysv, lval, hash)				\
    ((HE *) hv_common((hv), (keysv), NULL, 0, 0,			\
		      ((lval) ? HV_FETCH_LVALUE : 0), NULL, (hash)))
#define hv_delete_ent(hv, key, discard, hash)				\
    (MUTABLE_SV(hv_common((hv), (key), NULL, 0, 0, (discard)|HV_DELETE, \
			  NULL, (hash))))

#define hv_store_flags(hv, key, klen, val, hash, flags)			\
    ((SV**) hv_common((hv), NULL, (key), (klen), (flags),		\
		      (HV_FETCH_ISSTORE|HV_FETCH_JUST_SV), (val),	\
		      (hash)))

#define hv_store(hv, key, klen, val, hash)				\
    ((SV**) hv_common_key_len((hv), (key), (klen),			\
			      (HV_FETCH_ISSTORE|HV_FETCH_JUST_SV),	\
			      (val), (hash)))



#define hv_exists(hv, key, klen)					\
    cBOOL(hv_common_key_len((hv), (key), (klen), HV_FETCH_ISEXISTS, NULL, 0))

#define hv_fetch(hv, key, klen, lval)					\
    ((SV**) hv_common_key_len((hv), (key), (klen), (lval)		\
			      ? (HV_FETCH_JUST_SV | HV_FETCH_LVALUE)	\
			      : HV_FETCH_JUST_SV, NULL, 0))

#define hv_delete(hv, key, klen, discard)				\
    (MUTABLE_SV(hv_common_key_len((hv), (key), (klen),			\
				  (discard) | HV_DELETE, NULL, 0)))

/* Provide 's' suffix subs for constant strings (and avoid needing to count
 * chars). See STR_WITH_LEN in handy.h - because these are macros we cant use
 * STR_WITH_LEN to do the work, we have to unroll it. */
#define hv_existss(hv, key) \
    hv_exists((hv), ("" key ""), (sizeof(key)-1))

#define hv_fetchs(hv, key, lval) \
    hv_fetch((hv), ("" key ""), (sizeof(key)-1), (lval))

#define hv_deletes(hv, key, flags) \
    hv_delete((hv), ("" key ""), (sizeof(key)-1), (flags))

#define hv_name_sets(hv, name, flags) \
    hv_name_set((hv),("" name ""),(sizeof(name)-1), flags)

#define hv_stores(hv, key, val) \
    hv_store((hv), ("" key ""), (sizeof(key)-1), (val), 0)

/* Internal class fetchers which bypass not existing names */
#define hv_fetch_ifexists(hv, key, klen, lval) \
    ((SV**) hv_common_key_len((hv), (key), (klen), (lval)                \
              ? (HV_FETCH_ISEXISTS | HV_FETCH_JUST_SV | HV_FETCH_LVALUE) \
              : (HV_FETCH_ISEXISTS | HV_FETCH_JUST_SV), NULL, 0))
#define hv_fetchs_ifexists(hv, key, lval) \
    ((SV**) hv_common_key_len((hv), ("" key ""), (sizeof(key)-1), (lval) \
	      ? (HV_FETCH_ISEXISTS | HV_FETCH_JUST_SV | HV_FETCH_LVALUE) \
              : (HV_FETCH_ISEXISTS | HV_FETCH_JUST_SV), NULL, 0))
#define hv_fetch_ent_ifexists(hv, keysv, lval, hash)     \
    ((HE *) hv_common((hv), (keysv), NULL, 0, 0, (lval)  \
              ? (HV_FETCH_ISEXISTS | HV_FETCH_LVALUE)    \
              : HV_FETCH_ISEXISTS, NULL, (hash)))

#ifdef PERL_CORE

#define hv_fetchhek_ifexists(hv, hek, lval)     \
    ((SV **) hv_common((hv), NULL, HEK_KEY(hek), HEK_LEN(hek), HEK_UTF8(hek), \
                (lval) ? (HV_FETCH_ISEXISTS | HV_FETCH_JUST_SV | HV_FETCH_LVALUE) \
                       : (HV_FETCH_ISEXISTS | HV_FETCH_JUST_SV), \
                       NULL, HEK_HASH(hek)))

/* TODO: for HvSHAREKEYS(hv) use the SvIsCOW_shared_hash string
   after the hek as keysv, thus comparing ptrs not values.
   Maybe as new hv_{action}sharedhek() */

# define hv_storehek(hv, hek, val) \
    hv_common((hv), NULL, HEK_KEY(hek), HEK_LEN(hek), HEK_UTF8(hek),	\
	      HV_FETCH_ISSTORE|HV_FETCH_JUST_SV, (val), HEK_HASH(hek))
# define hv_fetchhek(hv, hek, lval) \
    ((SV **)								\
     hv_common((hv), NULL, HEK_KEY(hek), HEK_LEN(hek), HEK_UTF8(hek),	\
	       (lval)							\
		? (HV_FETCH_JUST_SV | HV_FETCH_LVALUE)			\
		: HV_FETCH_JUST_SV,					\
	       NULL, HEK_HASH(hek)))
# define hv_deletehek(hv, hek, discard) \
    hv_common((hv), NULL, HEK_KEY(hek), HEK_LEN(hek), HEK_UTF8(hek), \
	      (discard)|HV_DELETE, NULL, HEK_HASH(hek))
#endif

/* This refcounted he structure is used for storing the hints used for lexical
   pragmas. Without threads, it's basically struct he + refcount.
   With threads, life gets more complex as the structure needs to be shared
   between threads (because it hangs from OPs, which are shared), hence the
   alternate definition and mutex.  */

struct refcounted_he;

/* flags for the refcounted_he API */
#define REFCOUNTED_HE_KEY_UTF8		0x00000001
#ifdef PERL_CORE
# define REFCOUNTED_HE_EXISTS		0x00000002
#endif

#ifdef PERL_CORE

/* Gosh. This really isn't a good name any longer.  */
struct refcounted_he {
    struct refcounted_he *refcounted_he_next;	/* next entry in chain */
#ifdef USE_ITHREADS
    U32                   refcounted_he_hash;
    U32                   refcounted_he_keylen;
#else
    HEK                  *refcounted_he_hek;	/* hint key */
#endif
    union {
	IV                refcounted_he_u_iv;
	UV                refcounted_he_u_uv;
	STRLEN            refcounted_he_u_len;
	void		 *refcounted_he_u_ptr;	/* Might be useful in future */
    } refcounted_he_val;
    U32	                  refcounted_he_refcnt;	/* reference count */
    /* First byte is flags. Then NUL-terminated value. Then for ithreads,
       non-NUL terminated key.  */
    char                  refcounted_he_data[1];
};

/*
=for apidoc m|SV *|refcounted_he_fetch_pvs|const struct refcounted_he *chain|const char *key|U32 flags

Like L</refcounted_he_fetch_pvn>, but takes a C<NUL>-terminated literal string
instead of a string/length pair, and no precomputed hash.

=cut
*/

#define refcounted_he_fetch_pvs(chain, key, flags) \
    Perl_refcounted_he_fetch_pvn(aTHX_ chain, STR_WITH_LEN(key), 0, flags)

/*
=for apidoc m|struct refcounted_he *|refcounted_he_new_pvs|struct refcounted_he *parent|const char *key|SV *value|U32 flags

Like L</refcounted_he_new_pvn>, but takes a C<NUL>-terminated literal string
instead of a string/length pair, and no precomputed hash.

=cut
*/

#define refcounted_he_new_pvs(parent, key, value, flags) \
    Perl_refcounted_he_new_pvn(aTHX_ parent, STR_WITH_LEN(key), 0, value, flags)

/* Flag bits are HVhek_UTF8, HVhek_WASUTF8, then */
#define HVrhek_undef	0x00 /* Value is undef. */
#define HVrhek_delete	0x10 /* Value is placeholder - signifies delete. */
#define HVrhek_IV	0x20 /* Value is IV. */
#define HVrhek_UV	0x30 /* Value is UV. */
#define HVrhek_PV	0x40 /* Value is a (byte) string. */
#define HVrhek_PV_UTF8	0x50 /* Value is a (utf8) string. */
/* Two spare. As these have to live in the optree, you can't store anything
   interpreter specific, such as SVs. :-( */
#define HVrhek_typemask 0x70

#ifdef USE_ITHREADS
/* A big expression to find the key offset */
#define REF_HE_KEY(chain)						\
	((((chain->refcounted_he_data[0] & 0x60) == 0x40)		\
	    ? chain->refcounted_he_val.refcounted_he_u_len + 1 : 0)	\
	 + 1 + chain->refcounted_he_data)
#endif

#  ifdef USE_ITHREADS
#    define HINTS_REFCNT_LOCK		MUTEX_LOCK(&PL_hints_mutex)
#    define HINTS_REFCNT_UNLOCK		MUTEX_UNLOCK(&PL_hints_mutex)
#  else
#    define HINTS_REFCNT_LOCK		NOOP
#    define HINTS_REFCNT_UNLOCK		NOOP
#  endif
#endif

#ifdef USE_ITHREADS
#  define HINTS_REFCNT_INIT		MUTEX_INIT(&PL_hints_mutex)
#  define HINTS_REFCNT_TERM		MUTEX_DESTROY(&PL_hints_mutex)
#else
#  define HINTS_REFCNT_INIT		NOOP
#  define HINTS_REFCNT_TERM		NOOP
#endif

/* Hash actions
 * Passed in PERL_MAGIC_uvar calls
 */
#define HV_DISABLE_UVAR_XKEY	0x01
/* We need to ensure that these don't clash with G_DISCARD, which is 2, as it
   is documented as being passed to hv_delete().  */
#define HV_FETCH_ISSTORE	0x04
#define HV_FETCH_ISEXISTS	0x08
#define HV_FETCH_LVALUE		0x10
#define HV_FETCH_JUST_SV	0x20
#define HV_DELETE		0x40
#define HV_FETCH_EMPTY_HE	0x80 /* Leave HeVAL null. */
/* TODO: combinations
   fetch || store (STORE IFEMPTY) (strtab, magic) "fetch-as-store"
   fetch && delete (strtab)
   exists || lvalue (LVALUE IFNOTEXIST) (tied mderef)
*/

/* Must not conflict with HVhek_UTF8 */
#define HV_NAME_SETALL		0x02

#ifdef PERL_CORE
/* return actions for internal hv_common_magical() */
#define HV_COMMON_MAGICAL_RETURNS         0
#define HV_COMMON_MAGICAL_IGNORE          1
#define HV_COMMON_MAGICAL_ENV_IS_CASELESS 2
#endif

/*
=for apidoc newHV

Creates a new HV.  The reference count is set to 1.

=cut
*/

#define newHV()	MUTABLE_HV(newSV_type(SVt_PVHV))

/*#define HE_INC(entry) */
/* entry is the initial hash hit, check all collisions.
   an empty hash slot has entry==NULL. */
#ifdef HASH_OPEN_LINEAR
#define HE_EACH(hv,hindex,_entry,block)              \
    {   AHE* ahe;                                    \
        for (; ahe; ahe = &HvARRAY(hv)[++hindex]) {  \
            /*if (ahe->hent_hash != hash) continue;*/\
            _entry = ahe->hent_he;                   \
            block;                                   \
        }                                            \
    }
#else
#define HE_EACH(hv,hindex,_entry,block)       \
    for (; _entry; _entry = HeNEXT(_entry)) { \
      block; \
    }
#endif

#ifdef HASH_OPEN_LINEAR
#define HE_EACH_POST(hv,_entry,post,block)          \
    for (; _entry;  _entry = HvARRAY(hv)[++hindex], post) { \
      block; \
    }
#define HE_EACH_CMP(hv,_entry,cmp,block)  \
    for (; cmp; _entry = HvARRAY(hv)[++hindex])) { \
      block; \
    }
#else
#define HE_EACH_POST(hv,_entry,post,block)          \
    for (; _entry; _entry = HeNEXT(_entry), post) { \
      block; \
    }
#define HE_EACH_CMP(hv,_entry,cmp,block)  \
    for (; cmp; _entry = HeNEXT(_entry)) { \
      block; \
    }
#endif
#ifdef PERL_CORE
/* oentry is the changable entry ptr, entry the initial hash hit.
   check all collisions */
#ifdef HASH_OPEN_LINEAR
#define HE_OEACH(hv,hindex,oentry,_entry,block)       \
    {   AHE* ahe;                                     \
        for (; ahe; oentry = &HvARRAY(hv)[++hindex], ahe = *oentry) { \
            if (ahe->hent_hash != hash) continue;     \
            entry = AHe(ahe);                         \
            block;                                    \
        }                                             \
    }
#else
#define HE_OEACH(hv,hindex,oentry,_entry,block)                  \
    for (; _entry; oentry = &HeNEXT(_entry), _entry = *oentry) { \
      block; \
    }
#endif
#endif

#include "hv_func.h"

#define hv_begin(h)  (U32)0
#define hv_end(h)    HvMAX(h)
#define hv_empty(h,i) !HvARRAY(h)[i].hent_he
#define hv_key(h, i) HvARRAY(h)[i].hent_he
#define hv_val(h, i) HvARRAY(h)[i].hent_he->hent_val

/* ahe being &HvARRAY(h)[i] */
#define hv_isequal(ahe, hek) \
           HEK_HASH(hek) == ahe->hent_hash     \
        && HeKLEN(ahe->hent_he) == HEK_LEN(hek) \
        && memEQ(HeKEY(ahe->hent_he), HEK_KEY(hek), HEK_LEN(hek))

#define hv_foreach(h, kvar, vvar, code)                 \
  { U32 __i;                                            \
    for (__i = hv_begin(h); __i != hv_end(h); ++__i) {  \
      if (hv_empty(h,__i)) continue;                    \
      (kvar) = hv_key(h,__i);                           \
      (vvar) = hv_val(h,__i);                           \
      code;                                             \
    }                                                   \
  }
#define hv_foreach_value(h, vvar, code)                 \
  { U32 __i;                                            \
    for (__i = hv_begin(h); __i != hv_end(h); ++__i) {  \
      if (hv_empty(h,__i)) continue;                    \
      (vvar) = hv_val(h,__i);                           \
      code;                                             \
    }                                                   \
  }

/*
 * ex: set ts=8 sts=4 sw=4 et:
 */
