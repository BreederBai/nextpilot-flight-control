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

#include <mixer_module/mixer_module.hpp>

#include <gz/transport.hh>

#include <uORB/Publication.hpp>
#include <uORB/topics/esc_status.h>

// GZBridge mixing class for ESCs.
// It is separate from GZBridge to have separate WorkItems and therefore allowing independent scheduling
// All work items are expected to run on the same work queue.
class GZMixingInterfaceESC : public OutputModuleInterface {
public:
    static constexpr int MAX_ACTUATORS = MixingOutput::MAX_ACTUATORS;

    GZMixingInterfaceESC(gz::transport::Node &node, pthread_mutex_t &node_mutex) :
        OutputModuleInterface(MODULE_NAME "-actuators-esc", wq_configurations::rate_ctrl),
        _node(node),
        _node_mutex(node_mutex) {
    }

    bool updateOutputs(bool stop_motors, uint16_t outputs[MAX_ACTUATORS],
                       unsigned num_outputs, unsigned num_control_groups_updated) override;

    MixingOutput &mixingOutput() {
        return _mixing_output;
    }

    bool init(const std::string &model_name);

    void stop() {
        _mixing_output.unregister();
        ScheduleClear();
    }

private:
    friend class GZBridge;

    void Run() override;

    void motorSpeedCallback(const gz::msgs::Actuators &actuators);

    gz::transport::Node &_node;
    pthread_mutex_t     &_node_mutex;

    MixingOutput _mixing_output{"SIM_GZ_EC", MAX_ACTUATORS, *this, MixingOutput::SchedulingPolicy::Auto, false, false};

    gz::transport::Node::Publisher _actuators_pub;

    uORB::Publication<esc_status_s> _esc_status_pub{ORB_ID(esc_status)};
};
