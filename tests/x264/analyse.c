/*****************************************************************************
 * analyse.c: macroblock analysis
 *****************************************************************************
 * Copyright (C) 2003-2011 x264 project
 *
 * Authors: Laurent Aimar <fenrir@via.ecp.fr>
 *          Loren Merritt <lorenm@u.washington.edu>
 *          Jason Garrett-Glaser <darkshikari@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at licensing@x264.com.
 *****************************************************************************/

#define _ISOC99_SOURCE

#include "common.h"
#include "emacroblock.h"
#include "me.h"
#include "ratecontrol.h"
#include "analyse.h"
/*****************************************************************************
 * rdo.c: rate-distortion optimization
 *****************************************************************************
 * Copyright (C) 2005-2011 x264 project
 *
 * Authors: Loren Merritt <lorenm@u.washington.edu>
 *          Jason Garrett-Glaser <darkshikari@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at licensing@x264.com.
 *****************************************************************************/

/* duplicate all the writer functions, just calculating bit cost
 * instead of writing the bitstream.
 * TODO: use these for fast 1st pass too. */

#define RDO_SKIP_BS 1

/* Transition and size tables for abs<9 MVD and residual coding */
/* Consist of i_prefix-2 1s, one zero, and a bypass sign bit */
static uint8_t cabac_transition_unary[15][128];
static uint16_t cabac_size_unary[15][128];
/* Transition and size tables for abs>9 MVD */
/* Consist of 5 1s and a bypass sign bit */
static uint8_t cabac_transition_5ones[128];
static uint16_t cabac_size_5ones[128];

/* CAVLC: produces exactly the same bit count as a normal encode */
/* this probably still leaves some unnecessary computations */
#define bs_write1(s,v)     ((s)->i_bits_encoded += 1)
#define bs_write(s,n,v)    ((s)->i_bits_encoded += (n))
#define bs_write_ue(s,v)   ((s)->i_bits_encoded += bs_size_ue(v))
#define bs_write_se(s,v)   ((s)->i_bits_encoded += bs_size_se(v))
#define bs_write_te(s,v,l) ((s)->i_bits_encoded += bs_size_te(v,l))
#define x264_macroblock_write_cavlc  static x264_macroblock_size_cavlc
#include "cavlc.c"

/* CABAC: not exactly the same. x264_cabac_size_decision() keeps track of
 * fractional bits, but only finite precision. */
#undef  x264_cabac_encode_decision
#undef  x264_cabac_encode_decision_noup
#undef  x264_cabac_encode_bypass
#undef  x264_cabac_encode_terminal
#define x264_cabac_encode_decision(c,x,v) x264_cabac_size_decision(c,x,v)
#define x264_cabac_encode_decision_noup(c,x,v) x264_cabac_size_decision_noup(c,x,v)
#define x264_cabac_encode_terminal(c)     ((c)->f8_bits_encoded += 7)
#define x264_cabac_encode_bypass(c,v)     ((c)->f8_bits_encoded += 256)
#define x264_cabac_encode_ue_bypass(c,e,v) ((c)->f8_bits_encoded += (bs_size_ue_big(v+(1<<e)-1)-e)<<8)
#define x264_macroblock_write_cabac  static x264_macroblock_size_cabac
#include "ecabac.c"

#define COPY_CABAC h->mc.memcpy_aligned( &cabac_tmp.f8_bits_encoded, &h->cabac.f8_bits_encoded, \
        sizeof(x264_cabac_t) - offsetof(x264_cabac_t,f8_bits_encoded) - (CHROMA444 ? 0 : (1024+12)-460) )
#define COPY_CABAC_PART( pos, size )\
        memcpy( &cb->state[pos], &h->cabac.state[pos], size )

static ALWAYS_INLINE uint64_t cached_hadamard( x264_t *h, int size, int x, int y )
{
    static const uint8_t hadamard_shift_x[4] = {4,   4,   3,   3};
    static const uint8_t hadamard_shift_y[4] = {4-0, 3-0, 4-1, 3-1};
    static const uint8_t  hadamard_offset[4] = {0,   1,   3,   5};
    int cache_index = (x >> hadamard_shift_x[size]) + (y >> hadamard_shift_y[size])
                    + hadamard_offset[size];
    uint64_t res = h->mb.pic.fenc_hadamard_cache[cache_index];
    if( res )
        return res - 1;
    else
    {
        pixel *fenc = h->mb.pic.p_fenc[0] + x + y*FENC_STRIDE;
        res = h->pixf.hadamard_ac[size]( fenc, FENC_STRIDE );
        h->mb.pic.fenc_hadamard_cache[cache_index] = res + 1;
        return res;
    }
}

static ALWAYS_INLINE int cached_satd( x264_t *h, int size, int x, int y )
{
    static const uint8_t satd_shift_x[3] = {3,   2,   2};
    static const uint8_t satd_shift_y[3] = {2-1, 3-2, 2-2};
    static const uint8_t  satd_offset[3] = {0,   8,   16};
    ALIGNED_16( static pixel zero[16] ) = {0};
    int cache_index = (x >> satd_shift_x[size - PIXEL_8x4]) + (y >> satd_shift_y[size - PIXEL_8x4])
                    + satd_offset[size - PIXEL_8x4];
    int res = h->mb.pic.fenc_satd_cache[cache_index];
    if( res )
        return res - 1;
    else
    {
        pixel *fenc = h->mb.pic.p_fenc[0] + x + y*FENC_STRIDE;
        int dc = h->pixf.sad[size]( fenc, FENC_STRIDE, zero, 0 ) >> 1;
        res = h->pixf.satd[size]( fenc, FENC_STRIDE, zero, 0 ) - dc;
        h->mb.pic.fenc_satd_cache[cache_index] = res + 1;
        return res;
    }
}

/* Psy RD distortion metric: SSD plus "Absolute Difference of Complexities" */
/* SATD and SA8D are used to measure block complexity. */
/* The difference between SATD and SA8D scores are both used to avoid bias from the DCT size.  Using SATD */
/* only, for example, results in overusage of 8x8dct, while the opposite occurs when using SA8D. */

/* FIXME:  Is there a better metric than averaged SATD/SA8D difference for complexity difference? */
/* Hadamard transform is recursive, so a SATD+SA8D can be done faster by taking advantage of this fact. */
/* This optimization can also be used in non-RD transform decision. */

static inline int ssd_plane( x264_t *h, int size, int p, int x, int y )
{
    ALIGNED_16( static pixel zero[16] ) = {0};
    int satd = 0;
    pixel *fdec = h->mb.pic.p_fdec[p] + x + y*FDEC_STRIDE;
    pixel *fenc = h->mb.pic.p_fenc[p] + x + y*FENC_STRIDE;
    if( p == 0 && h->mb.i_psy_rd )
    {
        /* If the plane is smaller than 8x8, we can't do an SA8D; this probably isn't a big problem. */
        if( size <= PIXEL_8x8 )
        {
            uint64_t fdec_acs = h->pixf.hadamard_ac[size]( fdec, FDEC_STRIDE );
            uint64_t fenc_acs = cached_hadamard( h, size, x, y );
            satd = abs((int32_t)fdec_acs - (int32_t)fenc_acs)
                 + abs((int32_t)(fdec_acs>>32) - (int32_t)(fenc_acs>>32));
            satd >>= 1;
        }
        else
        {
            int dc = h->pixf.sad[size]( fdec, FDEC_STRIDE, zero, 0 ) >> 1;
            satd = abs(h->pixf.satd[size]( fdec, FDEC_STRIDE, zero, 0 ) - dc - cached_satd( h, size, x, y ));
        }
        satd = (satd * h->mb.i_psy_rd * h->mb.i_psy_rd_lambda + 128) >> 8;
    }
    return h->pixf.ssd[size](fenc, FENC_STRIDE, fdec, FDEC_STRIDE) + satd;
}

static inline int ssd_mb( x264_t *h )
{
    int chroma_size = h->luma2chroma_pixel[PIXEL_16x16];
    int chroma_ssd = ssd_plane(h, chroma_size, 1, 0, 0) + ssd_plane(h, chroma_size, 2, 0, 0);
    chroma_ssd = ((uint64_t)chroma_ssd * h->mb.i_chroma_lambda2_offset + 128) >> 8;
    return ssd_plane(h, PIXEL_16x16, 0, 0, 0) + chroma_ssd;
}

static int x264_rd_cost_mb( x264_t *h, int i_lambda2 )
{
    int b_transform_bak = h->mb.b_transform_8x8;
    int i_ssd;
    int i_bits;
    int type_bak = h->mb.i_type;

    x264_macroblock_encode( h );

    if( h->mb.b_deblock_rdo )
        x264_macroblock_deblock( h );

    i_ssd = ssd_mb( h );

    if( IS_SKIP( h->mb.i_type ) )
    {
        i_bits = (1 * i_lambda2 + 128) >> 8;
    }
    else if( h->param.b_cabac )
    {
        x264_cabac_t cabac_tmp;
        COPY_CABAC;
        x264_macroblock_size_cabac( h, &cabac_tmp );
        i_bits = ( (uint64_t)cabac_tmp.f8_bits_encoded * i_lambda2 + 32768 ) >> 16;
    }
    else
    {
        x264_macroblock_size_cavlc( h );
        i_bits = ( h->out.bs.i_bits_encoded * i_lambda2 + 128 ) >> 8;
    }

    h->mb.b_transform_8x8 = b_transform_bak;
    h->mb.i_type = type_bak;

    return i_ssd + i_bits;
}

/* partition RD functions use 8 bits more precision to avoid large rounding errors at low QPs */

static uint64_t x264_rd_cost_subpart( x264_t *h, int i_lambda2, int i4, int i_pixel )
{
    uint64_t i_ssd, i_bits;

    x264_macroblock_encode_p4x4( h, i4 );
    if( i_pixel == PIXEL_8x4 )
        x264_macroblock_encode_p4x4( h, i4+1 );
    if( i_pixel == PIXEL_4x8 )
        x264_macroblock_encode_p4x4( h, i4+2 );

    i_ssd = ssd_plane( h, i_pixel, 0, block_idx_x[i4]*4, block_idx_y[i4]*4 );
    if( CHROMA444 )
    {
        int chromassd = ssd_plane( h, i_pixel, 1, block_idx_x[i4]*4, block_idx_y[i4]*4 )
                      + ssd_plane( h, i_pixel, 2, block_idx_x[i4]*4, block_idx_y[i4]*4 );
        chromassd = ((uint64_t)chromassd * h->mb.i_chroma_lambda2_offset + 128) >> 8;
        i_ssd += chromassd;
    }

    if( h->param.b_cabac )
    {
        x264_cabac_t cabac_tmp;
        COPY_CABAC;
        x264_subpartition_size_cabac( h, &cabac_tmp, i4, i_pixel );
        i_bits = ( (uint64_t)cabac_tmp.f8_bits_encoded * i_lambda2 + 128 ) >> 8;
    }
    else
        i_bits = x264_subpartition_size_cavlc( h, i4, i_pixel );

    return (i_ssd<<8) + i_bits;
}

uint64_t x264_rd_cost_part( x264_t *h, int i_lambda2, int i4, int i_pixel )
{
    uint64_t i_ssd, i_bits;
    int i8 = i4 >> 2;

    if( i_pixel == PIXEL_16x16 )
    {
        int i_cost = x264_rd_cost_mb( h, i_lambda2 );
        return i_cost;
    }

    if( i_pixel > PIXEL_8x8 )
        return x264_rd_cost_subpart( h, i_lambda2, i4, i_pixel );

    h->mb.i_cbp_luma = 0;

    x264_macroblock_encode_p8x8( h, i8 );
    if( i_pixel == PIXEL_16x8 )
        x264_macroblock_encode_p8x8( h, i8+1 );
    if( i_pixel == PIXEL_8x16 )
        x264_macroblock_encode_p8x8( h, i8+2 );

    int ssd_x = 8*(i8&1);
    int ssd_y = 8*(i8>>1);
    i_ssd = ssd_plane( h, i_pixel, 0, ssd_x, ssd_y );
    int chromapix = h->luma2chroma_pixel[i_pixel];
    int chromassd = ssd_plane( h, chromapix, 1, ssd_x>>CHROMA_H_SHIFT, ssd_y>>CHROMA_V_SHIFT )
                  + ssd_plane( h, chromapix, 2, ssd_x>>CHROMA_H_SHIFT, ssd_y>>CHROMA_V_SHIFT );
    i_ssd += ((uint64_t)chromassd * h->mb.i_chroma_lambda2_offset + 128) >> 8;

    if( h->param.b_cabac )
    {
        x264_cabac_t cabac_tmp;
        COPY_CABAC;
        x264_partition_size_cabac( h, &cabac_tmp, i8, i_pixel );
        i_bits = ( (uint64_t)cabac_tmp.f8_bits_encoded * i_lambda2 + 128 ) >> 8;
    }
    else
        i_bits = x264_partition_size_cavlc( h, i8, i_pixel ) * i_lambda2;

    return (i_ssd<<8) + i_bits;
}

static uint64_t x264_rd_cost_i8x8( x264_t *h, int i_lambda2, int i8, int i_mode, pixel edge[4][32] )
{
    uint64_t i_ssd, i_bits;
    int plane_count = CHROMA444 ? 3 : 1;
    int i_qp = h->mb.i_qp;
    h->mb.i_cbp_luma &= ~(1<<i8);
    h->mb.b_transform_8x8 = 1;

    for( int p = 0; p < plane_count; p++ )
    {
        x264_mb_encode_i8x8( h, p, i8, i_qp, i_mode, edge[p], 1 );
        i_qp = h->mb.i_chroma_qp;
    }

    i_ssd = ssd_plane( h, PIXEL_8x8, 0, (i8&1)*8, (i8>>1)*8 );
    if( CHROMA444 )
    {
        int chromassd = ssd_plane( h, PIXEL_8x8, 1, (i8&1)*8, (i8>>1)*8 )
                      + ssd_plane( h, PIXEL_8x8, 2, (i8&1)*8, (i8>>1)*8 );
        chromassd = ((uint64_t)chromassd * h->mb.i_chroma_lambda2_offset + 128) >> 8;
        i_ssd += chromassd;
    }

    if( h->param.b_cabac )
    {
        x264_cabac_t cabac_tmp;
        COPY_CABAC;
        x264_partition_i8x8_size_cabac( h, &cabac_tmp, i8, i_mode );
        i_bits = ( (uint64_t)cabac_tmp.f8_bits_encoded * i_lambda2 + 128 ) >> 8;
    }
    else
        i_bits = x264_partition_i8x8_size_cavlc( h, i8, i_mode ) * i_lambda2;

    return (i_ssd<<8) + i_bits;
}

static uint64_t x264_rd_cost_i4x4( x264_t *h, int i_lambda2, int i4, int i_mode )
{
    uint64_t i_ssd, i_bits;
    int plane_count = CHROMA444 ? 3 : 1;
    int i_qp = h->mb.i_qp;

    for( int p = 0; p < plane_count; p++ )
    {
        x264_mb_encode_i4x4( h, p, i4, i_qp, i_mode, 1 );
        i_qp = h->mb.i_chroma_qp;
    }

    i_ssd = ssd_plane( h, PIXEL_4x4, 0, block_idx_x[i4]*4, block_idx_y[i4]*4 );
    if( CHROMA444 )
    {
        int chromassd = ssd_plane( h, PIXEL_4x4, 1, block_idx_x[i4]*4, block_idx_y[i4]*4 )
                      + ssd_plane( h, PIXEL_4x4, 2, block_idx_x[i4]*4, block_idx_y[i4]*4 );
        chromassd = ((uint64_t)chromassd * h->mb.i_chroma_lambda2_offset + 128) >> 8;
        i_ssd += chromassd;
    }

    if( h->param.b_cabac )
    {
        x264_cabac_t cabac_tmp;
        COPY_CABAC;
        x264_partition_i4x4_size_cabac( h, &cabac_tmp, i4, i_mode );
        i_bits = ( (uint64_t)cabac_tmp.f8_bits_encoded * i_lambda2 + 128 ) >> 8;
    }
    else
        i_bits = x264_partition_i4x4_size_cavlc( h, i4, i_mode ) * i_lambda2;

    return (i_ssd<<8) + i_bits;
}

static uint64_t x264_rd_cost_chroma( x264_t *h, int i_lambda2, int i_mode, int b_dct )
{
    uint64_t i_ssd, i_bits;

    if( b_dct )
        x264_mb_encode_chroma( h, 0, h->mb.i_chroma_qp );

    int chromapix = h->luma2chroma_pixel[PIXEL_16x16];
    i_ssd = ssd_plane( h, chromapix, 1, 0, 0 )
          + ssd_plane( h, chromapix, 2, 0, 0 );

    h->mb.i_chroma_pred_mode = i_mode;

    if( h->param.b_cabac )
    {
        x264_cabac_t cabac_tmp;
        COPY_CABAC;
        x264_chroma_size_cabac( h, &cabac_tmp );
        i_bits = ( (uint64_t)cabac_tmp.f8_bits_encoded * i_lambda2 + 128 ) >> 8;
    }
    else
        i_bits = x264_chroma_size_cavlc( h ) * i_lambda2;

    return (i_ssd<<8) + i_bits;
}
/****************************************************************************
 * Trellis RD quantization
 ****************************************************************************/

#define TRELLIS_SCORE_MAX ((uint64_t)1<<50)
#define CABAC_SIZE_BITS 8
#define SSD_WEIGHT_BITS 5
#define LAMBDA_BITS 4

/* precalculate the cost of coding various combinations of bits in a single context */
void x264_rdo_init( void )
{
    for( int i_prefix = 0; i_prefix < 15; i_prefix++ )
    {
        for( int i_ctx = 0; i_ctx < 128; i_ctx++ )
        {
            int f8_bits = 0;
            uint8_t ctx = i_ctx;

            for( int i = 1; i < i_prefix; i++ )
                f8_bits += x264_cabac_size_decision2( &ctx, 1 );
            if( i_prefix > 0 && i_prefix < 14 )
                f8_bits += x264_cabac_size_decision2( &ctx, 0 );
            f8_bits += 1 << CABAC_SIZE_BITS; //sign

            cabac_size_unary[i_prefix][i_ctx] = f8_bits;
            cabac_transition_unary[i_prefix][i_ctx] = ctx;
        }
    }
    for( int i_ctx = 0; i_ctx < 128; i_ctx++ )
    {
        int f8_bits = 0;
        uint8_t ctx = i_ctx;

        for( int i = 0; i < 5; i++ )
            f8_bits += x264_cabac_size_decision2( &ctx, 1 );
        f8_bits += 1 << CABAC_SIZE_BITS; //sign

        cabac_size_5ones[i_ctx] = f8_bits;
        cabac_transition_5ones[i_ctx] = ctx;
    }
}

typedef struct
{
    int64_t score;
    int level_idx; // index into level_tree[]
    uint8_t cabac_state[10]; //just the contexts relevant to coding abs_level_m1
} trellis_node_t;

// TODO:
// save cabac state between blocks?
// use trellis' RD score instead of x264_mb_decimate_score?
// code 8x8 sig/last flags forwards with deadzone and save the contexts at
//   each position?
// change weights when using CQMs?

// possible optimizations:
// make scores fit in 32bit
// save quantized coefs during rd, to avoid a duplicate trellis in the final encode
// if trellissing all MBRD modes, finish SSD calculation so we can skip all of
//   the normal dequant/idct/ssd/cabac

// the unquant_mf here is not the same as dequant_mf:
// in normal operation (dct->quant->dequant->idct) the dct and idct are not
// normalized. quant/dequant absorb those scaling factors.
// in this function, we just do (quant->unquant) and want the output to be
// comparable to the input. so unquant is the direct inverse of quant,
// and uses the dct scaling factors, not the idct ones.

#define SIGN(x,y) ((x^(y >> 31))-(y >> 31))

static ALWAYS_INLINE
int quant_trellis_cabac( x264_t *h, dctcoef *dct,
                         udctcoef *quant_mf, udctcoef *quant_bias, const int *unquant_mf,
                         const uint16_t *coef_weight, const uint8_t *zigzag,
                         int ctx_block_cat, int i_lambda2, int b_ac,
                         int b_chroma, int dc, int i_coefs, int idx )
{
    ALIGNED_ARRAY_16( dctcoef, coefs, [64] );
    ALIGNED_ARRAY_16( dctcoef, quant_coefs, [64] );
    trellis_node_t nodes[2][8];
    trellis_node_t *nodes_cur = nodes[0];
    trellis_node_t *nodes_prev = nodes[1];
    trellis_node_t *bnode;
    const int b_interlaced = MB_INTERLACED;
    uint8_t *cabac_state_sig = &h->cabac.state[ significant_coeff_flag_offset[b_interlaced][ctx_block_cat] ];
    uint8_t *cabac_state_last = &h->cabac.state[ last_coeff_flag_offset[b_interlaced][ctx_block_cat] ];
    const uint8_t *levelgt1_ctx = b_chroma && dc ? coeff_abs_levelgt1_ctx_chroma_dc : coeff_abs_levelgt1_ctx;

    // (# of coefs) * (# of ctx) * (# of levels tried) = 1024
    // we don't need to keep all of those: (# of coefs) * (# of ctx) would be enough,
    // but it takes more time to remove dead states than you gain in reduced memory.
    struct
    {
        uint16_t abs_level;
        uint16_t next;
    } level_tree[64*8*2];
    int i_levels_used = 1;

    if( dc )
    {
        if( i_coefs == 16 )
        {
            memcpy( coefs, dct, sizeof(dctcoef)*16 );
            if( !h->quantf.quant_4x4_dc( dct, quant_mf[0] >> 1, quant_bias[0] << 1 ) )
                return 0;
            h->zigzagf.scan_4x4( quant_coefs, dct );
        }
        else
        {
            memcpy( coefs, dct, sizeof(dctcoef)*i_coefs );
            int nz = h->quantf.quant_2x2_dc( &dct[0], quant_mf[0] >> 1, quant_bias[0] << 1 );
            if( i_coefs == 8 )
                nz |= h->quantf.quant_2x2_dc( &dct[4], quant_mf[0] >> 1, quant_bias[0] << 1 );
            if( !nz )
                return 0;
            for( int i = 0; i < i_coefs; i++ )
                quant_coefs[i] = dct[zigzag[i]];
        }
    }
    else
    {
        if( i_coefs == 64 )
        {
            h->mc.memcpy_aligned( coefs, dct, sizeof(dctcoef)*64 );
            if( !h->quantf.quant_8x8( dct, quant_mf, quant_bias ) )
                return 0;
            h->zigzagf.scan_8x8( quant_coefs, dct );
        }
        else //if( i_coefs == 16 )
        {
            memcpy( coefs, dct, sizeof(dctcoef)*16 );
            if( !h->quantf.quant_4x4( dct, quant_mf, quant_bias ) )
                return 0;
            h->zigzagf.scan_4x4( quant_coefs, dct );
        }
    }

    int i_last_nnz = h->quantf.coeff_last[ctx_block_cat]( quant_coefs+b_ac )+b_ac;

    idx &= i_coefs == 64 ? 3 : 15;

    /* init trellis */
    for( int j = 1; j < 8; j++ )
        nodes_cur[j].score = TRELLIS_SCORE_MAX;
    nodes_cur[0].score = 0;
    nodes_cur[0].level_idx = 0;
    level_tree[0].abs_level = 0;
    level_tree[0].next = 0;

    // coefs are processed in reverse order, because that's how the abs value is coded.
    // last_coef and significant_coef flags are normally coded in forward order, but
    // we have to reverse them to match the levels.
    // in 4x4 blocks, last_coef and significant_coef use a separate context for each
    // position, so the order doesn't matter, and we don't even have to update their contexts.
    // in 8x8 blocks, some positions share contexts, so we'll just have to hope that
    // cabac isn't too sensitive.

    memcpy( nodes_cur[0].cabac_state, &h->cabac.state[ coeff_abs_level_m1_offset[ctx_block_cat] ], 10 );

    int i;
    for( i = i_last_nnz; i >= b_ac; i-- )
    {
        // skip 0s: this doesn't affect the output, but saves some unnecessary computation.
        if( !quant_coefs[i] )
        {
            // no need to calculate ssd of 0s: it's the same in all nodes.
            // no need to modify level_tree for ctx=0: it starts with an infinite loop of 0s.
            int sigindex = !dc && i_coefs == 64 ? significant_coeff_flag_offset_8x8[b_interlaced][i] :
                           b_chroma && dc && i_coefs == 8 ? coeff_flag_offset_chroma_422_dc[i] : i;
            const uint32_t cost_sig0 = x264_cabac_size_decision_noup2( &cabac_state_sig[sigindex], 0 )
                                     * (uint64_t)i_lambda2 >> ( CABAC_SIZE_BITS - LAMBDA_BITS );
            for( int j = 1; j < 8; j++ )
            {
                if( nodes_cur[j].score != TRELLIS_SCORE_MAX )
                {
#define SET_LEVEL(n,l) \
                    level_tree[i_levels_used].abs_level = l; \
                    level_tree[i_levels_used].next = n.level_idx; \
                    n.level_idx = i_levels_used; \
                    i_levels_used++;

                    SET_LEVEL( nodes_cur[j], 0 );
                    nodes_cur[j].score += cost_sig0;
                }
            }
            continue;
        }

        int sign_coef = coefs[zigzag[i]];
        int i_coef = abs( sign_coef );
        int q = abs( quant_coefs[i] );
        int cost_sig[2], cost_last[2];
        trellis_node_t n;

        XCHG( trellis_node_t*, nodes_cur, nodes_prev );

        for( int j = 0; j < 8; j++ )
            nodes_cur[j].score = TRELLIS_SCORE_MAX;

        if( i < i_coefs-1 )
        {
            int sigindex  = !dc && i_coefs == 64 ? significant_coeff_flag_offset_8x8[b_interlaced][i] :
                            b_chroma && dc && i_coefs == 8 ? coeff_flag_offset_chroma_422_dc[i] : i;
            int lastindex = !dc && i_coefs == 64 ? last_coeff_flag_offset_8x8[i] :
                            b_chroma && dc && i_coefs == 8 ? coeff_flag_offset_chroma_422_dc[i] : i;
            cost_sig[0] = x264_cabac_size_decision_noup2( &cabac_state_sig[sigindex], 0 );
            cost_sig[1] = x264_cabac_size_decision_noup2( &cabac_state_sig[sigindex], 1 );
            cost_last[0] = x264_cabac_size_decision_noup2( &cabac_state_last[lastindex], 0 );
            cost_last[1] = x264_cabac_size_decision_noup2( &cabac_state_last[lastindex], 1 );
        }
        else
        {
            cost_sig[0] = cost_sig[1] = 0;
            cost_last[0] = cost_last[1] = 0;
        }

        // there are a few cases where increasing the coeff magnitude helps,
        // but it's only around .003 dB, and skipping them ~doubles the speed of trellis.
        // could also try q-2: that sometimes helps, but also sometimes decimates blocks
        // that are better left coded, especially at QP > 40.
        for( int abs_level = q; abs_level >= q-1; abs_level-- )
        {
            int unquant_abs_level = (((dc?unquant_mf[0]<<1:unquant_mf[zigzag[i]]) * abs_level + 128) >> 8);
            int d = i_coef - unquant_abs_level;
            int64_t ssd;
            /* Psy trellis: bias in favor of higher AC coefficients in the reconstructed frame. */
            if( h->mb.i_psy_trellis && i && !dc && !b_chroma )
            {
                int orig_coef = (i_coefs == 64) ? h->mb.pic.fenc_dct8[idx][zigzag[i]] : h->mb.pic.fenc_dct4[idx][zigzag[i]];
                int predicted_coef = orig_coef - sign_coef;
                int psy_value = h->mb.i_psy_trellis * abs(predicted_coef + SIGN(unquant_abs_level, sign_coef));
                int psy_weight = (i_coefs == 64) ? x264_dct8_weight_tab[zigzag[i]] : x264_dct4_weight_tab[zigzag[i]];
                ssd = (int64_t)d*d * coef_weight[i] - psy_weight * psy_value;
            }
            else
            /* FIXME: for i16x16 dc is this weight optimal? */
                ssd = (int64_t)d*d * (dc?256:coef_weight[i]);

            for( int j = 0; j < 8; j++ )
            {
                int node_ctx = j;
                if( nodes_prev[j].score == TRELLIS_SCORE_MAX )
                    continue;
                n = nodes_prev[j];

                /* code the proposed level, and count how much entropy it would take */
                if( abs_level || node_ctx )
                {
                    unsigned f8_bits = cost_sig[ abs_level != 0 ];
                    if( abs_level )
                    {
                        const int i_prefix = X264_MIN( abs_level - 1, 14 );
                        f8_bits += cost_last[ node_ctx == 0 ];
                        f8_bits += x264_cabac_size_decision2( &n.cabac_state[coeff_abs_level1_ctx[node_ctx]], i_prefix > 0 );
                        if( i_prefix > 0 )
                        {
                            uint8_t *ctx = &n.cabac_state[levelgt1_ctx[node_ctx]];
                            f8_bits += cabac_size_unary[i_prefix][*ctx];
                            *ctx = cabac_transition_unary[i_prefix][*ctx];
                            if( abs_level >= 15 )
                                f8_bits += bs_size_ue_big( abs_level - 15 ) << CABAC_SIZE_BITS;
                            node_ctx = coeff_abs_level_transition[1][node_ctx];
                        }
                        else
                        {
                            f8_bits += 1 << CABAC_SIZE_BITS;
                            node_ctx = coeff_abs_level_transition[0][node_ctx];
                        }
                    }
                    n.score += (uint64_t)f8_bits * i_lambda2 >> ( CABAC_SIZE_BITS - LAMBDA_BITS );
                }

                if( j || i || dc )
                    n.score += ssd;
                /* Optimize rounding for DC coefficients in DC-only luma 4x4/8x8 blocks. */
                else
                {
                    d = sign_coef - ((SIGN(unquant_abs_level, sign_coef) + 8)&~15);
                    n.score += (int64_t)d*d * coef_weight[i];
                }

                /* save the node if it's better than any existing node with the same cabac ctx */
                if( n.score < nodes_cur[node_ctx].score )
                {
                    SET_LEVEL( n, abs_level );
                    nodes_cur[node_ctx] = n;
                }
            }
        }
    }

    /* output levels from the best path through the trellis */
    bnode = &nodes_cur[0];
    for( int j = 1; j < 8; j++ )
        if( nodes_cur[j].score < bnode->score )
            bnode = &nodes_cur[j];

    if( bnode == &nodes_cur[0] )
    {
        /* We only need to zero an empty 4x4 block. 8x8 can be
           implicitly emptied via zero nnz, as can dc. */
        if( i_coefs == 16 && !dc )
            memset( dct, 0, 16 * sizeof(dctcoef) );
        return 0;
    }

    int level = bnode->level_idx;
    for( i = b_ac; i <= i_last_nnz; i++ )
    {
        dct[zigzag[i]] = SIGN(level_tree[level].abs_level, coefs[zigzag[i]]);
        level = level_tree[level].next;
    }

    return 1;
}

/* FIXME: This is a gigantic hack.  See below.
 *
 * CAVLC is much more difficult to trellis than CABAC.
 *
 * CABAC has only three states to track: significance map, last, and the
 * level state machine.
 * CAVLC, by comparison, has five: coeff_token (trailing + total),
 * total_zeroes, zero_run, and the level state machine.
 *
 * I know of no paper that has managed to design a close-to-optimal trellis
 * that covers all five of these and isn't exponential-time.  As a result, this
 * "trellis" isn't: it's just a QNS search.  Patches welcome for something better.
 * It's actually surprisingly fast, albeit not quite optimal.  It's pretty close
 * though; since CAVLC only has 2^16 possible rounding modes (assuming only two
 * roundings as options), a bruteforce search is feasible.  Testing shows
 * that this QNS is reasonably close to optimal in terms of compression.
 *
 * TODO:
 *  Don't bother changing large coefficients when it wouldn't affect bit cost
 *  (e.g. only affecting bypassed suffix bits).
 *  Don't re-run all parts of CAVLC bit cost calculation when not necessary.
 *  e.g. when changing a coefficient from one non-zero value to another in
 *  such a way that trailing ones and suffix length isn't affected. */
static ALWAYS_INLINE
int quant_trellis_cavlc( x264_t *h, dctcoef *dct,
                         const udctcoef *quant_mf, const int *unquant_mf,
                         const uint16_t *coef_weight, const uint8_t *zigzag,
                         int ctx_block_cat, int i_lambda2, int b_ac,
                         int b_chroma, int dc, int i_coefs, int idx, int b_8x8 )
{
    ALIGNED_16( dctcoef quant_coefs[2][16] );
    ALIGNED_16( dctcoef coefs[16] ) = {0};
    int delta_distortion[16];
    int64_t score = 1ULL<<62;
    int i, j;
    const int f = 1<<15;
    int nC = b_chroma && dc ? 3 + (i_coefs>>2)
                            : ct_index[x264_mb_predict_non_zero_code( h, !b_chroma && dc ? (idx - LUMA_DC)*16 : idx )];

    /* Code for handling 8x8dct -> 4x4dct CAVLC munging.  Input/output use a different
     * step/start/end than internal processing. */
    int step = 1;
    int start = b_ac;
    int end = i_coefs - 1;
    if( b_8x8 )
    {
        start = idx&3;
        end = 60 + start;
        step = 4;
    }
    idx &= 15;

    i_lambda2 <<= LAMBDA_BITS;

    /* Find last non-zero coefficient. */
    for( i = end; i >= start; i -= step )
        if( (unsigned)(dct[zigzag[i]] * (dc?quant_mf[0]>>1:quant_mf[zigzag[i]]) + f-1) >= 2*f )
            break;

    if( i < start )
        goto zeroblock;

    /* Prepare for QNS search: calculate distortion caused by each DCT coefficient
     * rounding to be searched.
     *
     * We only search two roundings (nearest and nearest-1) like in CABAC trellis,
     * so we just store the difference in distortion between them. */
    int i_last_nnz = b_8x8 ? i >> 2 : i;
    int coef_mask = 0;
    int round_mask = 0;
    for( i = b_ac, j = start; i <= i_last_nnz; i++, j += step )
    {
        int coef = dct[zigzag[j]];
        int abs_coef = abs(coef);
        int sign = coef < 0 ? -1 : 1;
        int nearest_quant = ( f + abs_coef * (dc?quant_mf[0]>>1:quant_mf[zigzag[j]]) ) >> 16;
        quant_coefs[1][i] = quant_coefs[0][i] = sign * nearest_quant;
        coefs[i] = quant_coefs[1][i];
        if( nearest_quant )
        {
            /* We initialize the trellis with a deadzone halfway between nearest rounding
             * and always-round-down.  This gives much better results than initializing to either
             * extreme.
             * FIXME: should we initialize to the deadzones used by deadzone quant? */
            int deadzone_quant = ( f/2 + abs_coef * (dc?quant_mf[0]>>1:quant_mf[zigzag[j]]) ) >> 16;
            int unquant1 = (((dc?unquant_mf[0]<<1:unquant_mf[zigzag[j]]) * (nearest_quant-0) + 128) >> 8);
            int unquant0 = (((dc?unquant_mf[0]<<1:unquant_mf[zigzag[j]]) * (nearest_quant-1) + 128) >> 8);
            int d1 = abs_coef - unquant1;
            int d0 = abs_coef - unquant0;
            delta_distortion[i] = (d0*d0 - d1*d1) * (dc?256:coef_weight[j]);

            /* Psy trellis: bias in favor of higher AC coefficients in the reconstructed frame. */
            if( h->mb.i_psy_trellis && j && !dc && !b_chroma )
            {
                int orig_coef = b_8x8 ? h->mb.pic.fenc_dct8[idx>>2][zigzag[j]] : h->mb.pic.fenc_dct4[idx][zigzag[j]];
                int predicted_coef = orig_coef - coef;
                int psy_weight = b_8x8 ? x264_dct8_weight_tab[zigzag[j]] : x264_dct4_weight_tab[zigzag[j]];
                int psy_value0 = h->mb.i_psy_trellis * abs(predicted_coef + unquant0 * sign);
                int psy_value1 = h->mb.i_psy_trellis * abs(predicted_coef + unquant1 * sign);
                delta_distortion[i] += (psy_value0 - psy_value1) * psy_weight;
            }

            quant_coefs[0][i] = sign * (nearest_quant-1);
            if( deadzone_quant != nearest_quant )
                coefs[i] = quant_coefs[0][i];
            else
                round_mask |= 1 << i;
        }
        else
            delta_distortion[i] = 0;
        coef_mask |= (!!coefs[i]) << i;
    }

    /* Calculate the cost of the starting state. */
    h->out.bs.i_bits_encoded = 0;
    if( !coef_mask )
        bs_write_vlc( &h->out.bs, x264_coeff0_token[nC] );
    else
        x264_cavlc_block_residual_internal( h, ctx_block_cat, coefs + b_ac, nC );
    score = (int64_t)h->out.bs.i_bits_encoded * i_lambda2;

    /* QNS loop: pick the change that improves RD the most, apply it, repeat.
     * coef_mask and round_mask are used to simplify tracking of nonzeroness
     * and rounding modes chosen. */
    while( 1 )
    {
        int64_t iter_score = score;
        int iter_distortion_delta = 0;
        int iter_coef = -1;
        int iter_mask = coef_mask;
        int iter_round = round_mask;
        for( i = b_ac; i <= i_last_nnz; i++ )
        {
            if( !delta_distortion[i] )
                continue;

            /* Set up all the variables for this iteration. */
            int cur_round = round_mask ^ (1 << i);
            int round_change = (cur_round >> i)&1;
            int old_coef = coefs[i];
            int new_coef = quant_coefs[round_change][i];
            int cur_mask = (coef_mask&~(1 << i))|(!!new_coef << i);
            int cur_distortion_delta = delta_distortion[i] * (round_change ? -1 : 1);
            int64_t cur_score = cur_distortion_delta;
            coefs[i] = new_coef;

            /* Count up bits. */
            h->out.bs.i_bits_encoded = 0;
            if( !cur_mask )
                bs_write_vlc( &h->out.bs, x264_coeff0_token[nC] );
            else
                x264_cavlc_block_residual_internal( h, ctx_block_cat, coefs + b_ac, nC );
            cur_score += (int64_t)h->out.bs.i_bits_encoded * i_lambda2;

            coefs[i] = old_coef;
            if( cur_score < iter_score )
            {
                iter_score = cur_score;
                iter_coef = i;
                iter_mask = cur_mask;
                iter_round = cur_round;
                iter_distortion_delta = cur_distortion_delta;
            }
        }
        if( iter_coef >= 0 )
        {
            score = iter_score - iter_distortion_delta;
            coef_mask = iter_mask;
            round_mask = iter_round;
            coefs[iter_coef] = quant_coefs[((round_mask >> iter_coef)&1)][iter_coef];
            /* Don't try adjusting coefficients we've already adjusted.
             * Testing suggests this doesn't hurt results -- and sometimes actually helps. */
            delta_distortion[iter_coef] = 0;
        }
        else
            break;
    }

    if( coef_mask )
    {
        for( i = b_ac, j = start; i < i_coefs; i++, j += step )
            dct[zigzag[j]] = coefs[i];
        return 1;
    }

zeroblock:
    if( !dc )
    {
        if( b_8x8 )
            for( i = start; i <= end; i+=step )
                dct[zigzag[i]] = 0;
        else
            memset( dct, 0, 16*sizeof(dctcoef) );
    }
    return 0;
}

int x264_quant_luma_dc_trellis( x264_t *h, dctcoef *dct, int i_quant_cat, int i_qp, int ctx_block_cat, int b_intra, int idx )
{
    if( h->param.b_cabac )
        return quant_trellis_cabac( h, dct,
            h->quant4_mf[i_quant_cat][i_qp], h->quant4_bias0[i_quant_cat][i_qp],
            h->unquant4_mf[i_quant_cat][i_qp], NULL, x264_zigzag_scan4[MB_INTERLACED],
            ctx_block_cat, h->mb.i_trellis_lambda2[0][b_intra], 0, 0, 1, 16, idx );

    return quant_trellis_cavlc( h, dct,
        h->quant4_mf[i_quant_cat][i_qp], h->unquant4_mf[i_quant_cat][i_qp], NULL, x264_zigzag_scan4[MB_INTERLACED],
        DCT_LUMA_DC, h->mb.i_trellis_lambda2[0][b_intra], 0, 0, 1, 16, idx, 0 );
}

static const uint8_t x264_zigzag_scan2x2[4] = { 0, 1, 2, 3 };
static const uint8_t x264_zigzag_scan2x4[8] = { 0, 2, 1, 4, 6, 3, 5, 7 };

int x264_quant_chroma_dc_trellis( x264_t *h, dctcoef *dct, int i_qp, int b_intra, int idx )
{
    const uint8_t *zigzag;
    int num_coefs;
    int quant_cat = CQM_4IC+1 - b_intra;

    if( CHROMA_FORMAT == CHROMA_422 )
    {
        zigzag = x264_zigzag_scan2x4;
        num_coefs = 8;
    }
    else
    {
        zigzag = x264_zigzag_scan2x2;
        num_coefs = 4;
    }

    if( h->param.b_cabac )
        return quant_trellis_cabac( h, dct,
            h->quant4_mf[quant_cat][i_qp], h->quant4_bias0[quant_cat][i_qp],
            h->unquant4_mf[quant_cat][i_qp], NULL, zigzag,
            DCT_CHROMA_DC, h->mb.i_trellis_lambda2[1][b_intra], 0, 1, 1, num_coefs, idx );

    return quant_trellis_cavlc( h, dct,
        h->quant4_mf[quant_cat][i_qp], h->unquant4_mf[quant_cat][i_qp], NULL, zigzag,
        DCT_CHROMA_DC, h->mb.i_trellis_lambda2[1][b_intra], 0, 1, 1, num_coefs, idx, 0 );
}

int x264_quant_4x4_trellis( x264_t *h, dctcoef *dct, int i_quant_cat,
                            int i_qp, int ctx_block_cat, int b_intra, int b_chroma, int idx )
{
    static const uint8_t ctx_ac[14] = {0,1,0,0,1,0,0,1,0,0,0,1,0,0};
    int b_ac = ctx_ac[ctx_block_cat];
    if( h->param.b_cabac )
        return quant_trellis_cabac( h, dct,
            h->quant4_mf[i_quant_cat][i_qp], h->quant4_bias0[i_quant_cat][i_qp],
            h->unquant4_mf[i_quant_cat][i_qp], x264_dct4_weight2_zigzag[MB_INTERLACED],
            x264_zigzag_scan4[MB_INTERLACED],
            ctx_block_cat, h->mb.i_trellis_lambda2[b_chroma][b_intra], b_ac, b_chroma, 0, 16, idx );

    return quant_trellis_cavlc( h, dct,
            h->quant4_mf[i_quant_cat][i_qp], h->unquant4_mf[i_quant_cat][i_qp],
            x264_dct4_weight2_zigzag[MB_INTERLACED],
            x264_zigzag_scan4[MB_INTERLACED],
            ctx_block_cat, h->mb.i_trellis_lambda2[b_chroma][b_intra], b_ac, b_chroma, 0, 16, idx, 0 );
}

int x264_quant_8x8_trellis( x264_t *h, dctcoef *dct, int i_quant_cat,
                            int i_qp, int ctx_block_cat, int b_intra, int b_chroma, int idx )
{
    if( h->param.b_cabac )
    {
        return quant_trellis_cabac( h, dct,
            h->quant8_mf[i_quant_cat][i_qp], h->quant8_bias0[i_quant_cat][i_qp],
            h->unquant8_mf[i_quant_cat][i_qp], x264_dct8_weight2_zigzag[MB_INTERLACED],
            x264_zigzag_scan8[MB_INTERLACED],
            ctx_block_cat, h->mb.i_trellis_lambda2[b_chroma][b_intra], 0, b_chroma, 0, 64, idx );
    }

    /* 8x8 CAVLC is split into 4 4x4 blocks */
    int nzaccum = 0;
    for( int i = 0; i < 4; i++ )
    {
        int nz = quant_trellis_cavlc( h, dct,
            h->quant8_mf[i_quant_cat][i_qp], h->unquant8_mf[i_quant_cat][i_qp],
            x264_dct8_weight2_zigzag[MB_INTERLACED],
            x264_zigzag_scan8[MB_INTERLACED],
            DCT_LUMA_4x4, h->mb.i_trellis_lambda2[b_chroma][b_intra], 0, b_chroma, 0, 16, idx*4+i, 1 );
        /* Set up nonzero count for future calls */
        h->mb.cache.non_zero_count[x264_scan8[idx*4+i]] = nz;
        nzaccum |= nz;
    }
    return nzaccum;
}

typedef struct
{
    /* 16x16 */
    int       i_rd16x16;
    x264_me_t me16x16;
    x264_me_t bi16x16;      /* for b16x16 BI mode, since MVs can differ from l0/l1 */

    /* 8x8 */
    int       i_cost8x8;
    /* [ref][0] is 16x16 mv, [ref][1..4] are 8x8 mv from partition [0..3] */
    ALIGNED_4( int16_t mvc[32][5][2] );
    x264_me_t me8x8[4];

    /* Sub 4x4 */
    int       i_cost4x4[4]; /* cost per 8x8 partition */
    x264_me_t me4x4[4][4];

    /* Sub 8x4 */
    int       i_cost8x4[4]; /* cost per 8x8 partition */
    x264_me_t me8x4[4][2];

    /* Sub 4x8 */
    int       i_cost4x8[4]; /* cost per 8x8 partition */
    x264_me_t me4x8[4][2];

    /* 16x8 */
    int       i_cost16x8;
    x264_me_t me16x8[2];

    /* 8x16 */
    int       i_cost8x16;
    x264_me_t me8x16[2];

} x264_mb_analysis_list_t;

typedef struct
{
    /* conduct the analysis using this lamda and QP */
    int i_lambda;
    int i_lambda2;
    int i_qp;
    uint16_t *p_cost_mv;
    uint16_t *p_cost_ref[2];
    int i_mbrd;


    /* I: Intra part */
    /* Take some shortcuts in intra search if intra is deemed unlikely */
    int b_fast_intra;
    int b_force_intra; /* For Periodic Intra Refresh.  Only supported in P-frames. */
    int b_avoid_topright; /* For Periodic Intra Refresh: don't predict from top-right pixels. */
    int b_try_skip;

    /* Luma part */
    int i_satd_i16x16;
    int i_satd_i16x16_dir[7];
    int i_predict16x16;

    int i_satd_i8x8;
    int i_cbp_i8x8_luma;
    ALIGNED_16( uint16_t i_satd_i8x8_dir[4][16] );
    int i_predict8x8[4];

    int i_satd_i4x4;
    int i_predict4x4[16];

    int i_satd_pcm;

    /* Chroma part */
    int i_satd_chroma;
    int i_satd_chroma_dir[7];
    int i_predict8x8chroma;

    /* II: Inter part P/B frame */
    x264_mb_analysis_list_t l0;
    x264_mb_analysis_list_t l1;

    int i_cost16x16bi; /* used the same ref and mv as l0 and l1 (at least for now) */
    int i_cost16x16direct;
    int i_cost8x8bi;
    int i_cost8x8direct[4];
    int i_satd8x8[3][4]; /* [L0,L1,BI][8x8 0..3] SATD only */
    int i_cost_est16x8[2]; /* Per-partition estimated cost */
    int i_cost_est8x16[2];
    int i_cost16x8bi;
    int i_cost8x16bi;
    int i_rd16x16bi;
    int i_rd16x16direct;
    int i_rd16x8bi;
    int i_rd8x16bi;
    int i_rd8x8bi;

    int i_mb_partition16x8[2]; /* mb_partition_e */
    int i_mb_partition8x16[2];
    int i_mb_type16x8; /* mb_class_e */
    int i_mb_type8x16;

    int b_direct_available;
    int b_early_terminate;

} x264_mb_analysis_t;

/* lambda = pow(2,qp/6-2) */
const uint16_t x264_lambda_tab[QP_MAX_MAX+1] =
{
   1,   1,   1,   1,   1,   1,   1,   1, /*  0- 7 */
   1,   1,   1,   1,   1,   1,   1,   1, /*  8-15 */
   2,   2,   2,   2,   3,   3,   3,   4, /* 16-23 */
   4,   4,   5,   6,   6,   7,   8,   9, /* 24-31 */
  10,  11,  13,  14,  16,  18,  20,  23, /* 32-39 */
  25,  29,  32,  36,  40,  45,  51,  57, /* 40-47 */
  64,  72,  81,  91, 102, 114, 128, 144, /* 48-55 */
 161, 181, 203, 228, 256, 287, 323, 362, /* 56-63 */
 406, 456, 512, 575, 645, 724, 813, 912, /* 64-71 */
1024,1149,1290,1448,1625,1825,2048,2299, /* 72-79 */
2048,2299,                               /* 80-81 */
};

/* lambda2 = pow(lambda,2) * .9 * 256 */
/* Capped to avoid overflow */
const int x264_lambda2_tab[QP_MAX_MAX+1] =
{
       14,       18,       22,       28,       36,       45,      57,      72, /*  0- 7 */
       91,      115,      145,      182,      230,      290,     365,     460, /*  8-15 */
      580,      731,      921,     1161,     1462,     1843,    2322,    2925, /* 16-23 */
     3686,     4644,     5851,     7372,     9289,    11703,   14745,   18578, /* 24-31 */
    23407,    29491,    37156,    46814,    58982,    74313,   93628,  117964, /* 32-39 */
   148626,   187257,   235929,   297252,   374514,   471859,  594505,  749029, /* 40-47 */
   943718,  1189010,  1498059,  1887436,  2378021,  2996119, 3774873, 4756042, /* 48-55 */
  5992238,  7549747,  9512085, 11984476, 15099494, 19024170,23968953,30198988, /* 56-63 */
 38048341, 47937906, 60397977, 76096683, 95875813,120795955,                   /* 64-69 */
134217727,134217727,134217727,134217727,134217727,134217727,                   /* 70-75 */
134217727,134217727,134217727,134217727,134217727,134217727,                   /* 76-81 */
};

const uint8_t x264_exp2_lut[64] =
{
      0,   3,   6,   8,  11,  14,  17,  20,  23,  26,  29,  32,  36,  39,  42,  45,
     48,  52,  55,  58,  62,  65,  69,  72,  76,  80,  83,  87,  91,  94,  98, 102,
    106, 110, 114, 118, 122, 126, 130, 135, 139, 143, 147, 152, 156, 161, 165, 170,
    175, 179, 184, 189, 194, 198, 203, 208, 214, 219, 224, 229, 234, 240, 245, 250
};

const float x264_log2_lut[128] =
{
    0.00000, 0.01123, 0.02237, 0.03342, 0.04439, 0.05528, 0.06609, 0.07682,
    0.08746, 0.09803, 0.10852, 0.11894, 0.12928, 0.13955, 0.14975, 0.15987,
    0.16993, 0.17991, 0.18982, 0.19967, 0.20945, 0.21917, 0.22882, 0.23840,
    0.24793, 0.25739, 0.26679, 0.27612, 0.28540, 0.29462, 0.30378, 0.31288,
    0.32193, 0.33092, 0.33985, 0.34873, 0.35755, 0.36632, 0.37504, 0.38370,
    0.39232, 0.40088, 0.40939, 0.41785, 0.42626, 0.43463, 0.44294, 0.45121,
    0.45943, 0.46761, 0.47573, 0.48382, 0.49185, 0.49985, 0.50779, 0.51570,
    0.52356, 0.53138, 0.53916, 0.54689, 0.55459, 0.56224, 0.56986, 0.57743,
    0.58496, 0.59246, 0.59991, 0.60733, 0.61471, 0.62205, 0.62936, 0.63662,
    0.64386, 0.65105, 0.65821, 0.66534, 0.67243, 0.67948, 0.68650, 0.69349,
    0.70044, 0.70736, 0.71425, 0.72110, 0.72792, 0.73471, 0.74147, 0.74819,
    0.75489, 0.76155, 0.76818, 0.77479, 0.78136, 0.78790, 0.79442, 0.80090,
    0.80735, 0.81378, 0.82018, 0.82655, 0.83289, 0.83920, 0.84549, 0.85175,
    0.85798, 0.86419, 0.87036, 0.87652, 0.88264, 0.88874, 0.89482, 0.90087,
    0.90689, 0.91289, 0.91886, 0.92481, 0.93074, 0.93664, 0.94251, 0.94837,
    0.95420, 0.96000, 0.96578, 0.97154, 0.97728, 0.98299, 0.98868, 0.99435,
};

/* Avoid an int/float conversion. */
const float x264_log2_lz_lut[32] =
{
    31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
};

// should the intra and inter lambdas be different?
// I'm just matching the behaviour of deadzone quant.
static const int x264_trellis_lambda2_tab[2][QP_MAX_MAX+1] =
{
    // inter lambda = .85 * .85 * 2**(qp/3. + 10 - LAMBDA_BITS)
    {
               46,       58,       73,       92,      117,      147,
              185,      233,      294,      370,      466,      587,
              740,      932,     1174,     1480,     1864,     2349,
             2959,     3728,     4697,     5918,     7457,     9395,
            11837,    14914,    18790,    23674,    29828,    37581,
            47349,    59656,    75163,    94699,   119313,   150326,
           189399,   238627,   300652,   378798,   477255,   601304,
           757596,   954511,  1202608,  1515192,  1909022,  2405217,
          3030384,  3818045,  4810435,  6060769,  7636091,  9620872,
         12121539, 15272182, 19241743, 24243077, 30544363, 38483486,
         48486154, 61088726, 76966972, 96972308,
        122177453,134217727,134217727,134217727,134217727,134217727,
        134217727,134217727,134217727,134217727,134217727,134217727,
    },
    // intra lambda = .65 * .65 * 2**(qp/3. + 10 - LAMBDA_BITS)
    {
               27,       34,       43,       54,       68,       86,
              108,      136,      172,      216,      273,      343,
              433,      545,      687,      865,     1090,     1374,
             1731,     2180,     2747,     3461,     4361,     5494,
             6922,     8721,    10988,    13844,    17442,    21976,
            27688,    34885,    43953,    55377,    69771,    87906,
           110755,   139543,   175813,   221511,   279087,   351627,
           443023,   558174,   703255,   886046,  1116348,  1406511,
          1772093,  2232697,  2813022,  3544186,  4465396,  5626046,
          7088374,  8930791, 11252092, 14176748, 17861583, 22504184,
         28353495, 35723165, 45008368, 56706990,
         71446330, 90016736,113413980,134217727,134217727,134217727,
        134217727,134217727,134217727,134217727,134217727,134217727,
        134217727,134217727,134217727,134217727,134217727,134217727,
    }
};

#define MAX_CHROMA_LAMBDA_OFFSET 36
static const uint16_t x264_chroma_lambda2_offset_tab[MAX_CHROMA_LAMBDA_OFFSET+1] =
{
       16,    20,    25,    32,    40,    50,
       64,    80,   101,   128,   161,   203,
      256,   322,   406,   512,   645,   812,
     1024,  1290,  1625,  2048,  2580,  3250,
     4096,  5160,  6501,  8192, 10321, 13003,
    16384, 20642, 26007, 32768, 41285, 52015,
    65535
};

/* TODO: calculate CABAC costs */
static const uint8_t i_mb_b_cost_table[X264_MBTYPE_MAX] =
{
    9, 9, 9, 9, 0, 0, 0, 1, 3, 7, 7, 7, 3, 7, 7, 7, 5, 9, 0
};
static const uint8_t i_mb_b16x8_cost_table[17] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 7, 7, 5, 7, 9, 9, 9
};
static const uint8_t i_sub_mb_b_cost_table[13] =
{
    7, 5, 5, 3, 7, 5, 7, 3, 7, 7, 7, 5, 1
};
static const uint8_t i_sub_mb_p_cost_table[4] =
{
    5, 3, 3, 1
};

static void x264_analyse_update_cache( x264_t *h, x264_mb_analysis_t *a );

static uint16_t x264_cost_ref[QP_MAX+1][3][33];
static UNUSED x264_pthread_mutex_t cost_ref_mutex = X264_PTHREAD_MUTEX_INITIALIZER;
static uint16_t x264_cost_i4x4_mode[(QP_MAX+2)*32];

float *x264_analyse_prepare_costs( x264_t *h )
{
    float *logs = x264_malloc( (2*4*2048+1)*sizeof(float) );
    if( !logs )
        return NULL;
    logs[0] = 0.718f;
    for( int i = 1; i <= 2*4*2048; i++ )
        logs[i] = log2f(i+1)*2 + 1.718f;
    return logs;
}

int x264_analyse_init_costs( x264_t *h, float *logs, int qp )
{
    int lambda = x264_lambda_tab[qp];
    if( h->cost_mv[qp] )
        return 0;
    /* factor of 4 from qpel, 2 from sign, and 2 because mv can be opposite from mvp */
    CHECKED_MALLOC( h->cost_mv[qp], (4*4*2048 + 1) * sizeof(uint16_t) );
    h->cost_mv[qp] += 2*4*2048;
    for( int i = 0; i <= 2*4*2048; i++ )
    {
        h->cost_mv[qp][-i] =
        h->cost_mv[qp][i]  = X264_MIN( lambda * logs[i] + .5f, (1<<16)-1 );
    }
    x264_pthread_mutex_lock( &cost_ref_mutex );
    for( int i = 0; i < 3; i++ )
        for( int j = 0; j < 33; j++ )
            x264_cost_ref[qp][i][j] = X264_MIN( i ? lambda * bs_size_te( i, j ) : 0, (1<<16)-1 );
    x264_pthread_mutex_unlock( &cost_ref_mutex );
    if( h->param.analyse.i_me_method >= X264_ME_ESA && !h->cost_mv_fpel[qp][0] )
    {
        for( int j = 0; j < 4; j++ )
        {
            CHECKED_MALLOC( h->cost_mv_fpel[qp][j], (4*2048 + 1) * sizeof(uint16_t) );
            h->cost_mv_fpel[qp][j] += 2*2048;
            for( int i = -2*2048; i < 2*2048; i++ )
                h->cost_mv_fpel[qp][j][i] = h->cost_mv[qp][i*4+j];
        }
    }
    uint16_t *cost_i4x4_mode = (uint16_t*)ALIGN((intptr_t)x264_cost_i4x4_mode,64) + qp*32;
    for( int i = 0; i < 17; i++ )
        cost_i4x4_mode[i] = 3*lambda*(i!=8);
    return 0;
fail:
    return -1;
}

void x264_analyse_free_costs( x264_t *h )
{
    for( int i = 0; i < QP_MAX+1; i++ )
    {
        if( h->cost_mv[i] )
            x264_free( h->cost_mv[i] - 2*4*2048 );
        if( h->cost_mv_fpel[i][0] )
            for( int j = 0; j < 4; j++ )
                x264_free( h->cost_mv_fpel[i][j] - 2*2048 );
    }
}

void x264_analyse_weight_frame( x264_t *h, int end )
{
    for( int j = 0; j < h->i_ref[0]; j++ )
    {
        if( h->sh.weight[j][0].weightfn )
        {
            x264_frame_t *frame = h->fref[0][j];
            int width = frame->i_width[0] + 2*PADH;
            int i_padv = PADV << PARAM_INTERLACED;
            int offset, height;
            pixel *src = frame->filtered[0][0] - frame->i_stride[0]*i_padv - PADH;
            height = X264_MIN( 16 + end + i_padv, h->fref[0][j]->i_lines[0] + i_padv*2 ) - h->fenc->i_lines_weighted;
            offset = h->fenc->i_lines_weighted*frame->i_stride[0];
            h->fenc->i_lines_weighted += height;
            if( height )
                for( int k = j; k < h->i_ref[0]; k++ )
                    if( h->sh.weight[k][0].weightfn )
                    {
                        pixel *dst = h->fenc->weighted[k] - h->fenc->i_stride[0]*i_padv - PADH;
                        x264_weight_scale_plane( h, dst + offset, frame->i_stride[0],
                                                 src + offset, frame->i_stride[0],
                                                 width, height, &h->sh.weight[k][0] );
                    }
            break;
        }
    }
}

/* initialize an array of lambda*nbits for all possible mvs */
static void x264_mb_analyse_load_costs( x264_t *h, x264_mb_analysis_t *a )
{
    a->p_cost_mv = h->cost_mv[a->i_qp];
    a->p_cost_ref[0] = x264_cost_ref[a->i_qp][x264_clip3(h->sh.i_num_ref_idx_l0_active-1,0,2)];
    a->p_cost_ref[1] = x264_cost_ref[a->i_qp][x264_clip3(h->sh.i_num_ref_idx_l1_active-1,0,2)];
}

static void x264_mb_analyse_init_qp( x264_t *h, x264_mb_analysis_t *a, int qp )
{
    int effective_chroma_qp = h->chroma_qp_table[SPEC_QP(qp)] + X264_MAX( qp - QP_MAX_SPEC, 0 );
    a->i_lambda = x264_lambda_tab[qp];
    a->i_lambda2 = x264_lambda2_tab[qp];

    h->mb.b_trellis = h->param.analyse.i_trellis > 1 && a->i_mbrd;
    if( h->param.analyse.i_trellis )
    {
        h->mb.i_trellis_lambda2[0][0] = x264_trellis_lambda2_tab[0][qp];
        h->mb.i_trellis_lambda2[0][1] = x264_trellis_lambda2_tab[1][qp];
        h->mb.i_trellis_lambda2[1][0] = x264_trellis_lambda2_tab[0][effective_chroma_qp];
        h->mb.i_trellis_lambda2[1][1] = x264_trellis_lambda2_tab[1][effective_chroma_qp];
    }
    h->mb.i_psy_rd_lambda = a->i_lambda;
    /* Adjusting chroma lambda based on QP offset hurts PSNR but improves visual quality. */
    int chroma_offset_idx = X264_MIN( qp-effective_chroma_qp+12, MAX_CHROMA_LAMBDA_OFFSET );
    h->mb.i_chroma_lambda2_offset = h->param.analyse.b_psy ? x264_chroma_lambda2_offset_tab[chroma_offset_idx] : 256;

    if( qp > QP_MAX_SPEC )
    {
        h->nr_offset = h->nr_offset_emergency[qp-QP_MAX_SPEC-1];
        h->nr_residual_sum = h->nr_residual_sum_buf[1];
        h->nr_count = h->nr_count_buf[1];
        h->mb.b_noise_reduction = 1;
        qp = QP_MAX_SPEC; /* Out-of-spec QPs are just used for calculating lambda values. */
    }
    else
    {
        h->nr_offset = h->nr_offset_denoise;
        h->nr_residual_sum = h->nr_residual_sum_buf[0];
        h->nr_count = h->nr_count_buf[0];
        h->mb.b_noise_reduction = 0;
    }

    a->i_qp = h->mb.i_qp = qp;
    h->mb.i_chroma_qp = h->chroma_qp_table[qp];
}

static void x264_mb_analyse_init( x264_t *h, x264_mb_analysis_t *a, int qp )
{
    int subme = h->param.analyse.i_subpel_refine - (h->sh.i_type == SLICE_TYPE_B);

    /* mbrd == 1 -> RD mode decision */
    /* mbrd == 2 -> RD refinement */
    /* mbrd == 3 -> QPRD */
    a->i_mbrd = (subme>=6) + (subme>=8) + (h->param.analyse.i_subpel_refine>=10);
    h->mb.b_deblock_rdo = h->param.analyse.i_subpel_refine >= 9 && h->sh.i_disable_deblocking_filter_idc != 1;
    a->b_early_terminate = h->param.analyse.i_subpel_refine < 11;

    x264_mb_analyse_init_qp( h, a, qp );

    h->mb.b_transform_8x8 = 0;

    /* I: Intra part */
    a->i_satd_i16x16 =
    a->i_satd_i8x8   =
    a->i_satd_i4x4   =
    a->i_satd_chroma = COST_MAX;

    /* non-RD PCM decision is inaccurate (as is psy-rd), so don't do it */
    a->i_satd_pcm = !h->mb.i_psy_rd && a->i_mbrd ? ((uint64_t)X264_PCM_COST*a->i_lambda2 + 128) >> 8 : COST_MAX;

    a->b_fast_intra = 0;
    a->b_avoid_topright = 0;
    h->mb.i_skip_intra =
        h->mb.b_lossless ? 0 :
        a->i_mbrd ? 2 :
        !h->param.analyse.i_trellis && !h->param.analyse.i_noise_reduction;

    /* II: Inter part P/B frame */
    if( h->sh.i_type != SLICE_TYPE_I )
    {
        int i_fmv_range = 4 * h->param.analyse.i_mv_range;
        // limit motion search to a slightly smaller range than the theoretical limit,
        // since the search may go a few iterations past its given range
        int i_fpel_border = 6; // umh: 1 for diamond, 2 for octagon, 2 for hpel

        /* Calculate max allowed MV range */
#define CLIP_FMV(mv) x264_clip3( mv, -i_fmv_range, i_fmv_range-1 )
        h->mb.mv_min[0] = 4*( -16*h->mb.i_mb_x - 24 );
        h->mb.mv_max[0] = 4*( 16*( h->mb.i_mb_width - h->mb.i_mb_x - 1 ) + 24 );
        h->mb.mv_min_spel[0] = CLIP_FMV( h->mb.mv_min[0] );
        h->mb.mv_max_spel[0] = CLIP_FMV( h->mb.mv_max[0] );
        if( h->param.b_intra_refresh && h->sh.i_type == SLICE_TYPE_P )
        {
            int max_x = (h->fref[0][0]->i_pir_end_col * 16 - 3)*4; /* 3 pixels of hpel border */
            int max_mv = max_x - 4*16*h->mb.i_mb_x;
            /* If we're left of the refresh bar, don't reference right of it. */
            if( max_mv > 0 && h->mb.i_mb_x < h->fdec->i_pir_start_col )
                h->mb.mv_max_spel[0] = X264_MIN( h->mb.mv_max_spel[0], max_mv );
        }
        h->mb.mv_min_fpel[0] = (h->mb.mv_min_spel[0]>>2) + i_fpel_border;
        h->mb.mv_max_fpel[0] = (h->mb.mv_max_spel[0]>>2) - i_fpel_border;
        if( h->mb.i_mb_x == 0 && !(h->mb.i_mb_y & PARAM_INTERLACED) )
        {
            int mb_y = h->mb.i_mb_y >> SLICE_MBAFF;
            int thread_mvy_range = i_fmv_range;

            if( h->i_thread_frames > 1 )
            {
                int pix_y = (h->mb.i_mb_y | PARAM_INTERLACED) * 16;
                int thresh = pix_y + h->param.analyse.i_mv_range_thread;
                for( int i = (h->sh.i_type == SLICE_TYPE_B); i >= 0; i-- )
                    for( int j = 0; j < h->i_ref[i]; j++ )
                    {
                        x264_frame_cond_wait( h->fref[i][j]->orig, thresh );
                        thread_mvy_range = X264_MIN( thread_mvy_range, h->fref[i][j]->orig->i_lines_completed - pix_y );
                    }

                if( h->param.b_deterministic )
                    thread_mvy_range = h->param.analyse.i_mv_range_thread;
                if( PARAM_INTERLACED )
                    thread_mvy_range >>= 1;

                x264_analyse_weight_frame( h, pix_y + thread_mvy_range );
            }

            if( PARAM_INTERLACED )
            {
                /* 0 == top progressive, 1 == bot progressive, 2 == interlaced */
                for( int i = 0; i < 3; i++ )
                {
                    int j = i == 2;
                    mb_y = (h->mb.i_mb_y >> j) + (i == 1);
                    h->mb.mv_miny_row[i] = 4*( -16*mb_y - 24 );
                    h->mb.mv_maxy_row[i] = 4*( 16*( (h->mb.i_mb_height>>j) - mb_y - 1 ) + 24 );
                    h->mb.mv_miny_spel_row[i] = x264_clip3( h->mb.mv_miny_row[i], -i_fmv_range, i_fmv_range );
                    h->mb.mv_maxy_spel_row[i] = CLIP_FMV( h->mb.mv_maxy_row[i] );
                    h->mb.mv_maxy_spel_row[i] = X264_MIN( h->mb.mv_maxy_spel_row[i], thread_mvy_range*4 );
                    h->mb.mv_miny_fpel_row[i] = (h->mb.mv_miny_spel_row[i]>>2) + i_fpel_border;
                    h->mb.mv_maxy_fpel_row[i] = (h->mb.mv_maxy_spel_row[i]>>2) - i_fpel_border;
                }
            }
            else
            {
                h->mb.mv_min[1] = 4*( -16*mb_y - 24 );
                h->mb.mv_max[1] = 4*( 16*( h->mb.i_mb_height - mb_y - 1 ) + 24 );
                h->mb.mv_min_spel[1] = x264_clip3( h->mb.mv_min[1], -i_fmv_range, i_fmv_range );
                h->mb.mv_max_spel[1] = CLIP_FMV( h->mb.mv_max[1] );
                h->mb.mv_max_spel[1] = X264_MIN( h->mb.mv_max_spel[1], thread_mvy_range*4 );
                h->mb.mv_min_fpel[1] = (h->mb.mv_min_spel[1]>>2) + i_fpel_border;
                h->mb.mv_max_fpel[1] = (h->mb.mv_max_spel[1]>>2) - i_fpel_border;
            }
        }
        if( PARAM_INTERLACED )
        {
            int i = MB_INTERLACED ? 2 : h->mb.i_mb_y&1;
            h->mb.mv_min[1] = h->mb.mv_miny_row[i];
            h->mb.mv_max[1] = h->mb.mv_maxy_row[i];
            h->mb.mv_min_spel[1] = h->mb.mv_miny_spel_row[i];
            h->mb.mv_max_spel[1] = h->mb.mv_maxy_spel_row[i];
            h->mb.mv_min_fpel[1] = h->mb.mv_miny_fpel_row[i];
            h->mb.mv_max_fpel[1] = h->mb.mv_maxy_fpel_row[i];
        }
#undef CLIP_FMV

        a->l0.me16x16.cost =
        a->l0.i_rd16x16    =
        a->l0.i_cost8x8    =
        a->l0.i_cost16x8   =
        a->l0.i_cost8x16   = COST_MAX;
        if( h->sh.i_type == SLICE_TYPE_B )
        {
            a->l1.me16x16.cost =
            a->l1.i_rd16x16    =
            a->l1.i_cost8x8    =
            a->i_cost8x8direct[0] =
            a->i_cost8x8direct[1] =
            a->i_cost8x8direct[2] =
            a->i_cost8x8direct[3] =
            a->l1.i_cost16x8   =
            a->l1.i_cost8x16   =
            a->i_rd16x16bi     =
            a->i_rd16x16direct =
            a->i_rd8x8bi       =
            a->i_rd16x8bi      =
            a->i_rd8x16bi      =
            a->i_cost16x16bi   =
            a->i_cost16x16direct =
            a->i_cost8x8bi     =
            a->i_cost16x8bi    =
            a->i_cost8x16bi    = COST_MAX;
        }
        else if( h->param.analyse.inter & X264_ANALYSE_PSUB8x8 )
            for( int i = 0; i < 4; i++ )
            {
                a->l0.i_cost4x4[i] =
                a->l0.i_cost8x4[i] =
                a->l0.i_cost4x8[i] = COST_MAX;
            }

        /* Fast intra decision */
        if( a->b_early_terminate && h->mb.i_mb_xy - h->sh.i_first_mb > 4 )
        {
            /* Always run in fast-intra mode for subme < 3 */
            if( h->mb.i_subpel_refine > 2 &&
              ( IS_INTRA( h->mb.i_mb_type_left[0] ) ||
                IS_INTRA( h->mb.i_mb_type_top ) ||
                IS_INTRA( h->mb.i_mb_type_topleft ) ||
                IS_INTRA( h->mb.i_mb_type_topright ) ||
                (h->sh.i_type == SLICE_TYPE_P && IS_INTRA( h->fref[0][0]->mb_type[h->mb.i_mb_xy] )) ||
                (h->mb.i_mb_xy - h->sh.i_first_mb < 3*(h->stat.frame.i_mb_count[I_4x4] + h->stat.frame.i_mb_count[I_8x8] + h->stat.frame.i_mb_count[I_16x16])) ) )
            { /* intra is likely */ }
            else
            {
                a->b_fast_intra = 1;
            }
        }
        h->mb.b_skip_mc = 0;
        if( h->param.b_intra_refresh && h->sh.i_type == SLICE_TYPE_P &&
            h->mb.i_mb_x >= h->fdec->i_pir_start_col && h->mb.i_mb_x <= h->fdec->i_pir_end_col )
        {
            a->b_force_intra = 1;
            a->b_fast_intra = 0;
            a->b_avoid_topright = h->mb.i_mb_x == h->fdec->i_pir_end_col;
        }
        else
            a->b_force_intra = 0;
    }
}

/* Prediction modes allowed for various combinations of neighbors. */
/* Terminated by a -1. */
/* In order, no neighbors, left, top, top/left, top/left/topleft */
static const int8_t i16x16_mode_available[5][5] =
{
    {I_PRED_16x16_DC_128, -1, -1, -1, -1},
    {I_PRED_16x16_DC_LEFT, I_PRED_16x16_H, -1, -1, -1},
    {I_PRED_16x16_DC_TOP, I_PRED_16x16_V, -1, -1, -1},
    {I_PRED_16x16_V, I_PRED_16x16_H, I_PRED_16x16_DC, -1, -1},
    {I_PRED_16x16_V, I_PRED_16x16_H, I_PRED_16x16_DC, I_PRED_16x16_P, -1},
};

static const int8_t chroma_mode_available[5][5] =
{
    {I_PRED_CHROMA_DC_128, -1, -1, -1, -1},
    {I_PRED_CHROMA_DC_LEFT, I_PRED_CHROMA_H, -1, -1, -1},
    {I_PRED_CHROMA_DC_TOP, I_PRED_CHROMA_V, -1, -1, -1},
    {I_PRED_CHROMA_V, I_PRED_CHROMA_H, I_PRED_CHROMA_DC, -1, -1},
    {I_PRED_CHROMA_V, I_PRED_CHROMA_H, I_PRED_CHROMA_DC, I_PRED_CHROMA_P, -1},
};

static const int8_t i4x4_mode_available[2][5][10] =
{
    {
        {I_PRED_4x4_DC_128, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {I_PRED_4x4_DC_LEFT, I_PRED_4x4_H, I_PRED_4x4_HU, -1, -1, -1, -1, -1, -1, -1},
        {I_PRED_4x4_DC_TOP, I_PRED_4x4_V, I_PRED_4x4_DDL, I_PRED_4x4_VL, -1, -1, -1, -1, -1, -1},
        {I_PRED_4x4_DC, I_PRED_4x4_H, I_PRED_4x4_V, I_PRED_4x4_DDL, I_PRED_4x4_VL, I_PRED_4x4_HU, -1, -1, -1, -1},
        {I_PRED_4x4_DC, I_PRED_4x4_H, I_PRED_4x4_V, I_PRED_4x4_DDL, I_PRED_4x4_DDR, I_PRED_4x4_VR, I_PRED_4x4_HD, I_PRED_4x4_VL, I_PRED_4x4_HU, -1},
    },
    {
        {I_PRED_4x4_DC_128, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {I_PRED_4x4_DC_LEFT, I_PRED_4x4_H, I_PRED_4x4_HU, -1, -1, -1, -1, -1, -1, -1},
        {I_PRED_4x4_DC_TOP, I_PRED_4x4_V, -1, -1, -1, -1, -1, -1, -1, -1},
        {I_PRED_4x4_DC, I_PRED_4x4_H, I_PRED_4x4_V, I_PRED_4x4_HU, -1, -1, -1, -1, -1, -1},
        {I_PRED_4x4_DC, I_PRED_4x4_H, I_PRED_4x4_V, I_PRED_4x4_DDR, I_PRED_4x4_VR, I_PRED_4x4_HD, I_PRED_4x4_HU, -1},
    }
};

static ALWAYS_INLINE const int8_t *predict_16x16_mode_available( int i_neighbour )
{
    int idx = i_neighbour & (MB_TOP|MB_LEFT|MB_TOPLEFT);
    idx = (idx == (MB_TOP|MB_LEFT|MB_TOPLEFT)) ? 4 : idx & (MB_TOP|MB_LEFT);
    return i16x16_mode_available[idx];
}

static ALWAYS_INLINE const int8_t *predict_chroma_mode_available( int i_neighbour )
{
    int idx = i_neighbour & (MB_TOP|MB_LEFT|MB_TOPLEFT);
    idx = (idx == (MB_TOP|MB_LEFT|MB_TOPLEFT)) ? 4 : idx & (MB_TOP|MB_LEFT);
    return chroma_mode_available[idx];
}

static ALWAYS_INLINE const int8_t *predict_8x8_mode_available( int force_intra, int i_neighbour, int i )
{
    int avoid_topright = force_intra && (i&1);
    int idx = i_neighbour & (MB_TOP|MB_LEFT|MB_TOPLEFT);
    idx = (idx == (MB_TOP|MB_LEFT|MB_TOPLEFT)) ? 4 : idx & (MB_TOP|MB_LEFT);
    return i4x4_mode_available[avoid_topright][idx];
}

static ALWAYS_INLINE const int8_t *predict_4x4_mode_available( int force_intra, int i_neighbour, int i )
{
    int avoid_topright = force_intra && ((i&5) == 5);
    int idx = i_neighbour & (MB_TOP|MB_LEFT|MB_TOPLEFT);
    idx = (idx == (MB_TOP|MB_LEFT|MB_TOPLEFT)) ? 4 : idx & (MB_TOP|MB_LEFT);
    return i4x4_mode_available[avoid_topright][idx];
}

/* For trellis=2, we need to do this for both sizes of DCT, for trellis=1 we only need to use it on the chosen mode. */
static void inline x264_psy_trellis_init( x264_t *h, int do_both_dct )
{
    ALIGNED_16( static pixel zero[16*FDEC_STRIDE] ) = {0};

    if( do_both_dct || h->mb.b_transform_8x8 )
        h->dctf.sub16x16_dct8( h->mb.pic.fenc_dct8, h->mb.pic.p_fenc[0], zero );
    if( do_both_dct || !h->mb.b_transform_8x8 )
        h->dctf.sub16x16_dct( h->mb.pic.fenc_dct4, h->mb.pic.p_fenc[0], zero );
}

/* Reset fenc satd scores cache for psy RD */
static inline void x264_mb_init_fenc_cache( x264_t *h, int b_satd )
{
    if( h->param.analyse.i_trellis == 2 && h->mb.i_psy_trellis )
        x264_psy_trellis_init( h, h->param.analyse.b_transform_8x8 );
    if( !h->mb.i_psy_rd )
        return;
    /* Writes beyond the end of the array, but not a problem since fenc_satd_cache is right after. */
    h->mc.memzero_aligned( h->mb.pic.fenc_hadamard_cache, sizeof(h->mb.pic.fenc_hadamard_cache) );
    if( b_satd )
        h->mc.memzero_aligned( h->mb.pic.fenc_satd_cache, sizeof(h->mb.pic.fenc_satd_cache) );
}

static void x264_mb_analyse_intra_chroma( x264_t *h, x264_mb_analysis_t *a )
{
    if( a->i_satd_chroma < COST_MAX )
        return;

    if( CHROMA444 )
    {
        if( !h->mb.b_chroma_me )
        {
            a->i_satd_chroma = 0;
            return;
        }

        /* Cheap approximation of chroma costs to avoid a full i4x4/i8x8 analysis. */
        if( h->mb.b_lossless )
        {
            x264_predict_lossless_16x16( h, 1, a->i_predict16x16 );
            x264_predict_lossless_16x16( h, 2, a->i_predict16x16 );
        }
        else
        {
            h->predict_16x16[a->i_predict16x16]( h->mb.pic.p_fdec[1] );
            h->predict_16x16[a->i_predict16x16]( h->mb.pic.p_fdec[2] );
        }
        a->i_satd_chroma = h->pixf.mbcmp[PIXEL_16x16]( h->mb.pic.p_fdec[1], FDEC_STRIDE, h->mb.pic.p_fenc[1], FENC_STRIDE )
                         + h->pixf.mbcmp[PIXEL_16x16]( h->mb.pic.p_fdec[2], FDEC_STRIDE, h->mb.pic.p_fenc[2], FENC_STRIDE );
        return;
    }

    const int8_t *predict_mode = predict_chroma_mode_available( h->mb.i_neighbour_intra );
    int chromapix = h->luma2chroma_pixel[PIXEL_16x16];

    /* Prediction selection for chroma */
    if( predict_mode[3] >= 0 && !h->mb.b_lossless )
    {
        int satdu[4], satdv[4];
        h->pixf.intra_mbcmp_x3_chroma( h->mb.pic.p_fenc[1], h->mb.pic.p_fdec[1], satdu );
        h->pixf.intra_mbcmp_x3_chroma( h->mb.pic.p_fenc[2], h->mb.pic.p_fdec[2], satdv );
        h->predict_chroma[I_PRED_CHROMA_P]( h->mb.pic.p_fdec[1] );
        h->predict_chroma[I_PRED_CHROMA_P]( h->mb.pic.p_fdec[2] );
        satdu[I_PRED_CHROMA_P] = h->pixf.mbcmp[chromapix]( h->mb.pic.p_fdec[1], FDEC_STRIDE, h->mb.pic.p_fenc[1], FENC_STRIDE );
        satdv[I_PRED_CHROMA_P] = h->pixf.mbcmp[chromapix]( h->mb.pic.p_fdec[2], FDEC_STRIDE, h->mb.pic.p_fenc[2], FENC_STRIDE );

        for( ; *predict_mode >= 0; predict_mode++ )
        {
            int i_mode = *predict_mode;
            int i_satd = satdu[i_mode] + satdv[i_mode] + a->i_lambda * bs_size_ue( i_mode );

            a->i_satd_chroma_dir[i_mode] = i_satd;
            COPY2_IF_LT( a->i_satd_chroma, i_satd, a->i_predict8x8chroma, i_mode );
        }
    }
    else
    {
        for( ; *predict_mode >= 0; predict_mode++ )
        {
            int i_satd;
            int i_mode = *predict_mode;

            /* we do the prediction */
            if( h->mb.b_lossless )
                x264_predict_lossless_chroma( h, i_mode );
            else
            {
                h->predict_chroma[i_mode]( h->mb.pic.p_fdec[1] );
                h->predict_chroma[i_mode]( h->mb.pic.p_fdec[2] );
            }

            /* we calculate the cost */
            i_satd = h->pixf.mbcmp[chromapix]( h->mb.pic.p_fdec[1], FDEC_STRIDE, h->mb.pic.p_fenc[1], FENC_STRIDE ) +
                     h->pixf.mbcmp[chromapix]( h->mb.pic.p_fdec[2], FDEC_STRIDE, h->mb.pic.p_fenc[2], FENC_STRIDE ) +
                     a->i_lambda * bs_size_ue( x264_mb_chroma_pred_mode_fix[i_mode] );

            a->i_satd_chroma_dir[i_mode] = i_satd;
            COPY2_IF_LT( a->i_satd_chroma, i_satd, a->i_predict8x8chroma, i_mode );
        }
    }

    h->mb.i_chroma_pred_mode = a->i_predict8x8chroma;
}

/* FIXME: should we do any sort of merged chroma analysis with 4:4:4? */
static void x264_mb_analyse_intra( x264_t *h, x264_mb_analysis_t *a, int i_satd_inter )
{
    const unsigned int flags = h->sh.i_type == SLICE_TYPE_I ? h->param.analyse.intra : h->param.analyse.inter;
    pixel *p_src = h->mb.pic.p_fenc[0];
    pixel *p_dst = h->mb.pic.p_fdec[0];
    static const int8_t intra_analysis_shortcut[2][2][2][5] =
    {
        {{{I_PRED_4x4_HU, -1, -1, -1, -1},
          {I_PRED_4x4_DDL, I_PRED_4x4_VL, -1, -1, -1}},
         {{I_PRED_4x4_DDR, I_PRED_4x4_HD, I_PRED_4x4_HU, -1, -1},
          {I_PRED_4x4_DDL, I_PRED_4x4_DDR, I_PRED_4x4_VR, I_PRED_4x4_VL, -1}}},
        {{{I_PRED_4x4_HU, -1, -1, -1, -1},
          {-1, -1, -1, -1, -1}},
         {{I_PRED_4x4_DDR, I_PRED_4x4_HD, I_PRED_4x4_HU, -1, -1},
          {I_PRED_4x4_DDR, I_PRED_4x4_VR, -1, -1, -1}}},
    };

    int idx;
    int lambda = a->i_lambda;

    /*---------------- Try all mode and calculate their score ---------------*/

    /* 16x16 prediction selection */
    const int8_t *predict_mode = predict_16x16_mode_available( h->mb.i_neighbour_intra );

    /* Not heavily tuned */
    static const uint8_t i16x16_thresh_lut[11] = { 2, 2, 2, 3, 3, 4, 4, 4, 4, 4, 4 };
    int i16x16_thresh = a->b_fast_intra ? (i16x16_thresh_lut[h->mb.i_subpel_refine]*i_satd_inter)>>1 : COST_MAX;

    if( !h->mb.b_lossless && predict_mode[3] >= 0 )
    {
        h->pixf.intra_mbcmp_x3_16x16( p_src, p_dst, a->i_satd_i16x16_dir );
        a->i_satd_i16x16_dir[0] += lambda * bs_size_ue(0);
        a->i_satd_i16x16_dir[1] += lambda * bs_size_ue(1);
        a->i_satd_i16x16_dir[2] += lambda * bs_size_ue(2);
        COPY2_IF_LT( a->i_satd_i16x16, a->i_satd_i16x16_dir[0], a->i_predict16x16, 0 );
        COPY2_IF_LT( a->i_satd_i16x16, a->i_satd_i16x16_dir[1], a->i_predict16x16, 1 );
        COPY2_IF_LT( a->i_satd_i16x16, a->i_satd_i16x16_dir[2], a->i_predict16x16, 2 );

        /* Plane is expensive, so don't check it unless one of the previous modes was useful. */
        if( a->i_satd_i16x16 <= i16x16_thresh )
        {
            h->predict_16x16[I_PRED_16x16_P]( p_dst );
            a->i_satd_i16x16_dir[I_PRED_16x16_P] = h->pixf.mbcmp[PIXEL_16x16]( p_dst, FDEC_STRIDE, p_src, FENC_STRIDE );
            a->i_satd_i16x16_dir[I_PRED_16x16_P] += lambda * bs_size_ue(3);
            COPY2_IF_LT( a->i_satd_i16x16, a->i_satd_i16x16_dir[I_PRED_16x16_P], a->i_predict16x16, 3 );
        }
    }
    else
    {
        for( ; *predict_mode >= 0; predict_mode++ )
        {
            int i_satd;
            int i_mode = *predict_mode;

            if( h->mb.b_lossless )
                x264_predict_lossless_16x16( h, 0, i_mode );
            else
                h->predict_16x16[i_mode]( p_dst );

            i_satd = h->pixf.mbcmp[PIXEL_16x16]( p_dst, FDEC_STRIDE, p_src, FENC_STRIDE ) +
                     lambda * bs_size_ue( x264_mb_pred_mode16x16_fix[i_mode] );
            COPY2_IF_LT( a->i_satd_i16x16, i_satd, a->i_predict16x16, i_mode );
            a->i_satd_i16x16_dir[i_mode] = i_satd;
        }
    }

    if( h->sh.i_type == SLICE_TYPE_B )
        /* cavlc mb type prefix */
        a->i_satd_i16x16 += lambda * i_mb_b_cost_table[I_16x16];

    if( a->i_satd_i16x16 > i16x16_thresh )
        return;

    uint16_t *cost_i4x4_mode = (uint16_t*)ALIGN((intptr_t)x264_cost_i4x4_mode,64) + a->i_qp*32 + 8;
    /* 8x8 prediction selection */
    if( flags & X264_ANALYSE_I8x8 )
    {
        ALIGNED_ARRAY_32( pixel, edge,[36] );
        x264_pixel_cmp_t sa8d = (h->pixf.mbcmp[0] == h->pixf.satd[0]) ? h->pixf.sa8d[PIXEL_8x8] : h->pixf.mbcmp[PIXEL_8x8];
        int i_satd_thresh = a->i_mbrd ? COST_MAX : X264_MIN( i_satd_inter, a->i_satd_i16x16 );

        // FIXME some bias like in i4x4?
        int i_cost = lambda * 4; /* base predmode costs */
        h->mb.i_cbp_luma = 0;

        if( h->sh.i_type == SLICE_TYPE_B )
            i_cost += lambda * i_mb_b_cost_table[I_8x8];

        for( idx = 0;; idx++ )
        {
            int x = idx&1;
            int y = idx>>1;
            pixel *p_src_by = p_src + 8*x + 8*y*FENC_STRIDE;
            pixel *p_dst_by = p_dst + 8*x + 8*y*FDEC_STRIDE;
            int i_best = COST_MAX;
            int i_pred_mode = x264_mb_predict_intra4x4_mode( h, 4*idx );

            predict_mode = predict_8x8_mode_available( a->b_avoid_topright, h->mb.i_neighbour8[idx], idx );
            h->predict_8x8_filter( p_dst_by, edge, h->mb.i_neighbour8[idx], ALL_NEIGHBORS );

            if( h->pixf.intra_mbcmp_x9_8x8 && predict_mode[8] >= 0 )
            {
                /* No shortcuts here. The SSSE3 implementation of intra_mbcmp_x9 is fast enough. */
                i_best = h->pixf.intra_mbcmp_x9_8x8( p_src_by, p_dst_by, edge, cost_i4x4_mode-i_pred_mode, a->i_satd_i8x8_dir[idx] );
                i_cost += i_best & 0xffff;
                i_best >>= 16;
                a->i_predict8x8[idx] = i_best;
                if( idx == 3 || i_cost > i_satd_thresh )
                    break;
                x264_macroblock_cache_intra8x8_pred( h, 2*x, 2*y, i_best );
            }
            else
            {
                if( !h->mb.b_lossless && predict_mode[5] >= 0 )
                {
                    int satd[9];
                    h->pixf.intra_mbcmp_x3_8x8( p_src_by, edge, satd );
                    int favor_vertical = satd[I_PRED_4x4_H] > satd[I_PRED_4x4_V];
                    satd[i_pred_mode] -= 3 * lambda;
                    for( int i = 2; i >= 0; i-- )
                    {
                        int cost = satd[i];
                        a->i_satd_i8x8_dir[idx][i] = cost + 4 * lambda;
                        COPY2_IF_LT( i_best, cost, a->i_predict8x8[idx], i );
                    }

                    /* Take analysis shortcuts: don't analyse modes that are too
                     * far away direction-wise from the favored mode. */
                    if( a->i_mbrd < 1 + a->b_fast_intra )
                        predict_mode = intra_analysis_shortcut[a->b_avoid_topright][predict_mode[8] >= 0][favor_vertical];
                    else
                        predict_mode += 3;
                }

                for( ; *predict_mode >= 0 && (i_best >= 0 || a->i_mbrd >= 2); predict_mode++ )
                {
                    int i_satd;
                    int i_mode = *predict_mode;

                    if( h->mb.b_lossless )
                        x264_predict_lossless_8x8( h, p_dst_by, 0, idx, i_mode, edge );
                    else
                        h->predict_8x8[i_mode]( p_dst_by, edge );

                    i_satd = sa8d( p_dst_by, FDEC_STRIDE, p_src_by, FENC_STRIDE );
                    if( i_pred_mode == x264_mb_pred_mode4x4_fix(i_mode) )
                        i_satd -= 3 * lambda;

                    COPY2_IF_LT( i_best, i_satd, a->i_predict8x8[idx], i_mode );
                    a->i_satd_i8x8_dir[idx][i_mode] = i_satd + 4 * lambda;
                }
                i_cost += i_best + 3*lambda;

                if( idx == 3 || i_cost > i_satd_thresh )
                    break;
                if( h->mb.b_lossless )
                    x264_predict_lossless_8x8( h, p_dst_by, 0, idx, a->i_predict8x8[idx], edge );
                else
                    h->predict_8x8[a->i_predict8x8[idx]]( p_dst_by, edge );
                x264_macroblock_cache_intra8x8_pred( h, 2*x, 2*y, a->i_predict8x8[idx] );
            }
            /* we need to encode this block now (for next ones) */
            x264_mb_encode_i8x8( h, 0, idx, a->i_qp, a->i_predict8x8[idx], edge, 0 );
        }

        if( idx == 3 )
        {
            a->i_satd_i8x8 = i_cost;
            if( h->mb.i_skip_intra )
            {
                h->mc.copy[PIXEL_16x16]( h->mb.pic.i8x8_fdec_buf, 16, p_dst, FDEC_STRIDE, 16 );
                h->mb.pic.i8x8_nnz_buf[0] = M32( &h->mb.cache.non_zero_count[x264_scan8[ 0]] );
                h->mb.pic.i8x8_nnz_buf[1] = M32( &h->mb.cache.non_zero_count[x264_scan8[ 2]] );
                h->mb.pic.i8x8_nnz_buf[2] = M32( &h->mb.cache.non_zero_count[x264_scan8[ 8]] );
                h->mb.pic.i8x8_nnz_buf[3] = M32( &h->mb.cache.non_zero_count[x264_scan8[10]] );
                h->mb.pic.i8x8_cbp = h->mb.i_cbp_luma;
                if( h->mb.i_skip_intra == 2 )
                    h->mc.memcpy_aligned( h->mb.pic.i8x8_dct_buf, h->dct.luma8x8, sizeof(h->mb.pic.i8x8_dct_buf) );
            }
        }
        else
        {
            static const uint16_t cost_div_fix8[3] = {1024,512,341};
            a->i_satd_i8x8 = COST_MAX;
            i_cost = (i_cost * cost_div_fix8[idx]) >> 8;
        }
        /* Not heavily tuned */
        static const uint8_t i8x8_thresh[11] = { 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6 };
        if( a->b_early_terminate && X264_MIN(i_cost, a->i_satd_i16x16) > (i_satd_inter*i8x8_thresh[h->mb.i_subpel_refine])>>2 )
            return;
    }

    /* 4x4 prediction selection */
    if( flags & X264_ANALYSE_I4x4 )
    {
        int i_cost = lambda * (24+16); /* 24from JVT (SATD0), 16 from base predmode costs */
        int i_satd_thresh = a->b_early_terminate ? X264_MIN3( i_satd_inter, a->i_satd_i16x16, a->i_satd_i8x8 ) : COST_MAX;
        h->mb.i_cbp_luma = 0;

        if( a->b_early_terminate && a->i_mbrd )
            i_satd_thresh = i_satd_thresh * (10-a->b_fast_intra)/8;

        if( h->sh.i_type == SLICE_TYPE_B )
            i_cost += lambda * i_mb_b_cost_table[I_4x4];

        for( idx = 0;; idx++ )
        {
            pixel *p_src_by = p_src + block_idx_xy_fenc[idx];
            pixel *p_dst_by = p_dst + block_idx_xy_fdec[idx];
            int i_best = COST_MAX;
            int i_pred_mode = x264_mb_predict_intra4x4_mode( h, idx );

            predict_mode = predict_4x4_mode_available( a->b_avoid_topright, h->mb.i_neighbour4[idx], idx );

            if( (h->mb.i_neighbour4[idx] & (MB_TOPRIGHT|MB_TOP)) == MB_TOP )
                /* emulate missing topright samples */
                MPIXEL_X4( &p_dst_by[4 - FDEC_STRIDE] ) = PIXEL_SPLAT_X4( p_dst_by[3 - FDEC_STRIDE] );

            if( h->pixf.intra_mbcmp_x9_4x4 && predict_mode[8] >= 0 )
            {
                /* No shortcuts here. The SSSE3 implementation of intra_mbcmp_x9 is fast enough. */
                i_best = h->pixf.intra_mbcmp_x9_4x4( p_src_by, p_dst_by, cost_i4x4_mode-i_pred_mode );
                i_cost += i_best & 0xffff;
                i_best >>= 16;
                a->i_predict4x4[idx] = i_best;
                if( i_cost > i_satd_thresh || idx == 15 )
                    break;
                h->mb.cache.intra4x4_pred_mode[x264_scan8[idx]] = i_best;
            }
            else
            {
                if( !h->mb.b_lossless && predict_mode[5] >= 0 )
                {
                    int satd[9];
                    h->pixf.intra_mbcmp_x3_4x4( p_src_by, p_dst_by, satd );
                    int favor_vertical = satd[I_PRED_4x4_H] > satd[I_PRED_4x4_V];
                    satd[i_pred_mode] -= 3 * lambda;
                    i_best = satd[I_PRED_4x4_DC]; a->i_predict4x4[idx] = I_PRED_4x4_DC;
                    COPY2_IF_LT( i_best, satd[I_PRED_4x4_H], a->i_predict4x4[idx], I_PRED_4x4_H );
                    COPY2_IF_LT( i_best, satd[I_PRED_4x4_V], a->i_predict4x4[idx], I_PRED_4x4_V );

                    /* Take analysis shortcuts: don't analyse modes that are too
                     * far away direction-wise from the favored mode. */
                    if( a->i_mbrd < 1 + a->b_fast_intra )
                        predict_mode = intra_analysis_shortcut[a->b_avoid_topright][predict_mode[8] >= 0][favor_vertical];
                    else
                        predict_mode += 3;
                }

                if( i_best > 0 )
                {
                    for( ; *predict_mode >= 0; predict_mode++ )
                    {
                        int i_satd;
                        int i_mode = *predict_mode;

                        if( h->mb.b_lossless )
                            x264_predict_lossless_4x4( h, p_dst_by, 0, idx, i_mode );
                        else
                            h->predict_4x4[i_mode]( p_dst_by );

                        i_satd = h->pixf.mbcmp[PIXEL_4x4]( p_dst_by, FDEC_STRIDE, p_src_by, FENC_STRIDE );
                        if( i_pred_mode == x264_mb_pred_mode4x4_fix(i_mode) )
                        {
                            i_satd -= lambda * 3;
                            if( i_satd <= 0 )
                            {
                                i_best = i_satd;
                                a->i_predict4x4[idx] = i_mode;
                                break;
                            }
                        }

                        COPY2_IF_LT( i_best, i_satd, a->i_predict4x4[idx], i_mode );
                    }
                }

                i_cost += i_best + 3 * lambda;
                if( i_cost > i_satd_thresh || idx == 15 )
                    break;
                if( h->mb.b_lossless )
                    x264_predict_lossless_4x4( h, p_dst_by, 0, idx, a->i_predict4x4[idx] );
                else
                    h->predict_4x4[a->i_predict4x4[idx]]( p_dst_by );
                h->mb.cache.intra4x4_pred_mode[x264_scan8[idx]] = a->i_predict4x4[idx];
            }
            /* we need to encode this block now (for next ones) */
            x264_mb_encode_i4x4( h, 0, idx, a->i_qp, a->i_predict4x4[idx], 0 );
        }
        if( idx == 15 )
        {
            a->i_satd_i4x4 = i_cost;
            if( h->mb.i_skip_intra )
            {
                h->mc.copy[PIXEL_16x16]( h->mb.pic.i4x4_fdec_buf, 16, p_dst, FDEC_STRIDE, 16 );
                h->mb.pic.i4x4_nnz_buf[0] = M32( &h->mb.cache.non_zero_count[x264_scan8[ 0]] );
                h->mb.pic.i4x4_nnz_buf[1] = M32( &h->mb.cache.non_zero_count[x264_scan8[ 2]] );
                h->mb.pic.i4x4_nnz_buf[2] = M32( &h->mb.cache.non_zero_count[x264_scan8[ 8]] );
                h->mb.pic.i4x4_nnz_buf[3] = M32( &h->mb.cache.non_zero_count[x264_scan8[10]] );
                h->mb.pic.i4x4_cbp = h->mb.i_cbp_luma;
                if( h->mb.i_skip_intra == 2 )
                    h->mc.memcpy_aligned( h->mb.pic.i4x4_dct_buf, h->dct.luma4x4, sizeof(h->mb.pic.i4x4_dct_buf) );
            }
        }
        else
            a->i_satd_i4x4 = COST_MAX;
    }
}

static void x264_intra_rd( x264_t *h, x264_mb_analysis_t *a, int i_satd_thresh )
{
    if( !a->b_early_terminate )
        i_satd_thresh = COST_MAX;

    if( a->i_satd_i16x16 < i_satd_thresh )
    {
        h->mb.i_type = I_16x16;
        x264_analyse_update_cache( h, a );
        a->i_satd_i16x16 = x264_rd_cost_mb( h, a->i_lambda2 );
    }
    else
        a->i_satd_i16x16 = COST_MAX;

    if( a->i_satd_i4x4 < i_satd_thresh )
    {
        h->mb.i_type = I_4x4;
        x264_analyse_update_cache( h, a );
        a->i_satd_i4x4 = x264_rd_cost_mb( h, a->i_lambda2 );
    }
    else
        a->i_satd_i4x4 = COST_MAX;

    if( a->i_satd_i8x8 < i_satd_thresh )
    {
        h->mb.i_type = I_8x8;
        x264_analyse_update_cache( h, a );
        a->i_satd_i8x8 = x264_rd_cost_mb( h, a->i_lambda2 );
        a->i_cbp_i8x8_luma = h->mb.i_cbp_luma;
    }
    else
        a->i_satd_i8x8 = COST_MAX;
}

static void x264_intra_rd_refine( x264_t *h, x264_mb_analysis_t *a )
{
    uint64_t i_satd, i_best;
    int plane_count = CHROMA444 ? 3 : 1;
    h->mb.i_skip_intra = 0;

    if( h->mb.i_type == I_16x16 )
    {
        int old_pred_mode = a->i_predict16x16;
        const int8_t *predict_mode = predict_16x16_mode_available( h->mb.i_neighbour_intra );
        int i_thresh = a->b_early_terminate ? a->i_satd_i16x16_dir[old_pred_mode] * 9/8 : COST_MAX;
        i_best = a->i_satd_i16x16;
        for( ; *predict_mode >= 0; predict_mode++ )
        {
            int i_mode = *predict_mode;
            if( i_mode == old_pred_mode || a->i_satd_i16x16_dir[i_mode] > i_thresh )
                continue;
            h->mb.i_intra16x16_pred_mode = i_mode;
            i_satd = x264_rd_cost_mb( h, a->i_lambda2 );
            COPY2_IF_LT( i_best, i_satd, a->i_predict16x16, i_mode );
        }
    }

    /* RD selection for chroma prediction */
    if( !CHROMA444 )
    {
        const int8_t *predict_mode = predict_chroma_mode_available( h->mb.i_neighbour_intra );
        if( predict_mode[1] >= 0 )
        {
            int8_t predict_mode_sorted[4];
            int i_max;
            int i_thresh = a->b_early_terminate ? a->i_satd_chroma * 5/4 : COST_MAX;

            for( i_max = 0; *predict_mode >= 0; predict_mode++ )
            {
                int i_mode = *predict_mode;
                if( a->i_satd_chroma_dir[i_mode] < i_thresh && i_mode != a->i_predict8x8chroma )
                    predict_mode_sorted[i_max++] = i_mode;
            }

            if( i_max > 0 )
            {
                int i_cbp_chroma_best = h->mb.i_cbp_chroma;
                int i_chroma_lambda = x264_lambda2_tab[h->mb.i_chroma_qp];
                /* the previous thing encoded was x264_intra_rd(), so the pixels and
                 * coefs for the current chroma mode are still around, so we only
                 * have to recount the bits. */
                i_best = x264_rd_cost_chroma( h, i_chroma_lambda, a->i_predict8x8chroma, 0 );
                for( int i = 0; i < i_max; i++ )
                {
                    int i_mode = predict_mode_sorted[i];
                    if( h->mb.b_lossless )
                        x264_predict_lossless_chroma( h, i_mode );
                    else
                    {
                        h->predict_chroma[i_mode]( h->mb.pic.p_fdec[1] );
                        h->predict_chroma[i_mode]( h->mb.pic.p_fdec[2] );
                    }
                    /* if we've already found a mode that needs no residual, then
                     * probably any mode with a residual will be worse.
                     * so avoid dct on the remaining modes to improve speed. */
                    i_satd = x264_rd_cost_chroma( h, i_chroma_lambda, i_mode, h->mb.i_cbp_chroma != 0x00 );
                    COPY3_IF_LT( i_best, i_satd, a->i_predict8x8chroma, i_mode, i_cbp_chroma_best, h->mb.i_cbp_chroma );
                }
                h->mb.i_chroma_pred_mode = a->i_predict8x8chroma;
                h->mb.i_cbp_chroma = i_cbp_chroma_best;
            }
        }
    }

    if( h->mb.i_type == I_4x4 )
    {
        pixel4 pels[3][4] = {{0}}; // doesn't need initting, just shuts up a gcc warning
        int nnz[3] = {0};
        for( int idx = 0; idx < 16; idx++ )
        {
            pixel *dst[3] = {h->mb.pic.p_fdec[0] + block_idx_xy_fdec[idx],
                             h->mb.pic.p_fdec[1] + block_idx_xy_fdec[idx],
                             h->mb.pic.p_fdec[2] + block_idx_xy_fdec[idx]};
            i_best = COST_MAX64;

            const int8_t *predict_mode = predict_4x4_mode_available( a->b_avoid_topright, h->mb.i_neighbour4[idx], idx );

            if( (h->mb.i_neighbour4[idx] & (MB_TOPRIGHT|MB_TOP)) == MB_TOP )
                for( int p = 0; p < plane_count; p++ )
                    /* emulate missing topright samples */
                    MPIXEL_X4( dst[p]+4-FDEC_STRIDE ) = PIXEL_SPLAT_X4( dst[p][3-FDEC_STRIDE] );

            for( ; *predict_mode >= 0; predict_mode++ )
            {
                int i_mode = *predict_mode;
                i_satd = x264_rd_cost_i4x4( h, a->i_lambda2, idx, i_mode );

                if( i_best > i_satd )
                {
                    a->i_predict4x4[idx] = i_mode;
                    i_best = i_satd;
                    for( int p = 0; p < plane_count; p++ )
                    {
                        pels[p][0] = MPIXEL_X4( dst[p]+0*FDEC_STRIDE );
                        pels[p][1] = MPIXEL_X4( dst[p]+1*FDEC_STRIDE );
                        pels[p][2] = MPIXEL_X4( dst[p]+2*FDEC_STRIDE );
                        pels[p][3] = MPIXEL_X4( dst[p]+3*FDEC_STRIDE );
                        nnz[p] = h->mb.cache.non_zero_count[x264_scan8[idx+p*16]];
                    }
                }
            }

            for( int p = 0; p < plane_count; p++ )
            {
                MPIXEL_X4( dst[p]+0*FDEC_STRIDE ) = pels[p][0];
                MPIXEL_X4( dst[p]+1*FDEC_STRIDE ) = pels[p][1];
                MPIXEL_X4( dst[p]+2*FDEC_STRIDE ) = pels[p][2];
                MPIXEL_X4( dst[p]+3*FDEC_STRIDE ) = pels[p][3];
                h->mb.cache.non_zero_count[x264_scan8[idx+p*16]] = nnz[p];
            }

            h->mb.cache.intra4x4_pred_mode[x264_scan8[idx]] = a->i_predict4x4[idx];
        }
    }
    else if( h->mb.i_type == I_8x8 )
    {
        ALIGNED_ARRAY_32( pixel, edge,[4],[32] ); // really [3][36], but they can overlap
        pixel4 pels_h[3][2] = {{0}};
        pixel pels_v[3][7] = {{0}};
        uint16_t nnz[3][2] = {{0}}; //shut up gcc
        for( int idx = 0; idx < 4; idx++ )
        {
            int x = idx&1;
            int y = idx>>1;
            int s8 = X264_SCAN8_0 + 2*x + 16*y;
            pixel *dst[3] = {h->mb.pic.p_fdec[0] + 8*x + 8*y*FDEC_STRIDE,
                             h->mb.pic.p_fdec[1] + 8*x + 8*y*FDEC_STRIDE,
                             h->mb.pic.p_fdec[2] + 8*x + 8*y*FDEC_STRIDE};
            int cbp_luma_new = 0;
            int i_thresh = a->b_early_terminate ? a->i_satd_i8x8_dir[idx][a->i_predict8x8[idx]] * 11/8 : COST_MAX;

            i_best = COST_MAX64;

            const int8_t *predict_mode = predict_8x8_mode_available( a->b_avoid_topright, h->mb.i_neighbour8[idx], idx );
            for( int p = 0; p < plane_count; p++ )
                h->predict_8x8_filter( dst[p], edge[p], h->mb.i_neighbour8[idx], ALL_NEIGHBORS );

            for( ; *predict_mode >= 0; predict_mode++ )
            {
                int i_mode = *predict_mode;
                if( a->i_satd_i8x8_dir[idx][i_mode] > i_thresh )
                    continue;

                h->mb.i_cbp_luma = a->i_cbp_i8x8_luma;
                i_satd = x264_rd_cost_i8x8( h, a->i_lambda2, idx, i_mode, edge );

                if( i_best > i_satd )
                {
                    a->i_predict8x8[idx] = i_mode;
                    cbp_luma_new = h->mb.i_cbp_luma;
                    i_best = i_satd;

                    for( int p = 0; p < plane_count; p++ )
                    {
                        pels_h[p][0] = MPIXEL_X4( dst[p]+7*FDEC_STRIDE+0 );
                        pels_h[p][1] = MPIXEL_X4( dst[p]+7*FDEC_STRIDE+4 );
                        if( !(idx&1) )
                            for( int j = 0; j < 7; j++ )
                                pels_v[p][j] = dst[p][7+j*FDEC_STRIDE];
                        nnz[p][0] = M16( &h->mb.cache.non_zero_count[s8 + 0*8 + p*16] );
                        nnz[p][1] = M16( &h->mb.cache.non_zero_count[s8 + 1*8 + p*16] );
                    }
                }
            }
            a->i_cbp_i8x8_luma = cbp_luma_new;
            for( int p = 0; p < plane_count; p++ )
            {
                MPIXEL_X4( dst[p]+7*FDEC_STRIDE+0 ) = pels_h[p][0];
                MPIXEL_X4( dst[p]+7*FDEC_STRIDE+4 ) = pels_h[p][1];
                if( !(idx&1) )
                    for( int j = 0; j < 7; j++ )
                        dst[p][7+j*FDEC_STRIDE] = pels_v[p][j];
                M16( &h->mb.cache.non_zero_count[s8 + 0*8 + p*16] ) = nnz[p][0];
                M16( &h->mb.cache.non_zero_count[s8 + 1*8 + p*16] ) = nnz[p][1];
            }

            x264_macroblock_cache_intra8x8_pred( h, 2*x, 2*y, a->i_predict8x8[idx] );
        }
    }
}

#define LOAD_FENC(m, src, xoff, yoff) \
{ \
    (m)->p_cost_mv = a->p_cost_mv; \
    (m)->i_stride[0] = h->mb.pic.i_stride[0]; \
    (m)->i_stride[1] = h->mb.pic.i_stride[1]; \
    (m)->i_stride[2] = h->mb.pic.i_stride[2]; \
    (m)->p_fenc[0] = &(src)[0][(xoff)+(yoff)*FENC_STRIDE]; \
    (m)->p_fenc[1] = &(src)[1][((xoff)>>CHROMA_H_SHIFT)+((yoff)>>CHROMA_V_SHIFT)*FENC_STRIDE]; \
    (m)->p_fenc[2] = &(src)[2][((xoff)>>CHROMA_H_SHIFT)+((yoff)>>CHROMA_V_SHIFT)*FENC_STRIDE]; \
}

#define LOAD_HPELS(m, src, list, ref, xoff, yoff) \
{ \
    (m)->p_fref_w = (m)->p_fref[0] = &(src)[0][(xoff)+(yoff)*(m)->i_stride[0]]; \
    (m)->p_fref[1] = &(src)[1][(xoff)+(yoff)*(m)->i_stride[0]]; \
    (m)->p_fref[2] = &(src)[2][(xoff)+(yoff)*(m)->i_stride[0]]; \
    (m)->p_fref[3] = &(src)[3][(xoff)+(yoff)*(m)->i_stride[0]]; \
    if( CHROMA444 ) \
    { \
        (m)->p_fref[ 4] = &(src)[ 4][(xoff)+(yoff)*(m)->i_stride[1]]; \
        (m)->p_fref[ 5] = &(src)[ 5][(xoff)+(yoff)*(m)->i_stride[1]]; \
        (m)->p_fref[ 6] = &(src)[ 6][(xoff)+(yoff)*(m)->i_stride[1]]; \
        (m)->p_fref[ 7] = &(src)[ 7][(xoff)+(yoff)*(m)->i_stride[1]]; \
        (m)->p_fref[ 8] = &(src)[ 8][(xoff)+(yoff)*(m)->i_stride[2]]; \
        (m)->p_fref[ 9] = &(src)[ 9][(xoff)+(yoff)*(m)->i_stride[2]]; \
        (m)->p_fref[10] = &(src)[10][(xoff)+(yoff)*(m)->i_stride[2]]; \
        (m)->p_fref[11] = &(src)[11][(xoff)+(yoff)*(m)->i_stride[2]]; \
    } \
    else \
        (m)->p_fref[4] = &(src)[4][(xoff)+((yoff)>>CHROMA_V_SHIFT)*(m)->i_stride[1]]; \
    (m)->integral = &h->mb.pic.p_integral[list][ref][(xoff)+(yoff)*(m)->i_stride[0]]; \
    (m)->weight = x264_weight_none; \
    (m)->i_ref = ref; \
}

#define LOAD_WPELS(m, src, list, ref, xoff, yoff) \
    (m)->p_fref_w = &(src)[(xoff)+(yoff)*(m)->i_stride[0]]; \
    (m)->weight = h->sh.weight[i_ref];

#define REF_COST(list, ref) \
    (a->p_cost_ref[list][ref])

static void x264_mb_analyse_inter_p16x16( x264_t *h, x264_mb_analysis_t *a )
{
    x264_me_t m;
    int i_mvc;
    ALIGNED_4( int16_t mvc[8][2] );
    int i_halfpel_thresh = INT_MAX;
    int *p_halfpel_thresh = (a->b_early_terminate && h->mb.pic.i_fref[0]>1) ? &i_halfpel_thresh : NULL;

    /* 16x16 Search on all ref frame */
    m.i_pixel = PIXEL_16x16;
    LOAD_FENC( &m, h->mb.pic.p_fenc, 0, 0 );

    a->l0.me16x16.cost = INT_MAX;
    for( int i_ref = 0; i_ref < h->mb.pic.i_fref[0]; i_ref++ )
    {
        m.i_ref_cost = REF_COST( 0, i_ref );
        i_halfpel_thresh -= m.i_ref_cost;

        /* search with ref */
        LOAD_HPELS( &m, h->mb.pic.p_fref[0][i_ref], 0, i_ref, 0, 0 );
        LOAD_WPELS( &m, h->mb.pic.p_fref_w[i_ref], 0, i_ref, 0, 0 );

        x264_mb_predict_mv_16x16( h, 0, i_ref, m.mvp );

        if( h->mb.ref_blind_dupe == i_ref )
        {
            CP32( m.mv, a->l0.mvc[0][0] );
            x264_me_refine_qpel_refdupe( h, &m, p_halfpel_thresh );
        }
        else
        {
            x264_mb_predict_mv_ref16x16( h, 0, i_ref, mvc, &i_mvc );
            x264_me_search_ref( h, &m, mvc, i_mvc, p_halfpel_thresh );
        }

        /* save mv for predicting neighbors */
        CP32( h->mb.mvr[0][i_ref][h->mb.i_mb_xy], m.mv );
        CP32( a->l0.mvc[i_ref][0], m.mv );

        /* early termination
         * SSD threshold would probably be better than SATD */
        if( i_ref == 0
            && a->b_try_skip
            && m.cost-m.cost_mv < 300*a->i_lambda
            &&  abs(m.mv[0]-h->mb.cache.pskip_mv[0])
              + abs(m.mv[1]-h->mb.cache.pskip_mv[1]) <= 1
            && x264_macroblock_probe_pskip( h ) )
        {
            h->mb.i_type = P_SKIP;
            x264_analyse_update_cache( h, a );
            assert( h->mb.cache.pskip_mv[1] <= h->mb.mv_max_spel[1] || h->i_thread_frames == 1 );
            return;
        }

        m.cost += m.i_ref_cost;
        i_halfpel_thresh += m.i_ref_cost;

        if( m.cost < a->l0.me16x16.cost )
            h->mc.memcpy_aligned( &a->l0.me16x16, &m, sizeof(x264_me_t) );
    }

    x264_macroblock_cache_ref( h, 0, 0, 4, 4, 0, a->l0.me16x16.i_ref );
    assert( a->l0.me16x16.mv[1] <= h->mb.mv_max_spel[1] || h->i_thread_frames == 1 );

    h->mb.i_type = P_L0;
    if( a->i_mbrd )
    {
        x264_mb_init_fenc_cache( h, a->i_mbrd >= 2 || h->param.analyse.inter & X264_ANALYSE_PSUB8x8 );
        if( a->l0.me16x16.i_ref == 0 && M32( a->l0.me16x16.mv ) == M32( h->mb.cache.pskip_mv ) && !a->b_force_intra )
        {
            h->mb.i_partition = D_16x16;
            x264_macroblock_cache_mv_ptr( h, 0, 0, 4, 4, 0, a->l0.me16x16.mv );
            a->l0.i_rd16x16 = x264_rd_cost_mb( h, a->i_lambda2 );
            if( !(h->mb.i_cbp_luma|h->mb.i_cbp_chroma) )
                h->mb.i_type = P_SKIP;
        }
    }
}

static void x264_mb_analyse_inter_p8x8_mixed_ref( x264_t *h, x264_mb_analysis_t *a )
{
    x264_me_t m;
    pixel **p_fenc = h->mb.pic.p_fenc;
    int i_maxref = h->mb.pic.i_fref[0]-1;

    h->mb.i_partition = D_8x8;

    #define CHECK_NEIGHBOUR(i)\
    {\
        int ref = h->mb.cache.ref[0][X264_SCAN8_0+i];\
        if( ref > i_maxref && ref != h->mb.ref_blind_dupe )\
            i_maxref = ref;\
    }

    /* early termination: if 16x16 chose ref 0, then evalute no refs older
     * than those used by the neighbors */
    if( a->b_early_terminate && (i_maxref > 0 && (a->l0.me16x16.i_ref == 0 || a->l0.me16x16.i_ref == h->mb.ref_blind_dupe) &&
        h->mb.i_mb_type_top > 0 && h->mb.i_mb_type_left[0] > 0) )
    {
        i_maxref = 0;
        CHECK_NEIGHBOUR(  -8 - 1 );
        CHECK_NEIGHBOUR(  -8 + 0 );
        CHECK_NEIGHBOUR(  -8 + 2 );
        CHECK_NEIGHBOUR(  -8 + 4 );
        CHECK_NEIGHBOUR(   0 - 1 );
        CHECK_NEIGHBOUR( 2*8 - 1 );
    }
    #undef CHECK_NEIGHBOUR

    for( int i_ref = 0; i_ref <= i_maxref; i_ref++ )
        CP32( a->l0.mvc[i_ref][0], h->mb.mvr[0][i_ref][h->mb.i_mb_xy] );

    for( int i = 0; i < 4; i++ )
    {
        x264_me_t *l0m = &a->l0.me8x8[i];
        int x8 = i&1;
        int y8 = i>>1;

        m.i_pixel = PIXEL_8x8;

        LOAD_FENC( &m, p_fenc, 8*x8, 8*y8 );
        l0m->cost = INT_MAX;
        for( int i_ref = 0; i_ref <= i_maxref || i_ref == h->mb.ref_blind_dupe; )
        {
            m.i_ref_cost = REF_COST( 0, i_ref );

            LOAD_HPELS( &m, h->mb.pic.p_fref[0][i_ref], 0, i_ref, 8*x8, 8*y8 );
            LOAD_WPELS( &m, h->mb.pic.p_fref_w[i_ref], 0, i_ref, 8*x8, 8*y8 );

            x264_macroblock_cache_ref( h, 2*x8, 2*y8, 2, 2, 0, i_ref );
            x264_mb_predict_mv( h, 0, 4*i, 2, m.mvp );
            if( h->mb.ref_blind_dupe == i_ref )
            {
                CP32( m.mv, a->l0.mvc[0][i+1] );
                x264_me_refine_qpel_refdupe( h, &m, NULL );
            }
            else
                x264_me_search( h, &m, a->l0.mvc[i_ref], i+1 );

            m.cost += m.i_ref_cost;

            CP32( a->l0.mvc[i_ref][i+1], m.mv );

            if( m.cost < l0m->cost )
                h->mc.memcpy_aligned( l0m, &m, sizeof(x264_me_t) );
            if( i_ref == i_maxref && i_maxref < h->mb.ref_blind_dupe )
                i_ref = h->mb.ref_blind_dupe;
            else
                i_ref++;
        }
        x264_macroblock_cache_mv_ptr( h, 2*x8, 2*y8, 2, 2, 0, l0m->mv );
        x264_macroblock_cache_ref( h, 2*x8, 2*y8, 2, 2, 0, l0m->i_ref );

        a->i_satd8x8[0][i] = l0m->cost - ( l0m->cost_mv + l0m->i_ref_cost );

        /* If CABAC is on and we're not doing sub-8x8 analysis, the costs
           are effectively zero. */
        if( !h->param.b_cabac || (h->param.analyse.inter & X264_ANALYSE_PSUB8x8) )
            l0m->cost += a->i_lambda * i_sub_mb_p_cost_table[D_L0_8x8];
    }

    a->l0.i_cost8x8 = a->l0.me8x8[0].cost + a->l0.me8x8[1].cost +
                      a->l0.me8x8[2].cost + a->l0.me8x8[3].cost;
    /* P_8x8 ref0 has no ref cost */
    if( !h->param.b_cabac && !(a->l0.me8x8[0].i_ref | a->l0.me8x8[1].i_ref |
                               a->l0.me8x8[2].i_ref | a->l0.me8x8[3].i_ref) )
        a->l0.i_cost8x8 -= REF_COST( 0, 0 ) * 4;
    h->mb.i_sub_partition[0] = h->mb.i_sub_partition[1] =
    h->mb.i_sub_partition[2] = h->mb.i_sub_partition[3] = D_L0_8x8;
}

static void x264_mb_analyse_inter_p8x8( x264_t *h, x264_mb_analysis_t *a )
{
    /* Duplicate refs are rarely useful in p8x8 due to the high cost of the
     * reference frame flags.  Thus, if we're not doing mixedrefs, just
     * don't bother analysing the dupes. */
    const int i_ref = h->mb.ref_blind_dupe == a->l0.me16x16.i_ref ? 0 : a->l0.me16x16.i_ref;
    const int i_ref_cost = h->param.b_cabac || i_ref ? REF_COST( 0, i_ref ) : 0;
    pixel **p_fenc = h->mb.pic.p_fenc;
    int i_mvc;
    int16_t (*mvc)[2] = a->l0.mvc[i_ref];

    /* XXX Needed for x264_mb_predict_mv */
    h->mb.i_partition = D_8x8;

    i_mvc = 1;
    CP32( mvc[0], a->l0.me16x16.mv );

    for( int i = 0; i < 4; i++ )
    {
        x264_me_t *m = &a->l0.me8x8[i];
        int x8 = i&1;
        int y8 = i>>1;

        m->i_pixel = PIXEL_8x8;
        m->i_ref_cost = i_ref_cost;

        LOAD_FENC( m, p_fenc, 8*x8, 8*y8 );
        LOAD_HPELS( m, h->mb.pic.p_fref[0][i_ref], 0, i_ref, 8*x8, 8*y8 );
        LOAD_WPELS( m, h->mb.pic.p_fref_w[i_ref], 0, i_ref, 8*x8, 8*y8 );

        x264_mb_predict_mv( h, 0, 4*i, 2, m->mvp );
        x264_me_search( h, m, mvc, i_mvc );

        x264_macroblock_cache_mv_ptr( h, 2*x8, 2*y8, 2, 2, 0, m->mv );

        CP32( mvc[i_mvc], m->mv );
        i_mvc++;

        a->i_satd8x8[0][i] = m->cost - m->cost_mv;

        /* mb type cost */
        m->cost += i_ref_cost;
        if( !h->param.b_cabac || (h->param.analyse.inter & X264_ANALYSE_PSUB8x8) )
            m->cost += a->i_lambda * i_sub_mb_p_cost_table[D_L0_8x8];
    }

    a->l0.i_cost8x8 = a->l0.me8x8[0].cost + a->l0.me8x8[1].cost +
                      a->l0.me8x8[2].cost + a->l0.me8x8[3].cost;
    /* theoretically this should include 4*ref_cost,
     * but 3 seems a better approximation of cabac. */
    if( h->param.b_cabac )
        a->l0.i_cost8x8 -= i_ref_cost;
    h->mb.i_sub_partition[0] = h->mb.i_sub_partition[1] =
    h->mb.i_sub_partition[2] = h->mb.i_sub_partition[3] = D_L0_8x8;
}

static void x264_mb_analyse_inter_p16x8( x264_t *h, x264_mb_analysis_t *a, int i_best_satd )
{
    x264_me_t m;
    pixel **p_fenc = h->mb.pic.p_fenc;
    ALIGNED_4( int16_t mvc[3][2] );

    /* XXX Needed for x264_mb_predict_mv */
    h->mb.i_partition = D_16x8;

    for( int i = 0; i < 2; i++ )
    {
        x264_me_t *l0m = &a->l0.me16x8[i];
        const int minref = X264_MIN( a->l0.me8x8[2*i].i_ref, a->l0.me8x8[2*i+1].i_ref );
        const int maxref = X264_MAX( a->l0.me8x8[2*i].i_ref, a->l0.me8x8[2*i+1].i_ref );
        const int ref8[2] = { minref, maxref };
        const int i_ref8s = ( ref8[0] == ref8[1] ) ? 1 : 2;

        m.i_pixel = PIXEL_16x8;

        LOAD_FENC( &m, p_fenc, 0, 8*i );
        l0m->cost = INT_MAX;
        for( int j = 0; j < i_ref8s; j++ )
        {
            const int i_ref = ref8[j];
            m.i_ref_cost = REF_COST( 0, i_ref );

            /* if we skipped the 16x16 predictor, we wouldn't have to copy anything... */
            CP32( mvc[0], a->l0.mvc[i_ref][0] );
            CP32( mvc[1], a->l0.mvc[i_ref][2*i+1] );
            CP32( mvc[2], a->l0.mvc[i_ref][2*i+2] );

            LOAD_HPELS( &m, h->mb.pic.p_fref[0][i_ref], 0, i_ref, 0, 8*i );
            LOAD_WPELS( &m, h->mb.pic.p_fref_w[i_ref], 0, i_ref, 0, 8*i );

            x264_macroblock_cache_ref( h, 0, 2*i, 4, 2, 0, i_ref );
            x264_mb_predict_mv( h, 0, 8*i, 4, m.mvp );
            /* We can only take this shortcut if the first search was performed on ref0. */
            if( h->mb.ref_blind_dupe == i_ref && !ref8[0] )
            {
                /* We can just leave the MV from the previous ref search. */
                x264_me_refine_qpel_refdupe( h, &m, NULL );
            }
            else
                x264_me_search( h, &m, mvc, 3 );

            m.cost += m.i_ref_cost;

            if( m.cost < l0m->cost )
                h->mc.memcpy_aligned( l0m, &m, sizeof(x264_me_t) );
        }

        /* Early termination based on the current SATD score of partition[0]
           plus the estimated SATD score of partition[1] */
        if( a->b_early_terminate && (!i && l0m->cost + a->i_cost_est16x8[1] > i_best_satd * (4 + !!a->i_mbrd) / 4) )
        {
            a->l0.i_cost16x8 = COST_MAX;
            return;
        }

        x264_macroblock_cache_mv_ptr( h, 0, 2*i, 4, 2, 0, l0m->mv );
        x264_macroblock_cache_ref( h, 0, 2*i, 4, 2, 0, l0m->i_ref );
    }

    a->l0.i_cost16x8 = a->l0.me16x8[0].cost + a->l0.me16x8[1].cost;
}

static void x264_mb_analyse_inter_p8x16( x264_t *h, x264_mb_analysis_t *a, int i_best_satd )
{
    x264_me_t m;
    pixel **p_fenc = h->mb.pic.p_fenc;
    ALIGNED_4( int16_t mvc[3][2] );

    /* XXX Needed for x264_mb_predict_mv */
    h->mb.i_partition = D_8x16;

    for( int i = 0; i < 2; i++ )
    {
        x264_me_t *l0m = &a->l0.me8x16[i];
        const int minref = X264_MIN( a->l0.me8x8[i].i_ref, a->l0.me8x8[i+2].i_ref );
        const int maxref = X264_MAX( a->l0.me8x8[i].i_ref, a->l0.me8x8[i+2].i_ref );
        const int ref8[2] = { minref, maxref };
        const int i_ref8s = ( ref8[0] == ref8[1] ) ? 1 : 2;

        m.i_pixel = PIXEL_8x16;

        LOAD_FENC( &m, p_fenc, 8*i, 0 );
        l0m->cost = INT_MAX;
        for( int j = 0; j < i_ref8s; j++ )
        {
            const int i_ref = ref8[j];
            m.i_ref_cost = REF_COST( 0, i_ref );

            CP32( mvc[0], a->l0.mvc[i_ref][0] );
            CP32( mvc[1], a->l0.mvc[i_ref][i+1] );
            CP32( mvc[2], a->l0.mvc[i_ref][i+3] );

            LOAD_HPELS( &m, h->mb.pic.p_fref[0][i_ref], 0, i_ref, 8*i, 0 );
            LOAD_WPELS( &m, h->mb.pic.p_fref_w[i_ref], 0, i_ref, 8*i, 0 );

            x264_macroblock_cache_ref( h, 2*i, 0, 2, 4, 0, i_ref );
            x264_mb_predict_mv( h, 0, 4*i, 2, m.mvp );
            /* We can only take this shortcut if the first search was performed on ref0. */
            if( h->mb.ref_blind_dupe == i_ref && !ref8[0] )
            {
                /* We can just leave the MV from the previous ref search. */
                x264_me_refine_qpel_refdupe( h, &m, NULL );
            }
            else
                x264_me_search( h, &m, mvc, 3 );

            m.cost += m.i_ref_cost;

            if( m.cost < l0m->cost )
                h->mc.memcpy_aligned( l0m, &m, sizeof(x264_me_t) );
        }

        /* Early termination based on the current SATD score of partition[0]
           plus the estimated SATD score of partition[1] */
        if( a->b_early_terminate && (!i && l0m->cost + a->i_cost_est8x16[1] > i_best_satd * (4 + !!a->i_mbrd) / 4) )
        {
            a->l0.i_cost8x16 = COST_MAX;
            return;
        }

        x264_macroblock_cache_mv_ptr( h, 2*i, 0, 2, 4, 0, l0m->mv );
        x264_macroblock_cache_ref( h, 2*i, 0, 2, 4, 0, l0m->i_ref );
    }

    a->l0.i_cost8x16 = a->l0.me8x16[0].cost + a->l0.me8x16[1].cost;
}

static ALWAYS_INLINE int x264_mb_analyse_inter_p4x4_chroma_internal( x264_t *h, x264_mb_analysis_t *a,
                                                                     pixel **p_fref, int i8x8, int size, int chroma )
{
    ALIGNED_ARRAY_16( pixel, pix1,[16*16] );
    pixel *pix2 = pix1+8;
    int i_stride = h->mb.pic.i_stride[1];
    int chroma_h_shift = chroma <= CHROMA_422;
    int chroma_v_shift = chroma == CHROMA_420;
    int or = 8*(i8x8&1) + (4>>chroma_v_shift)*(i8x8&2)*i_stride;
    int i_ref = a->l0.me8x8[i8x8].i_ref;
    int mvy_offset = chroma_v_shift && MB_INTERLACED & i_ref ? (h->mb.i_mb_y & 1)*4 - 2 : 0;
    x264_weight_t *weight = h->sh.weight[i_ref];

    // FIXME weight can be done on 4x4 blocks even if mc is smaller
#define CHROMA4x4MC( width, height, me, x, y ) \
    if( chroma == CHROMA_444 ) \
    { \
        int mvx = (me).mv[0] + 4*2*x; \
        int mvy = (me).mv[1] + 4*2*y; \
        h->mc.mc_luma( &pix1[2*x+2*y*16], 16, &h->mb.pic.p_fref[0][i_ref][4], i_stride, \
                       mvx, mvy, 2*width, 2*height, &h->sh.weight[i_ref][1] ); \
        h->mc.mc_luma( &pix2[2*x+2*y*16], 16, &h->mb.pic.p_fref[0][i_ref][8], i_stride, \
                       mvx, mvy, 2*width, 2*height, &h->sh.weight[i_ref][2] ); \
    } \
    else \
    { \
        int offset = x + (2>>chroma_v_shift)*16*y; \
        int chroma_height = (2>>chroma_v_shift)*height; \
        h->mc.mc_chroma( &pix1[offset], &pix2[offset], 16, &p_fref[4][or+2*x+(2>>chroma_v_shift)*y*i_stride], i_stride, \
                         (me).mv[0], (2>>chroma_v_shift)*((me).mv[1]+mvy_offset), width, chroma_height ); \
        if( weight[1].weightfn ) \
            weight[1].weightfn[width>>2]( &pix1[offset], 16, &pix1[offset], 16, &weight[1], chroma_height ); \
        if( weight[2].weightfn ) \
            weight[2].weightfn[width>>2]( &pix2[offset], 16, &pix2[offset], 16, &weight[2], chroma_height ); \
    }

    if( size == PIXEL_4x4 )
    {
        x264_me_t *m = a->l0.me4x4[i8x8];
        CHROMA4x4MC( 2,2, m[0], 0,0 );
        CHROMA4x4MC( 2,2, m[1], 2,0 );
        CHROMA4x4MC( 2,2, m[2], 0,2 );
        CHROMA4x4MC( 2,2, m[3], 2,2 );
    }
    else if( size == PIXEL_8x4 )
    {
        x264_me_t *m = a->l0.me8x4[i8x8];
        CHROMA4x4MC( 4,2, m[0], 0,0 );
        CHROMA4x4MC( 4,2, m[1], 0,2 );
    }
    else
    {
        x264_me_t *m = a->l0.me4x8[i8x8];
        CHROMA4x4MC( 2,4, m[0], 0,0 );
        CHROMA4x4MC( 2,4, m[1], 2,0 );
    }
#undef CHROMA4x4MC

    int oe = (8>>chroma_h_shift)*(i8x8&1) + (4>>chroma_v_shift)*(i8x8&2)*FENC_STRIDE;
    int chromapix = chroma == CHROMA_444 ? PIXEL_8x8 : chroma == CHROMA_422 ? PIXEL_4x8 : PIXEL_4x4;
    return h->pixf.mbcmp[chromapix]( &h->mb.pic.p_fenc[1][oe], FENC_STRIDE, pix1, 16 )
         + h->pixf.mbcmp[chromapix]( &h->mb.pic.p_fenc[2][oe], FENC_STRIDE, pix2, 16 );
}

static int x264_mb_analyse_inter_p4x4_chroma( x264_t *h, x264_mb_analysis_t *a, pixel **p_fref, int i8x8, int size )
{
    if( CHROMA_FORMAT == CHROMA_444 )
        return x264_mb_analyse_inter_p4x4_chroma_internal( h, a, p_fref, i8x8, size, CHROMA_444 );
    else if( CHROMA_FORMAT == CHROMA_422 )
        return x264_mb_analyse_inter_p4x4_chroma_internal( h, a, p_fref, i8x8, size, CHROMA_422 );
    else
        return x264_mb_analyse_inter_p4x4_chroma_internal( h, a, p_fref, i8x8, size, CHROMA_420 );
}

static void x264_mb_analyse_inter_p4x4( x264_t *h, x264_mb_analysis_t *a, int i8x8 )
{
    pixel **p_fref = h->mb.pic.p_fref[0][a->l0.me8x8[i8x8].i_ref];
    pixel **p_fenc = h->mb.pic.p_fenc;
    const int i_ref = a->l0.me8x8[i8x8].i_ref;

    /* XXX Needed for x264_mb_predict_mv */
    h->mb.i_partition = D_8x8;

    for( int i4x4 = 0; i4x4 < 4; i4x4++ )
    {
        const int idx = 4*i8x8 + i4x4;
        const int x4 = block_idx_x[idx];
        const int y4 = block_idx_y[idx];
        const int i_mvc = (i4x4 == 0);

        x264_me_t *m = &a->l0.me4x4[i8x8][i4x4];

        m->i_pixel = PIXEL_4x4;

        LOAD_FENC( m, p_fenc, 4*x4, 4*y4 );
        LOAD_HPELS( m, p_fref, 0, i_ref, 4*x4, 4*y4 );
        LOAD_WPELS( m, h->mb.pic.p_fref_w[i_ref], 0, i_ref, 4*x4, 4*y4 );

        x264_mb_predict_mv( h, 0, idx, 1, m->mvp );
        x264_me_search( h, m, &a->l0.me8x8[i8x8].mv, i_mvc );

        x264_macroblock_cache_mv_ptr( h, x4, y4, 1, 1, 0, m->mv );
    }
    a->l0.i_cost4x4[i8x8] = a->l0.me4x4[i8x8][0].cost +
                            a->l0.me4x4[i8x8][1].cost +
                            a->l0.me4x4[i8x8][2].cost +
                            a->l0.me4x4[i8x8][3].cost +
                            REF_COST( 0, i_ref ) +
                            a->i_lambda * i_sub_mb_p_cost_table[D_L0_4x4];
    if( h->mb.b_chroma_me )
        a->l0.i_cost4x4[i8x8] += x264_mb_analyse_inter_p4x4_chroma( h, a, p_fref, i8x8, PIXEL_4x4 );
}

static void x264_mb_analyse_inter_p8x4( x264_t *h, x264_mb_analysis_t *a, int i8x8 )
{
    pixel **p_fref = h->mb.pic.p_fref[0][a->l0.me8x8[i8x8].i_ref];
    pixel **p_fenc = h->mb.pic.p_fenc;
    const int i_ref = a->l0.me8x8[i8x8].i_ref;

    /* XXX Needed for x264_mb_predict_mv */
    h->mb.i_partition = D_8x8;

    for( int i8x4 = 0; i8x4 < 2; i8x4++ )
    {
        const int idx = 4*i8x8 + 2*i8x4;
        const int x4 = block_idx_x[idx];
        const int y4 = block_idx_y[idx];
        const int i_mvc = (i8x4 == 0);

        x264_me_t *m = &a->l0.me8x4[i8x8][i8x4];

        m->i_pixel = PIXEL_8x4;

        LOAD_FENC( m, p_fenc, 4*x4, 4*y4 );
        LOAD_HPELS( m, p_fref, 0, i_ref, 4*x4, 4*y4 );
        LOAD_WPELS( m, h->mb.pic.p_fref_w[i_ref], 0, i_ref, 4*x4, 4*y4 );

        x264_mb_predict_mv( h, 0, idx, 2, m->mvp );
        x264_me_search( h, m, &a->l0.me4x4[i8x8][0].mv, i_mvc );

        x264_macroblock_cache_mv_ptr( h, x4, y4, 2, 1, 0, m->mv );
    }
    a->l0.i_cost8x4[i8x8] = a->l0.me8x4[i8x8][0].cost + a->l0.me8x4[i8x8][1].cost +
                            REF_COST( 0, i_ref ) +
                            a->i_lambda * i_sub_mb_p_cost_table[D_L0_8x4];
    if( h->mb.b_chroma_me )
        a->l0.i_cost8x4[i8x8] += x264_mb_analyse_inter_p4x4_chroma( h, a, p_fref, i8x8, PIXEL_8x4 );
}

static void x264_mb_analyse_inter_p4x8( x264_t *h, x264_mb_analysis_t *a, int i8x8 )
{
    pixel **p_fref = h->mb.pic.p_fref[0][a->l0.me8x8[i8x8].i_ref];
    pixel **p_fenc = h->mb.pic.p_fenc;
    const int i_ref = a->l0.me8x8[i8x8].i_ref;

    /* XXX Needed for x264_mb_predict_mv */
    h->mb.i_partition = D_8x8;

    for( int i4x8 = 0; i4x8 < 2; i4x8++ )
    {
        const int idx = 4*i8x8 + i4x8;
        const int x4 = block_idx_x[idx];
        const int y4 = block_idx_y[idx];
        const int i_mvc = (i4x8 == 0);

        x264_me_t *m = &a->l0.me4x8[i8x8][i4x8];

        m->i_pixel = PIXEL_4x8;

        LOAD_FENC( m, p_fenc, 4*x4, 4*y4 );
        LOAD_HPELS( m, p_fref, 0, i_ref, 4*x4, 4*y4 );
        LOAD_WPELS( m, h->mb.pic.p_fref_w[i_ref], 0, i_ref, 4*x4, 4*y4 );

        x264_mb_predict_mv( h, 0, idx, 1, m->mvp );
        x264_me_search( h, m, &a->l0.me4x4[i8x8][0].mv, i_mvc );

        x264_macroblock_cache_mv_ptr( h, x4, y4, 1, 2, 0, m->mv );
    }
    a->l0.i_cost4x8[i8x8] = a->l0.me4x8[i8x8][0].cost + a->l0.me4x8[i8x8][1].cost +
                            REF_COST( 0, i_ref ) +
                            a->i_lambda * i_sub_mb_p_cost_table[D_L0_4x8];
    if( h->mb.b_chroma_me )
        a->l0.i_cost4x8[i8x8] += x264_mb_analyse_inter_p4x4_chroma( h, a, p_fref, i8x8, PIXEL_4x8 );
}

static ALWAYS_INLINE int x264_analyse_bi_chroma( x264_t *h, x264_mb_analysis_t *a, int idx, int i_pixel )
{
    ALIGNED_ARRAY_16( pixel, pix, [4],[16*16] );
    ALIGNED_ARRAY_16( pixel,  bi, [2],[16*16] );
    int i_chroma_cost = 0;
    int chromapix = h->luma2chroma_pixel[i_pixel];

#define COST_BI_CHROMA( m0, m1, width, height ) \
{ \
    if( CHROMA444 ) \
    { \
        h->mc.mc_luma( pix[0], 16, &m0.p_fref[4], m0.i_stride[1], \
                       m0.mv[0], m0.mv[1], width, height, x264_weight_none ); \
        h->mc.mc_luma( pix[1], 16, &m0.p_fref[8], m0.i_stride[2], \
                       m0.mv[0], m0.mv[1], width, height, x264_weight_none ); \
        h->mc.mc_luma( pix[2], 16, &m1.p_fref[4], m1.i_stride[1], \
                       m1.mv[0], m1.mv[1], width, height, x264_weight_none ); \
        h->mc.mc_luma( pix[3], 16, &m1.p_fref[8], m1.i_stride[2], \
                       m1.mv[0], m1.mv[1], width, height, x264_weight_none ); \
    } \
    else \
    { \
        int v_shift = CHROMA_V_SHIFT; \
        int l0_mvy_offset = v_shift & MB_INTERLACED & m0.i_ref ? (h->mb.i_mb_y & 1)*4 - 2 : 0; \
        int l1_mvy_offset = v_shift & MB_INTERLACED & m1.i_ref ? (h->mb.i_mb_y & 1)*4 - 2 : 0; \
        h->mc.mc_chroma( pix[0], pix[1], 16, m0.p_fref[4], m0.i_stride[1], \
                         m0.mv[0], 2*(m0.mv[1]+l0_mvy_offset)>>v_shift, width>>1, height>>v_shift ); \
        h->mc.mc_chroma( pix[2], pix[3], 16, m1.p_fref[4], m1.i_stride[1], \
                         m1.mv[0], 2*(m1.mv[1]+l1_mvy_offset)>>v_shift, width>>1, height>>v_shift ); \
    } \
    h->mc.avg[chromapix]( bi[0], 16, pix[0], 16, pix[2], 16, h->mb.bipred_weight[m0.i_ref][m1.i_ref] ); \
    h->mc.avg[chromapix]( bi[1], 16, pix[1], 16, pix[3], 16, h->mb.bipred_weight[m0.i_ref][m1.i_ref] ); \
    i_chroma_cost = h->pixf.mbcmp[chromapix]( m0.p_fenc[1], FENC_STRIDE, bi[0], 16 ) \
                  + h->pixf.mbcmp[chromapix]( m0.p_fenc[2], FENC_STRIDE, bi[1], 16 ); \
}

    if( i_pixel == PIXEL_16x16 )
        COST_BI_CHROMA( a->l0.bi16x16, a->l1.bi16x16, 16, 16 )
    else if( i_pixel == PIXEL_16x8 )
        COST_BI_CHROMA( a->l0.me16x8[idx], a->l1.me16x8[idx], 16, 8 )
    else if( i_pixel == PIXEL_8x16 )
        COST_BI_CHROMA( a->l0.me8x16[idx], a->l1.me8x16[idx], 8, 16 )
    else
        COST_BI_CHROMA( a->l0.me8x8[idx], a->l1.me8x8[idx], 8, 8 )

    return i_chroma_cost;
}

static void x264_mb_analyse_inter_direct( x264_t *h, x264_mb_analysis_t *a )
{
    /* Assumes that fdec still contains the results of
     * x264_mb_predict_mv_direct16x16 and x264_mb_mc */

    pixel *p_fenc = h->mb.pic.p_fenc[0];
    pixel *p_fdec = h->mb.pic.p_fdec[0];

    a->i_cost16x16direct = a->i_lambda * i_mb_b_cost_table[B_DIRECT];
    if( h->param.analyse.inter & X264_ANALYSE_BSUB16x16 )
    {
        int chromapix = h->luma2chroma_pixel[PIXEL_8x8];

        for( int i = 0; i < 4; i++ )
        {
            const int x = (i&1)*8;
            const int y = (i>>1)*8;
            a->i_cost8x8direct[i] = h->pixf.mbcmp[PIXEL_8x8]( &p_fenc[x+y*FENC_STRIDE], FENC_STRIDE,
                                                              &p_fdec[x+y*FDEC_STRIDE], FDEC_STRIDE );
            if( h->mb.b_chroma_me )
            {
                int fenc_offset = (x>>CHROMA_H_SHIFT) + (y>>CHROMA_V_SHIFT)*FENC_STRIDE;
                int fdec_offset = (x>>CHROMA_H_SHIFT) + (y>>CHROMA_V_SHIFT)*FDEC_STRIDE;
                a->i_cost8x8direct[i] += h->pixf.mbcmp[chromapix]( &h->mb.pic.p_fenc[1][fenc_offset], FENC_STRIDE,
                                                                   &h->mb.pic.p_fdec[1][fdec_offset], FDEC_STRIDE )
                                       + h->pixf.mbcmp[chromapix]( &h->mb.pic.p_fenc[2][fenc_offset], FENC_STRIDE,
                                                                   &h->mb.pic.p_fdec[2][fdec_offset], FDEC_STRIDE );
            }
            a->i_cost16x16direct += a->i_cost8x8direct[i];

            /* mb type cost */
            a->i_cost8x8direct[i] += a->i_lambda * i_sub_mb_b_cost_table[D_DIRECT_8x8];
        }
    }
    else
    {
        a->i_cost16x16direct += h->pixf.mbcmp[PIXEL_16x16]( p_fenc, FENC_STRIDE, p_fdec, FDEC_STRIDE );
        if( h->mb.b_chroma_me )
        {
            int chromapix = h->luma2chroma_pixel[PIXEL_16x16];
            a->i_cost16x16direct += h->pixf.mbcmp[chromapix]( h->mb.pic.p_fenc[1], FENC_STRIDE, h->mb.pic.p_fdec[1], FDEC_STRIDE )
                                 +  h->pixf.mbcmp[chromapix]( h->mb.pic.p_fenc[2], FENC_STRIDE, h->mb.pic.p_fdec[2], FDEC_STRIDE );
        }
    }
}

static void x264_mb_analyse_inter_b16x16( x264_t *h, x264_mb_analysis_t *a )
{
    ALIGNED_ARRAY_16( pixel, pix0,[16*16] );
    ALIGNED_ARRAY_16( pixel, pix1,[16*16] );
    pixel *src0, *src1;
    int stride0 = 16, stride1 = 16;
    int i_ref, i_mvc;
    ALIGNED_4( int16_t mvc[9][2] );
    int try_skip = a->b_try_skip;
    int list1_skipped = 0;
    int i_halfpel_thresh[2] = {INT_MAX, INT_MAX};
    int *p_halfpel_thresh[2] = {(a->b_early_terminate && h->mb.pic.i_fref[0]>1) ? &i_halfpel_thresh[0] : NULL,
                                (a->b_early_terminate && h->mb.pic.i_fref[1]>1) ? &i_halfpel_thresh[1] : NULL};

    x264_me_t m;
    m.i_pixel = PIXEL_16x16;

    LOAD_FENC( &m, h->mb.pic.p_fenc, 0, 0 );

    /* 16x16 Search on list 0 and list 1 */
    a->l0.me16x16.cost = INT_MAX;
    a->l1.me16x16.cost = INT_MAX;
    for( int l = 1; l >= 0; )
    {
        x264_mb_analysis_list_t *lX = l ? &a->l1 : &a->l0;

        /* This loop is extremely munged in order to facilitate the following order of operations,
         * necessary for an efficient fast skip.
         * 1.  Search list1 ref0.
         * 2.  Search list0 ref0.
         * 3.  Try skip.
         * 4.  Search the rest of list0.
         * 5.  Go back and finish list1.
         */
        for( i_ref = (list1_skipped && l == 1) ? 1 : 0; i_ref < h->mb.pic.i_fref[l]; i_ref++ )
        {
            if( try_skip && l == 1 && i_ref > 0 )
            {
                list1_skipped = 1;
                break;
            }

            m.i_ref_cost = REF_COST( l, i_ref );

            /* search with ref */
            LOAD_HPELS( &m, h->mb.pic.p_fref[l][i_ref], l, i_ref, 0, 0 );
            x264_mb_predict_mv_16x16( h, l, i_ref, m.mvp );
            x264_mb_predict_mv_ref16x16( h, l, i_ref, mvc, &i_mvc );
            x264_me_search_ref( h, &m, mvc, i_mvc, p_halfpel_thresh[l] );

            /* add ref cost */
            m.cost += m.i_ref_cost;

            if( m.cost < lX->me16x16.cost )
                h->mc.memcpy_aligned( &lX->me16x16, &m, sizeof(x264_me_t) );

            /* save mv for predicting neighbors */
            CP32( lX->mvc[i_ref][0], m.mv );
            CP32( h->mb.mvr[l][i_ref][h->mb.i_mb_xy], m.mv );

            /* Fast skip detection. */
            if( i_ref == 0 && try_skip )
            {
                if( abs(lX->me16x16.mv[0]-h->mb.cache.direct_mv[l][0][0]) +
                    abs(lX->me16x16.mv[1]-h->mb.cache.direct_mv[l][0][1]) > 1 )
                {
                    try_skip = 0;
                }
                else if( !l )
                {
                    /* We already tested skip */
                    h->mb.i_type = B_SKIP;
                    x264_analyse_update_cache( h, a );
                    return;
                }
            }
        }
        if( list1_skipped && l == 1 && i_ref == h->mb.pic.i_fref[1] )
            break;
        if( list1_skipped && l == 0 )
            l = 1;
        else
            l--;
    }

    /* get cost of BI mode */
    h->mc.memcpy_aligned( &a->l0.bi16x16, &a->l0.me16x16, sizeof(x264_me_t) );
    h->mc.memcpy_aligned( &a->l1.bi16x16, &a->l1.me16x16, sizeof(x264_me_t) );
    int ref_costs = REF_COST( 0, a->l0.bi16x16.i_ref ) + REF_COST( 1, a->l1.bi16x16.i_ref );
    src0 = h->mc.get_ref( pix0, &stride0,
                          h->mb.pic.p_fref[0][a->l0.bi16x16.i_ref], h->mb.pic.i_stride[0],
                          a->l0.bi16x16.mv[0], a->l0.bi16x16.mv[1], 16, 16, x264_weight_none );
    src1 = h->mc.get_ref( pix1, &stride1,
                          h->mb.pic.p_fref[1][a->l1.bi16x16.i_ref], h->mb.pic.i_stride[0],
                          a->l1.bi16x16.mv[0], a->l1.bi16x16.mv[1], 16, 16, x264_weight_none );

    h->mc.avg[PIXEL_16x16]( pix0, 16, src0, stride0, src1, stride1, h->mb.bipred_weight[a->l0.bi16x16.i_ref][a->l1.bi16x16.i_ref] );

    a->i_cost16x16bi = h->pixf.mbcmp[PIXEL_16x16]( h->mb.pic.p_fenc[0], FENC_STRIDE, pix0, 16 )
                     + ref_costs
                     + a->l0.bi16x16.cost_mv
                     + a->l1.bi16x16.cost_mv;

    if( h->mb.b_chroma_me )
        a->i_cost16x16bi += x264_analyse_bi_chroma( h, a, 0, PIXEL_16x16 );

    /* Always try the 0,0,0,0 vector; helps avoid errant motion vectors in fades */
    if( M32( a->l0.bi16x16.mv ) | M32( a->l1.bi16x16.mv ) )
    {
        int l0_mv_cost = a->l0.bi16x16.p_cost_mv[-a->l0.bi16x16.mvp[0]]
                       + a->l0.bi16x16.p_cost_mv[-a->l0.bi16x16.mvp[1]];
        int l1_mv_cost = a->l1.bi16x16.p_cost_mv[-a->l1.bi16x16.mvp[0]]
                       + a->l1.bi16x16.p_cost_mv[-a->l1.bi16x16.mvp[1]];
        h->mc.avg[PIXEL_16x16]( pix0, 16, h->mb.pic.p_fref[0][a->l0.bi16x16.i_ref][0], h->mb.pic.i_stride[0],
                                h->mb.pic.p_fref[1][a->l1.bi16x16.i_ref][0], h->mb.pic.i_stride[0],
                                h->mb.bipred_weight[a->l0.bi16x16.i_ref][a->l1.bi16x16.i_ref] );
        int cost00 = h->pixf.mbcmp[PIXEL_16x16]( h->mb.pic.p_fenc[0], FENC_STRIDE, pix0, 16 )
                   + ref_costs + l0_mv_cost + l1_mv_cost;

        if( h->mb.b_chroma_me )
        {
            ALIGNED_ARRAY_16( pixel, bi, [16*FENC_STRIDE] );

            if( CHROMA444 )
            {
                h->mc.avg[PIXEL_16x16]( bi, FENC_STRIDE, h->mb.pic.p_fref[0][a->l0.bi16x16.i_ref][4], h->mb.pic.i_stride[1],
                                        h->mb.pic.p_fref[1][a->l1.bi16x16.i_ref][4], h->mb.pic.i_stride[1],
                                        h->mb.bipred_weight[a->l0.bi16x16.i_ref][a->l1.bi16x16.i_ref] );
                cost00 += h->pixf.mbcmp[PIXEL_16x16]( h->mb.pic.p_fenc[1], FENC_STRIDE, bi, FENC_STRIDE );
                h->mc.avg[PIXEL_16x16]( bi, FENC_STRIDE, h->mb.pic.p_fref[0][a->l0.bi16x16.i_ref][8], h->mb.pic.i_stride[2],
                                        h->mb.pic.p_fref[1][a->l1.bi16x16.i_ref][8], h->mb.pic.i_stride[2],
                                        h->mb.bipred_weight[a->l0.bi16x16.i_ref][a->l1.bi16x16.i_ref] );
                cost00 += h->pixf.mbcmp[PIXEL_16x16]( h->mb.pic.p_fenc[2], FENC_STRIDE, bi, FENC_STRIDE );
            }
            else
            {
                ALIGNED_ARRAY_16( pixel, pixuv, [2],[16*FENC_STRIDE] );
                int chromapix = h->luma2chroma_pixel[PIXEL_16x16];
                int v_shift = CHROMA_V_SHIFT;

                if( v_shift & MB_INTERLACED & a->l0.bi16x16.i_ref )
                {
                    int l0_mvy_offset = (h->mb.i_mb_y & 1)*4 - 2;
                    h->mc.mc_chroma( pixuv[0], pixuv[0]+8, FENC_STRIDE, h->mb.pic.p_fref[0][a->l0.bi16x16.i_ref][4],
                                     h->mb.pic.i_stride[1], 0, 0 + l0_mvy_offset, 8, 8 );
                }
                else
                    h->mc.load_deinterleave_chroma_fenc( pixuv[0], h->mb.pic.p_fref[0][a->l0.bi16x16.i_ref][4],
                                                         h->mb.pic.i_stride[1], 16>>v_shift );

                if( v_shift & MB_INTERLACED & a->l1.bi16x16.i_ref )
                {
                    int l1_mvy_offset = (h->mb.i_mb_y & 1)*4 - 2;
                    h->mc.mc_chroma( pixuv[1], pixuv[1]+8, FENC_STRIDE, h->mb.pic.p_fref[1][a->l1.bi16x16.i_ref][4],
                                     h->mb.pic.i_stride[1], 0, 0 + l1_mvy_offset, 8, 8 );
                }
                else
                    h->mc.load_deinterleave_chroma_fenc( pixuv[1], h->mb.pic.p_fref[1][a->l1.bi16x16.i_ref][4],
                                                         h->mb.pic.i_stride[1], 16>>v_shift );

                h->mc.avg[chromapix]( bi,   FENC_STRIDE, pixuv[0],   FENC_STRIDE, pixuv[1],   FENC_STRIDE,
                                      h->mb.bipred_weight[a->l0.bi16x16.i_ref][a->l1.bi16x16.i_ref] );
                h->mc.avg[chromapix]( bi+8, FENC_STRIDE, pixuv[0]+8, FENC_STRIDE, pixuv[1]+8, FENC_STRIDE,
                                      h->mb.bipred_weight[a->l0.bi16x16.i_ref][a->l1.bi16x16.i_ref] );

                cost00 += h->pixf.mbcmp[chromapix]( h->mb.pic.p_fenc[1], FENC_STRIDE, bi,   FENC_STRIDE )
                       +  h->pixf.mbcmp[chromapix]( h->mb.pic.p_fenc[2], FENC_STRIDE, bi+8, FENC_STRIDE );
            }
        }

        if( cost00 < a->i_cost16x16bi )
        {
            M32( a->l0.bi16x16.mv ) = 0;
            M32( a->l1.bi16x16.mv ) = 0;
            a->l0.bi16x16.cost_mv = l0_mv_cost;
            a->l1.bi16x16.cost_mv = l1_mv_cost;
            a->i_cost16x16bi = cost00;
        }
    }

    /* mb type cost */
    a->i_cost16x16bi   += a->i_lambda * i_mb_b_cost_table[B_BI_BI];
    a->l0.me16x16.cost += a->i_lambda * i_mb_b_cost_table[B_L0_L0];
    a->l1.me16x16.cost += a->i_lambda * i_mb_b_cost_table[B_L1_L1];
}

static inline void x264_mb_cache_mv_p8x8( x264_t *h, x264_mb_analysis_t *a, int i )
{
    int x = 2*(i&1);
    int y = i&2;

    switch( h->mb.i_sub_partition[i] )
    {
        case D_L0_8x8:
            x264_macroblock_cache_mv_ptr( h, x, y, 2, 2, 0, a->l0.me8x8[i].mv );
            break;
        case D_L0_8x4:
            x264_macroblock_cache_mv_ptr( h, x, y+0, 2, 1, 0, a->l0.me8x4[i][0].mv );
            x264_macroblock_cache_mv_ptr( h, x, y+1, 2, 1, 0, a->l0.me8x4[i][1].mv );
            break;
        case D_L0_4x8:
            x264_macroblock_cache_mv_ptr( h, x+0, y, 1, 2, 0, a->l0.me4x8[i][0].mv );
            x264_macroblock_cache_mv_ptr( h, x+1, y, 1, 2, 0, a->l0.me4x8[i][1].mv );
            break;
        case D_L0_4x4:
            x264_macroblock_cache_mv_ptr( h, x+0, y+0, 1, 1, 0, a->l0.me4x4[i][0].mv );
            x264_macroblock_cache_mv_ptr( h, x+1, y+0, 1, 1, 0, a->l0.me4x4[i][1].mv );
            x264_macroblock_cache_mv_ptr( h, x+0, y+1, 1, 1, 0, a->l0.me4x4[i][2].mv );
            x264_macroblock_cache_mv_ptr( h, x+1, y+1, 1, 1, 0, a->l0.me4x4[i][3].mv );
            break;
        default:
            x264_log( h, X264_LOG_ERROR, "internal error\n" );
            break;
    }
}

static void x264_mb_load_mv_direct8x8( x264_t *h, int idx )
{
    int x = 2*(idx&1);
    int y = idx&2;
    x264_macroblock_cache_ref( h, x, y, 2, 2, 0, h->mb.cache.direct_ref[0][idx] );
    x264_macroblock_cache_ref( h, x, y, 2, 2, 1, h->mb.cache.direct_ref[1][idx] );
    x264_macroblock_cache_mv_ptr( h, x, y, 2, 2, 0, h->mb.cache.direct_mv[0][idx] );
    x264_macroblock_cache_mv_ptr( h, x, y, 2, 2, 1, h->mb.cache.direct_mv[1][idx] );
}

#define CACHE_MV_BI(x,y,dx,dy,me0,me1,part) \
    if( x264_mb_partition_listX_table[0][part] ) \
    { \
        x264_macroblock_cache_ref( h, x,y,dx,dy, 0, me0.i_ref ); \
        x264_macroblock_cache_mv_ptr( h, x,y,dx,dy, 0, me0.mv ); \
    } \
    else \
    { \
        x264_macroblock_cache_ref( h, x,y,dx,dy, 0, -1 ); \
        x264_macroblock_cache_mv(  h, x,y,dx,dy, 0, 0 ); \
        if( b_mvd ) \
            x264_macroblock_cache_mvd( h, x,y,dx,dy, 0, 0 ); \
    } \
    if( x264_mb_partition_listX_table[1][part] ) \
    { \
        x264_macroblock_cache_ref( h, x,y,dx,dy, 1, me1.i_ref ); \
        x264_macroblock_cache_mv_ptr( h, x,y,dx,dy, 1, me1.mv ); \
    } \
    else \
    { \
        x264_macroblock_cache_ref( h, x,y,dx,dy, 1, -1 ); \
        x264_macroblock_cache_mv(  h, x,y,dx,dy, 1, 0 ); \
        if( b_mvd ) \
            x264_macroblock_cache_mvd( h, x,y,dx,dy, 1, 0 ); \
    }

static inline void x264_mb_cache_mv_b8x8( x264_t *h, x264_mb_analysis_t *a, int i, int b_mvd )
{
    int x = 2*(i&1);
    int y = i&2;
    if( h->mb.i_sub_partition[i] == D_DIRECT_8x8 )
    {
        x264_mb_load_mv_direct8x8( h, i );
        if( b_mvd )
        {
            x264_macroblock_cache_mvd(  h, x, y, 2, 2, 0, 0 );
            x264_macroblock_cache_mvd(  h, x, y, 2, 2, 1, 0 );
            x264_macroblock_cache_skip( h, x, y, 2, 2, 1 );
        }
    }
    else
    {
        CACHE_MV_BI( x, y, 2, 2, a->l0.me8x8[i], a->l1.me8x8[i], h->mb.i_sub_partition[i] );
    }
}
static inline void x264_mb_cache_mv_b16x8( x264_t *h, x264_mb_analysis_t *a, int i, int b_mvd )
{
    CACHE_MV_BI( 0, 2*i, 4, 2, a->l0.me16x8[i], a->l1.me16x8[i], a->i_mb_partition16x8[i] );
}
static inline void x264_mb_cache_mv_b8x16( x264_t *h, x264_mb_analysis_t *a, int i, int b_mvd )
{
    CACHE_MV_BI( 2*i, 0, 2, 4, a->l0.me8x16[i], a->l1.me8x16[i], a->i_mb_partition8x16[i] );
}
#undef CACHE_MV_BI

static void x264_mb_analyse_inter_b8x8_mixed_ref( x264_t *h, x264_mb_analysis_t *a )
{
    ALIGNED_ARRAY_16( pixel, pix,[2],[8*8] );
    int i_maxref[2] = {h->mb.pic.i_fref[0]-1, h->mb.pic.i_fref[1]-1};

    /* early termination: if 16x16 chose ref 0, then evalute no refs older
     * than those used by the neighbors */
    #define CHECK_NEIGHBOUR(i)\
    {\
        int ref = h->mb.cache.ref[l][X264_SCAN8_0+i];\
        if( ref > i_maxref[l] )\
            i_maxref[l] = ref;\
    }

    for( int l = 0; l < 2; l++ )
    {
        x264_mb_analysis_list_t *lX = l ? &a->l1 : &a->l0;
        if( i_maxref[l] > 0 && lX->me16x16.i_ref == 0 &&
            h->mb.i_mb_type_top > 0 && h->mb.i_mb_type_left[0] > 0 )
        {
            i_maxref[l] = 0;
            CHECK_NEIGHBOUR(  -8 - 1 );
            CHECK_NEIGHBOUR(  -8 + 0 );
            CHECK_NEIGHBOUR(  -8 + 2 );
            CHECK_NEIGHBOUR(  -8 + 4 );
            CHECK_NEIGHBOUR(   0 - 1 );
            CHECK_NEIGHBOUR( 2*8 - 1 );
        }
    }

    /* XXX Needed for x264_mb_predict_mv */
    h->mb.i_partition = D_8x8;

    a->i_cost8x8bi = 0;

    for( int i = 0; i < 4; i++ )
    {
        int x8 = i&1;
        int y8 = i>>1;
        int i_part_cost;
        int i_part_cost_bi;
        int stride[2] = {8,8};
        pixel *src[2];
        x264_me_t m;
        m.i_pixel = PIXEL_8x8;
        LOAD_FENC( &m, h->mb.pic.p_fenc, 8*x8, 8*y8 );

        for( int l = 0; l < 2; l++ )
        {
            x264_mb_analysis_list_t *lX = l ? &a->l1 : &a->l0;

            lX->me8x8[i].cost = INT_MAX;
            for( int i_ref = 0; i_ref <= i_maxref[l]; i_ref++ )
            {
                m.i_ref_cost = REF_COST( l, i_ref );

                LOAD_HPELS( &m, h->mb.pic.p_fref[l][i_ref], l, i_ref, 8*x8, 8*y8 );

                x264_macroblock_cache_ref( h, x8*2, y8*2, 2, 2, l, i_ref );
                x264_mb_predict_mv( h, l, 4*i, 2, m.mvp );
                x264_me_search( h, &m, lX->mvc[i_ref], i+1 );
                m.cost += m.i_ref_cost;

                if( m.cost < lX->me8x8[i].cost )
                {
                    h->mc.memcpy_aligned( &lX->me8x8[i], &m, sizeof(x264_me_t) );
                    a->i_satd8x8[l][i] = m.cost - ( m.cost_mv + m.i_ref_cost );
                }

                /* save mv for predicting other partitions within this MB */
                CP32( lX->mvc[i_ref][i+1], m.mv );
            }
        }

        /* BI mode */
        src[0] = h->mc.get_ref( pix[0], &stride[0], a->l0.me8x8[i].p_fref, a->l0.me8x8[i].i_stride[0],
                                a->l0.me8x8[i].mv[0], a->l0.me8x8[i].mv[1], 8, 8, x264_weight_none );
        src[1] = h->mc.get_ref( pix[1], &stride[1], a->l1.me8x8[i].p_fref, a->l1.me8x8[i].i_stride[0],
                                a->l1.me8x8[i].mv[0], a->l1.me8x8[i].mv[1], 8, 8, x264_weight_none );
        h->mc.avg[PIXEL_8x8]( pix[0], 8, src[0], stride[0], src[1], stride[1],
                                h->mb.bipred_weight[a->l0.me8x8[i].i_ref][a->l1.me8x8[i].i_ref] );

        a->i_satd8x8[2][i] = h->pixf.mbcmp[PIXEL_8x8]( a->l0.me8x8[i].p_fenc[0], FENC_STRIDE, pix[0], 8 );
        i_part_cost_bi = a->i_satd8x8[2][i] + a->l0.me8x8[i].cost_mv + a->l1.me8x8[i].cost_mv
                         + a->l0.me8x8[i].i_ref_cost + a->l1.me8x8[i].i_ref_cost
                         + a->i_lambda * i_sub_mb_b_cost_table[D_BI_8x8];

        if( h->mb.b_chroma_me )
        {
            int i_chroma_cost = x264_analyse_bi_chroma( h, a, i, PIXEL_8x8 );
            i_part_cost_bi += i_chroma_cost;
            a->i_satd8x8[2][i] += i_chroma_cost;
        }

        a->l0.me8x8[i].cost += a->i_lambda * i_sub_mb_b_cost_table[D_L0_8x8];
        a->l1.me8x8[i].cost += a->i_lambda * i_sub_mb_b_cost_table[D_L1_8x8];

        i_part_cost = a->l0.me8x8[i].cost;
        h->mb.i_sub_partition[i] = D_L0_8x8;
        COPY2_IF_LT( i_part_cost, a->l1.me8x8[i].cost, h->mb.i_sub_partition[i], D_L1_8x8 );
        COPY2_IF_LT( i_part_cost, i_part_cost_bi, h->mb.i_sub_partition[i], D_BI_8x8 );
        COPY2_IF_LT( i_part_cost, a->i_cost8x8direct[i], h->mb.i_sub_partition[i], D_DIRECT_8x8 );
        a->i_cost8x8bi += i_part_cost;

        /* XXX Needed for x264_mb_predict_mv */
        x264_mb_cache_mv_b8x8( h, a, i, 0 );
    }

    /* mb type cost */
    a->i_cost8x8bi += a->i_lambda * i_mb_b_cost_table[B_8x8];
}

static void x264_mb_analyse_inter_b8x8( x264_t *h, x264_mb_analysis_t *a )
{
    pixel **p_fref[2] =
        { h->mb.pic.p_fref[0][a->l0.me16x16.i_ref],
          h->mb.pic.p_fref[1][a->l1.me16x16.i_ref] };
    ALIGNED_ARRAY_16( pixel, pix,[2],[8*8] );

    /* XXX Needed for x264_mb_predict_mv */
    h->mb.i_partition = D_8x8;

    a->i_cost8x8bi = 0;

    for( int i = 0; i < 4; i++ )
    {
        int x8 = i&1;
        int y8 = i>>1;
        int i_part_cost;
        int i_part_cost_bi = 0;
        int stride[2] = {8,8};
        pixel *src[2];

        for( int l = 0; l < 2; l++ )
        {
            x264_mb_analysis_list_t *lX = l ? &a->l1 : &a->l0;
            x264_me_t *m = &lX->me8x8[i];
            m->i_pixel = PIXEL_8x8;
            LOAD_FENC( m, h->mb.pic.p_fenc, 8*x8, 8*y8 );

            m->i_ref_cost = REF_COST( l, lX->me16x16.i_ref );
            m->i_ref = lX->me16x16.i_ref;

            LOAD_HPELS( m, p_fref[l], l, lX->me16x16.i_ref, 8*x8, 8*y8 );

            x264_macroblock_cache_ref( h, x8*2, y8*2, 2, 2, l, lX->me16x16.i_ref );
            x264_mb_predict_mv( h, l, 4*i, 2, m->mvp );
            x264_me_search( h, m, &lX->me16x16.mv, 1 );
            a->i_satd8x8[l][i] = m->cost - m->cost_mv;
            m->cost += m->i_ref_cost;

            x264_macroblock_cache_mv_ptr( h, 2*x8, 2*y8, 2, 2, l, m->mv );

            /* save mv for predicting other partitions within this MB */
            CP32( lX->mvc[lX->me16x16.i_ref][i+1], m->mv );

            /* BI mode */
            src[l] = h->mc.get_ref( pix[l], &stride[l], m->p_fref, m->i_stride[0],
                                    m->mv[0], m->mv[1], 8, 8, x264_weight_none );
            i_part_cost_bi += m->cost_mv + m->i_ref_cost;
        }
        h->mc.avg[PIXEL_8x8]( pix[0], 8, src[0], stride[0], src[1], stride[1], h->mb.bipred_weight[a->l0.me16x16.i_ref][a->l1.me16x16.i_ref] );
        a->i_satd8x8[2][i] = h->pixf.mbcmp[PIXEL_8x8]( a->l0.me8x8[i].p_fenc[0], FENC_STRIDE, pix[0], 8 );
        i_part_cost_bi += a->i_satd8x8[2][i] + a->i_lambda * i_sub_mb_b_cost_table[D_BI_8x8];
        a->l0.me8x8[i].cost += a->i_lambda * i_sub_mb_b_cost_table[D_L0_8x8];
        a->l1.me8x8[i].cost += a->i_lambda * i_sub_mb_b_cost_table[D_L1_8x8];

        if( h->mb.b_chroma_me )
        {
            int i_chroma_cost = x264_analyse_bi_chroma( h, a, i, PIXEL_8x8 );
            i_part_cost_bi += i_chroma_cost;
            a->i_satd8x8[2][i] += i_chroma_cost;
        }

        i_part_cost = a->l0.me8x8[i].cost;
        h->mb.i_sub_partition[i] = D_L0_8x8;
        COPY2_IF_LT( i_part_cost, a->l1.me8x8[i].cost, h->mb.i_sub_partition[i], D_L1_8x8 );
        COPY2_IF_LT( i_part_cost, i_part_cost_bi, h->mb.i_sub_partition[i], D_BI_8x8 );
        COPY2_IF_LT( i_part_cost, a->i_cost8x8direct[i], h->mb.i_sub_partition[i], D_DIRECT_8x8 );
        a->i_cost8x8bi += i_part_cost;

        /* XXX Needed for x264_mb_predict_mv */
        x264_mb_cache_mv_b8x8( h, a, i, 0 );
    }

    /* mb type cost */
    a->i_cost8x8bi += a->i_lambda * i_mb_b_cost_table[B_8x8];
}

static void x264_mb_analyse_inter_b16x8( x264_t *h, x264_mb_analysis_t *a, int i_best_satd )
{
    ALIGNED_ARRAY_16( pixel, pix,[2],[16*8] );
    ALIGNED_4( int16_t mvc[3][2] );

    h->mb.i_partition = D_16x8;
    a->i_cost16x8bi = 0;

    for( int i = 0; i < 2; i++ )
    {
        int i_part_cost;
        int i_part_cost_bi = 0;
        int stride[2] = {16,16};
        pixel *src[2];
        x264_me_t m;
        m.i_pixel = PIXEL_16x8;
        LOAD_FENC( &m, h->mb.pic.p_fenc, 0, 8*i );

        for( int l = 0; l < 2; l++ )
        {
            x264_mb_analysis_list_t *lX = l ? &a->l1 : &a->l0;
            int ref8[2] = { lX->me8x8[2*i].i_ref, lX->me8x8[2*i+1].i_ref };
            int i_ref8s = ( ref8[0] == ref8[1] ) ? 1 : 2;
            lX->me16x8[i].cost = INT_MAX;
            for( int j = 0; j < i_ref8s; j++ )
            {
                int i_ref = ref8[j];
                m.i_ref_cost = REF_COST( l, i_ref );

                LOAD_HPELS( &m, h->mb.pic.p_fref[l][i_ref], l, i_ref, 0, 8*i );

                CP32( mvc[0], lX->mvc[i_ref][0] );
                CP32( mvc[1], lX->mvc[i_ref][2*i+1] );
                CP32( mvc[2], lX->mvc[i_ref][2*i+2] );

                x264_macroblock_cache_ref( h, 0, 2*i, 4, 2, l, i_ref );
                x264_mb_predict_mv( h, l, 8*i, 4, m.mvp );
                x264_me_search( h, &m, mvc, 3 );
                m.cost += m.i_ref_cost;

                if( m.cost < lX->me16x8[i].cost )
                    h->mc.memcpy_aligned( &lX->me16x8[i], &m, sizeof(x264_me_t) );
            }
        }

        /* BI mode */
        src[0] = h->mc.get_ref( pix[0], &stride[0], a->l0.me16x8[i].p_fref, a->l0.me16x8[i].i_stride[0],
                                a->l0.me16x8[i].mv[0], a->l0.me16x8[i].mv[1], 16, 8, x264_weight_none );
        src[1] = h->mc.get_ref( pix[1], &stride[1], a->l1.me16x8[i].p_fref, a->l1.me16x8[i].i_stride[0],
                                a->l1.me16x8[i].mv[0], a->l1.me16x8[i].mv[1], 16, 8, x264_weight_none );
        h->mc.avg[PIXEL_16x8]( pix[0], 16, src[0], stride[0], src[1], stride[1],
                                h->mb.bipred_weight[a->l0.me16x8[i].i_ref][a->l1.me16x8[i].i_ref] );

        i_part_cost_bi = h->pixf.mbcmp[PIXEL_16x8]( a->l0.me16x8[i].p_fenc[0], FENC_STRIDE, pix[0], 16 )
                        + a->l0.me16x8[i].cost_mv + a->l1.me16x8[i].cost_mv + a->l0.me16x8[i].i_ref_cost
                        + a->l1.me16x8[i].i_ref_cost;

        if( h->mb.b_chroma_me )
            i_part_cost_bi += x264_analyse_bi_chroma( h, a, i, PIXEL_16x8 );

        i_part_cost = a->l0.me16x8[i].cost;
        a->i_mb_partition16x8[i] = D_L0_8x8; /* not actually 8x8, only the L0 matters */

        if( a->l1.me16x8[i].cost < i_part_cost )
        {
            i_part_cost = a->l1.me16x8[i].cost;
            a->i_mb_partition16x8[i] = D_L1_8x8;
        }
        if( i_part_cost_bi + a->i_lambda * 1 < i_part_cost )
        {
            i_part_cost = i_part_cost_bi;
            a->i_mb_partition16x8[i] = D_BI_8x8;
        }
        a->i_cost16x8bi += i_part_cost;

        /* Early termination based on the current SATD score of partition[0]
           plus the estimated SATD score of partition[1] */
        if( a->b_early_terminate && (!i && i_part_cost + a->i_cost_est16x8[1] > i_best_satd
            * (16 + (!!a->i_mbrd + !!h->mb.i_psy_rd))/16) )
        {
            a->i_cost16x8bi = COST_MAX;
            return;
        }

        x264_mb_cache_mv_b16x8( h, a, i, 0 );
    }

    /* mb type cost */
    a->i_mb_type16x8 = B_L0_L0
        + (a->i_mb_partition16x8[0]>>2) * 3
        + (a->i_mb_partition16x8[1]>>2);
    a->i_cost16x8bi += a->i_lambda * i_mb_b16x8_cost_table[a->i_mb_type16x8];
}

static void x264_mb_analyse_inter_b8x16( x264_t *h, x264_mb_analysis_t *a, int i_best_satd )
{
    ALIGNED_ARRAY_16( pixel, pix,[2],[8*16] );
    ALIGNED_4( int16_t mvc[3][2] );

    h->mb.i_partition = D_8x16;
    a->i_cost8x16bi = 0;

    for( int i = 0; i < 2; i++ )
    {
        int i_part_cost;
        int i_part_cost_bi = 0;
        int stride[2] = {8,8};
        pixel *src[2];
        x264_me_t m;
        m.i_pixel = PIXEL_8x16;
        LOAD_FENC( &m, h->mb.pic.p_fenc, 8*i, 0 );

        for( int l = 0; l < 2; l++ )
        {
            x264_mb_analysis_list_t *lX = l ? &a->l1 : &a->l0;
            int ref8[2] = { lX->me8x8[i].i_ref, lX->me8x8[i+2].i_ref };
            int i_ref8s = ( ref8[0] == ref8[1] ) ? 1 : 2;
            lX->me8x16[i].cost = INT_MAX;
            for( int j = 0; j < i_ref8s; j++ )
            {
                int i_ref = ref8[j];
                m.i_ref_cost = REF_COST( l, i_ref );

                LOAD_HPELS( &m, h->mb.pic.p_fref[l][i_ref], l, i_ref, 8*i, 0 );

                CP32( mvc[0], lX->mvc[i_ref][0] );
                CP32( mvc[1], lX->mvc[i_ref][i+1] );
                CP32( mvc[2], lX->mvc[i_ref][i+3] );

                x264_macroblock_cache_ref( h, 2*i, 0, 2, 4, l, i_ref );
                x264_mb_predict_mv( h, l, 4*i, 2, m.mvp );
                x264_me_search( h, &m, mvc, 3 );
                m.cost += m.i_ref_cost;

                if( m.cost < lX->me8x16[i].cost )
                    h->mc.memcpy_aligned( &lX->me8x16[i], &m, sizeof(x264_me_t) );
            }
        }

        /* BI mode */
        src[0] = h->mc.get_ref( pix[0], &stride[0], a->l0.me8x16[i].p_fref, a->l0.me8x16[i].i_stride[0],
                                a->l0.me8x16[i].mv[0], a->l0.me8x16[i].mv[1], 8, 16, x264_weight_none );
        src[1] = h->mc.get_ref( pix[1], &stride[1], a->l1.me8x16[i].p_fref, a->l1.me8x16[i].i_stride[0],
                                a->l1.me8x16[i].mv[0], a->l1.me8x16[i].mv[1], 8, 16, x264_weight_none );
        h->mc.avg[PIXEL_8x16]( pix[0], 8, src[0], stride[0], src[1], stride[1], h->mb.bipred_weight[a->l0.me8x16[i].i_ref][a->l1.me8x16[i].i_ref] );

        i_part_cost_bi = h->pixf.mbcmp[PIXEL_8x16]( a->l0.me8x16[i].p_fenc[0], FENC_STRIDE, pix[0], 8 )
                        + a->l0.me8x16[i].cost_mv + a->l1.me8x16[i].cost_mv + a->l0.me8x16[i].i_ref_cost
                        + a->l1.me8x16[i].i_ref_cost;

        if( h->mb.b_chroma_me )
            i_part_cost_bi += x264_analyse_bi_chroma( h, a, i, PIXEL_8x16 );

        i_part_cost = a->l0.me8x16[i].cost;
        a->i_mb_partition8x16[i] = D_L0_8x8;

        if( a->l1.me8x16[i].cost < i_part_cost )
        {
            i_part_cost = a->l1.me8x16[i].cost;
            a->i_mb_partition8x16[i] = D_L1_8x8;
        }
        if( i_part_cost_bi + a->i_lambda * 1 < i_part_cost )
        {
            i_part_cost = i_part_cost_bi;
            a->i_mb_partition8x16[i] = D_BI_8x8;
        }
        a->i_cost8x16bi += i_part_cost;

        /* Early termination based on the current SATD score of partition[0]
           plus the estimated SATD score of partition[1] */
        if( a->b_early_terminate && (!i && i_part_cost + a->i_cost_est8x16[1] > i_best_satd
            * (16 + (!!a->i_mbrd + !!h->mb.i_psy_rd))/16) )
        {
            a->i_cost8x16bi = COST_MAX;
            return;
        }

        x264_mb_cache_mv_b8x16( h, a, i, 0 );
    }

    /* mb type cost */
    a->i_mb_type8x16 = B_L0_L0
        + (a->i_mb_partition8x16[0]>>2) * 3
        + (a->i_mb_partition8x16[1]>>2);
    a->i_cost8x16bi += a->i_lambda * i_mb_b16x8_cost_table[a->i_mb_type8x16];
}

static void x264_mb_analyse_p_rd( x264_t *h, x264_mb_analysis_t *a, int i_satd )
{
    int thresh = a->b_early_terminate ? i_satd * 5/4 + 1 : COST_MAX;

    h->mb.i_type = P_L0;
    if( a->l0.i_rd16x16 == COST_MAX && (!a->b_early_terminate || a->l0.me16x16.cost <= i_satd * 3/2) )
    {
        h->mb.i_partition = D_16x16;
        x264_analyse_update_cache( h, a );
        a->l0.i_rd16x16 = x264_rd_cost_mb( h, a->i_lambda2 );
    }

    if( a->l0.i_cost16x8 < thresh )
    {
        h->mb.i_partition = D_16x8;
        x264_analyse_update_cache( h, a );
        a->l0.i_cost16x8 = x264_rd_cost_mb( h, a->i_lambda2 );
    }
    else
        a->l0.i_cost16x8 = COST_MAX;

    if( a->l0.i_cost8x16 < thresh )
    {
        h->mb.i_partition = D_8x16;
        x264_analyse_update_cache( h, a );
        a->l0.i_cost8x16 = x264_rd_cost_mb( h, a->i_lambda2 );
    }
    else
        a->l0.i_cost8x16 = COST_MAX;

    if( a->l0.i_cost8x8 < thresh )
    {
        h->mb.i_type = P_8x8;
        h->mb.i_partition = D_8x8;
        if( h->param.analyse.inter & X264_ANALYSE_PSUB8x8 )
        {
            x264_macroblock_cache_ref( h, 0, 0, 2, 2, 0, a->l0.me8x8[0].i_ref );
            x264_macroblock_cache_ref( h, 2, 0, 2, 2, 0, a->l0.me8x8[1].i_ref );
            x264_macroblock_cache_ref( h, 0, 2, 2, 2, 0, a->l0.me8x8[2].i_ref );
            x264_macroblock_cache_ref( h, 2, 2, 2, 2, 0, a->l0.me8x8[3].i_ref );
            /* FIXME: In the 8x8 blocks where RDO isn't run, the NNZ values used for context selection
             * for future blocks are those left over from previous RDO calls. */
            for( int i = 0; i < 4; i++ )
            {
                int costs[4] = {a->l0.i_cost4x4[i], a->l0.i_cost8x4[i], a->l0.i_cost4x8[i], a->l0.me8x8[i].cost};
                int sub8x8_thresh = a->b_early_terminate ? X264_MIN4( costs[0], costs[1], costs[2], costs[3] ) * 5 / 4 : COST_MAX;
                int subtype, btype = D_L0_8x8;
                uint64_t bcost = COST_MAX64;
                for( subtype = D_L0_4x4; subtype <= D_L0_8x8; subtype++ )
                {
                    uint64_t cost;
                    if( costs[subtype] > sub8x8_thresh )
                        continue;
                    h->mb.i_sub_partition[i] = subtype;
                    x264_mb_cache_mv_p8x8( h, a, i );
                    if( subtype == btype )
                        continue;
                    cost = x264_rd_cost_part( h, a->i_lambda2, i<<2, PIXEL_8x8 );
                    COPY2_IF_LT( bcost, cost, btype, subtype );
                }
                if( h->mb.i_sub_partition[i] != btype )
                {
                    h->mb.i_sub_partition[i] = btype;
                    x264_mb_cache_mv_p8x8( h, a, i );
                }
            }
        }
        else
            x264_analyse_update_cache( h, a );
        a->l0.i_cost8x8 = x264_rd_cost_mb( h, a->i_lambda2 );
    }
    else
        a->l0.i_cost8x8 = COST_MAX;
}

static void x264_mb_analyse_b_rd( x264_t *h, x264_mb_analysis_t *a, int i_satd_inter )
{
    int thresh = a->b_early_terminate ? i_satd_inter * (17 + (!!h->mb.i_psy_rd))/16 + 1 : COST_MAX;

    if( a->b_direct_available && a->i_rd16x16direct == COST_MAX )
    {
        h->mb.i_type = B_DIRECT;
        /* Assumes direct/skip MC is still in fdec */
        /* Requires b-rdo to be done before intra analysis */
        h->mb.b_skip_mc = 1;
        x264_analyse_update_cache( h, a );
        a->i_rd16x16direct = x264_rd_cost_mb( h, a->i_lambda2 );
        h->mb.b_skip_mc = 0;
    }

    //FIXME not all the update_cache calls are needed
    h->mb.i_partition = D_16x16;
    /* L0 */
    if( a->l0.me16x16.cost < thresh && a->l0.i_rd16x16 == COST_MAX )
    {
        h->mb.i_type = B_L0_L0;
        x264_analyse_update_cache( h, a );
        a->l0.i_rd16x16 = x264_rd_cost_mb( h, a->i_lambda2 );
    }

    /* L1 */
    if( a->l1.me16x16.cost < thresh && a->l1.i_rd16x16 == COST_MAX )
    {
        h->mb.i_type = B_L1_L1;
        x264_analyse_update_cache( h, a );
        a->l1.i_rd16x16 = x264_rd_cost_mb( h, a->i_lambda2 );
    }

    /* BI */
    if( a->i_cost16x16bi < thresh && a->i_rd16x16bi == COST_MAX )
    {
        h->mb.i_type = B_BI_BI;
        x264_analyse_update_cache( h, a );
        a->i_rd16x16bi = x264_rd_cost_mb( h, a->i_lambda2 );
    }

    /* 8x8 */
    if( a->i_cost8x8bi < thresh && a->i_rd8x8bi == COST_MAX )
    {
        h->mb.i_type = B_8x8;
        h->mb.i_partition = D_8x8;
        x264_analyse_update_cache( h, a );
        a->i_rd8x8bi = x264_rd_cost_mb( h, a->i_lambda2 );
        x264_macroblock_cache_skip( h, 0, 0, 4, 4, 0 );
    }

    /* 16x8 */
    if( a->i_cost16x8bi < thresh && a->i_rd16x8bi == COST_MAX )
    {
        h->mb.i_type = a->i_mb_type16x8;
        h->mb.i_partition = D_16x8;
        x264_analyse_update_cache( h, a );
        a->i_rd16x8bi = x264_rd_cost_mb( h, a->i_lambda2 );
    }

    /* 8x16 */
    if( a->i_cost8x16bi < thresh && a->i_rd8x16bi == COST_MAX )
    {
        h->mb.i_type = a->i_mb_type8x16;
        h->mb.i_partition = D_8x16;
        x264_analyse_update_cache( h, a );
        a->i_rd8x16bi = x264_rd_cost_mb( h, a->i_lambda2 );
    }
}

static void x264_refine_bidir( x264_t *h, x264_mb_analysis_t *a )
{
    int i_biweight;

    if( IS_INTRA(h->mb.i_type) )
        return;

    switch( h->mb.i_partition )
    {
        case D_16x16:
            if( h->mb.i_type == B_BI_BI )
            {
                i_biweight = h->mb.bipred_weight[a->l0.bi16x16.i_ref][a->l1.bi16x16.i_ref];
                x264_me_refine_bidir_satd( h, &a->l0.bi16x16, &a->l1.bi16x16, i_biweight );
            }
            break;
        case D_16x8:
            for( int i = 0; i < 2; i++ )
                if( a->i_mb_partition16x8[i] == D_BI_8x8 )
                {
                    i_biweight = h->mb.bipred_weight[a->l0.me16x8[i].i_ref][a->l1.me16x8[i].i_ref];
                    x264_me_refine_bidir_satd( h, &a->l0.me16x8[i], &a->l1.me16x8[i], i_biweight );
                }
            break;
        case D_8x16:
            for( int i = 0; i < 2; i++ )
                if( a->i_mb_partition8x16[i] == D_BI_8x8 )
                {
                    i_biweight = h->mb.bipred_weight[a->l0.me8x16[i].i_ref][a->l1.me8x16[i].i_ref];
                    x264_me_refine_bidir_satd( h, &a->l0.me8x16[i], &a->l1.me8x16[i], i_biweight );
                }
            break;
        case D_8x8:
            for( int i = 0; i < 4; i++ )
                if( h->mb.i_sub_partition[i] == D_BI_8x8 )
                {
                    i_biweight = h->mb.bipred_weight[a->l0.me8x8[i].i_ref][a->l1.me8x8[i].i_ref];
                    x264_me_refine_bidir_satd( h, &a->l0.me8x8[i], &a->l1.me8x8[i], i_biweight );
                }
            break;
    }
}

static inline void x264_mb_analyse_transform( x264_t *h )
{
    if( x264_mb_transform_8x8_allowed( h ) && h->param.analyse.b_transform_8x8 && !h->mb.b_lossless )
    {
        /* Only luma MC is really needed for 4:2:0, but the full MC is re-used in macroblock_encode. */
        x264_mb_mc( h );

        int plane_count = CHROMA444 && h->mb.b_chroma_me ? 3 : 1;
        int i_cost8 = 0, i_cost4 = 0;
        for( int p = 0; p < plane_count; p++ )
        {
            i_cost8 += h->pixf.sa8d[PIXEL_16x16]( h->mb.pic.p_fenc[p], FENC_STRIDE,
                                                  h->mb.pic.p_fdec[p], FDEC_STRIDE );
            i_cost4 += h->pixf.satd[PIXEL_16x16]( h->mb.pic.p_fenc[p], FENC_STRIDE,
                                                  h->mb.pic.p_fdec[p], FDEC_STRIDE );
        }

        h->mb.b_transform_8x8 = i_cost8 < i_cost4;
        h->mb.b_skip_mc = 1;
    }
}

static inline void x264_mb_analyse_transform_rd( x264_t *h, x264_mb_analysis_t *a, int *i_satd, int *i_rd )
{
    if( x264_mb_transform_8x8_allowed( h ) && h->param.analyse.b_transform_8x8 )
    {
        x264_analyse_update_cache( h, a );
        h->mb.b_transform_8x8 ^= 1;
        /* FIXME only luma is needed for 4:2:0, but the score for comparison already includes chroma */
        int i_rd8 = x264_rd_cost_mb( h, a->i_lambda2 );

        if( *i_rd >= i_rd8 )
        {
            if( *i_rd > 0 )
                *i_satd = (int64_t)(*i_satd) * i_rd8 / *i_rd;
            *i_rd = i_rd8;
        }
        else
            h->mb.b_transform_8x8 ^= 1;
    }
}

/* Rate-distortion optimal QP selection.
 * FIXME: More than half of the benefit of this function seems to be
 * in the way it improves the coding of chroma DC (by decimating or
 * finding a better way to code a single DC coefficient.)
 * There must be a more efficient way to get that portion of the benefit
 * without doing full QP-RD, but RD-decimation doesn't seem to do the
 * trick. */
static inline void x264_mb_analyse_qp_rd( x264_t *h, x264_mb_analysis_t *a )
{
    int bcost, cost, failures, prevcost, origcost;
    int orig_qp = h->mb.i_qp, bqp = h->mb.i_qp;
    int last_qp_tried = 0;
    origcost = bcost = x264_rd_cost_mb( h, a->i_lambda2 );
    int origcbp = h->mb.cbp[h->mb.i_mb_xy];

    /* If CBP is already zero, don't raise the quantizer any higher. */
    for( int direction = origcbp ? 1 : -1; direction >= -1; direction-=2 )
    {
        /* Without psy-RD, require monotonicity when moving quant away from previous
         * macroblock's quant; allow 1 failure when moving quant towards previous quant.
         * With psy-RD, allow 1 failure when moving quant away from previous quant,
         * allow 2 failures when moving quant towards previous quant.
         * Psy-RD generally seems to result in more chaotic RD score-vs-quantizer curves. */
        int threshold = (!!h->mb.i_psy_rd);
        /* Raise the threshold for failures if we're moving towards the last QP. */
        if( ( h->mb.i_last_qp < orig_qp && direction == -1 ) ||
            ( h->mb.i_last_qp > orig_qp && direction ==  1 ) )
            threshold++;
        h->mb.i_qp = orig_qp;
        failures = 0;
        prevcost = origcost;

        /* If the current QP results in an empty CBP, it's highly likely that lower QPs
         * (up to a point) will too.  So, jump down to where the threshold will kick in
         * and check the QP there.  If the CBP is still empty, skip the main loop.
         * If it isn't empty, we would have ended up having to check this QP anyways,
         * so as long as we store it for later lookup, we lose nothing. */
        int already_checked_qp = -1;
        int already_checked_cost = COST_MAX;
        if( direction == -1 )
        {
            if( !origcbp )
            {
                h->mb.i_qp = X264_MAX( h->mb.i_qp - threshold - 1, h->param.rc.i_qp_min );
                h->mb.i_chroma_qp = h->chroma_qp_table[h->mb.i_qp];
                already_checked_cost = x264_rd_cost_mb( h, a->i_lambda2 );
                if( !h->mb.cbp[h->mb.i_mb_xy] )
                {
                    /* If our empty-CBP block is lower QP than the last QP,
                     * the last QP almost surely doesn't have a CBP either. */
                    if( h->mb.i_last_qp > h->mb.i_qp )
                        last_qp_tried = 1;
                    break;
                }
                already_checked_qp = h->mb.i_qp;
                h->mb.i_qp = orig_qp;
            }
        }

        h->mb.i_qp += direction;
        while( h->mb.i_qp >= h->param.rc.i_qp_min && h->mb.i_qp <= SPEC_QP( h->param.rc.i_qp_max ) )
        {
            if( h->mb.i_last_qp == h->mb.i_qp )
                last_qp_tried = 1;
            if( h->mb.i_qp == already_checked_qp )
                cost = already_checked_cost;
            else
            {
                h->mb.i_chroma_qp = h->chroma_qp_table[h->mb.i_qp];
                cost = x264_rd_cost_mb( h, a->i_lambda2 );
                COPY2_IF_LT( bcost, cost, bqp, h->mb.i_qp );
            }

            /* We can't assume that the costs are monotonic over QPs.
             * Tie case-as-failure seems to give better results. */
            if( cost < prevcost )
                failures = 0;
            else
                failures++;
            prevcost = cost;

            if( failures > threshold )
                break;
            if( direction == 1 && !h->mb.cbp[h->mb.i_mb_xy] )
                break;
            h->mb.i_qp += direction;
        }
    }

    /* Always try the last block's QP. */
    if( !last_qp_tried )
    {
        h->mb.i_qp = h->mb.i_last_qp;
        h->mb.i_chroma_qp = h->chroma_qp_table[h->mb.i_qp];
        cost = x264_rd_cost_mb( h, a->i_lambda2 );
        COPY2_IF_LT( bcost, cost, bqp, h->mb.i_qp );
    }

    h->mb.i_qp = bqp;
    h->mb.i_chroma_qp = h->chroma_qp_table[h->mb.i_qp];

    /* Check transform again; decision from before may no longer be optimal. */
    if( h->mb.i_qp != orig_qp && h->param.analyse.b_transform_8x8 &&
        x264_mb_transform_8x8_allowed( h ) )
    {
        h->mb.b_transform_8x8 ^= 1;
        cost = x264_rd_cost_mb( h, a->i_lambda2 );
        if( cost > bcost )
            h->mb.b_transform_8x8 ^= 1;
    }
}

/*****************************************************************************
 * x264_macroblock_analyse:
 *****************************************************************************/
void x264_macroblock_analyse( x264_t *h )
{
    x264_mb_analysis_t analysis;
    int i_cost = COST_MAX;

    h->mb.i_qp = x264_ratecontrol_mb_qp( h );
    /* If the QP of this MB is within 1 of the previous MB, code the same QP as the previous MB,
     * to lower the bit cost of the qp_delta.  Don't do this if QPRD is enabled. */
    if( h->param.rc.i_aq_mode && h->param.analyse.i_subpel_refine < 10 && abs(h->mb.i_qp - h->mb.i_last_qp) == 1 )
        h->mb.i_qp = h->mb.i_last_qp;

    x264_mb_analyse_init( h, &analysis, h->mb.i_qp );

    /*--------------------------- Do the analysis ---------------------------*/
    if( h->sh.i_type == SLICE_TYPE_I )
    {
intra_analysis:
        if( analysis.i_mbrd )
            x264_mb_init_fenc_cache( h, analysis.i_mbrd >= 2 );
        x264_mb_analyse_intra( h, &analysis, COST_MAX );
        if( analysis.i_mbrd )
            x264_intra_rd( h, &analysis, COST_MAX );

        i_cost = analysis.i_satd_i16x16;
        h->mb.i_type = I_16x16;
        COPY2_IF_LT( i_cost, analysis.i_satd_i4x4, h->mb.i_type, I_4x4 );
        COPY2_IF_LT( i_cost, analysis.i_satd_i8x8, h->mb.i_type, I_8x8 );
        if( analysis.i_satd_pcm < i_cost )
            h->mb.i_type = I_PCM;

        else if( analysis.i_mbrd >= 2 )
            x264_intra_rd_refine( h, &analysis );
    }
    else if( h->sh.i_type == SLICE_TYPE_P )
    {
        int b_skip = 0;

        h->mc.prefetch_ref( h->mb.pic.p_fref[0][0][h->mb.i_mb_x&3], h->mb.pic.i_stride[0], 0 );

        analysis.b_try_skip = 0;
        if( analysis.b_force_intra )
        {
            if( !h->param.analyse.b_psy )
            {
                x264_mb_analyse_init_qp( h, &analysis, X264_MAX( h->mb.i_qp - h->mb.ip_offset, h->param.rc.i_qp_min ) );
                goto intra_analysis;
            }
        }
        else
        {
            int skip_invalid = h->i_thread_frames > 1 && h->mb.cache.pskip_mv[1] > h->mb.mv_max_spel[1];
            /* If the current macroblock is off the frame, just skip it. */
            if( HAVE_INTERLACED && !MB_INTERLACED && h->mb.i_mb_y * 16 >= h->param.i_height && !skip_invalid )
                b_skip = 1;
            /* Fast P_SKIP detection */
            else if( h->param.analyse.b_fast_pskip )
            {
                if( skip_invalid )
                    // FIXME don't need to check this if the reference frame is done
                    {}
                else if( h->param.analyse.i_subpel_refine >= 3 )
                    analysis.b_try_skip = 1;
                else if( h->mb.i_mb_type_left[0] == P_SKIP ||
                         h->mb.i_mb_type_top == P_SKIP ||
                         h->mb.i_mb_type_topleft == P_SKIP ||
                         h->mb.i_mb_type_topright == P_SKIP )
                    b_skip = x264_macroblock_probe_pskip( h );
            }
        }

        h->mc.prefetch_ref( h->mb.pic.p_fref[0][0][h->mb.i_mb_x&3], h->mb.pic.i_stride[0], 1 );

        if( b_skip )
        {
            h->mb.i_type = P_SKIP;
            h->mb.i_partition = D_16x16;
            assert( h->mb.cache.pskip_mv[1] <= h->mb.mv_max_spel[1] || h->i_thread_frames == 1 );
            /* Set up MVs for future predictors */
            for( int i = 0; i < h->mb.pic.i_fref[0]; i++ )
                M32( h->mb.mvr[0][i][h->mb.i_mb_xy] ) = 0;
        }
        else
        {
            const unsigned int flags = h->param.analyse.inter;
            int i_type;
            int i_partition;
            int i_satd_inter, i_satd_intra;

            x264_mb_analyse_load_costs( h, &analysis );

            x264_mb_analyse_inter_p16x16( h, &analysis );

            if( h->mb.i_type == P_SKIP )
            {
                for( int i = 1; i < h->mb.pic.i_fref[0]; i++ )
                    M32( h->mb.mvr[0][i][h->mb.i_mb_xy] ) = 0;
                return;
            }

            if( flags & X264_ANALYSE_PSUB16x16 )
            {
                if( h->param.analyse.b_mixed_references )
                    x264_mb_analyse_inter_p8x8_mixed_ref( h, &analysis );
                else
                    x264_mb_analyse_inter_p8x8( h, &analysis );
            }

            /* Select best inter mode */
            i_type = P_L0;
            i_partition = D_16x16;
            i_cost = analysis.l0.me16x16.cost;

            if( ( flags & X264_ANALYSE_PSUB16x16 ) && (!analysis.b_early_terminate ||
                analysis.l0.i_cost8x8 < analysis.l0.me16x16.cost) )
            {
                i_type = P_8x8;
                i_partition = D_8x8;
                i_cost = analysis.l0.i_cost8x8;

                /* Do sub 8x8 */
                if( flags & X264_ANALYSE_PSUB8x8 )
                {
                    for( int i = 0; i < 4; i++ )
                    {
                        x264_mb_analyse_inter_p4x4( h, &analysis, i );
                        int i_thresh8x4 = analysis.l0.me4x4[i][1].cost_mv + analysis.l0.me4x4[i][2].cost_mv;
                        if( !analysis.b_early_terminate || analysis.l0.i_cost4x4[i] < analysis.l0.me8x8[i].cost + i_thresh8x4 )
                        {
                            int i_cost8x8 = analysis.l0.i_cost4x4[i];
                            h->mb.i_sub_partition[i] = D_L0_4x4;

                            x264_mb_analyse_inter_p8x4( h, &analysis, i );
                            COPY2_IF_LT( i_cost8x8, analysis.l0.i_cost8x4[i],
                                         h->mb.i_sub_partition[i], D_L0_8x4 );

                            x264_mb_analyse_inter_p4x8( h, &analysis, i );
                            COPY2_IF_LT( i_cost8x8, analysis.l0.i_cost4x8[i],
                                         h->mb.i_sub_partition[i], D_L0_4x8 );

                            i_cost += i_cost8x8 - analysis.l0.me8x8[i].cost;
                        }
                        x264_mb_cache_mv_p8x8( h, &analysis, i );
                    }
                    analysis.l0.i_cost8x8 = i_cost;
                }
            }

            /* Now do 16x8/8x16 */
            int i_thresh16x8 = analysis.l0.me8x8[1].cost_mv + analysis.l0.me8x8[2].cost_mv;
            if( ( flags & X264_ANALYSE_PSUB16x16 ) && (!analysis.b_early_terminate ||
                analysis.l0.i_cost8x8 < analysis.l0.me16x16.cost + i_thresh16x8) )
            {
                int i_avg_mv_ref_cost = (analysis.l0.me8x8[2].cost_mv + analysis.l0.me8x8[2].i_ref_cost
                                      + analysis.l0.me8x8[3].cost_mv + analysis.l0.me8x8[3].i_ref_cost + 1) >> 1;
                analysis.i_cost_est16x8[1] = analysis.i_satd8x8[0][2] + analysis.i_satd8x8[0][3] + i_avg_mv_ref_cost;

                x264_mb_analyse_inter_p16x8( h, &analysis, i_cost );
                COPY3_IF_LT( i_cost, analysis.l0.i_cost16x8, i_type, P_L0, i_partition, D_16x8 );

                i_avg_mv_ref_cost = (analysis.l0.me8x8[1].cost_mv + analysis.l0.me8x8[1].i_ref_cost
                                  + analysis.l0.me8x8[3].cost_mv + analysis.l0.me8x8[3].i_ref_cost + 1) >> 1;
                analysis.i_cost_est8x16[1] = analysis.i_satd8x8[0][1] + analysis.i_satd8x8[0][3] + i_avg_mv_ref_cost;

                x264_mb_analyse_inter_p8x16( h, &analysis, i_cost );
                COPY3_IF_LT( i_cost, analysis.l0.i_cost8x16, i_type, P_L0, i_partition, D_8x16 );
            }

            h->mb.i_partition = i_partition;

            /* refine qpel */
            //FIXME mb_type costs?
            if( analysis.i_mbrd || !h->mb.i_subpel_refine )
            {
                /* refine later */
            }
            else if( i_partition == D_16x16 )
            {
                x264_me_refine_qpel( h, &analysis.l0.me16x16 );
                i_cost = analysis.l0.me16x16.cost;
            }
            else if( i_partition == D_16x8 )
            {
                x264_me_refine_qpel( h, &analysis.l0.me16x8[0] );
                x264_me_refine_qpel( h, &analysis.l0.me16x8[1] );
                i_cost = analysis.l0.me16x8[0].cost + analysis.l0.me16x8[1].cost;
            }
            else if( i_partition == D_8x16 )
            {
                x264_me_refine_qpel( h, &analysis.l0.me8x16[0] );
                x264_me_refine_qpel( h, &analysis.l0.me8x16[1] );
                i_cost = analysis.l0.me8x16[0].cost + analysis.l0.me8x16[1].cost;
            }
            else if( i_partition == D_8x8 )
            {
                i_cost = 0;
                for( int i8x8 = 0; i8x8 < 4; i8x8++ )
                {
                    switch( h->mb.i_sub_partition[i8x8] )
                    {
                        case D_L0_8x8:
                            x264_me_refine_qpel( h, &analysis.l0.me8x8[i8x8] );
                            i_cost += analysis.l0.me8x8[i8x8].cost;
                            break;
                        case D_L0_8x4:
                            x264_me_refine_qpel( h, &analysis.l0.me8x4[i8x8][0] );
                            x264_me_refine_qpel( h, &analysis.l0.me8x4[i8x8][1] );
                            i_cost += analysis.l0.me8x4[i8x8][0].cost +
                                      analysis.l0.me8x4[i8x8][1].cost;
                            break;
                        case D_L0_4x8:
                            x264_me_refine_qpel( h, &analysis.l0.me4x8[i8x8][0] );
                            x264_me_refine_qpel( h, &analysis.l0.me4x8[i8x8][1] );
                            i_cost += analysis.l0.me4x8[i8x8][0].cost +
                                      analysis.l0.me4x8[i8x8][1].cost;
                            break;

                        case D_L0_4x4:
                            x264_me_refine_qpel( h, &analysis.l0.me4x4[i8x8][0] );
                            x264_me_refine_qpel( h, &analysis.l0.me4x4[i8x8][1] );
                            x264_me_refine_qpel( h, &analysis.l0.me4x4[i8x8][2] );
                            x264_me_refine_qpel( h, &analysis.l0.me4x4[i8x8][3] );
                            i_cost += analysis.l0.me4x4[i8x8][0].cost +
                                      analysis.l0.me4x4[i8x8][1].cost +
                                      analysis.l0.me4x4[i8x8][2].cost +
                                      analysis.l0.me4x4[i8x8][3].cost;
                            break;
                        default:
                            x264_log( h, X264_LOG_ERROR, "internal error (!8x8 && !4x4)\n" );
                            break;
                    }
                }
            }

            if( h->mb.b_chroma_me )
            {
                if( CHROMA444 )
                {
                    x264_mb_analyse_intra( h, &analysis, i_cost );
                    x264_mb_analyse_intra_chroma( h, &analysis );
                }
                else
                {
                    x264_mb_analyse_intra_chroma( h, &analysis );
                    x264_mb_analyse_intra( h, &analysis, i_cost - analysis.i_satd_chroma );
                }
                analysis.i_satd_i16x16 += analysis.i_satd_chroma;
                analysis.i_satd_i8x8   += analysis.i_satd_chroma;
                analysis.i_satd_i4x4   += analysis.i_satd_chroma;
            }
            else
                x264_mb_analyse_intra( h, &analysis, i_cost );

            i_satd_inter = i_cost;
            i_satd_intra = X264_MIN3( analysis.i_satd_i16x16,
                                      analysis.i_satd_i8x8,
                                      analysis.i_satd_i4x4 );

            if( analysis.i_mbrd )
            {
                x264_mb_analyse_p_rd( h, &analysis, X264_MIN(i_satd_inter, i_satd_intra) );
                i_type = P_L0;
                i_partition = D_16x16;
                i_cost = analysis.l0.i_rd16x16;
                COPY2_IF_LT( i_cost, analysis.l0.i_cost16x8, i_partition, D_16x8 );
                COPY2_IF_LT( i_cost, analysis.l0.i_cost8x16, i_partition, D_8x16 );
                COPY3_IF_LT( i_cost, analysis.l0.i_cost8x8, i_partition, D_8x8, i_type, P_8x8 );
                h->mb.i_type = i_type;
                h->mb.i_partition = i_partition;
                if( i_cost < COST_MAX )
                    x264_mb_analyse_transform_rd( h, &analysis, &i_satd_inter, &i_cost );
                x264_intra_rd( h, &analysis, i_satd_inter * 5/4 + 1 );
            }

            COPY2_IF_LT( i_cost, analysis.i_satd_i16x16, i_type, I_16x16 );
            COPY2_IF_LT( i_cost, analysis.i_satd_i8x8, i_type, I_8x8 );
            COPY2_IF_LT( i_cost, analysis.i_satd_i4x4, i_type, I_4x4 );
            COPY2_IF_LT( i_cost, analysis.i_satd_pcm, i_type, I_PCM );

            h->mb.i_type = i_type;

            if( analysis.b_force_intra && !IS_INTRA(i_type) )
            {
                /* Intra masking: copy fdec to fenc and re-encode the block as intra in order to make it appear as if
                 * it was an inter block. */
                x264_analyse_update_cache( h, &analysis );
                x264_macroblock_encode( h );
                for( int p = 0; p < (CHROMA444 ? 3 : 1); p++ )
                    h->mc.copy[PIXEL_16x16]( h->mb.pic.p_fenc[p], FENC_STRIDE, h->mb.pic.p_fdec[p], FDEC_STRIDE, 16 );
                if( !CHROMA444 )
                {
                    int height = 16 >> CHROMA_V_SHIFT;
                    h->mc.copy[PIXEL_8x8]  ( h->mb.pic.p_fenc[1], FENC_STRIDE, h->mb.pic.p_fdec[1], FDEC_STRIDE, height );
                    h->mc.copy[PIXEL_8x8]  ( h->mb.pic.p_fenc[2], FENC_STRIDE, h->mb.pic.p_fdec[2], FDEC_STRIDE, height );
                }
                x264_mb_analyse_init_qp( h, &analysis, X264_MAX( h->mb.i_qp - h->mb.ip_offset, h->param.rc.i_qp_min ) );
                goto intra_analysis;
            }

            if( analysis.i_mbrd >= 2 && h->mb.i_type != I_PCM )
            {
                if( IS_INTRA( h->mb.i_type ) )
                {
                    x264_intra_rd_refine( h, &analysis );
                }
                else if( i_partition == D_16x16 )
                {
                    x264_macroblock_cache_ref( h, 0, 0, 4, 4, 0, analysis.l0.me16x16.i_ref );
                    analysis.l0.me16x16.cost = i_cost;
                    x264_me_refine_qpel_rd( h, &analysis.l0.me16x16, analysis.i_lambda2, 0, 0 );
                }
                else if( i_partition == D_16x8 )
                {
                    h->mb.i_sub_partition[0] = h->mb.i_sub_partition[1] =
                    h->mb.i_sub_partition[2] = h->mb.i_sub_partition[3] = D_L0_8x8;
                    x264_macroblock_cache_ref( h, 0, 0, 4, 2, 0, analysis.l0.me16x8[0].i_ref );
                    x264_macroblock_cache_ref( h, 0, 2, 4, 2, 0, analysis.l0.me16x8[1].i_ref );
                    x264_me_refine_qpel_rd( h, &analysis.l0.me16x8[0], analysis.i_lambda2, 0, 0 );
                    x264_me_refine_qpel_rd( h, &analysis.l0.me16x8[1], analysis.i_lambda2, 8, 0 );
                }
                else if( i_partition == D_8x16 )
                {
                    h->mb.i_sub_partition[0] = h->mb.i_sub_partition[1] =
                    h->mb.i_sub_partition[2] = h->mb.i_sub_partition[3] = D_L0_8x8;
                    x264_macroblock_cache_ref( h, 0, 0, 2, 4, 0, analysis.l0.me8x16[0].i_ref );
                    x264_macroblock_cache_ref( h, 2, 0, 2, 4, 0, analysis.l0.me8x16[1].i_ref );
                    x264_me_refine_qpel_rd( h, &analysis.l0.me8x16[0], analysis.i_lambda2, 0, 0 );
                    x264_me_refine_qpel_rd( h, &analysis.l0.me8x16[1], analysis.i_lambda2, 4, 0 );
                }
                else if( i_partition == D_8x8 )
                {
                    x264_analyse_update_cache( h, &analysis );
                    for( int i8x8 = 0; i8x8 < 4; i8x8++ )
                    {
                        if( h->mb.i_sub_partition[i8x8] == D_L0_8x8 )
                        {
                            x264_me_refine_qpel_rd( h, &analysis.l0.me8x8[i8x8], analysis.i_lambda2, i8x8*4, 0 );
                        }
                        else if( h->mb.i_sub_partition[i8x8] == D_L0_8x4 )
                        {
                            x264_me_refine_qpel_rd( h, &analysis.l0.me8x4[i8x8][0], analysis.i_lambda2, i8x8*4+0, 0 );
                            x264_me_refine_qpel_rd( h, &analysis.l0.me8x4[i8x8][1], analysis.i_lambda2, i8x8*4+2, 0 );
                        }
                        else if( h->mb.i_sub_partition[i8x8] == D_L0_4x8 )
                        {
                            x264_me_refine_qpel_rd( h, &analysis.l0.me4x8[i8x8][0], analysis.i_lambda2, i8x8*4+0, 0 );
                            x264_me_refine_qpel_rd( h, &analysis.l0.me4x8[i8x8][1], analysis.i_lambda2, i8x8*4+1, 0 );
                        }
                        else if( h->mb.i_sub_partition[i8x8] == D_L0_4x4 )
                        {
                            x264_me_refine_qpel_rd( h, &analysis.l0.me4x4[i8x8][0], analysis.i_lambda2, i8x8*4+0, 0 );
                            x264_me_refine_qpel_rd( h, &analysis.l0.me4x4[i8x8][1], analysis.i_lambda2, i8x8*4+1, 0 );
                            x264_me_refine_qpel_rd( h, &analysis.l0.me4x4[i8x8][2], analysis.i_lambda2, i8x8*4+2, 0 );
                            x264_me_refine_qpel_rd( h, &analysis.l0.me4x4[i8x8][3], analysis.i_lambda2, i8x8*4+3, 0 );
                        }
                    }
                }
            }
        }
    }
    else if( h->sh.i_type == SLICE_TYPE_B )
    {
        int i_bskip_cost = COST_MAX;
        int b_skip = 0;

        if( analysis.i_mbrd )
            x264_mb_init_fenc_cache( h, analysis.i_mbrd >= 2 );

        h->mb.i_type = B_SKIP;
        if( h->mb.b_direct_auto_write )
        {
            /* direct=auto heuristic: prefer whichever mode allows more Skip macroblocks */
            for( int i = 0; i < 2; i++ )
            {
                int b_changed = 1;
                h->sh.b_direct_spatial_mv_pred ^= 1;
                analysis.b_direct_available = x264_mb_predict_mv_direct16x16( h, i && analysis.b_direct_available ? &b_changed : NULL );
                if( analysis.b_direct_available )
                {
                    if( b_changed )
                    {
                        x264_mb_mc( h );
                        b_skip = x264_macroblock_probe_bskip( h );
                    }
                    h->stat.frame.i_direct_score[ h->sh.b_direct_spatial_mv_pred ] += b_skip;
                }
                else
                    b_skip = 0;
            }
        }
        else
            analysis.b_direct_available = x264_mb_predict_mv_direct16x16( h, NULL );

        analysis.b_try_skip = 0;
        if( analysis.b_direct_available )
        {
            if( !h->mb.b_direct_auto_write )
                x264_mb_mc( h );
            /* If the current macroblock is off the frame, just skip it. */
            if( HAVE_INTERLACED && !MB_INTERLACED && h->mb.i_mb_y * 16 >= h->param.i_height )
                b_skip = 1;
            else if( analysis.i_mbrd )
            {
                i_bskip_cost = ssd_mb( h );
                /* 6 = minimum cavlc cost of a non-skipped MB */
                b_skip = h->mb.b_skip_mc = i_bskip_cost <= ((6 * analysis.i_lambda2 + 128) >> 8);
            }
            else if( !h->mb.b_direct_auto_write )
            {
                /* Conditioning the probe on neighboring block types
                 * doesn't seem to help speed or quality. */
                analysis.b_try_skip = x264_macroblock_probe_bskip( h );
                if( h->param.analyse.i_subpel_refine < 3 )
                    b_skip = analysis.b_try_skip;
            }
            /* Set up MVs for future predictors */
            if( b_skip )
            {
                for( int i = 0; i < h->mb.pic.i_fref[0]; i++ )
                    M32( h->mb.mvr[0][i][h->mb.i_mb_xy] ) = 0;
                for( int i = 0; i < h->mb.pic.i_fref[1]; i++ )
                    M32( h->mb.mvr[1][i][h->mb.i_mb_xy] ) = 0;
            }
        }

        if( !b_skip )
        {
            const unsigned int flags = h->param.analyse.inter;
            int i_type;
            int i_partition;
            int i_satd_inter;
            h->mb.b_skip_mc = 0;
            h->mb.i_type = B_DIRECT;

            x264_mb_analyse_load_costs( h, &analysis );

            /* select best inter mode */
            /* direct must be first */
            if( analysis.b_direct_available )
                x264_mb_analyse_inter_direct( h, &analysis );

            x264_mb_analyse_inter_b16x16( h, &analysis );

            if( h->mb.i_type == B_SKIP )
            {
                for( int i = 1; i < h->mb.pic.i_fref[0]; i++ )
                    M32( h->mb.mvr[0][i][h->mb.i_mb_xy] ) = 0;
                for( int i = 1; i < h->mb.pic.i_fref[1]; i++ )
                    M32( h->mb.mvr[1][i][h->mb.i_mb_xy] ) = 0;
                return;
            }

            i_type = B_L0_L0;
            i_partition = D_16x16;
            i_cost = analysis.l0.me16x16.cost;
            COPY2_IF_LT( i_cost, analysis.l1.me16x16.cost, i_type, B_L1_L1 );
            COPY2_IF_LT( i_cost, analysis.i_cost16x16bi, i_type, B_BI_BI );
            COPY2_IF_LT( i_cost, analysis.i_cost16x16direct, i_type, B_DIRECT );

            if( analysis.i_mbrd && analysis.b_early_terminate && analysis.i_cost16x16direct <= i_cost * 33/32 )
            {
                x264_mb_analyse_b_rd( h, &analysis, i_cost );
                if( i_bskip_cost < analysis.i_rd16x16direct &&
                    i_bskip_cost < analysis.i_rd16x16bi &&
                    i_bskip_cost < analysis.l0.i_rd16x16 &&
                    i_bskip_cost < analysis.l1.i_rd16x16 )
                {
                    h->mb.i_type = B_SKIP;
                    x264_analyse_update_cache( h, &analysis );
                    return;
                }
            }

            if( flags & X264_ANALYSE_BSUB16x16 )
            {
                if( h->param.analyse.b_mixed_references )
                    x264_mb_analyse_inter_b8x8_mixed_ref( h, &analysis );
                else
                    x264_mb_analyse_inter_b8x8( h, &analysis );

                COPY3_IF_LT( i_cost, analysis.i_cost8x8bi, i_type, B_8x8, i_partition, D_8x8 );

                /* Try to estimate the cost of b16x8/b8x16 based on the satd scores of the b8x8 modes */
                int i_cost_est16x8bi_total = 0, i_cost_est8x16bi_total = 0;
                int i_mb_type, i_partition16x8[2], i_partition8x16[2];
                for( int i = 0; i < 2; i++ )
                {
                    int avg_l0_mv_ref_cost, avg_l1_mv_ref_cost;
                    int i_l0_satd, i_l1_satd, i_bi_satd, i_best_cost;
                    // 16x8
                    i_best_cost = COST_MAX;
                    i_l0_satd = analysis.i_satd8x8[0][i*2] + analysis.i_satd8x8[0][i*2+1];
                    i_l1_satd = analysis.i_satd8x8[1][i*2] + analysis.i_satd8x8[1][i*2+1];
                    i_bi_satd = analysis.i_satd8x8[2][i*2] + analysis.i_satd8x8[2][i*2+1];
                    avg_l0_mv_ref_cost = ( analysis.l0.me8x8[i*2].cost_mv + analysis.l0.me8x8[i*2].i_ref_cost
                                         + analysis.l0.me8x8[i*2+1].cost_mv + analysis.l0.me8x8[i*2+1].i_ref_cost + 1 ) >> 1;
                    avg_l1_mv_ref_cost = ( analysis.l1.me8x8[i*2].cost_mv + analysis.l1.me8x8[i*2].i_ref_cost
                                         + analysis.l1.me8x8[i*2+1].cost_mv + analysis.l1.me8x8[i*2+1].i_ref_cost + 1 ) >> 1;
                    COPY2_IF_LT( i_best_cost, i_l0_satd + avg_l0_mv_ref_cost, i_partition16x8[i], D_L0_8x8 );
                    COPY2_IF_LT( i_best_cost, i_l1_satd + avg_l1_mv_ref_cost, i_partition16x8[i], D_L1_8x8 );
                    COPY2_IF_LT( i_best_cost, i_bi_satd + avg_l0_mv_ref_cost + avg_l1_mv_ref_cost, i_partition16x8[i], D_BI_8x8 );
                    analysis.i_cost_est16x8[i] = i_best_cost;

                    // 8x16
                    i_best_cost = COST_MAX;
                    i_l0_satd = analysis.i_satd8x8[0][i] + analysis.i_satd8x8[0][i+2];
                    i_l1_satd = analysis.i_satd8x8[1][i] + analysis.i_satd8x8[1][i+2];
                    i_bi_satd = analysis.i_satd8x8[2][i] + analysis.i_satd8x8[2][i+2];
                    avg_l0_mv_ref_cost = ( analysis.l0.me8x8[i].cost_mv + analysis.l0.me8x8[i].i_ref_cost
                                         + analysis.l0.me8x8[i+2].cost_mv + analysis.l0.me8x8[i+2].i_ref_cost + 1 ) >> 1;
                    avg_l1_mv_ref_cost = ( analysis.l1.me8x8[i].cost_mv + analysis.l1.me8x8[i].i_ref_cost
                                         + analysis.l1.me8x8[i+2].cost_mv + analysis.l1.me8x8[i+2].i_ref_cost + 1 ) >> 1;
                    COPY2_IF_LT( i_best_cost, i_l0_satd + avg_l0_mv_ref_cost, i_partition8x16[i], D_L0_8x8 );
                    COPY2_IF_LT( i_best_cost, i_l1_satd + avg_l1_mv_ref_cost, i_partition8x16[i], D_L1_8x8 );
                    COPY2_IF_LT( i_best_cost, i_bi_satd + avg_l0_mv_ref_cost + avg_l1_mv_ref_cost, i_partition8x16[i], D_BI_8x8 );
                    analysis.i_cost_est8x16[i] = i_best_cost;
                }
                i_mb_type = B_L0_L0 + (i_partition16x8[0]>>2) * 3 + (i_partition16x8[1]>>2);
                analysis.i_cost_est16x8[1] += analysis.i_lambda * i_mb_b16x8_cost_table[i_mb_type];
                i_cost_est16x8bi_total = analysis.i_cost_est16x8[0] + analysis.i_cost_est16x8[1];
                i_mb_type = B_L0_L0 + (i_partition8x16[0]>>2) * 3 + (i_partition8x16[1]>>2);
                analysis.i_cost_est8x16[1] += analysis.i_lambda * i_mb_b16x8_cost_table[i_mb_type];
                i_cost_est8x16bi_total = analysis.i_cost_est8x16[0] + analysis.i_cost_est8x16[1];

                /* We can gain a little speed by checking the mode with the lowest estimated cost first */
                int try_16x8_first = i_cost_est16x8bi_total < i_cost_est8x16bi_total;
                if( try_16x8_first && (!analysis.b_early_terminate || i_cost_est16x8bi_total < i_cost) )
                {
                    x264_mb_analyse_inter_b16x8( h, &analysis, i_cost );
                    COPY3_IF_LT( i_cost, analysis.i_cost16x8bi, i_type, analysis.i_mb_type16x8, i_partition, D_16x8 );
                }
                if( !analysis.b_early_terminate || i_cost_est8x16bi_total < i_cost )
                {
                    x264_mb_analyse_inter_b8x16( h, &analysis, i_cost );
                    COPY3_IF_LT( i_cost, analysis.i_cost8x16bi, i_type, analysis.i_mb_type8x16, i_partition, D_8x16 );
                }
                if( !try_16x8_first && (!analysis.b_early_terminate || i_cost_est16x8bi_total < i_cost) )
                {
                    x264_mb_analyse_inter_b16x8( h, &analysis, i_cost );
                    COPY3_IF_LT( i_cost, analysis.i_cost16x8bi, i_type, analysis.i_mb_type16x8, i_partition, D_16x8 );
                }
            }

            if( analysis.i_mbrd || !h->mb.i_subpel_refine )
            {
                /* refine later */
            }
            /* refine qpel */
            else if( i_partition == D_16x16 )
            {
                analysis.l0.me16x16.cost -= analysis.i_lambda * i_mb_b_cost_table[B_L0_L0];
                analysis.l1.me16x16.cost -= analysis.i_lambda * i_mb_b_cost_table[B_L1_L1];
                if( i_type == B_L0_L0 )
                {
                    x264_me_refine_qpel( h, &analysis.l0.me16x16 );
                    i_cost = analysis.l0.me16x16.cost
                           + analysis.i_lambda * i_mb_b_cost_table[B_L0_L0];
                }
                else if( i_type == B_L1_L1 )
                {
                    x264_me_refine_qpel( h, &analysis.l1.me16x16 );
                    i_cost = analysis.l1.me16x16.cost
                           + analysis.i_lambda * i_mb_b_cost_table[B_L1_L1];
                }
                else if( i_type == B_BI_BI )
                {
                    x264_me_refine_qpel( h, &analysis.l0.bi16x16 );
                    x264_me_refine_qpel( h, &analysis.l1.bi16x16 );
                }
            }
            else if( i_partition == D_16x8 )
            {
                for( int i = 0; i < 2; i++ )
                {
                    if( analysis.i_mb_partition16x8[i] != D_L1_8x8 )
                        x264_me_refine_qpel( h, &analysis.l0.me16x8[i] );
                    if( analysis.i_mb_partition16x8[i] != D_L0_8x8 )
                        x264_me_refine_qpel( h, &analysis.l1.me16x8[i] );
                }
            }
            else if( i_partition == D_8x16 )
            {
                for( int i = 0; i < 2; i++ )
                {
                    if( analysis.i_mb_partition8x16[i] != D_L1_8x8 )
                        x264_me_refine_qpel( h, &analysis.l0.me8x16[i] );
                    if( analysis.i_mb_partition8x16[i] != D_L0_8x8 )
                        x264_me_refine_qpel( h, &analysis.l1.me8x16[i] );
                }
            }
            else if( i_partition == D_8x8 )
            {
                for( int i = 0; i < 4; i++ )
                {
                    x264_me_t *m;
                    int i_part_cost_old;
                    int i_type_cost;
                    int i_part_type = h->mb.i_sub_partition[i];
                    int b_bidir = (i_part_type == D_BI_8x8);

                    if( i_part_type == D_DIRECT_8x8 )
                        continue;
                    if( x264_mb_partition_listX_table[0][i_part_type] )
                    {
                        m = &analysis.l0.me8x8[i];
                        i_part_cost_old = m->cost;
                        i_type_cost = analysis.i_lambda * i_sub_mb_b_cost_table[D_L0_8x8];
                        m->cost -= i_type_cost;
                        x264_me_refine_qpel( h, m );
                        if( !b_bidir )
                            analysis.i_cost8x8bi += m->cost + i_type_cost - i_part_cost_old;
                    }
                    if( x264_mb_partition_listX_table[1][i_part_type] )
                    {
                        m = &analysis.l1.me8x8[i];
                        i_part_cost_old = m->cost;
                        i_type_cost = analysis.i_lambda * i_sub_mb_b_cost_table[D_L1_8x8];
                        m->cost -= i_type_cost;
                        x264_me_refine_qpel( h, m );
                        if( !b_bidir )
                            analysis.i_cost8x8bi += m->cost + i_type_cost - i_part_cost_old;
                    }
                    /* TODO: update mvp? */
                }
            }

            i_satd_inter = i_cost;

            if( analysis.i_mbrd )
            {
                x264_mb_analyse_b_rd( h, &analysis, i_satd_inter );
                i_type = B_SKIP;
                i_cost = i_bskip_cost;
                i_partition = D_16x16;
                COPY2_IF_LT( i_cost, analysis.l0.i_rd16x16, i_type, B_L0_L0 );
                COPY2_IF_LT( i_cost, analysis.l1.i_rd16x16, i_type, B_L1_L1 );
                COPY2_IF_LT( i_cost, analysis.i_rd16x16bi, i_type, B_BI_BI );
                COPY2_IF_LT( i_cost, analysis.i_rd16x16direct, i_type, B_DIRECT );
                COPY3_IF_LT( i_cost, analysis.i_rd16x8bi, i_type, analysis.i_mb_type16x8, i_partition, D_16x8 );
                COPY3_IF_LT( i_cost, analysis.i_rd8x16bi, i_type, analysis.i_mb_type8x16, i_partition, D_8x16 );
                COPY3_IF_LT( i_cost, analysis.i_rd8x8bi, i_type, B_8x8, i_partition, D_8x8 );

                h->mb.i_type = i_type;
                h->mb.i_partition = i_partition;
            }

            if( h->mb.b_chroma_me )
            {
                if( CHROMA444 )
                {
                    x264_mb_analyse_intra( h, &analysis, i_satd_inter );
                    x264_mb_analyse_intra_chroma( h, &analysis );
                }
                else
                {
                    x264_mb_analyse_intra_chroma( h, &analysis );
                    x264_mb_analyse_intra( h, &analysis, i_satd_inter - analysis.i_satd_chroma );
                }
                analysis.i_satd_i16x16 += analysis.i_satd_chroma;
                analysis.i_satd_i8x8   += analysis.i_satd_chroma;
                analysis.i_satd_i4x4   += analysis.i_satd_chroma;
            }
            else
                x264_mb_analyse_intra( h, &analysis, i_satd_inter );

            if( analysis.i_mbrd )
            {
                x264_mb_analyse_transform_rd( h, &analysis, &i_satd_inter, &i_cost );
                x264_intra_rd( h, &analysis, i_satd_inter * 17/16 + 1 );
            }

            COPY2_IF_LT( i_cost, analysis.i_satd_i16x16, i_type, I_16x16 );
            COPY2_IF_LT( i_cost, analysis.i_satd_i8x8, i_type, I_8x8 );
            COPY2_IF_LT( i_cost, analysis.i_satd_i4x4, i_type, I_4x4 );
            COPY2_IF_LT( i_cost, analysis.i_satd_pcm, i_type, I_PCM );

            h->mb.i_type = i_type;
            h->mb.i_partition = i_partition;

            if( analysis.i_mbrd >= 2 && IS_INTRA( i_type ) && i_type != I_PCM )
                x264_intra_rd_refine( h, &analysis );
            if( h->mb.i_subpel_refine >= 5 )
                x264_refine_bidir( h, &analysis );

            if( analysis.i_mbrd >= 2 && i_type > B_DIRECT && i_type < B_SKIP )
            {
                int i_biweight;
                x264_analyse_update_cache( h, &analysis );

                if( i_partition == D_16x16 )
                {
                    if( i_type == B_L0_L0 )
                    {
                        analysis.l0.me16x16.cost = i_cost;
                        x264_me_refine_qpel_rd( h, &analysis.l0.me16x16, analysis.i_lambda2, 0, 0 );
                    }
                    else if( i_type == B_L1_L1 )
                    {
                        analysis.l1.me16x16.cost = i_cost;
                        x264_me_refine_qpel_rd( h, &analysis.l1.me16x16, analysis.i_lambda2, 0, 1 );
                    }
                    else if( i_type == B_BI_BI )
                    {
                        i_biweight = h->mb.bipred_weight[analysis.l0.bi16x16.i_ref][analysis.l1.bi16x16.i_ref];
                        x264_me_refine_bidir_rd( h, &analysis.l0.bi16x16, &analysis.l1.bi16x16, i_biweight, 0, analysis.i_lambda2 );
                    }
                }
                else if( i_partition == D_16x8 )
                {
                    for( int i = 0; i < 2; i++ )
                    {
                        h->mb.i_sub_partition[i*2] = h->mb.i_sub_partition[i*2+1] = analysis.i_mb_partition16x8[i];
                        if( analysis.i_mb_partition16x8[i] == D_L0_8x8 )
                            x264_me_refine_qpel_rd( h, &analysis.l0.me16x8[i], analysis.i_lambda2, i*8, 0 );
                        else if( analysis.i_mb_partition16x8[i] == D_L1_8x8 )
                            x264_me_refine_qpel_rd( h, &analysis.l1.me16x8[i], analysis.i_lambda2, i*8, 1 );
                        else if( analysis.i_mb_partition16x8[i] == D_BI_8x8 )
                        {
                            i_biweight = h->mb.bipred_weight[analysis.l0.me16x8[i].i_ref][analysis.l1.me16x8[i].i_ref];
                            x264_me_refine_bidir_rd( h, &analysis.l0.me16x8[i], &analysis.l1.me16x8[i], i_biweight, i*2, analysis.i_lambda2 );
                        }
                    }
                }
                else if( i_partition == D_8x16 )
                {
                    for( int i = 0; i < 2; i++ )
                    {
                        h->mb.i_sub_partition[i] = h->mb.i_sub_partition[i+2] = analysis.i_mb_partition8x16[i];
                        if( analysis.i_mb_partition8x16[i] == D_L0_8x8 )
                            x264_me_refine_qpel_rd( h, &analysis.l0.me8x16[i], analysis.i_lambda2, i*4, 0 );
                        else if( analysis.i_mb_partition8x16[i] == D_L1_8x8 )
                            x264_me_refine_qpel_rd( h, &analysis.l1.me8x16[i], analysis.i_lambda2, i*4, 1 );
                        else if( analysis.i_mb_partition8x16[i] == D_BI_8x8 )
                        {
                            i_biweight = h->mb.bipred_weight[analysis.l0.me8x16[i].i_ref][analysis.l1.me8x16[i].i_ref];
                            x264_me_refine_bidir_rd( h, &analysis.l0.me8x16[i], &analysis.l1.me8x16[i], i_biweight, i, analysis.i_lambda2 );
                        }
                    }
                }
                else if( i_partition == D_8x8 )
                {
                    for( int i = 0; i < 4; i++ )
                    {
                        if( h->mb.i_sub_partition[i] == D_L0_8x8 )
                            x264_me_refine_qpel_rd( h, &analysis.l0.me8x8[i], analysis.i_lambda2, i*4, 0 );
                        else if( h->mb.i_sub_partition[i] == D_L1_8x8 )
                            x264_me_refine_qpel_rd( h, &analysis.l1.me8x8[i], analysis.i_lambda2, i*4, 1 );
                        else if( h->mb.i_sub_partition[i] == D_BI_8x8 )
                        {
                            i_biweight = h->mb.bipred_weight[analysis.l0.me8x8[i].i_ref][analysis.l1.me8x8[i].i_ref];
                            x264_me_refine_bidir_rd( h, &analysis.l0.me8x8[i], &analysis.l1.me8x8[i], i_biweight, i, analysis.i_lambda2 );
                        }
                    }
                }
            }
        }
    }

    x264_analyse_update_cache( h, &analysis );

    /* In rare cases we can end up qpel-RDing our way back to a larger partition size
     * without realizing it.  Check for this and account for it if necessary. */
    if( analysis.i_mbrd >= 2 )
    {
        /* Don't bother with bipred or 8x8-and-below, the odds are incredibly low. */
        static const uint8_t check_mv_lists[X264_MBTYPE_MAX] = {[P_L0]=1, [B_L0_L0]=1, [B_L1_L1]=2};
        int list = check_mv_lists[h->mb.i_type] - 1;
        if( list >= 0 && h->mb.i_partition != D_16x16 &&
            M32( &h->mb.cache.mv[list][x264_scan8[0]] ) == M32( &h->mb.cache.mv[list][x264_scan8[12]] ) &&
            h->mb.cache.ref[list][x264_scan8[0]] == h->mb.cache.ref[list][x264_scan8[12]] )
                h->mb.i_partition = D_16x16;
    }

    if( !analysis.i_mbrd )
        x264_mb_analyse_transform( h );

    if( analysis.i_mbrd == 3 && !IS_SKIP(h->mb.i_type) )
        x264_mb_analyse_qp_rd( h, &analysis );

    h->mb.b_trellis = h->param.analyse.i_trellis;
    h->mb.b_noise_reduction = h->mb.b_noise_reduction || (!!h->param.analyse.i_noise_reduction && !IS_INTRA( h->mb.i_type ));

    if( !IS_SKIP(h->mb.i_type) && h->mb.i_psy_trellis && h->param.analyse.i_trellis == 1 )
        x264_psy_trellis_init( h, 0 );
    if( h->mb.b_trellis == 1 || h->mb.b_noise_reduction )
        h->mb.i_skip_intra = 0;
}

/*-------------------- Update MB from the analysis ----------------------*/
static void x264_analyse_update_cache( x264_t *h, x264_mb_analysis_t *a  )
{
    switch( h->mb.i_type )
    {
        case I_4x4:
            for( int i = 0; i < 16; i++ )
                h->mb.cache.intra4x4_pred_mode[x264_scan8[i]] = a->i_predict4x4[i];

            x264_mb_analyse_intra_chroma( h, a );
            break;
        case I_8x8:
            for( int i = 0; i < 4; i++ )
                x264_macroblock_cache_intra8x8_pred( h, 2*(i&1), 2*(i>>1), a->i_predict8x8[i] );

            x264_mb_analyse_intra_chroma( h, a );
            break;
        case I_16x16:
            h->mb.i_intra16x16_pred_mode = a->i_predict16x16;
            x264_mb_analyse_intra_chroma( h, a );
            break;

        case I_PCM:
            break;

        case P_L0:
            switch( h->mb.i_partition )
            {
                case D_16x16:
                    x264_macroblock_cache_ref( h, 0, 0, 4, 4, 0, a->l0.me16x16.i_ref );
                    x264_macroblock_cache_mv_ptr( h, 0, 0, 4, 4, 0, a->l0.me16x16.mv );
                    break;

                case D_16x8:
                    x264_macroblock_cache_ref( h, 0, 0, 4, 2, 0, a->l0.me16x8[0].i_ref );
                    x264_macroblock_cache_ref( h, 0, 2, 4, 2, 0, a->l0.me16x8[1].i_ref );
                    x264_macroblock_cache_mv_ptr( h, 0, 0, 4, 2, 0, a->l0.me16x8[0].mv );
                    x264_macroblock_cache_mv_ptr( h, 0, 2, 4, 2, 0, a->l0.me16x8[1].mv );
                    break;

                case D_8x16:
                    x264_macroblock_cache_ref( h, 0, 0, 2, 4, 0, a->l0.me8x16[0].i_ref );
                    x264_macroblock_cache_ref( h, 2, 0, 2, 4, 0, a->l0.me8x16[1].i_ref );
                    x264_macroblock_cache_mv_ptr( h, 0, 0, 2, 4, 0, a->l0.me8x16[0].mv );
                    x264_macroblock_cache_mv_ptr( h, 2, 0, 2, 4, 0, a->l0.me8x16[1].mv );
                    break;

                default:
                    x264_log( h, X264_LOG_ERROR, "internal error P_L0 and partition=%d\n", h->mb.i_partition );
                    break;
            }
            break;

        case P_8x8:
            x264_macroblock_cache_ref( h, 0, 0, 2, 2, 0, a->l0.me8x8[0].i_ref );
            x264_macroblock_cache_ref( h, 2, 0, 2, 2, 0, a->l0.me8x8[1].i_ref );
            x264_macroblock_cache_ref( h, 0, 2, 2, 2, 0, a->l0.me8x8[2].i_ref );
            x264_macroblock_cache_ref( h, 2, 2, 2, 2, 0, a->l0.me8x8[3].i_ref );
            for( int i = 0; i < 4; i++ )
                x264_mb_cache_mv_p8x8( h, a, i );
            break;

        case P_SKIP:
        {
            h->mb.i_partition = D_16x16;
            x264_macroblock_cache_ref( h, 0, 0, 4, 4, 0, 0 );
            x264_macroblock_cache_mv_ptr( h, 0, 0, 4, 4, 0, h->mb.cache.pskip_mv );
            break;
        }

        case B_SKIP:
        case B_DIRECT:
            h->mb.i_partition = h->mb.cache.direct_partition;
            x264_mb_load_mv_direct8x8( h, 0 );
            x264_mb_load_mv_direct8x8( h, 1 );
            x264_mb_load_mv_direct8x8( h, 2 );
            x264_mb_load_mv_direct8x8( h, 3 );
            break;

        case B_8x8:
            /* optimize: cache might not need to be rewritten */
            for( int i = 0; i < 4; i++ )
                x264_mb_cache_mv_b8x8( h, a, i, 1 );
            break;

        default: /* the rest of the B types */
            switch( h->mb.i_partition )
            {
            case D_16x16:
                switch( h->mb.i_type )
                {
                case B_L0_L0:
                    x264_macroblock_cache_ref( h, 0, 0, 4, 4, 0, a->l0.me16x16.i_ref );
                    x264_macroblock_cache_mv_ptr( h, 0, 0, 4, 4, 0, a->l0.me16x16.mv );

                    x264_macroblock_cache_ref( h, 0, 0, 4, 4, 1, -1 );
                    x264_macroblock_cache_mv ( h, 0, 0, 4, 4, 1, 0 );
                    x264_macroblock_cache_mvd( h, 0, 0, 4, 4, 1, 0 );
                    break;
                case B_L1_L1:
                    x264_macroblock_cache_ref( h, 0, 0, 4, 4, 0, -1 );
                    x264_macroblock_cache_mv ( h, 0, 0, 4, 4, 0, 0 );
                    x264_macroblock_cache_mvd( h, 0, 0, 4, 4, 0, 0 );

                    x264_macroblock_cache_ref( h, 0, 0, 4, 4, 1, a->l1.me16x16.i_ref );
                    x264_macroblock_cache_mv_ptr( h, 0, 0, 4, 4, 1, a->l1.me16x16.mv );
                    break;
                case B_BI_BI:
                    x264_macroblock_cache_ref( h, 0, 0, 4, 4, 0, a->l0.bi16x16.i_ref );
                    x264_macroblock_cache_mv_ptr( h, 0, 0, 4, 4, 0, a->l0.bi16x16.mv );

                    x264_macroblock_cache_ref( h, 0, 0, 4, 4, 1, a->l1.bi16x16.i_ref );
                    x264_macroblock_cache_mv_ptr( h, 0, 0, 4, 4, 1, a->l1.bi16x16.mv );
                    break;
                }
                break;
            case D_16x8:
                x264_mb_cache_mv_b16x8( h, a, 0, 1 );
                x264_mb_cache_mv_b16x8( h, a, 1, 1 );
                break;
            case D_8x16:
                x264_mb_cache_mv_b8x16( h, a, 0, 1 );
                x264_mb_cache_mv_b8x16( h, a, 1, 1 );
                break;
            default:
                x264_log( h, X264_LOG_ERROR, "internal error (invalid MB type)\n" );
                break;
            }
    }

#ifndef NDEBUG
    if( h->i_thread_frames > 1 && !IS_INTRA(h->mb.i_type) )
    {
        for( int l = 0; l <= (h->sh.i_type == SLICE_TYPE_B); l++ )
        {
            int completed;
            int ref = h->mb.cache.ref[l][x264_scan8[0]];
            if( ref < 0 )
                continue;
            completed = h->fref[l][ ref >> MB_INTERLACED ]->orig->i_lines_completed;
            if( (h->mb.cache.mv[l][x264_scan8[15]][1] >> (2 - MB_INTERLACED)) + h->mb.i_mb_y*16 > completed )
            {
                x264_log( h, X264_LOG_WARNING, "internal error (MV out of thread range)\n");
                x264_log( h, X264_LOG_DEBUG, "mb type: %d \n", h->mb.i_type);
                x264_log( h, X264_LOG_DEBUG, "mv: l%dr%d (%d,%d) \n", l, ref,
                                h->mb.cache.mv[l][x264_scan8[15]][0],
                                h->mb.cache.mv[l][x264_scan8[15]][1] );
                x264_log( h, X264_LOG_DEBUG, "limit: %d \n", h->mb.mv_max_spel[1]);
                x264_log( h, X264_LOG_DEBUG, "mb_xy: %d,%d \n", h->mb.i_mb_x, h->mb.i_mb_y);
                x264_log( h, X264_LOG_DEBUG, "completed: %d \n", completed );
                x264_log( h, X264_LOG_WARNING, "recovering by using intra mode\n");
                x264_mb_analyse_intra( h, a, COST_MAX );
                h->mb.i_type = I_16x16;
                h->mb.i_intra16x16_pred_mode = a->i_predict16x16;
                x264_mb_analyse_intra_chroma( h, a );
            }
        }
    }
#endif
}

/*****************************************************************************
 * slicetype.c: lookahead analysis
 *****************************************************************************
 * Copyright (C) 2005-2011 x264 project
 *
 * Authors: Jason Garrett-Glaser <darkshikari@gmail.com>
 *          Loren Merritt <lorenm@u.washington.edu>
 *          Dylan Yudaken <dyudaken@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at licensing@x264.com.
 *****************************************************************************/

#include "common.h"
#include "emacroblock.h"
#include "me.h"

// Indexed by pic_struct values
static const uint8_t delta_tfi_divisor[10] = { 0, 2, 1, 1, 2, 2, 3, 3, 4, 6 };

static int x264_slicetype_frame_cost( x264_t *h, x264_mb_analysis_t *a,
                                      x264_frame_t **frames, int p0, int p1, int b,
                                      int b_intra_penalty );

static void x264_lowres_context_init( x264_t *h, x264_mb_analysis_t *a )
{
    a->i_qp = X264_LOOKAHEAD_QP;
    a->i_lambda = x264_lambda_tab[ a->i_qp ];
    x264_mb_analyse_load_costs( h, a );
    if( h->param.analyse.i_subpel_refine > 1 )
    {
        h->mb.i_me_method = X264_MIN( X264_ME_HEX, h->param.analyse.i_me_method );
        h->mb.i_subpel_refine = 4;
    }
    else
    {
        h->mb.i_me_method = X264_ME_DIA;
        h->mb.i_subpel_refine = 2;
    }
    h->mb.b_chroma_me = 0;
}

/* makes a non-h264 weight (i.e. fix7), into an h264 weight */
static void x264_weight_get_h264( int weight_nonh264, int offset, x264_weight_t *w )
{
    w->i_offset = offset;
    w->i_denom = 7;
    w->i_scale = weight_nonh264;
    while( w->i_denom > 0 && (w->i_scale > 127 || !(w->i_scale & 1)) )
    {
        w->i_denom--;
        w->i_scale >>= 1;
    }
    w->i_scale = X264_MIN( w->i_scale, 127 );
}

static NOINLINE pixel *x264_weight_cost_init_luma( x264_t *h, x264_frame_t *fenc, x264_frame_t *ref, pixel *dest )
{
    int ref0_distance = fenc->i_frame - ref->i_frame - 1;
    /* Note: this will never run during lookahead as weights_analyse is only called if no
     * motion search has been done. */
    if( fenc->lowres_mvs[0][ref0_distance][0][0] != 0x7FFF )
    {
        int i_stride = fenc->i_stride_lowres;
        int i_lines = fenc->i_lines_lowres;
        int i_width = fenc->i_width_lowres;
        int i_mb_xy = 0;
        pixel *p = dest;

        for( int y = 0; y < i_lines; y += 8, p += i_stride*8 )
            for( int x = 0; x < i_width; x += 8, i_mb_xy++ )
            {
                int mvx = fenc->lowres_mvs[0][ref0_distance][i_mb_xy][0];
                int mvy = fenc->lowres_mvs[0][ref0_distance][i_mb_xy][1];
                h->mc.mc_luma( p+x, i_stride, ref->lowres, i_stride,
                               mvx+(x<<2), mvy+(y<<2), 8, 8, x264_weight_none );
            }
        x264_emms();
        return dest;
    }
    x264_emms();
    return ref->lowres[0];
}

/* How data is organized for 4:2:0/4:2:2 chroma weightp:
 * [U: ref] [U: fenc]
 * [V: ref] [V: fenc]
 * fenc = ref + offset
 * v = u + stride * chroma height */

static NOINLINE void x264_weight_cost_init_chroma( x264_t *h, x264_frame_t *fenc, x264_frame_t *ref, pixel *dstu, pixel *dstv )
{
    int ref0_distance = fenc->i_frame - ref->i_frame - 1;
    int i_stride = fenc->i_stride[1];
    int i_offset = i_stride / 2;
    int i_lines = fenc->i_lines[1];
    int i_width = fenc->i_width[1];
    int v_shift = CHROMA_V_SHIFT;
    int cw = 8*h->mb.i_mb_width;
    int ch = 16*h->mb.i_mb_height >> v_shift;
    int height = 16 >> v_shift;

    if( fenc->lowres_mvs[0][ref0_distance][0][0] != 0x7FFF )
    {
        x264_frame_expand_border_chroma( h, ref, 1 );
        for( int y = 0, mb_xy = 0, pel_offset_y = 0; y < i_lines; y += height, pel_offset_y = y*i_stride )
            for( int x = 0, pel_offset_x = 0; x < i_width; x += 8, mb_xy++, pel_offset_x += 8 )
            {
                pixel *pixu = dstu + pel_offset_y + pel_offset_x;
                pixel *pixv = dstv + pel_offset_y + pel_offset_x;
                pixel *src1 =  ref->plane[1] + pel_offset_y + pel_offset_x*2; /* NV12/NV16 */
                int mvx = fenc->lowres_mvs[0][ref0_distance][mb_xy][0];
                int mvy = fenc->lowres_mvs[0][ref0_distance][mb_xy][1];
                h->mc.mc_chroma( pixu, pixv, i_stride, src1, i_stride, mvx, 2*mvy>>v_shift, 8, height );
            }
    }
    else
        h->mc.plane_copy_deinterleave( dstu, i_stride, dstv, i_stride, ref->plane[1], i_stride, cw, ch );
    h->mc.plane_copy_deinterleave( dstu+i_offset, i_stride, dstv+i_offset, i_stride, fenc->plane[1], i_stride, cw, ch );
    x264_emms();
}

static NOINLINE pixel *x264_weight_cost_init_chroma444( x264_t *h, x264_frame_t *fenc, x264_frame_t *ref, pixel *dst, int p )
{
    int ref0_distance = fenc->i_frame - ref->i_frame - 1;
    int i_stride = fenc->i_stride[p];
    int i_lines = fenc->i_lines[p];
    int i_width = fenc->i_width[p];

    if( fenc->lowres_mvs[0][ref0_distance][0][0] != 0x7FFF )
    {
        x264_frame_expand_border_chroma( h, ref, p );
        for( int y = 0, mb_xy = 0, pel_offset_y = 0; y < i_lines; y += 16, pel_offset_y = y*i_stride )
            for( int x = 0, pel_offset_x = 0; x < i_width; x += 16, mb_xy++, pel_offset_x += 16 )
            {
                pixel *pix = dst + pel_offset_y + pel_offset_x;
                pixel *src = ref->plane[p] + pel_offset_y + pel_offset_x;
                int mvx = fenc->lowres_mvs[0][ref0_distance][mb_xy][0] / 2;
                int mvy = fenc->lowres_mvs[0][ref0_distance][mb_xy][1] / 2;
                /* We don't want to calculate hpels for fenc frames, so we round the motion
                 * vectors to fullpel here.  It's not too bad, I guess? */
                h->mc.copy_16x16_unaligned( pix, i_stride, src+mvx+mvy*i_stride, i_stride, 16 );
            }
        x264_emms();
        return dst;
    }
    x264_emms();
    return ref->plane[p];
}

static int x264_weight_slice_header_cost( x264_t *h, x264_weight_t *w, int b_chroma )
{
    /* Add cost of weights in the slice header. */
    int lambda = x264_lambda_tab[X264_LOOKAHEAD_QP];
    /* 4 times higher, because chroma is analyzed at full resolution. */
    if( b_chroma )
        lambda *= 4;
    int numslices;
    if( h->param.i_slice_count )
        numslices = h->param.i_slice_count;
    else if( h->param.i_slice_max_mbs )
        numslices = (h->mb.i_mb_width * h->mb.i_mb_height + h->param.i_slice_max_mbs-1) / h->param.i_slice_max_mbs;
    else
        numslices = 1;
    /* FIXME: find a way to account for --slice-max-size?
     * Multiply by 2 as there will be a duplicate. 10 bits added as if there is a weighted frame, then an additional duplicate is used.
     * Cut denom cost in half if chroma, since it's shared between the two chroma planes. */
    int denom_cost = bs_size_ue( w[0].i_denom ) * (2 - b_chroma);
    return lambda * numslices * ( 10 + denom_cost + 2 * (bs_size_se( w[0].i_scale ) + bs_size_se( w[0].i_offset )) );
}

static NOINLINE unsigned int x264_weight_cost_luma( x264_t *h, x264_frame_t *fenc, pixel *src, x264_weight_t *w )
{
    unsigned int cost = 0;
    int i_stride = fenc->i_stride_lowres;
    int i_lines = fenc->i_lines_lowres;
    int i_width = fenc->i_width_lowres;
    pixel *fenc_plane = fenc->lowres[0];
    ALIGNED_ARRAY_16( pixel, buf,[8*8] );
    int pixoff = 0;
    int i_mb = 0;

    if( w )
    {
        for( int y = 0; y < i_lines; y += 8, pixoff = y*i_stride )
            for( int x = 0; x < i_width; x += 8, i_mb++, pixoff += 8)
            {
                w->weightfn[8>>2]( buf, 8, &src[pixoff], i_stride, w, 8 );
                int cmp = h->pixf.mbcmp[PIXEL_8x8]( buf, 8, &fenc_plane[pixoff], i_stride );
                cost += X264_MIN( cmp, fenc->i_intra_cost[i_mb] );
            }
        cost += x264_weight_slice_header_cost( h, w, 0 );
    }
    else
        for( int y = 0; y < i_lines; y += 8, pixoff = y*i_stride )
            for( int x = 0; x < i_width; x += 8, i_mb++, pixoff += 8 )
            {
                int cmp = h->pixf.mbcmp[PIXEL_8x8]( &src[pixoff], i_stride, &fenc_plane[pixoff], i_stride );
                cost += X264_MIN( cmp, fenc->i_intra_cost[i_mb] );
            }
    x264_emms();
    return cost;
}

static NOINLINE unsigned int x264_weight_cost_chroma( x264_t *h, x264_frame_t *fenc, pixel *ref, x264_weight_t *w )
{
    unsigned int cost = 0;
    int i_stride = fenc->i_stride[1];
    int i_offset = i_stride / 2;
    int i_lines = fenc->i_lines[1];
    int i_width = fenc->i_width[1];
    pixel *src = ref + i_offset;
    ALIGNED_ARRAY_16( pixel, buf, [8*16] );
    int pixoff = 0;
    int chromapix = h->luma2chroma_pixel[PIXEL_16x16];
    int height = 16 >> CHROMA_V_SHIFT;
    ALIGNED_16( static pixel flat[8] ) = {0};
    if( w )
    {
        for( int y = 0; y < i_lines; y += height, pixoff = y*i_stride )
            for( int x = 0; x < i_width; x += 8, pixoff += 8 )
            {
                w->weightfn[8>>2]( buf, 8, &ref[pixoff], i_stride, w, height );
                /* The naive and seemingly sensible algorithm is to use mbcmp as in luma.
                 * But testing shows that for chroma the DC coefficient is by far the most
                 * important part of the coding cost.  Thus a more useful chroma weight is
                 * obtained by comparing each block's DC coefficient instead of the actual
                 * pixels.
                 *
                 * FIXME: add a (faster) asm sum function to replace sad. */
                cost += abs( h->pixf.sad_aligned[chromapix](          buf,        8, flat, 0 ) -
                             h->pixf.sad_aligned[chromapix]( &src[pixoff], i_stride, flat, 0 ) );
            }
        cost += x264_weight_slice_header_cost( h, w, 1 );
    }
    else
        for( int y = 0; y < i_lines; y += height, pixoff = y*i_stride )
            for( int x = 0; x < i_width; x += 8, pixoff += 8 )
                cost += abs( h->pixf.sad_aligned[chromapix]( &ref[pixoff], i_stride, flat, 0 ) -
                             h->pixf.sad_aligned[chromapix]( &src[pixoff], i_stride, flat, 0 ) );
    x264_emms();
    return cost;
}

static NOINLINE unsigned int x264_weight_cost_chroma444( x264_t *h, x264_frame_t *fenc, pixel *ref, x264_weight_t *w, int p )
{
    unsigned int cost = 0;
    int i_stride = fenc->i_stride[p];
    int i_lines = fenc->i_lines[p];
    int i_width = fenc->i_width[p];
    pixel *src = fenc->plane[p];
    ALIGNED_ARRAY_16( pixel, buf, [16*16] );
    int pixoff = 0;
    if( w )
    {
        for( int y = 0; y < i_lines; y += 16, pixoff = y*i_stride )
            for( int x = 0; x < i_width; x += 16, pixoff += 16 )
            {
                w->weightfn[16>>2]( buf, 16, &ref[pixoff], i_stride, w, 16 );
                cost += h->pixf.mbcmp[PIXEL_16x16]( buf, 16, &src[pixoff], i_stride );
            }
        cost += x264_weight_slice_header_cost( h, w, 1 );
    }
    else
        for( int y = 0; y < i_lines; y += 16, pixoff = y*i_stride )
            for( int x = 0; x < i_width; x += 16, pixoff += 16 )
                cost += h->pixf.mbcmp[PIXEL_16x16]( &ref[pixoff], i_stride, &src[pixoff], i_stride );
    x264_emms();
    return cost;
}

static void x264_weights_analyse( x264_t *h, x264_frame_t *fenc, x264_frame_t *ref, int b_lookahead )
{
    int i_delta_index = fenc->i_frame - ref->i_frame - 1;
    /* epsilon is chosen to require at least a numerator of 127 (with denominator = 128) */
    const float epsilon = 1.f/128.f;
    x264_weight_t *weights = fenc->weight[0];
    SET_WEIGHT( weights[0], 0, 1, 0, 0 );
    SET_WEIGHT( weights[1], 0, 1, 0, 0 );
    SET_WEIGHT( weights[2], 0, 1, 0, 0 );
    int chroma_initted = 0;
    /* Don't check chroma in lookahead, or if there wasn't a luma weight. */
    for( int plane = 0; plane <= 2 && !( plane && ( !weights[0].weightfn || b_lookahead ) ); plane++ )
    {
        int cur_offset, start_offset, end_offset;
        int minoff, minscale, mindenom;
        unsigned int minscore, origscore;
        int found;
        float fenc_var = fenc->i_pixel_ssd[plane] + !ref->i_pixel_ssd[plane];
        float ref_var  =  ref->i_pixel_ssd[plane] + !ref->i_pixel_ssd[plane];
        float guess_scale = sqrtf( fenc_var / ref_var );
        float fenc_mean = (float)fenc->i_pixel_sum[plane] / (fenc->i_lines[!!plane] * fenc->i_width[!!plane]) / (1 << (BIT_DEPTH - 8));
        float ref_mean  = (float) ref->i_pixel_sum[plane] / (fenc->i_lines[!!plane] * fenc->i_width[!!plane]) / (1 << (BIT_DEPTH - 8));

        //early termination
        if( fabsf( ref_mean - fenc_mean ) < 0.5f && fabsf( 1.f - guess_scale ) < epsilon )
        {
            SET_WEIGHT( weights[plane], 0, 1, 0, 0 );
            continue;
        }

        if( plane )
        {
            weights[plane].i_denom = 6;
            weights[plane].i_scale = x264_clip3( round( guess_scale * 64 ), 0, 255 );
            if( weights[plane].i_scale > 127 )
            {
                weights[1].weightfn = weights[2].weightfn = NULL;
                break;
            }
        }
        else
            x264_weight_get_h264( round( guess_scale * 128 ), 0, &weights[plane] );

        found = 0;
        mindenom = weights[plane].i_denom;
        minscale = weights[plane].i_scale;
        minoff = 0;

        pixel *mcbuf;
        if( !plane )
        {
            if( !fenc->b_intra_calculated )
            {
                x264_mb_analysis_t a;
                x264_lowres_context_init( h, &a );
                x264_slicetype_frame_cost( h, &a, &fenc, 0, 0, 0, 0 );
            }
            mcbuf = x264_weight_cost_init_luma( h, fenc, ref, h->mb.p_weight_buf[0] );
            origscore = minscore = x264_weight_cost_luma( h, fenc, mcbuf, NULL );
        }
        else
        {
            if( CHROMA444 )
            {
                mcbuf = x264_weight_cost_init_chroma444( h, fenc, ref, h->mb.p_weight_buf[0], plane );
                origscore = minscore = x264_weight_cost_chroma444( h, fenc, mcbuf, NULL, plane );
            }
            else
            {
                pixel *dstu = h->mb.p_weight_buf[0];
                pixel *dstv = h->mb.p_weight_buf[0]+fenc->i_stride[1]*fenc->i_lines[1];
                if( !chroma_initted++ )
                    x264_weight_cost_init_chroma( h, fenc, ref, dstu, dstv );
                mcbuf = plane == 1 ? dstu : dstv;
                origscore = minscore = x264_weight_cost_chroma( h, fenc, mcbuf, NULL );
            }
        }

        if( !minscore )
            continue;

        // This gives a slight improvement due to rounding errors but only tests one offset in lookahead.
        // Currently only searches within +/- 1 of the best offset found so far.
        // TODO: Try other offsets/multipliers/combinations thereof?
        cur_offset = fenc_mean - ref_mean * minscale / (1 << mindenom) + 0.5f * b_lookahead;
        start_offset = x264_clip3( cur_offset - !b_lookahead, -128, 127 );
        end_offset   = x264_clip3( cur_offset + !b_lookahead, -128, 127 );
        for( int i_off = start_offset; i_off <= end_offset; i_off++ )
        {
            SET_WEIGHT( weights[plane], 1, minscale, mindenom, i_off );
            unsigned int s;
            if( plane )
            {
                if( CHROMA444 )
                    s = x264_weight_cost_chroma444( h, fenc, mcbuf, &weights[plane], plane );
                else
                    s = x264_weight_cost_chroma( h, fenc, mcbuf, &weights[plane] );
            }
            else
                s = x264_weight_cost_luma( h, fenc, mcbuf, &weights[plane] );
            COPY3_IF_LT( minscore, s, minoff, i_off, found, 1 );

            // Don't check any more offsets if the previous one had a lower cost than the current one
            if( minoff == start_offset && i_off != start_offset )
                break;
        }
        x264_emms();

        /* FIXME: More analysis can be done here on SAD vs. SATD termination. */
        /* 0.2% termination derived experimentally to avoid weird weights in frames that are mostly intra. */
        if( !found || (minscale == 1 << mindenom && minoff == 0) || (float)minscore / origscore > 0.998f )
        {
            SET_WEIGHT( weights[plane], 0, 1, 0, 0 );
            continue;
        }
        else
            SET_WEIGHT( weights[plane], 1, minscale, mindenom, minoff );

        if( h->param.analyse.i_weighted_pred == X264_WEIGHTP_FAKE && weights[0].weightfn && !plane )
            fenc->f_weighted_cost_delta[i_delta_index] = (float)minscore / origscore;
    }

    //FIXME, what is the correct way to deal with this?
    if( weights[1].weightfn && weights[2].weightfn && weights[1].i_denom != weights[2].i_denom )
    {
        int denom = X264_MIN( weights[1].i_denom, weights[2].i_denom );
        int i;
        for( i = 1; i <= 2; i++ )
        {
            weights[i].i_scale = x264_clip3( weights[i].i_scale >> ( weights[i].i_denom - denom ), 0, 255 );
            weights[i].i_denom = denom;
            h->mc.weight_cache( h, &weights[i] );
        }
    }

    if( weights[0].weightfn && b_lookahead )
    {
        //scale lowres in lookahead for slicetype_frame_cost
        pixel *src = ref->buffer_lowres[0];
        pixel *dst = h->mb.p_weight_buf[0];
        int width = ref->i_width_lowres + PADH*2;
        int height = ref->i_lines_lowres + PADV*2;
        x264_weight_scale_plane( h, dst, ref->i_stride_lowres, src, ref->i_stride_lowres,
                                 width, height, &weights[0] );
        fenc->weighted[0] = h->mb.p_weight_buf[0] + PADH + ref->i_stride_lowres * PADV;
    }
}

static void x264_slicetype_mb_cost( x264_t *h, x264_mb_analysis_t *a,
                                    x264_frame_t **frames, int p0, int p1, int b,
                                    int dist_scale_factor, int do_search[2], const x264_weight_t *w )
{
    x264_frame_t *fref0 = frames[p0];
    x264_frame_t *fref1 = frames[p1];
    x264_frame_t *fenc  = frames[b];
    const int b_bidir = (b < p1);
    const int i_mb_x = h->mb.i_mb_x;
    const int i_mb_y = h->mb.i_mb_y;
    const int i_mb_stride = h->mb.i_mb_width;
    const int i_mb_xy = i_mb_x + i_mb_y * i_mb_stride;
    const int i_stride = fenc->i_stride_lowres;
    const int i_pel_offset = 8 * (i_mb_x + i_mb_y * i_stride);
    const int i_bipred_weight = h->param.analyse.b_weighted_bipred ? 64 - (dist_scale_factor>>2) : 32;
    int16_t (*fenc_mvs[2])[2] = { &fenc->lowres_mvs[0][b-p0-1][i_mb_xy], &fenc->lowres_mvs[1][p1-b-1][i_mb_xy] };
    int (*fenc_costs[2]) = { &fenc->lowres_mv_costs[0][b-p0-1][i_mb_xy], &fenc->lowres_mv_costs[1][p1-b-1][i_mb_xy] };
    int b_frame_score_mb = (i_mb_x > 0 && i_mb_x < h->mb.i_mb_width - 1 &&
                            i_mb_y > 0 && i_mb_y < h->mb.i_mb_height - 1) ||
                            h->mb.i_mb_width <= 2 || h->mb.i_mb_height <= 2;

    ALIGNED_ARRAY_16( pixel, pix1,[9*FDEC_STRIDE] );
    pixel *pix2 = pix1+8;
    x264_me_t m[2];
    int i_bcost = COST_MAX;
    int list_used = 0;

    h->mb.pic.p_fenc[0] = h->mb.pic.fenc_buf;
    h->mc.copy[PIXEL_8x8]( h->mb.pic.p_fenc[0], FENC_STRIDE, &fenc->lowres[0][i_pel_offset], i_stride, 8 );

    if( p0 == p1 )
        goto lowres_intra_mb;

    // no need for h->mb.mv_min[]
    h->mb.mv_min_fpel[0] = -8*h->mb.i_mb_x - 4;
    h->mb.mv_max_fpel[0] = 8*( h->mb.i_mb_width - h->mb.i_mb_x - 1 ) + 4;
    h->mb.mv_min_spel[0] = 4*( h->mb.mv_min_fpel[0] - 8 );
    h->mb.mv_max_spel[0] = 4*( h->mb.mv_max_fpel[0] + 8 );
    if( h->mb.i_mb_x >= h->mb.i_mb_width - 2 )
    {
        h->mb.mv_min_fpel[1] = -8*h->mb.i_mb_y - 4;
        h->mb.mv_max_fpel[1] = 8*( h->mb.i_mb_height - h->mb.i_mb_y - 1 ) + 4;
        h->mb.mv_min_spel[1] = 4*( h->mb.mv_min_fpel[1] - 8 );
        h->mb.mv_max_spel[1] = 4*( h->mb.mv_max_fpel[1] + 8 );
    }

#define LOAD_HPELS_LUMA(dst, src) \
    { \
        (dst)[0] = &(src)[0][i_pel_offset]; \
        (dst)[1] = &(src)[1][i_pel_offset]; \
        (dst)[2] = &(src)[2][i_pel_offset]; \
        (dst)[3] = &(src)[3][i_pel_offset]; \
    }
#define LOAD_WPELS_LUMA(dst,src) \
    (dst) = &(src)[i_pel_offset];

#define CLIP_MV( mv ) \
    { \
        mv[0] = x264_clip3( mv[0], h->mb.mv_min_spel[0], h->mb.mv_max_spel[0] ); \
        mv[1] = x264_clip3( mv[1], h->mb.mv_min_spel[1], h->mb.mv_max_spel[1] ); \
    }
#define TRY_BIDIR( mv0, mv1, penalty ) \
    { \
        int i_cost; \
        if( h->param.analyse.i_subpel_refine <= 1 ) \
        { \
            int hpel_idx1 = (((mv0)[0]&2)>>1) + ((mv0)[1]&2); \
            int hpel_idx2 = (((mv1)[0]&2)>>1) + ((mv1)[1]&2); \
            pixel *src1 = m[0].p_fref[hpel_idx1] + ((mv0)[0]>>2) + ((mv0)[1]>>2) * m[0].i_stride[0]; \
            pixel *src2 = m[1].p_fref[hpel_idx2] + ((mv1)[0]>>2) + ((mv1)[1]>>2) * m[1].i_stride[0]; \
            h->mc.avg[PIXEL_8x8]( pix1, 16, src1, m[0].i_stride[0], src2, m[1].i_stride[0], i_bipred_weight ); \
        } \
        else \
        { \
            int stride1 = 16, stride2 = 16; \
            pixel *src1, *src2; \
            src1 = h->mc.get_ref( pix1, &stride1, m[0].p_fref, m[0].i_stride[0], \
                                  (mv0)[0], (mv0)[1], 8, 8, w ); \
            src2 = h->mc.get_ref( pix2, &stride2, m[1].p_fref, m[1].i_stride[0], \
                                  (mv1)[0], (mv1)[1], 8, 8, w ); \
            h->mc.avg[PIXEL_8x8]( pix1, 16, src1, stride1, src2, stride2, i_bipred_weight ); \
        } \
        i_cost = penalty * a->i_lambda + h->pixf.mbcmp[PIXEL_8x8]( \
                           m[0].p_fenc[0], FENC_STRIDE, pix1, 16 ); \
        COPY2_IF_LT( i_bcost, i_cost, list_used, 3 ); \
    }

    m[0].i_pixel = PIXEL_8x8;
    m[0].p_cost_mv = a->p_cost_mv;
    m[0].i_stride[0] = i_stride;
    m[0].p_fenc[0] = h->mb.pic.p_fenc[0];
    m[0].weight = w;
    m[0].i_ref = 0;
    LOAD_HPELS_LUMA( m[0].p_fref, fref0->lowres );
    m[0].p_fref_w = m[0].p_fref[0];
    if( w[0].weightfn )
        LOAD_WPELS_LUMA( m[0].p_fref_w, fenc->weighted[0] );

    if( b_bidir )
    {
        int16_t *mvr = fref1->lowres_mvs[0][p1-p0-1][i_mb_xy];
        ALIGNED_ARRAY_8( int16_t, dmv,[2],[2] );

        m[1].i_pixel = PIXEL_8x8;
        m[1].p_cost_mv = a->p_cost_mv;
        m[1].i_stride[0] = i_stride;
        m[1].p_fenc[0] = h->mb.pic.p_fenc[0];
        m[1].i_ref = 0;
        m[1].weight = x264_weight_none;
        LOAD_HPELS_LUMA( m[1].p_fref, fref1->lowres );
        m[1].p_fref_w = m[1].p_fref[0];

        dmv[0][0] = ( mvr[0] * dist_scale_factor + 128 ) >> 8;
        dmv[0][1] = ( mvr[1] * dist_scale_factor + 128 ) >> 8;
        dmv[1][0] = dmv[0][0] - mvr[0];
        dmv[1][1] = dmv[0][1] - mvr[1];
        CLIP_MV( dmv[0] );
        CLIP_MV( dmv[1] );
        if( h->param.analyse.i_subpel_refine <= 1 )
            M64( dmv ) &= ~0x0001000100010001ULL; /* mv & ~1 */

        TRY_BIDIR( dmv[0], dmv[1], 0 );
        if( M64( dmv ) )
        {
            int i_cost;
            h->mc.avg[PIXEL_8x8]( pix1, 16, m[0].p_fref[0], m[0].i_stride[0], m[1].p_fref[0], m[1].i_stride[0], i_bipred_weight );
            i_cost = h->pixf.mbcmp[PIXEL_8x8]( m[0].p_fenc[0], FENC_STRIDE, pix1, 16 );
            COPY2_IF_LT( i_bcost, i_cost, list_used, 3 );
        }
    }

    for( int l = 0; l < 1 + b_bidir; l++ )
    {
        if( do_search[l] )
        {
            int i_mvc = 0;
            int16_t (*fenc_mv)[2] = fenc_mvs[l];
            ALIGNED_4( int16_t mvc[4][2] );

            /* Reverse-order MV prediction. */
            M32( mvc[0] ) = 0;
            M32( mvc[2] ) = 0;
#define MVC(mv) { CP32( mvc[i_mvc], mv ); i_mvc++; }
            if( i_mb_x < h->mb.i_mb_width - 1 )
                MVC( fenc_mv[1] );
            if( i_mb_y < h->mb.i_mb_height - 1 )
            {
                MVC( fenc_mv[i_mb_stride] );
                if( i_mb_x > 0 )
                    MVC( fenc_mv[i_mb_stride-1] );
                if( i_mb_x < h->mb.i_mb_width - 1 )
                    MVC( fenc_mv[i_mb_stride+1] );
            }
#undef MVC
            if( i_mvc <= 1 )
                CP32( m[l].mvp, mvc[0] );
            else
                x264_median_mv( m[l].mvp, mvc[0], mvc[1], mvc[2] );

            /* Fast skip for cases of near-zero residual.  Shortcut: don't bother except in the mv0 case,
             * since anything else is likely to have enough residual to not trigger the skip. */
            if( !M32( m[l].mvp ) )
            {
                m[l].cost = h->pixf.mbcmp[PIXEL_8x8]( m[l].p_fenc[0], FENC_STRIDE, m[l].p_fref[0], m[l].i_stride[0] );
                if( m[l].cost < 64 )
                {
                    M32( m[l].mv ) = 0;
                    goto skip_motionest;
                }
            }

            x264_me_search( h, &m[l], mvc, i_mvc );
            m[l].cost -= a->p_cost_mv[0]; // remove mvcost from skip mbs
            if( M32( m[l].mv ) )
                m[l].cost += 5 * a->i_lambda;

skip_motionest:
            CP32( fenc_mvs[l], m[l].mv );
            *fenc_costs[l] = m[l].cost;
        }
        else
        {
            CP32( m[l].mv, fenc_mvs[l] );
            m[l].cost = *fenc_costs[l];
        }
        COPY2_IF_LT( i_bcost, m[l].cost, list_used, l+1 );
    }

    if( b_bidir && ( M32( m[0].mv ) || M32( m[1].mv ) ) )
        TRY_BIDIR( m[0].mv, m[1].mv, 5 );

lowres_intra_mb:
    if( !fenc->b_intra_calculated )
    {
        ALIGNED_ARRAY_16( pixel, edge,[36] );
        pixel *pix = &pix1[8+FDEC_STRIDE - 1];
        pixel *src = &fenc->lowres[0][i_pel_offset - 1];
        const int intra_penalty = 5 * a->i_lambda;
        int satds[3];

        memcpy( pix-FDEC_STRIDE, src-i_stride, 17 * sizeof(pixel) );
        for( int i = 0; i < 8; i++ )
            pix[i*FDEC_STRIDE] = src[i*i_stride];
        pix++;

        h->pixf.intra_mbcmp_x3_8x8c( h->mb.pic.p_fenc[0], pix, satds );
        int i_icost = X264_MIN3( satds[0], satds[1], satds[2] );

        if( h->param.analyse.i_subpel_refine > 1 )
        {
            h->predict_8x8c[I_PRED_CHROMA_P]( pix );
            int satd = h->pixf.mbcmp[PIXEL_8x8]( pix, FDEC_STRIDE, h->mb.pic.p_fenc[0], FENC_STRIDE );
            i_icost = X264_MIN( i_icost, satd );
            h->predict_8x8_filter( pix, edge, ALL_NEIGHBORS, ALL_NEIGHBORS );
            for( int i = 3; i < 9; i++ )
            {
                h->predict_8x8[i]( pix, edge );
                satd = h->pixf.mbcmp[PIXEL_8x8]( pix, FDEC_STRIDE, h->mb.pic.p_fenc[0], FENC_STRIDE );
                i_icost = X264_MIN( i_icost, satd );
            }
        }

        i_icost += intra_penalty;
        fenc->i_intra_cost[i_mb_xy] = i_icost;
        int i_icost_aq = i_icost;
        if( h->param.rc.i_aq_mode )
            i_icost_aq = (i_icost_aq * fenc->i_inv_qscale_factor[i_mb_xy] + 128) >> 8;
        fenc->i_row_satds[0][0][h->mb.i_mb_y] += i_icost_aq;
        if( b_frame_score_mb )
        {
            fenc->i_cost_est[0][0] += i_icost;
            fenc->i_cost_est_aq[0][0] += i_icost_aq;
        }
    }

    /* forbid intra-mbs in B-frames, because it's rare and not worth checking */
    /* FIXME: Should we still forbid them now that we cache intra scores? */
    if( !b_bidir )
    {
        int i_icost = fenc->i_intra_cost[i_mb_xy];
        int b_intra = i_icost < i_bcost;
        if( b_intra )
        {
            i_bcost = i_icost;
            list_used = 0;
        }
        if( b_frame_score_mb )
            fenc->i_intra_mbs[b-p0] += b_intra;
    }

    /* In an I-frame, we've already added the results above in the intra section. */
    if( p0 != p1 )
    {
        int i_bcost_aq = i_bcost;
        if( h->param.rc.i_aq_mode )
            i_bcost_aq = (i_bcost_aq * fenc->i_inv_qscale_factor[i_mb_xy] + 128) >> 8;
        fenc->i_row_satds[b-p0][p1-b][h->mb.i_mb_y] += i_bcost_aq;
        if( b_frame_score_mb )
        {
            /* Don't use AQ-weighted costs for slicetype decision, only for ratecontrol. */
            fenc->i_cost_est[b-p0][p1-b] += i_bcost;
            fenc->i_cost_est_aq[b-p0][p1-b] += i_bcost_aq;
        }
    }

    fenc->lowres_costs[b-p0][p1-b][i_mb_xy] = X264_MIN( i_bcost, LOWRES_COST_MASK ) + (list_used << LOWRES_COST_SHIFT);
}
#undef TRY_BIDIR

#define NUM_MBS\
   (h->mb.i_mb_width > 2 && h->mb.i_mb_height > 2 ?\
   (h->mb.i_mb_width - 2) * (h->mb.i_mb_height - 2) :\
    h->mb.i_mb_width * h->mb.i_mb_height)

static int x264_slicetype_frame_cost( x264_t *h, x264_mb_analysis_t *a,
                                      x264_frame_t **frames, int p0, int p1, int b,
                                      int b_intra_penalty )
{
    int i_score = 0;
    int do_search[2];
    const x264_weight_t *w = x264_weight_none;
    /* Check whether we already evaluated this frame
     * If we have tried this frame as P, then we have also tried
     * the preceding frames as B. (is this still true?) */
    /* Also check that we already calculated the row SATDs for the current frame. */
    if( frames[b]->i_cost_est[b-p0][p1-b] >= 0 && (!h->param.rc.i_vbv_buffer_size || frames[b]->i_row_satds[b-p0][p1-b][0] != -1) )
        i_score = frames[b]->i_cost_est[b-p0][p1-b];
    else
    {
        int dist_scale_factor = 128;
        int *row_satd = frames[b]->i_row_satds[b-p0][p1-b];
        int *row_satd_intra = frames[b]->i_row_satds[0][0];

        /* For each list, check to see whether we have lowres motion-searched this reference frame before. */
        do_search[0] = b != p0 && frames[b]->lowres_mvs[0][b-p0-1][0][0] == 0x7FFF;
        do_search[1] = b != p1 && frames[b]->lowres_mvs[1][p1-b-1][0][0] == 0x7FFF;
        if( do_search[0] )
        {
            if( h->param.analyse.i_weighted_pred && b == p1 )
            {
                x264_emms();
                x264_weights_analyse( h, frames[b], frames[p0], 1 );
                w = frames[b]->weight[0];
            }
            frames[b]->lowres_mvs[0][b-p0-1][0][0] = 0;
        }
        if( do_search[1] ) frames[b]->lowres_mvs[1][p1-b-1][0][0] = 0;

        if( b == p1 )
            frames[b]->i_intra_mbs[b-p0] = 0;
        if( !frames[b]->b_intra_calculated )
        {
            frames[b]->i_cost_est[0][0] = 0;
            frames[b]->i_cost_est_aq[0][0] = 0;
        }
        if( p1 != p0 )
            dist_scale_factor = ( ((b-p0) << 8) + ((p1-p0) >> 1) ) / (p1-p0);

        frames[b]->i_cost_est[b-p0][p1-b] = 0;
        frames[b]->i_cost_est_aq[b-p0][p1-b] = 0;

        /* Lowres lookahead goes backwards because the MVs are used as predictors in the main encode.
         * This considerably improves MV prediction overall. */

        /* The edge mbs seem to reduce the predictive quality of the
         * whole frame's score, but are needed for a spatial distribution. */
        if( h->param.rc.b_mb_tree || h->param.rc.i_vbv_buffer_size ||
            h->mb.i_mb_width <= 2 || h->mb.i_mb_height <= 2 )
        {
            for( h->mb.i_mb_y = h->mb.i_mb_height - 1; h->mb.i_mb_y >= 0; h->mb.i_mb_y-- )
            {
                row_satd[h->mb.i_mb_y] = 0;
                if( !frames[b]->b_intra_calculated )
                    row_satd_intra[h->mb.i_mb_y] = 0;
                for( h->mb.i_mb_x = h->mb.i_mb_width - 1; h->mb.i_mb_x >= 0; h->mb.i_mb_x-- )
                    x264_slicetype_mb_cost( h, a, frames, p0, p1, b, dist_scale_factor, do_search, w );
            }
        }
        else
        {
            for( h->mb.i_mb_y = h->mb.i_mb_height - 2; h->mb.i_mb_y >= 1; h->mb.i_mb_y-- )
                for( h->mb.i_mb_x = h->mb.i_mb_width - 2; h->mb.i_mb_x >= 1; h->mb.i_mb_x-- )
                    x264_slicetype_mb_cost( h, a, frames, p0, p1, b, dist_scale_factor, do_search, w );
        }

        i_score = frames[b]->i_cost_est[b-p0][p1-b];
        if( b != p1 )
            i_score = (uint64_t)i_score * 100 / (120 + h->param.i_bframe_bias);
        else
            frames[b]->b_intra_calculated = 1;

        frames[b]->i_cost_est[b-p0][p1-b] = i_score;
        x264_emms();
    }

    if( b_intra_penalty )
    {
        // arbitrary penalty for I-blocks after B-frames
        int nmb = NUM_MBS;
        i_score += (uint64_t)i_score * frames[b]->i_intra_mbs[b-p0] / (nmb * 8);
    }
    return i_score;
}

/* If MB-tree changes the quantizers, we need to recalculate the frame cost without
 * re-running lookahead. */
static int x264_slicetype_frame_cost_recalculate( x264_t *h, x264_frame_t **frames, int p0, int p1, int b )
{
    int i_score = 0;
    int *row_satd = frames[b]->i_row_satds[b-p0][p1-b];
    float *qp_offset = IS_X264_TYPE_B(frames[b]->i_type) ? frames[b]->f_qp_offset_aq : frames[b]->f_qp_offset;
    x264_emms();
    for( h->mb.i_mb_y = h->mb.i_mb_height - 1; h->mb.i_mb_y >= 0; h->mb.i_mb_y-- )
    {
        row_satd[ h->mb.i_mb_y ] = 0;
        for( h->mb.i_mb_x = h->mb.i_mb_width - 1; h->mb.i_mb_x >= 0; h->mb.i_mb_x-- )
        {
            int i_mb_xy = h->mb.i_mb_x + h->mb.i_mb_y*h->mb.i_mb_stride;
            int i_mb_cost = frames[b]->lowres_costs[b-p0][p1-b][i_mb_xy] & LOWRES_COST_MASK;
            float qp_adj = qp_offset[i_mb_xy];
            i_mb_cost = (i_mb_cost * x264_exp2fix8(qp_adj) + 128) >> 8;
            row_satd[ h->mb.i_mb_y ] += i_mb_cost;
            if( (h->mb.i_mb_y > 0 && h->mb.i_mb_y < h->mb.i_mb_height - 1 &&
                 h->mb.i_mb_x > 0 && h->mb.i_mb_x < h->mb.i_mb_width - 1) ||
                 h->mb.i_mb_width <= 2 || h->mb.i_mb_height <= 2 )
            {
                i_score += i_mb_cost;
            }
        }
    }
    return i_score;
}

static void x264_macroblock_tree_finish( x264_t *h, x264_frame_t *frame, float average_duration, int ref0_distance )
{
    int fps_factor = round( CLIP_DURATION(average_duration) / CLIP_DURATION(frame->f_duration) * 256 );
    float weightdelta = 0.0;
    if( ref0_distance && frame->f_weighted_cost_delta[ref0_distance-1] > 0 )
        weightdelta = (1.0 - frame->f_weighted_cost_delta[ref0_distance-1]);

    /* Allow the strength to be adjusted via qcompress, since the two
     * concepts are very similar. */
    float strength = 5.0f * (1.0f - h->param.rc.f_qcompress);
    for( int mb_index = 0; mb_index < h->mb.i_mb_count; mb_index++ )
    {
        int intra_cost = (frame->i_intra_cost[mb_index] * frame->i_inv_qscale_factor[mb_index] + 128) >> 8;
        if( intra_cost )
        {
            int propagate_cost = (frame->i_propagate_cost[mb_index] * fps_factor + 128) >> 8;
            float log2_ratio = x264_log2(intra_cost + propagate_cost) - x264_log2(intra_cost) + weightdelta;
            frame->f_qp_offset[mb_index] = frame->f_qp_offset_aq[mb_index] - strength * log2_ratio;
        }
    }
}

static void x264_macroblock_tree_propagate( x264_t *h, x264_frame_t **frames, float average_duration, int p0, int p1, int b, int referenced )
{
    uint16_t *ref_costs[2] = {frames[p0]->i_propagate_cost,frames[p1]->i_propagate_cost};
    int dist_scale_factor = ( ((b-p0) << 8) + ((p1-p0) >> 1) ) / (p1-p0);
    int i_bipred_weight = h->param.analyse.b_weighted_bipred ? 64 - (dist_scale_factor>>2) : 32;
    int16_t (*mvs[2])[2] = { frames[b]->lowres_mvs[0][b-p0-1], frames[b]->lowres_mvs[1][p1-b-1] };
    int bipred_weights[2] = {i_bipred_weight, 64 - i_bipred_weight};
    int *buf = h->scratch_buffer;
    uint16_t *propagate_cost = frames[b]->i_propagate_cost;

    x264_emms();
    float fps_factor = CLIP_DURATION(frames[b]->f_duration) / CLIP_DURATION(average_duration);

    /* For non-reffed frames the source costs are always zero, so just memset one row and re-use it. */
    if( !referenced )
        memset( frames[b]->i_propagate_cost, 0, h->mb.i_mb_width * sizeof(uint16_t) );

    for( h->mb.i_mb_y = 0; h->mb.i_mb_y < h->mb.i_mb_height; h->mb.i_mb_y++ )
    {
        int mb_index = h->mb.i_mb_y*h->mb.i_mb_stride;
        h->mc.mbtree_propagate_cost( buf, propagate_cost,
            frames[b]->i_intra_cost+mb_index, frames[b]->lowres_costs[b-p0][p1-b]+mb_index,
            frames[b]->i_inv_qscale_factor+mb_index, &fps_factor, h->mb.i_mb_width );
        if( referenced )
            propagate_cost += h->mb.i_mb_width;
        for( h->mb.i_mb_x = 0; h->mb.i_mb_x < h->mb.i_mb_width; h->mb.i_mb_x++, mb_index++ )
        {
            int propagate_amount = buf[h->mb.i_mb_x];
            /* Don't propagate for an intra block. */
            if( propagate_amount > 0 )
            {
                /* Access width-2 bitfield. */
                int lists_used = frames[b]->lowres_costs[b-p0][p1-b][mb_index] >> LOWRES_COST_SHIFT;
                /* Follow the MVs to the previous frame(s). */
                for( int list = 0; list < 2; list++ )
                    if( (lists_used >> list)&1 )
                    {
#define CLIP_ADD(s,x) (s) = X264_MIN((s)+(x),(1<<16)-1)
                        int listamount = propagate_amount;
                        /* Apply bipred weighting. */
                        if( lists_used == 3 )
                            listamount = (listamount * bipred_weights[list] + 32) >> 6;

                        /* Early termination for simple case of mv0. */
                        if( !M32( mvs[list][mb_index] ) )
                        {
                            CLIP_ADD( ref_costs[list][mb_index], listamount );
                            continue;
                        }

                        int x = mvs[list][mb_index][0];
                        int y = mvs[list][mb_index][1];
                        int mbx = (x>>5)+h->mb.i_mb_x;
                        int mby = (y>>5)+h->mb.i_mb_y;
                        int idx0 = mbx + mby * h->mb.i_mb_stride;
                        int idx1 = idx0 + 1;
                        int idx2 = idx0 + h->mb.i_mb_stride;
                        int idx3 = idx0 + h->mb.i_mb_stride + 1;
                        x &= 31;
                        y &= 31;
                        int idx0weight = (32-y)*(32-x);
                        int idx1weight = (32-y)*x;
                        int idx2weight = y*(32-x);
                        int idx3weight = y*x;

                        /* We could just clip the MVs, but pixels that lie outside the frame probably shouldn't
                         * be counted. */
                        if( mbx < h->mb.i_mb_width-1 && mby < h->mb.i_mb_height-1 && mbx >= 0 && mby >= 0 )
                        {
                            CLIP_ADD( ref_costs[list][idx0], (listamount*idx0weight+512)>>10 );
                            CLIP_ADD( ref_costs[list][idx1], (listamount*idx1weight+512)>>10 );
                            CLIP_ADD( ref_costs[list][idx2], (listamount*idx2weight+512)>>10 );
                            CLIP_ADD( ref_costs[list][idx3], (listamount*idx3weight+512)>>10 );
                        }
                        else /* Check offsets individually */
                        {
                            if( mbx < h->mb.i_mb_width && mby < h->mb.i_mb_height && mbx >= 0 && mby >= 0 )
                                CLIP_ADD( ref_costs[list][idx0], (listamount*idx0weight+512)>>10 );
                            if( mbx+1 < h->mb.i_mb_width && mby < h->mb.i_mb_height && mbx+1 >= 0 && mby >= 0 )
                                CLIP_ADD( ref_costs[list][idx1], (listamount*idx1weight+512)>>10 );
                            if( mbx < h->mb.i_mb_width && mby+1 < h->mb.i_mb_height && mbx >= 0 && mby+1 >= 0 )
                                CLIP_ADD( ref_costs[list][idx2], (listamount*idx2weight+512)>>10 );
                            if( mbx+1 < h->mb.i_mb_width && mby+1 < h->mb.i_mb_height && mbx+1 >= 0 && mby+1 >= 0 )
                                CLIP_ADD( ref_costs[list][idx3], (listamount*idx3weight+512)>>10 );
                        }
                    }
            }
        }
    }

    if( h->param.rc.i_vbv_buffer_size && h->param.rc.i_lookahead && referenced )
        x264_macroblock_tree_finish( h, frames[b], average_duration, b == p1 ? b - p0 : 0 );
}

static void x264_macroblock_tree( x264_t *h, x264_mb_analysis_t *a, x264_frame_t **frames, int num_frames, int b_intra )
{
    int idx = !b_intra;
    int last_nonb, cur_nonb = 1;
    int bframes = 0;

    x264_emms();
    float total_duration = 0.0;
    for( int j = 0; j <= num_frames; j++ )
        total_duration += frames[j]->f_duration;
    float average_duration = total_duration / (num_frames + 1);

    int i = num_frames;

    if( b_intra )
        x264_slicetype_frame_cost( h, a, frames, 0, 0, 0, 0 );

    while( i > 0 && frames[i]->i_type == X264_TYPE_B )
        i--;
    last_nonb = i;

    /* Lookaheadless MB-tree is not a theoretically distinct case; the same extrapolation could
     * be applied to the end of a lookahead buffer of any size.  However, it's most needed when
     * lookahead=0, so that's what's currently implemented. */
    if( !h->param.rc.i_lookahead )
    {
        if( b_intra )
        {
            memset( frames[0]->i_propagate_cost, 0, h->mb.i_mb_count * sizeof(uint16_t) );
            memcpy( frames[0]->f_qp_offset, frames[0]->f_qp_offset_aq, h->mb.i_mb_count * sizeof(float) );
            return;
        }
        XCHG( uint16_t*, frames[last_nonb]->i_propagate_cost, frames[0]->i_propagate_cost );
        memset( frames[0]->i_propagate_cost, 0, h->mb.i_mb_count * sizeof(uint16_t) );
    }
    else
    {
        if( last_nonb < idx )
            return;
        memset( frames[last_nonb]->i_propagate_cost, 0, h->mb.i_mb_count * sizeof(uint16_t) );
    }

    while( i-- > idx )
    {
        cur_nonb = i;
        while( frames[cur_nonb]->i_type == X264_TYPE_B && cur_nonb > 0 )
            cur_nonb--;
        if( cur_nonb < idx )
            break;
        x264_slicetype_frame_cost( h, a, frames, cur_nonb, last_nonb, last_nonb, 0 );
        memset( frames[cur_nonb]->i_propagate_cost, 0, h->mb.i_mb_count * sizeof(uint16_t) );
        bframes = last_nonb - cur_nonb - 1;
        if( h->param.i_bframe_pyramid && bframes > 1 )
        {
            int middle = (bframes + 1)/2 + cur_nonb;
            x264_slicetype_frame_cost( h, a, frames, cur_nonb, last_nonb, middle, 0 );
            memset( frames[middle]->i_propagate_cost, 0, h->mb.i_mb_count * sizeof(uint16_t) );
            while( i > cur_nonb )
            {
                int p0 = i > middle ? middle : cur_nonb;
                int p1 = i < middle ? middle : last_nonb;
                if( i != middle )
                {
                    x264_slicetype_frame_cost( h, a, frames, p0, p1, i, 0 );
                    x264_macroblock_tree_propagate( h, frames, average_duration, p0, p1, i, 0 );
                }
                i--;
            }
            x264_macroblock_tree_propagate( h, frames, average_duration, cur_nonb, last_nonb, middle, 1 );
        }
        else
        {
            while( i > cur_nonb )
            {
                x264_slicetype_frame_cost( h, a, frames, cur_nonb, last_nonb, i, 0 );
                x264_macroblock_tree_propagate( h, frames, average_duration, cur_nonb, last_nonb, i, 0 );
                i--;
            }
        }
        x264_macroblock_tree_propagate( h, frames, average_duration, cur_nonb, last_nonb, last_nonb, 1 );
        last_nonb = cur_nonb;
    }

    if( !h->param.rc.i_lookahead )
    {
        x264_macroblock_tree_propagate( h, frames, average_duration, 0, last_nonb, last_nonb, 1 );
        XCHG( uint16_t*, frames[last_nonb]->i_propagate_cost, frames[0]->i_propagate_cost );
    }

    x264_macroblock_tree_finish( h, frames[last_nonb], average_duration, last_nonb );
    if( h->param.i_bframe_pyramid && bframes > 1 && !h->param.rc.i_vbv_buffer_size )
        x264_macroblock_tree_finish( h, frames[last_nonb+(bframes+1)/2], average_duration, 0 );
}

static int x264_vbv_frame_cost( x264_t *h, x264_mb_analysis_t *a, x264_frame_t **frames, int p0, int p1, int b )
{
    int cost = x264_slicetype_frame_cost( h, a, frames, p0, p1, b, 0 );
    if( h->param.rc.i_aq_mode )
    {
        if( h->param.rc.b_mb_tree )
            return x264_slicetype_frame_cost_recalculate( h, frames, p0, p1, b );
        else
            return frames[b]->i_cost_est_aq[b-p0][p1-b];
    }
    return cost;
}

static void x264_calculate_durations( x264_t *h, x264_frame_t *cur_frame, x264_frame_t *prev_frame, int64_t *i_cpb_delay, int64_t *i_coded_fields )
{
    cur_frame->i_cpb_delay = *i_cpb_delay;
    cur_frame->i_dpb_output_delay = cur_frame->i_field_cnt - *i_coded_fields;

    // add a correction term for frame reordering
    cur_frame->i_dpb_output_delay += h->sps->vui.i_num_reorder_frames*2;

    // fix possible negative dpb_output_delay because of pulldown changes and reordering
    if( cur_frame->i_dpb_output_delay < 0 )
    {
        cur_frame->i_cpb_delay += cur_frame->i_dpb_output_delay;
        cur_frame->i_dpb_output_delay = 0;
        if( prev_frame )
            prev_frame->i_cpb_duration += cur_frame->i_dpb_output_delay;
    }

    if( cur_frame->b_keyframe )
        *i_cpb_delay = 0;

    *i_cpb_delay += cur_frame->i_duration;
    *i_coded_fields += cur_frame->i_duration;
    cur_frame->i_cpb_duration = cur_frame->i_duration;
}

static void x264_vbv_lookahead( x264_t *h, x264_mb_analysis_t *a, x264_frame_t **frames, int num_frames, int keyframe )
{
    int last_nonb = 0, cur_nonb = 1, idx = 0;
    x264_frame_t *prev_frame = NULL;
    int prev_frame_idx = 0;
    while( cur_nonb < num_frames && frames[cur_nonb]->i_type == X264_TYPE_B )
        cur_nonb++;
    int next_nonb = keyframe ? last_nonb : cur_nonb;

    if( frames[cur_nonb]->i_coded_fields_lookahead >= 0 )
    {
        h->i_coded_fields_lookahead = frames[cur_nonb]->i_coded_fields_lookahead;
        h->i_cpb_delay_lookahead = frames[cur_nonb]->i_cpb_delay_lookahead;
    }

    while( cur_nonb < num_frames )
    {
        /* P/I cost: This shouldn't include the cost of next_nonb */
        if( next_nonb != cur_nonb )
        {
            int p0 = IS_X264_TYPE_I( frames[cur_nonb]->i_type ) ? cur_nonb : last_nonb;
            frames[next_nonb]->i_planned_satd[idx] = x264_vbv_frame_cost( h, a, frames, p0, cur_nonb, cur_nonb );
            frames[next_nonb]->i_planned_type[idx] = frames[cur_nonb]->i_type;
            frames[cur_nonb]->i_coded_fields_lookahead = h->i_coded_fields_lookahead;
            frames[cur_nonb]->i_cpb_delay_lookahead = h->i_cpb_delay_lookahead;
            x264_calculate_durations( h, frames[cur_nonb], prev_frame, &h->i_cpb_delay_lookahead, &h->i_coded_fields_lookahead );
            if( prev_frame )
            {
                frames[next_nonb]->f_planned_cpb_duration[prev_frame_idx] = (double)prev_frame->i_cpb_duration *
                                                                            h->sps->vui.i_num_units_in_tick / h->sps->vui.i_time_scale;
            }
            frames[next_nonb]->f_planned_cpb_duration[idx] = (double)frames[cur_nonb]->i_cpb_duration *
                                                             h->sps->vui.i_num_units_in_tick / h->sps->vui.i_time_scale;
            prev_frame = frames[cur_nonb];
            prev_frame_idx = idx;
            idx++;
        }
        /* Handle the B-frames: coded order */
        for( int i = last_nonb+1; i < cur_nonb; i++, idx++ )
        {
            frames[next_nonb]->i_planned_satd[idx] = x264_vbv_frame_cost( h, a, frames, last_nonb, cur_nonb, i );
            frames[next_nonb]->i_planned_type[idx] = X264_TYPE_B;
            frames[i]->i_coded_fields_lookahead = h->i_coded_fields_lookahead;
            frames[i]->i_cpb_delay_lookahead = h->i_cpb_delay_lookahead;
            x264_calculate_durations( h, frames[i], prev_frame, &h->i_cpb_delay_lookahead, &h->i_coded_fields_lookahead );
            if( prev_frame )
            {
                frames[next_nonb]->f_planned_cpb_duration[prev_frame_idx] = (double)prev_frame->i_cpb_duration *
                                                                            h->sps->vui.i_num_units_in_tick / h->sps->vui.i_time_scale;
            }
            frames[next_nonb]->f_planned_cpb_duration[idx] = (double)frames[i]->i_cpb_duration *
                                                             h->sps->vui.i_num_units_in_tick / h->sps->vui.i_time_scale;
            prev_frame = frames[i];
            prev_frame_idx = idx;
        }
        last_nonb = cur_nonb;
        cur_nonb++;
        while( cur_nonb <= num_frames && frames[cur_nonb]->i_type == X264_TYPE_B )
            cur_nonb++;
    }
    frames[next_nonb]->i_planned_type[idx] = X264_TYPE_AUTO;
}

static int x264_slicetype_path_cost( x264_t *h, x264_mb_analysis_t *a, x264_frame_t **frames, char *path, int threshold )
{
    int loc = 1;
    int cost = 0;
    int cur_p = 0;
    path--; /* Since the 1st path element is really the second frame */
    while( path[loc] )
    {
        int next_p = loc;
        /* Find the location of the next P-frame. */
        while( path[next_p] != 'P' )
            next_p++;

        /* Add the cost of the P-frame found above */
        cost += x264_slicetype_frame_cost( h, a, frames, cur_p, next_p, next_p, 0 );
        /* Early terminate if the cost we have found is larger than the best path cost so far */
        if( cost > threshold )
            break;

        if( h->param.i_bframe_pyramid && next_p - cur_p > 2 )
        {
            int middle = cur_p + (next_p - cur_p)/2;
            cost += x264_slicetype_frame_cost( h, a, frames, cur_p, next_p, middle, 0 );
            for( int next_b = loc; next_b < middle && cost < threshold; next_b++ )
                cost += x264_slicetype_frame_cost( h, a, frames, cur_p, middle, next_b, 0 );
            for( int next_b = middle+1; next_b < next_p && cost < threshold; next_b++ )
                cost += x264_slicetype_frame_cost( h, a, frames, middle, next_p, next_b, 0 );
        }
        else
            for( int next_b = loc; next_b < next_p && cost < threshold; next_b++ )
                cost += x264_slicetype_frame_cost( h, a, frames, cur_p, next_p, next_b, 0 );

        loc = next_p + 1;
        cur_p = next_p;
    }
    return cost;
}

/* Viterbi/trellis slicetype decision algorithm. */
/* Uses strings due to the fact that the speed of the control functions is
   negligible compared to the cost of running slicetype_frame_cost, and because
   it makes debugging easier. */
static void x264_slicetype_path( x264_t *h, x264_mb_analysis_t *a, x264_frame_t **frames, int length, char (*best_paths)[X264_LOOKAHEAD_MAX+1] )
{
    char paths[2][X264_LOOKAHEAD_MAX+1];
    int num_paths = X264_MIN( h->param.i_bframe+1, length );
    int best_cost = COST_MAX;
    int idx = 0;

    /* Iterate over all currently possible paths */
    for( int path = 0; path < num_paths; path++ )
    {
        /* Add suffixes to the current path */
        int len = length - (path + 1);
        memcpy( paths[idx], best_paths[len % (X264_BFRAME_MAX+1)], len );
        memset( paths[idx]+len, 'B', path );
        strcpy( paths[idx]+len+path, "P" );

        /* Calculate the actual cost of the current path */
        int cost = x264_slicetype_path_cost( h, a, frames, paths[idx], best_cost );
        if( cost < best_cost )
        {
            best_cost = cost;
            idx ^= 1;
        }
    }

    /* Store the best path. */
    memcpy( best_paths[length % (X264_BFRAME_MAX+1)], paths[idx^1], length );
}

static int scenecut_internal( x264_t *h, x264_mb_analysis_t *a, x264_frame_t **frames, int p0, int p1, int real_scenecut )
{
    x264_frame_t *frame = frames[p1];

    /* Don't do scenecuts on the right view of a frame-packed video. */
    if( real_scenecut && h->param.i_frame_packing == 5 && (frame->i_frame&1) )
        return 0;

    x264_slicetype_frame_cost( h, a, frames, p0, p1, p1, 0 );

    int icost = frame->i_cost_est[0][0];
    int pcost = frame->i_cost_est[p1-p0][0];
    float f_bias;
    int i_gop_size = frame->i_frame - h->lookahead->i_last_keyframe;
    float f_thresh_max = h->param.i_scenecut_threshold / 100.0;
    /* magic numbers pulled out of thin air */
    float f_thresh_min = f_thresh_max * 0.25;
    int res;

    if( h->param.i_keyint_min == h->param.i_keyint_max )
        f_thresh_min = f_thresh_max;
    if( i_gop_size <= h->param.i_keyint_min / 4 || h->param.b_intra_refresh )
        f_bias = f_thresh_min / 4;
    else if( i_gop_size <= h->param.i_keyint_min )
        f_bias = f_thresh_min * i_gop_size / h->param.i_keyint_min;
    else
    {
        f_bias = f_thresh_min
                 + ( f_thresh_max - f_thresh_min )
                 * ( i_gop_size - h->param.i_keyint_min )
                 / ( h->param.i_keyint_max - h->param.i_keyint_min );
    }

    res = pcost >= (1.0 - f_bias) * icost;
    if( res && real_scenecut )
    {
        int imb = frame->i_intra_mbs[p1-p0];
        int pmb = NUM_MBS - imb;
        x264_log( h, X264_LOG_DEBUG, "scene cut at %d Icost:%d Pcost:%d ratio:%.4f bias:%.4f gop:%d (imb:%d pmb:%d)\n",
                  frame->i_frame,
                  icost, pcost, 1. - (double)pcost / icost,
                  f_bias, i_gop_size, imb, pmb );
    }
    return res;
}

static int scenecut( x264_t *h, x264_mb_analysis_t *a, x264_frame_t **frames, int p0, int p1, int real_scenecut, int num_frames, int i_max_search )
{
    /* Only do analysis during a normal scenecut check. */
    if( real_scenecut && h->param.i_bframe )
    {
        int origmaxp1 = p0 + 1;
        /* Look ahead to avoid coding short flashes as scenecuts. */
        if( h->param.i_bframe_adaptive == X264_B_ADAPT_TRELLIS )
            /* Don't analyse any more frames than the trellis would have covered. */
            origmaxp1 += h->param.i_bframe;
        else
            origmaxp1++;
        int maxp1 = X264_MIN( origmaxp1, num_frames );

        /* Where A and B are scenes: AAAAAABBBAAAAAA
         * If BBB is shorter than (maxp1-p0), it is detected as a flash
         * and not considered a scenecut. */
        for( int curp1 = p1; curp1 <= maxp1; curp1++ )
            if( !scenecut_internal( h, a, frames, p0, curp1, 0 ) )
                /* Any frame in between p0 and cur_p1 cannot be a real scenecut. */
                for( int i = curp1; i > p0; i-- )
                    frames[i]->b_scenecut = 0;

        /* Where A-F are scenes: AAAAABBCCDDEEFFFFFF
         * If each of BB ... EE are shorter than (maxp1-p0), they are
         * detected as flashes and not considered scenecuts.
         * Instead, the first F frame becomes a scenecut.
         * If the video ends before F, no frame becomes a scenecut. */
        for( int curp0 = p0; curp0 <= maxp1; curp0++ )
            if( origmaxp1 > i_max_search || (curp0 < maxp1 && scenecut_internal( h, a, frames, curp0, maxp1, 0 )) )
                /* If cur_p0 is the p0 of a scenecut, it cannot be the p1 of a scenecut. */
                    frames[curp0]->b_scenecut = 0;
    }

    /* Ignore frames that are part of a flash, i.e. cannot be real scenecuts. */
    if( !frames[p1]->b_scenecut )
        return 0;
    return scenecut_internal( h, a, frames, p0, p1, real_scenecut );
}

void x264_slicetype_analyse( x264_t *h, int keyframe )
{
    x264_mb_analysis_t a;
    x264_frame_t *frames[X264_LOOKAHEAD_MAX+3] = { NULL, };
    int num_frames, orig_num_frames, keyint_limit, framecnt;
    int i_mb_count = NUM_MBS;
    int cost1p0, cost2p0, cost1b1, cost2p1;
    int i_max_search = X264_MIN( h->lookahead->next.i_size, X264_LOOKAHEAD_MAX );
    int vbv_lookahead = h->param.rc.i_vbv_buffer_size && h->param.rc.i_lookahead;
    if( h->param.b_deterministic )
        i_max_search = X264_MIN( i_max_search, h->lookahead->i_slicetype_length + !keyframe );

    assert( h->frames.b_have_lowres );

    if( !h->lookahead->last_nonb )
        return;
    frames[0] = h->lookahead->last_nonb;
    for( framecnt = 0; framecnt < i_max_search && h->lookahead->next.list[framecnt]->i_type == X264_TYPE_AUTO; framecnt++ )
        frames[framecnt+1] = h->lookahead->next.list[framecnt];

    x264_lowres_context_init( h, &a );

    if( !framecnt )
    {
        if( h->param.rc.b_mb_tree )
            x264_macroblock_tree( h, &a, frames, 0, keyframe );
        return;
    }

    keyint_limit = h->param.i_keyint_max - frames[0]->i_frame + h->lookahead->i_last_keyframe - 1;
    orig_num_frames = num_frames = h->param.b_intra_refresh ? framecnt : X264_MIN( framecnt, keyint_limit );

    /* This is important psy-wise: if we have a non-scenecut keyframe,
     * there will be significant visual artifacts if the frames just before
     * go down in quality due to being referenced less, despite it being
     * more RD-optimal. */
    if( (h->param.analyse.b_psy && h->param.rc.b_mb_tree) || vbv_lookahead )
        num_frames = framecnt;
    else if( h->param.b_open_gop && num_frames < framecnt )
        num_frames++;
    else if( num_frames == 0 )
    {
        frames[1]->i_type = X264_TYPE_I;
        return;
    }

    int num_bframes = 0;
    int num_analysed_frames = num_frames;
    int reset_start;
    if( h->param.i_scenecut_threshold && scenecut( h, &a, frames, 0, 1, 1, orig_num_frames, i_max_search ) )
    {
        frames[1]->i_type = X264_TYPE_I;
        return;
    }

    if( h->param.i_bframe )
    {
        if( h->param.i_bframe_adaptive == X264_B_ADAPT_TRELLIS )
        {
            if( num_frames > 1 )
            {
                char best_paths[X264_BFRAME_MAX+1][X264_LOOKAHEAD_MAX+1] = {"","P"};
                int best_path_index = num_frames % (X264_BFRAME_MAX+1);

                /* Perform the frametype analysis. */
                for( int j = 2; j <= num_frames; j++ )
                    x264_slicetype_path( h, &a, frames, j, best_paths );

                num_bframes = strspn( best_paths[best_path_index], "B" );
                /* Load the results of the analysis into the frame types. */
                for( int j = 1; j < num_frames; j++ )
                    frames[j]->i_type = best_paths[best_path_index][j-1] == 'B' ? X264_TYPE_B : X264_TYPE_P;
            }
            frames[num_frames]->i_type = X264_TYPE_P;
        }
        else if( h->param.i_bframe_adaptive == X264_B_ADAPT_FAST )
        {
            for( int i = 0; i <= num_frames-2; )
            {
                cost2p1 = x264_slicetype_frame_cost( h, &a, frames, i+0, i+2, i+2, 1 );
                if( frames[i+2]->i_intra_mbs[2] > i_mb_count / 2 )
                {
                    frames[i+1]->i_type = X264_TYPE_P;
                    frames[i+2]->i_type = X264_TYPE_P;
                    i += 2;
                    continue;
                }

                cost1b1 = x264_slicetype_frame_cost( h, &a, frames, i+0, i+2, i+1, 0 );
                cost1p0 = x264_slicetype_frame_cost( h, &a, frames, i+0, i+1, i+1, 0 );
                cost2p0 = x264_slicetype_frame_cost( h, &a, frames, i+1, i+2, i+2, 0 );

                if( cost1p0 + cost2p0 < cost1b1 + cost2p1 )
                {
                    frames[i+1]->i_type = X264_TYPE_P;
                    i += 1;
                    continue;
                }

                // arbitrary and untuned
                #define INTER_THRESH 300
                #define P_SENS_BIAS (50 - h->param.i_bframe_bias)
                frames[i+1]->i_type = X264_TYPE_B;

                int j;
                for( j = i+2; j <= X264_MIN( i+h->param.i_bframe, num_frames-1 ); j++ )
                {
                    int pthresh = X264_MAX(INTER_THRESH - P_SENS_BIAS * (j-i-1), INTER_THRESH/10);
                    int pcost = x264_slicetype_frame_cost( h, &a, frames, i+0, j+1, j+1, 1 );
                    if( pcost > pthresh*i_mb_count || frames[j+1]->i_intra_mbs[j-i+1] > i_mb_count/3 )
                        break;
                    frames[j]->i_type = X264_TYPE_B;
                }
                frames[j]->i_type = X264_TYPE_P;
                i = j;
            }
            frames[num_frames]->i_type = X264_TYPE_P;
            num_bframes = 0;
            while( num_bframes < num_frames && frames[num_bframes+1]->i_type == X264_TYPE_B )
                num_bframes++;
        }
        else
        {
            num_bframes = X264_MIN(num_frames-1, h->param.i_bframe);
            for( int j = 1; j < num_frames; j++ )
                frames[j]->i_type = (j%(num_bframes+1)) ? X264_TYPE_B : X264_TYPE_P;
            frames[num_frames]->i_type = X264_TYPE_P;
        }

        /* Check scenecut on the first minigop. */
        for( int j = 1; j < num_bframes+1; j++ )
            if( h->param.i_scenecut_threshold && scenecut( h, &a, frames, j, j+1, 0, orig_num_frames, i_max_search ) )
            {
                frames[j]->i_type = X264_TYPE_P;
                num_analysed_frames = j;
                break;
            }

        reset_start = keyframe ? 1 : X264_MIN( num_bframes+2, num_analysed_frames+1 );
    }
    else
    {
        for( int j = 1; j <= num_frames; j++ )
            frames[j]->i_type = X264_TYPE_P;
        reset_start = !keyframe + 1;
        num_bframes = 0;
    }

    /* Perform the actual macroblock tree analysis.
     * Don't go farther than the maximum keyframe interval; this helps in short GOPs. */
    if( h->param.rc.b_mb_tree )
        x264_macroblock_tree( h, &a, frames, X264_MIN(num_frames, h->param.i_keyint_max), keyframe );

    /* Enforce keyframe limit. */
    if( !h->param.b_intra_refresh )
        for( int i = keyint_limit+1; i <= num_frames; i += h->param.i_keyint_max )
        {
            frames[i]->i_type = X264_TYPE_I;
            reset_start = X264_MIN( reset_start, i+1 );
            if( h->param.b_open_gop && h->param.b_bluray_compat )
                while( IS_X264_TYPE_B( frames[i-1]->i_type ) )
                    i--;
        }

    if( vbv_lookahead )
        x264_vbv_lookahead( h, &a, frames, num_frames, keyframe );

    /* Restore frametypes for all frames that haven't actually been decided yet. */
    for( int j = reset_start; j <= num_frames; j++ )
        frames[j]->i_type = X264_TYPE_AUTO;
}

void x264_slicetype_decide( x264_t *h )
{
    x264_frame_t *frames[X264_BFRAME_MAX+2];
    x264_frame_t *frm;
    int bframes;
    int brefs;

    if( !h->lookahead->next.i_size )
        return;

    int lookahead_size = h->lookahead->next.i_size;

    for( int i = 0; i < h->lookahead->next.i_size; i++ )
    {
        if( h->param.b_vfr_input )
        {
            if( lookahead_size-- > 1 )
                h->lookahead->next.list[i]->i_duration = 2 * (h->lookahead->next.list[i+1]->i_pts - h->lookahead->next.list[i]->i_pts);
            else
                h->lookahead->next.list[i]->i_duration = h->i_prev_duration;
        }
        else
            h->lookahead->next.list[i]->i_duration = delta_tfi_divisor[h->lookahead->next.list[i]->i_pic_struct];
        h->i_prev_duration = h->lookahead->next.list[i]->i_duration;
        h->lookahead->next.list[i]->f_duration = (double)h->lookahead->next.list[i]->i_duration
                                               * h->sps->vui.i_num_units_in_tick
                                               / h->sps->vui.i_time_scale;

        if( h->lookahead->next.list[i]->i_frame > h->i_disp_fields_last_frame && lookahead_size > 0 )
        {
            h->lookahead->next.list[i]->i_field_cnt = h->i_disp_fields;
            h->i_disp_fields += h->lookahead->next.list[i]->i_duration;
            h->i_disp_fields_last_frame = h->lookahead->next.list[i]->i_frame;
        }
        else if( lookahead_size == 0 )
        {
            h->lookahead->next.list[i]->i_field_cnt = h->i_disp_fields;
            h->lookahead->next.list[i]->i_duration = h->i_prev_duration;
        }
    }

    if( h->param.rc.b_stat_read )
    {
        /* Use the frame types from the first pass */
        for( int i = 0; i < h->lookahead->next.i_size; i++ )
            h->lookahead->next.list[i]->i_type =
                x264_ratecontrol_slice_type( h, h->lookahead->next.list[i]->i_frame );
    }
    else if( (h->param.i_bframe && h->param.i_bframe_adaptive)
             || h->param.i_scenecut_threshold
             || h->param.rc.b_mb_tree
             || (h->param.rc.i_vbv_buffer_size && h->param.rc.i_lookahead) )
        x264_slicetype_analyse( h, 0 );

    for( bframes = 0, brefs = 0;; bframes++ )
    {
        frm = h->lookahead->next.list[bframes];
        if( frm->i_type == X264_TYPE_BREF && h->param.i_bframe_pyramid < X264_B_PYRAMID_NORMAL &&
            brefs == h->param.i_bframe_pyramid )
        {
            frm->i_type = X264_TYPE_B;
            x264_log( h, X264_LOG_WARNING, "B-ref at frame %d incompatible with B-pyramid %s \n",
                      frm->i_frame, x264_b_pyramid_names[h->param.i_bframe_pyramid] );
        }
        /* pyramid with multiple B-refs needs a big enough dpb that the preceding P-frame stays available.
           smaller dpb could be supported by smart enough use of mmco, but it's easier just to forbid it. */
        else if( frm->i_type == X264_TYPE_BREF && h->param.i_bframe_pyramid == X264_B_PYRAMID_NORMAL &&
            brefs && h->param.i_frame_reference <= (brefs+3) )
        {
            frm->i_type = X264_TYPE_B;
            x264_log( h, X264_LOG_WARNING, "B-ref at frame %d incompatible with B-pyramid %s and %d reference frames\n",
                      frm->i_frame, x264_b_pyramid_names[h->param.i_bframe_pyramid], h->param.i_frame_reference );
        }

        if( frm->i_type == X264_TYPE_KEYFRAME )
            frm->i_type = h->param.b_open_gop ? X264_TYPE_I : X264_TYPE_IDR;

        /* Limit GOP size */
        if( (!h->param.b_intra_refresh || frm->i_frame == 0) && frm->i_frame - h->lookahead->i_last_keyframe >= h->param.i_keyint_max )
        {
            if( frm->i_type == X264_TYPE_AUTO || frm->i_type == X264_TYPE_I )
                frm->i_type = h->param.b_open_gop && h->lookahead->i_last_keyframe >= 0 ? X264_TYPE_I : X264_TYPE_IDR;
            int warn = frm->i_type != X264_TYPE_IDR;
            if( warn && h->param.b_open_gop )
                warn &= frm->i_type != X264_TYPE_I;
            if( warn )
                x264_log( h, X264_LOG_WARNING, "specified frame type (%d) at %d is not compatible with keyframe interval\n", frm->i_type, frm->i_frame );
        }
        if( frm->i_type == X264_TYPE_I && frm->i_frame - h->lookahead->i_last_keyframe >= h->param.i_keyint_min )
        {
            if( h->param.b_open_gop )
            {
                h->lookahead->i_last_keyframe = frm->i_frame; // Use display order
                if( h->param.b_bluray_compat )
                    h->lookahead->i_last_keyframe -= bframes; // Use bluray order
                frm->b_keyframe = 1;
            }
            else
                frm->i_type = X264_TYPE_IDR;
        }
        if( frm->i_type == X264_TYPE_IDR )
        {
            /* Close GOP */
            h->lookahead->i_last_keyframe = frm->i_frame;
            frm->b_keyframe = 1;
            if( bframes > 0 )
            {
                bframes--;
                h->lookahead->next.list[bframes]->i_type = X264_TYPE_P;
            }
        }

        if( bframes == h->param.i_bframe ||
            !h->lookahead->next.list[bframes+1] )
        {
            if( IS_X264_TYPE_B( frm->i_type ) )
                x264_log( h, X264_LOG_WARNING, "specified frame type is not compatible with max B-frames\n" );
            if( frm->i_type == X264_TYPE_AUTO
                || IS_X264_TYPE_B( frm->i_type ) )
                frm->i_type = X264_TYPE_P;
        }

        if( frm->i_type == X264_TYPE_BREF )
            brefs++;

        if( frm->i_type == X264_TYPE_AUTO )
            frm->i_type = X264_TYPE_B;

        else if( !IS_X264_TYPE_B( frm->i_type ) ) break;
    }

    if( bframes )
        h->lookahead->next.list[bframes-1]->b_last_minigop_bframe = 1;
    h->lookahead->next.list[bframes]->i_bframes = bframes;

    /* insert a bref into the sequence */
    if( h->param.i_bframe_pyramid && bframes > 1 && !brefs )
    {
        h->lookahead->next.list[bframes/2]->i_type = X264_TYPE_BREF;
        brefs++;
    }

    /* calculate the frame costs ahead of time for x264_rc_analyse_slice while we still have lowres */
    if( h->param.rc.i_rc_method != X264_RC_CQP )
    {
        x264_mb_analysis_t a;
        int p0, p1, b;
        p1 = b = bframes + 1;

        x264_lowres_context_init( h, &a );

        frames[0] = h->lookahead->last_nonb;
        memcpy( &frames[1], h->lookahead->next.list, (bframes+1) * sizeof(x264_frame_t*) );
        if( IS_X264_TYPE_I( h->lookahead->next.list[bframes]->i_type ) )
            p0 = bframes + 1;
        else // P
            p0 = 0;

        x264_slicetype_frame_cost( h, &a, frames, p0, p1, b, 0 );

        if( (p0 != p1 || bframes) && h->param.rc.i_vbv_buffer_size )
        {
            /* We need the intra costs for row SATDs. */
            x264_slicetype_frame_cost( h, &a, frames, b, b, b, 0 );

            /* We need B-frame costs for row SATDs. */
            p0 = 0;
            for( b = 1; b <= bframes; b++ )
            {
                if( frames[b]->i_type == X264_TYPE_B )
                    for( p1 = b; frames[p1]->i_type == X264_TYPE_B; )
                        p1++;
                else
                    p1 = bframes + 1;
                x264_slicetype_frame_cost( h, &a, frames, p0, p1, b, 0 );
                if( frames[b]->i_type == X264_TYPE_BREF )
                    p0 = b;
            }
        }
    }

    /* Analyse for weighted P frames */
    if( !h->param.rc.b_stat_read && h->lookahead->next.list[bframes]->i_type == X264_TYPE_P
        && h->param.analyse.i_weighted_pred >= X264_WEIGHTP_SIMPLE )
    {
        x264_emms();
        x264_weights_analyse( h, h->lookahead->next.list[bframes], h->lookahead->last_nonb, 0 );
    }

    /* shift sequence to coded order.
       use a small temporary list to avoid shifting the entire next buffer around */
    int i_coded = h->lookahead->next.list[0]->i_frame;
    if( bframes )
    {
        int idx_list[] = { brefs+1, 1 };
        for( int i = 0; i < bframes; i++ )
        {
            int idx = idx_list[h->lookahead->next.list[i]->i_type == X264_TYPE_BREF]++;
            frames[idx] = h->lookahead->next.list[i];
            frames[idx]->i_reordered_pts = h->lookahead->next.list[idx]->i_pts;
        }
        frames[0] = h->lookahead->next.list[bframes];
        frames[0]->i_reordered_pts = h->lookahead->next.list[0]->i_pts;
        memcpy( h->lookahead->next.list, frames, (bframes+1) * sizeof(x264_frame_t*) );
    }

    for( int i = 0; i <= bframes; i++ )
    {
        h->lookahead->next.list[i]->i_coded = i_coded++;
        if( i )
        {
            x264_calculate_durations( h, h->lookahead->next.list[i], h->lookahead->next.list[i-1], &h->i_cpb_delay, &h->i_coded_fields );
            h->lookahead->next.list[0]->f_planned_cpb_duration[i-1] = (double)h->lookahead->next.list[i-1]->i_cpb_duration *
                                                                      h->sps->vui.i_num_units_in_tick / h->sps->vui.i_time_scale;
        }
        else
            x264_calculate_durations( h, h->lookahead->next.list[i], NULL, &h->i_cpb_delay, &h->i_coded_fields );

        h->lookahead->next.list[0]->f_planned_cpb_duration[i] = (double)h->lookahead->next.list[i]->i_cpb_duration *
                                                                h->sps->vui.i_num_units_in_tick / h->sps->vui.i_time_scale;
    }
}

int x264_rc_analyse_slice( x264_t *h )
{
    int p0 = 0, p1, b;
    int cost;
    x264_emms();

    if( IS_X264_TYPE_I(h->fenc->i_type) )
        p1 = b = 0;
    else if( h->fenc->i_type == X264_TYPE_P )
        p1 = b = h->fenc->i_bframes + 1;
    else //B
    {
        p1 = (h->fref_nearest[1]->i_poc - h->fref_nearest[0]->i_poc)/2;
        b  = (h->fenc->i_poc - h->fref_nearest[0]->i_poc)/2;
    }
    /* We don't need to assign p0/p1 since we are not performing any real analysis here. */
    x264_frame_t **frames = &h->fenc - b;

    /* cost should have been already calculated by x264_slicetype_decide */
    cost = frames[b]->i_cost_est[b-p0][p1-b];
    assert( cost >= 0 );

    if( h->param.rc.b_mb_tree && !h->param.rc.b_stat_read )
    {
        cost = x264_slicetype_frame_cost_recalculate( h, frames, p0, p1, b );
        if( b && h->param.rc.i_vbv_buffer_size )
            x264_slicetype_frame_cost_recalculate( h, frames, b, b, b );
    }
    /* In AQ, use the weighted score instead. */
    else if( h->param.rc.i_aq_mode )
        cost = frames[b]->i_cost_est_aq[b-p0][p1-b];

    h->fenc->i_row_satd = h->fenc->i_row_satds[b-p0][p1-b];
    h->fdec->i_row_satd = h->fdec->i_row_satds[b-p0][p1-b];
    h->fdec->i_satd = cost;
    memcpy( h->fdec->i_row_satd, h->fenc->i_row_satd, h->mb.i_mb_height * sizeof(int) );
    if( !IS_X264_TYPE_I(h->fenc->i_type) )
        memcpy( h->fdec->i_row_satds[0][0], h->fenc->i_row_satds[0][0], h->mb.i_mb_height * sizeof(int) );

    if( h->param.b_intra_refresh && h->param.rc.i_vbv_buffer_size && h->fenc->i_type == X264_TYPE_P )
    {
        int ip_factor = 256 * h->param.rc.f_ip_factor; /* fix8 */
        for( int y = 0; y < h->mb.i_mb_height; y++ )
        {
            int mb_xy = y * h->mb.i_mb_stride + h->fdec->i_pir_start_col;
            for( int x = h->fdec->i_pir_start_col; x <= h->fdec->i_pir_end_col; x++, mb_xy++ )
            {
                int intra_cost = (h->fenc->i_intra_cost[mb_xy] * ip_factor + 128) >> 8;
                int inter_cost = h->fenc->lowres_costs[b-p0][p1-b][mb_xy] & LOWRES_COST_MASK;
                int diff = intra_cost - inter_cost;
                if( h->param.rc.i_aq_mode )
                    h->fdec->i_row_satd[y] += (diff * frames[b]->i_inv_qscale_factor[mb_xy] + 128) >> 8;
                else
                    h->fdec->i_row_satd[y] += diff;
                cost += diff;
            }
        }
    }

    if( BIT_DEPTH > 8 )
        for( int y = 0; y < h->mb.i_mb_height; y++ )
            h->fdec->i_row_satd[y] >>= (BIT_DEPTH - 8);

    return cost >> (BIT_DEPTH - 8);
}
