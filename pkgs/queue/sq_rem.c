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
 * Name: sq_rem
 *
 * Description:
 *   sq_rem removes a 'node' for 'queue.'
 *
 ****************************************************************************/

void sq_rem(sq_entry_t *node, sq_queue_t *queue) {
    if (queue->head && node) {
        if (node == queue->head) {
            queue->head = node->flink;
            if (node == queue->tail) {
                queue->tail = NULL;
            }
        } else {
            sq_entry_t *prev;

            for (prev = (sq_entry_t *)queue->head;
                 prev && prev->flink != node;
                 prev = prev->flink)
                ;

            if (prev) {
                sq_remafter(prev, queue);
            }
        }
    }
}
