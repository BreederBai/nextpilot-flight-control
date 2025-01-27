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

#include "../Common.hpp"
#include <uORB/Subscription.hpp>

using namespace nextpilot;
using namespace nextpilot::param;

class ModeChecks : public HealthAndArmingCheckBase {
public:
    ModeChecks()  = default;
    ~ModeChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    void checkArmingRequirement(const Context &context, Report &reporter);

    DEFINE_PARAMETERS_CUSTOM_PARENT(HealthAndArmingCheckBase,
                                    (ParamBool<params_id::COM_ARM_MIS_REQ>)_param_com_arm_mis_req,
                                    (ParamInt<params_id::COM_RC_IN_MODE>)_param_com_rc_in_mode);
};
