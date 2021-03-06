/*
 * Authors: Yevgeniy Kiveisha <yevgeniy.kiveisha@intel.com>
 *          Mihai Tudor Panu <mihai.tudor.panu@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * This program and the accompanying materials are made available under the
 * terms of the The MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include <string>
#include <stdexcept>
#include <stdlib.h>
#include <time.h>
#include "stepmotor.hpp"

using namespace upm;
using namespace std;

StepMotor::StepMotor (int dirPin, int stePin, int steps, int enPin)
                    : m_dirPinCtx(dirPin),
                      m_stePinCtx(stePin),
                      m_enPinCtx(0),
                      m_steps(steps) {
    m_name = "StepMotor";
    setSpeed(60);
    setPosition(0);

    if (m_dirPinCtx.dir(mraa::DIR_OUT) != mraa::SUCCESS) {
        throw std::runtime_error(string(__FUNCTION__) +
                               ": Could not initialize dirPin as output");
        return;
    }
    m_dirPinCtx.write(0);

    if (m_stePinCtx.dir(mraa::DIR_OUT) != mraa::SUCCESS) {
        throw std::runtime_error(string(__FUNCTION__) +
                               ": Could not initialize stePin as output");
        return;
    }
    m_stePinCtx.write(0);

    if (enPin >= 0) {
        m_enPinCtx = new mraa::Gpio(enPin);
        if(m_enPinCtx->dir(mraa::DIR_OUT) != mraa::SUCCESS) {
            throw std::runtime_error(string(__FUNCTION__) +
                               ": Could not initialize enPin as output");
            return;
        }
        enable(true);
    }
}

StepMotor::~StepMotor () {
    if (m_enPinCtx)
        delete m_enPinCtx;
}

void
StepMotor::enable (bool flag) {
    if (m_enPinCtx) {
        m_enPinCtx->write(flag);
    } else {
        throw std::runtime_error(string(__FUNCTION__) +
                                ": Enable pin not defined");
    }
}

void
StepMotor::setSpeed (int speed) {
    if (speed > 0) {
        m_delay = 60000000 / (speed * m_steps);
    } else {
        throw std::invalid_argument(string(__FUNCTION__) +
                                    ": Parameter must be greater than 0");
    }
}

mraa::Result
StepMotor::step (int ticks) {
    if (ticks < 0) {
        return stepBackward(abs(ticks));
    } else {
        return stepForward(ticks);
    }
}

mraa::Result
StepMotor::stepForward (int ticks) {
    dirForward();
    for (int i = 0; i < ticks; i++) {
        move();
        m_position++;
        delayus(m_delay - MINPULSE_US - OVERHEAD_US);
    }
    return mraa::SUCCESS;
}

mraa::Result
StepMotor::stepBackward (int ticks) {
    dirBackward();
    for (int i = 0; i < ticks; i++) {
        move();
        m_position--;
        delayus(m_delay - MINPULSE_US - OVERHEAD_US);
    }
    return mraa::SUCCESS;
}

void
StepMotor::setPosition (int pos) {
    m_position = pos;
}

int
StepMotor::getPosition () {
    return m_position;
}

int
StepMotor::getStep () {
    return m_position < 0 ? m_steps + m_position % m_steps :
                            m_position % m_steps;
}

void
StepMotor::move () {
    m_stePinCtx.write(1);
    delayus(MINPULSE_US);
    m_stePinCtx.write(0);
}

mraa::Result
StepMotor::dirForward () {
    mraa::Result error = m_dirPinCtx.write(HIGH);
    if (error != mraa::SUCCESS) {
        throw std::runtime_error(string(__FUNCTION__) +
                                       ": Could not write to dirPin");
    }
    return error;
}

mraa::Result
StepMotor::dirBackward () {
    mraa::Result error = m_dirPinCtx.write(LOW);
    if (error != mraa::SUCCESS) {
        throw std::runtime_error(string(__FUNCTION__) +
                                       ": Could not write to dirPin");
    }
    return error;
}

void upm::StepMotor::delayus (int us) {
    int diff = 0;
    struct timespec gettime_now;

    clock_gettime(CLOCK_REALTIME, &gettime_now);
    int start = gettime_now.tv_nsec;
    while (diff < us * 1000)
    {
        clock_gettime(CLOCK_REALTIME, &gettime_now);
        diff = gettime_now.tv_nsec - start;
        if (diff < 0)
            diff += 1000000000;
    }
}
