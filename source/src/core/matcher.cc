// Copyright (c) 2017 Leosocy. All rights reserved.
// Use of this source code is governed by a MIT-style license 
// that can be found in the LICENSE file.

#include "core/matcher.h"
#include "core/palmprintcode.h"
#include "core/edccoding.h"
#include "core/checker.h"
#include "util/status.h"

namespace edcc
{

Status Matcher::MatchingProcess(const EDCC_CODING_T *lhs_coding,
                                const EDCC_CODING_T *rhs_coding,
                                double *score)
{
    assert(lhs_coding && rhs_coding);
    if (memcmp(&lhs_coding->cfg, &rhs_coding->cfg, sizeof(EDCC_CFG_T)))
    {
        *score = .0;
        EDCC_Log("Two palmprint instance coding buffer config difference.");
        return Status::CodingsConfigDiff();
    }

    switch (lhs_coding->cfg.codingMode)
    {
        case COMPRESSION_CODING_MODE:
        {
            return ExecuteMatchingWhenCompressionCodingMode(lhs_coding, rhs_coding, score);
        }
        case FAST_CODING_MODE:
        {
            return ExecuteMatchingWhenFastCodingMode(lhs_coding, rhs_coding, score);
        }
        default:
        {
            *score = .0;
            EDCC_Log("Coding Mode [%d] not supported!", lhs_coding->cfg.codingMode);
            return Status::CodingInvalid();
        }
    }
}

Status Matcher::ExecuteMatchingWhenCompressionCodingMode(const EDCC_CODING_T *lhs_coding,
                                                         const EDCC_CODING_T *rhs_coding,
                                                         double *score)
{
    if (lhs_coding->cfg.matchingMode == RELIABLE_MATCHING_MODE)
    {
        if (!Checker::CheckCodingBuffer(lhs_coding)
            || !Checker::CheckCodingBuffer(rhs_coding))
    {
            *score = .0;
            return Status::CodingInvalid();
        }
    }

    int match_score = 0;
    size_t image_size = lhs_coding->cfg.imageSizeW * lhs_coding->cfg.imageSizeH;
    size_t coding_c_len = (size_t)ceil(image_size / 2.0);
    const u_char *c_x_start_pos = lhs_coding->data;
    const u_char *c_y_start_pos = rhs_coding->data;
    const u_char *c_end = lhs_coding->data + coding_c_len;
    const u_char *cs_x_start_pos = lhs_coding->data + coding_c_len;
    const u_char *cs_y_start_pos = rhs_coding->data + coding_c_len;

    u_char c_mask = 0xf0;
    u_char cs_mask = 0x80;
    while (c_x_start_pos < c_end)
    {
        if ((*c_x_start_pos & c_mask) == (*c_y_start_pos & c_mask))
        {
            match_score += 1;
            if ((*cs_x_start_pos & cs_mask) == (*cs_y_start_pos & cs_mask))
            {
                match_score += 1;
            }
        }
        (c_mask & 0x0f) ? ++c_x_start_pos, ++c_y_start_pos, c_mask = 0xf0 : c_mask = 0x0f;
        (cs_mask & 0x01) ? ++cs_x_start_pos, ++cs_y_start_pos, cs_mask = 0x80 : cs_mask >>= 1;
    }
    *score = (image_size % 2 == 0 ? (match_score / (2.0 * image_size)) : ((match_score - 2) / (2.0 * image_size)));
    return Status::Ok();
}

Status Matcher::ExecuteMatchingWhenFastCodingMode(const EDCC_CODING_T *lhs_coding,
                                                  const EDCC_CODING_T *rhs_coding,
                                                  double *score)
{
    if (lhs_coding->cfg.matchingMode == RELIABLE_MATCHING_MODE)
    {
        if (!Checker::CheckCodingBuffer(lhs_coding)
            || !Checker::CheckCodingBuffer(rhs_coding))
        {
            *score = .0;
            EDCC_Log("Coding Invalid!");
            return Status::CodingInvalid();
        }
    }

    int match_score = 0;
    size_t image_size = lhs_coding->cfg.imageSizeW * lhs_coding->cfg.imageSizeH;
    const u_char *lhs_coding_buffer_cur = lhs_coding->data;
    const u_char *lhs_coding_buffer_end = lhs_coding_buffer_cur + image_size;
    const u_char *rhs_coding_buffer_cur = rhs_coding->data;
    while (lhs_coding_buffer_cur < lhs_coding_buffer_end)
    {
        u_char cmp_value = *lhs_coding_buffer_cur ^ *rhs_coding_buffer_cur;
        if (cmp_value == 0x00)
        {
            match_score += 2;
        }
        else if (cmp_value < 0x10)
        {
            match_score += 1;
        }
        else
        {
            match_score += 0;
        }
        ++lhs_coding_buffer_cur;
        ++rhs_coding_buffer_cur;
    }

    *score = match_score / (2.0 * image_size);
    return Status::Ok();
}

} // namespace edcc
