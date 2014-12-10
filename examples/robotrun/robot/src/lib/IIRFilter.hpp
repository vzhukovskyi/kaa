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
 * IIRFilter.hpp
 *
 *   Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
 *   Command line: /www/usr/fisher/helpers/mkfilter -Ch -1.0000000000e+00 -Lp -o 2 -a 2.0000000000e-01 0.0000000000e+00 -l
 *
 *  Created on: Nov 27, 2014
 *      Author: Andriy Panasenko <apanasenko@cybervisiontech.com>
 */

#ifndef LIB_IIRFILTER_HPP_
#define LIB_IIRFILTER_HPP_

#define NZEROS 2

class IIRFilter {
	private:
		float coff0;
		float coff1;
		float gain;
		float xv[NZEROS+1], yv[NZEROS+1];
	public:
		IIRFilter(float Gain, float COFF0, float COFF1);
		float getFillteredValue(float v);
		void init();
};


#endif /* LIB_IIRFILTER_HPP_ */
