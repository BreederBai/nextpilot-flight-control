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
 * Name: sq_addfirst
 *
 * Description:
 *   The sq_addfirst function places the 'node' at the head of the 'queue'
 *
 ****************************************************************************/

void sq_addfirst(sq_entry_t *node, sq_queue_t *queue) {
    node->flink = queue->head;
    if (!queue->head) {
        queue->tail = node;
    }

    queue->head = node;
}
