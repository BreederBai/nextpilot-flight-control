/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <queue.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: dq_addafter
 *
 * Description:
 *  dq_addafter function adds 'node' after 'prev' in the 'queue.'
 *
 ****************************************************************************/

void dq_addafter(dq_entry_t *prev, dq_entry_t *node,
                 dq_queue_t *queue) {
    if (!queue->head || prev == queue->tail) {
        dq_addlast(node, queue);
    } else {
        dq_entry_t *next = prev->flink;
        node->blink      = prev;
        node->flink      = next;
        next->blink      = node;
        prev->flink      = node;
    }
}
