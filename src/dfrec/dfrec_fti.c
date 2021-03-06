/*
 * Author: Jon Trulson <jtrulson@ics.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * This program and the accompanying materials are made available under the
 * terms of the The MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 */

#include "dfrec.h"
#include "upm_fti.h"

/**
 * This file implements the Function Table Interface (FTI) for this sensor
 */

const char upm_dfrec_name[] = "DFREC";
const char upm_dfrec_description[] = "DFRobot Analog EC Sensor";
const upm_protocol_t upm_dfrec_protocol[] = {UPM_ANALOG};
const upm_sensor_t upm_dfrec_category[] = {UPM_EC};

// forward declarations
const void* upm_dfrec_get_ft(upm_sensor_t sensor_type);
void* upm_dfrec_init_name();
void upm_dfrec_close(void *dev);
upm_result_t upm_dfrec_get_value(const void *dev, float *value);
upm_result_t upm_dfrec_set_scale(const void *dev, float scale);
upm_result_t upm_dfrec_set_offset(const void *dev, float offset);

static const upm_sensor_ft ft =
{
    .upm_sensor_init_name = &upm_dfrec_init_name,
    .upm_sensor_close = &upm_dfrec_close,
};

static const upm_ec_ft ecft =
{
    .upm_ec_set_offset = upm_dfrec_set_offset,
    .upm_ec_set_scale = upm_dfrec_set_scale,
    .upm_ec_get_value = upm_dfrec_get_value
};

const void* upm_dfrec_get_ft(upm_sensor_t sensor_type)
{
  switch(sensor_type)
    {
    case UPM_SENSOR:
      return &ft;

    case UPM_EC:
      return &ecft;

    default:
      return NULL;
    }
}

void* upm_dfrec_init_name()
{
    return NULL;
}

void upm_dfrec_close(void *dev)
{
    dfrec_close((dfrec_context)dev);
}

upm_result_t upm_dfrec_set_scale(const void *dev, float scale)
{
  dfrec_set_scale((dfrec_context)dev, scale);
  return UPM_SUCCESS;
}

upm_result_t upm_dfrec_set_offset(const void *dev, float offset)
{
  dfrec_set_offset((dfrec_context)dev, offset);
  return UPM_SUCCESS;
}

upm_result_t upm_dfrec_get_value(const void *dev, float *value)
{
  upm_result_t rv;

  if ((rv = dfrec_update((dfrec_context)dev)))
    return rv;

  *value = dfrec_get_ec((dfrec_context)dev);

  return UPM_SUCCESS;
}
