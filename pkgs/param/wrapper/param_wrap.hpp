/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file param.h
 *
 * C++ Parameter class
 */

#pragma once

#include <float.h>
#include <math.h>
#include "param_autogen.hpp"

/**
 * get the parameter handle from a parameter enum
 */
inline static param_t param_handle(nextpilot::param::params_id p) {
    return (param_t)p;
}

// This namespace never needs to be used directly. Use the DEFINE_PARAMETERS_CUSTOM_PARENT and
// DEFINE_PARAMETERS macros instead (the Param classes don't depend on using the macro, the macro just
// makes sure that update() is automatically called).
namespace do_not_explicitly_use_this_namespace {

template <typename T, nextpilot::param::params_id p>
class Param {
};

// We use partial template specialization for each param type. This is only supported for classes, not individual methods,
// which is why we have to repeat the whole class
template <nextpilot::param::params_id p>
class Param<float, p> {
public:
    // static type-check
    static_assert(nextpilot::param::params_type[(int)p] == PARAM_TYPE_FLOAT, "parameter type must be float");

    Param() {
        param_set_used(handle());
        update();
    }

    float get() const {
        return _val;
    }

    const float &reference() const {
        return _val;
    }

    /// Store the parameter value to the parameter storage (@see param_set())
    bool commit() const {
        return param_set(handle(), &_val) == 0;
    }

    /// Store the parameter value to the parameter storage, w/o notifying the system (@see param_set_no_notification())
    bool commit_no_notification() const {
        return param_set_no_notification(handle(), &_val) == 0;
    }

    /// Set and commit a new value. Returns true if the value changed.
    bool commit_no_notification(float val) {
        if (fabsf(val - _val) > FLT_EPSILON) {
            set(val);
            commit_no_notification();
            return true;
        }

        return false;
    }

    void set(float val) {
        _val = val;
    }

    void reset() {
        param_reset_no_notification(handle());
        update();
    }

    bool update() {
        return param_get(handle(), &_val) == 0;
    }

    param_t handle() const {
        return param_handle(p);
    }

private:
    float _val;
};

// external version
template <nextpilot::param::params_id p>
class Param<float &, p> {
public:
    // static type-check
    static_assert(nextpilot::param::params_type[(int)p] == PARAM_TYPE_FLOAT, "parameter type must be float");

    Param(float &external_val) :
        _val(external_val) {
        param_set_used(handle());
        update();
    }

    float get() const {
        return _val;
    }

    const float &reference() const {
        return _val;
    }

    /// Store the parameter value to the parameter storage (@see param_set())
    bool commit() const {
        return param_set(handle(), &_val) == 0;
    }

    /// Store the parameter value to the parameter storage, w/o notifying the system (@see param_set_no_notification())
    bool commit_no_notification() const {
        return param_set_no_notification(handle(), &_val) == 0;
    }

    /// Set and commit a new value. Returns true if the value changed.
    bool commit_no_notification(float val) {
        if (fabsf(val - _val) > FLT_EPSILON) {
            set(val);
            commit_no_notification();
            return true;
        }

        return false;
    }

    void set(float val) {
        _val = val;
    }

    void reset() {
        param_reset_no_notification(handle());
        update();
    }

    bool update() {
        return param_get(handle(), &_val) == 0;
    }

    param_t handle() const {
        return param_handle(p);
    }

private:
    float &_val;
};

template <nextpilot::param::params_id p>
class Param<int32_t, p> {
public:
    // static type-check
    static_assert(nextpilot::param::params_type[(int)p] == PARAM_TYPE_INT32, "parameter type must be int32_t");

    Param() {
        param_set_used(handle());
        update();
    }

    int32_t get() const {
        return _val;
    }

    const int32_t &reference() const {
        return _val;
    }

    /// Store the parameter value to the parameter storage (@see param_set())
    bool commit() const {
        return param_set(handle(), &_val) == 0;
    }

    /// Store the parameter value to the parameter storage, w/o notifying the system (@see param_set_no_notification())
    bool commit_no_notification() const {
        return param_set_no_notification(handle(), &_val) == 0;
    }

    /// Set and commit a new value. Returns true if the value changed.
    bool commit_no_notification(int32_t val) {
        if (val != _val) {
            set(val);
            commit_no_notification();
            return true;
        }

        return false;
    }

    void set(int32_t val) {
        _val = val;
    }

    void reset() {
        param_reset_no_notification(handle());
        update();
    }

    bool update() {
        return param_get(handle(), &_val) == 0;
    }

    param_t handle() const {
        return param_handle(p);
    }

private:
    int32_t _val;
};

// external version
template <nextpilot::param::params_id p>
class Param<int32_t &, p> {
public:
    // static type-check
    static_assert(nextpilot::param::params_type[(int)p] == PARAM_TYPE_INT32, "parameter type must be int32_t");

    Param(int32_t &external_val) :
        _val(external_val) {
        param_set_used(handle());
        update();
    }

    int32_t get() const {
        return _val;
    }

    const int32_t &reference() const {
        return _val;
    }

    /// Store the parameter value to the parameter storage (@see param_set())
    bool commit() const {
        return param_set(handle(), &_val) == 0;
    }

    /// Store the parameter value to the parameter storage, w/o notifying the system (@see param_set_no_notification())
    bool commit_no_notification() const {
        return param_set_no_notification(handle(), &_val) == 0;
    }

    /// Set and commit a new value. Returns true if the value changed.
    bool commit_no_notification(int32_t val) {
        if (val != _val) {
            set(val);
            commit_no_notification();
            return true;
        }

        return false;
    }

    void set(int32_t val) {
        _val = val;
    }

    void reset() {
        param_reset_no_notification(handle());
        update();
    }

    bool update() {
        return param_get(handle(), &_val) == 0;
    }

    param_t handle() const {
        return param_handle(p);
    }

private:
    int32_t &_val;
};

template <nextpilot::param::params_id p>
class Param<bool, p> {
public:
    // static type-check
    static_assert(nextpilot::param::params_type[(int)p] == PARAM_TYPE_INT32, "parameter type must be int32_t");

    Param() {
        param_set_used(handle());
        update();
    }

    bool get() const {
        return _val;
    }

    const bool &reference() const {
        return _val;
    }

    /// Store the parameter value to the parameter storage (@see param_set())
    bool commit() const {
        int32_t value_int = (int32_t)_val;
        return param_set(handle(), &value_int) == 0;
    }

    /// Store the parameter value to the parameter storage, w/o notifying the system (@see param_set_no_notification())
    bool commit_no_notification() const {
        int32_t value_int = (int32_t)_val;
        return param_set_no_notification(handle(), &value_int) == 0;
    }

    /// Set and commit a new value. Returns true if the value changed.
    bool commit_no_notification(bool val) {
        if (val != _val) {
            set(val);
            commit_no_notification();
            return true;
        }

        return false;
    }

    void set(bool val) {
        _val = val;
    }

    void reset() {
        param_reset_no_notification(handle());
        update();
    }

    bool update() {
        int32_t value_int;
        int     ret = param_get(handle(), &value_int);

        if (ret == 0) {
            _val = value_int != 0;
            return true;
        }

        return false;
    }

    param_t handle() const {
        return param_handle(p);
    }

private:
    bool _val;
};

template <nextpilot::param::params_id p>
using ParamFloat = Param<float, p>;

template <nextpilot::param::params_id p>
using ParamInt = Param<int32_t, p>;

template <nextpilot::param::params_id p>
using ParamExtFloat = Param<float &, p>;

template <nextpilot::param::params_id p>
using ParamExtInt = Param<int32_t &, p>;

template <nextpilot::param::params_id p>
using ParamBool = Param<bool, p>;

} /* namespace do_not_explicitly_use_this_namespace */

// Raise an appropriate compile error if a Param class is used directly (just to simplify debugging)
template <nextpilot::param::params_id p>
class ParamInt {
    static_assert((int)p && false, "Do not use this class directly, use the DEFINE_PARAMETERS macro instead");
};

template <nextpilot::param::params_id p>
class ParamFloat {
    static_assert((int)p && false, "Do not use this class directly, use the DEFINE_PARAMETERS macro instead");
};
