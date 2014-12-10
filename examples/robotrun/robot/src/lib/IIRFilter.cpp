/*
 * Copyright 2014 CyberVision, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/*
 * IIRFilter.cpp
 *
 *   Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
 *   Command line: /www/usr/fisher/helpers/mkfilter -Ch -1.0000000000e+00 -Lp -o 2 -a 2.0000000000e-01 0.0000000000e+00 -l
 *
 *  Created on: Nov 27, 2014
 *      Author: Andriy Panasenko <apanasenko@cybervisiontech.com>
 */

#include "IIRFilter.hpp"

IIRFilter::IIRFilter(float Gain, float COFF0, float COFF1) {
	gain = Gain;
	coff0=COFF0;
	coff1=COFF1;
	init();
};

float IIRFilter::getFillteredValue(float v) {
	xv[0] = xv[1]; xv[1] = xv[2];
	xv[2] = v / gain;
	yv[0] = yv[1]; yv[1] = yv[2];
	yv[2] =   (xv[0] + xv[2]) + 2 * xv[1]
						 + ( coff0 * yv[0]) + (  coff1 * yv[1]);
	return yv[2];
};

void IIRFilter::init() {
	for(int i=0;i<=NZEROS;i++) {
		xv[i]=0;
		yv[i]=0;
	}
};
