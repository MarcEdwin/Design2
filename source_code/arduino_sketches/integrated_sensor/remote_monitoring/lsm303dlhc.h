// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __LSM303DLHC_H
#define __LSM303DLHC_H


#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
void initLsm(void);

///////////////////////////////////////////////////////////////////////////////////////////////////
void getNextSample(float* AccelerationX, float* AccelerationY, float* AccelerationZ);

#ifdef __cplusplus
}
#endif


#endif //__LSM303DLHC_H

