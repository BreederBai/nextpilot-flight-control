/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <stdint.h>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/ulog_stream.h>
#include <uORB/topics/ulog_stream_ack.h>

namespace nextpilot::logger {

/**
 * @class LogWriterMavlink
 * Writes logging data to uORB, and then sent via mavlink
 */
class LogWriterMavlink {
public:
    LogWriterMavlink();
    ~LogWriterMavlink();

    bool init();

    void start_log();

    void stop_log();

    bool is_started() const {
        return _is_started;
    }

    /** @see LogWriter::write_message() */
    int write_message(void *ptr, size_t size);

    void set_need_reliable_transfer(bool need_reliable);

    bool need_reliable_transfer() const {
        return _need_reliable_transfer;
    }

private:
    /** publish message, wait for ack if needed & reset message */
    int publish_message();

    ulog_stream_s                    _ulog_stream_data{};
    uORB::Publication<ulog_stream_s> _ulog_stream_pub{ORB_ID(ulog_stream)};
    // orb_subscr_t                     _ulog_stream_ack_sub{nullptr};
    uORB::SubscriptionPolling _ulog_stream_ack_sub{ORB_ID(ulog_stream_ack)};
    bool                      _need_reliable_transfer{false};
    bool                      _is_started{false};
};

} // namespace nextpilot::logger
