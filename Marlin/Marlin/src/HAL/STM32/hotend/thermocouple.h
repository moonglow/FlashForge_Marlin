/******************************************************************************
 * Additional info
 * ****************************************************************************
 * Changelog:
 * - v. 2.0 Optimize memory usage. Option to use binary search.
 *   2016-01-09 by Joakim Soderberg joakim.soderberg@gmail.com
 * - v. 1.0 Initial release)
 *   2014-04-24 by Albertas Mickėnas mic@wemakethings.net
 *
 * ****************************************************************************
 * Bugs, feedback, questions and modifications can be posted on the github page
 * on https://github.com/Miceuz/k-thermocouple-lib/
 * ****************************************************************************
 * - LICENSE -
 * GNU GPL v2 (http://www.gnu.org/licenses/gpl.txt)
 * This program is free software. You can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 * ****************************************************************************
 */
#ifndef __THERMOCOUPLE_H
#define __THERMOCOUPLE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns temperature as a function of the ambient temperature and measured thermocouple voltage
 **/
long thermocoupleConvertWithCJCompensation(unsigned long microvoltsMeasured, unsigned long ambient);

/**
 * Returns temperature equivalent the voltage provided in microvolts
 */
long thermocoupleMvToC(unsigned long microvolts);

#ifdef __cplusplus
}
#endif
#endif // _THERMOCOUPLE_H

