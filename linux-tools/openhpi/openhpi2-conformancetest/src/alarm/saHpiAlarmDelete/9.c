/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004, 2005
 *
 *   This program is free software; you can redistribute it and/or modify 
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or 
 *   (at your option) any later version.
 *   This program is distributed in the hope that it will be useful, 
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *   GNU General Public License for more details. 
 *   You should have received a copy of the GNU General Public License 
 *   along with this program; if not, write to the Free Software 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
 *   USA 
 *
 * Author(s):
 *      Carl McAdams <carlmc@us.ibm.com>
 *
 * Spec:        HPI-B.01.01
 * Function:    saHpiAlarmDelete
 * Description:
 *      Add two new user alarms of the same severity in each 
 *      domain and delete both of them together with one delete 
 *      command.
 *      saHpiAlarmDelete() returns SA_OK.
 * Line:        P73-29:P73-31
 *    
 */
#include <stdio.h>
#include "saf_test.h"

int Test_Domain(SaHpiSessionIdT session_id)
{
	SaHpiAlarmT NewAlarm;
	SaHpiAlarmIdT FirstAlarm, SecondAlarm;
	SaHpiBoolT AlarmFound = SAHPI_FALSE;

	SaErrorT status;
	int retval = SAF_TEST_UNKNOWN;

	//
	// Create a new Alarm
	//
	NewAlarm.Severity = SAHPI_MINOR;
	NewAlarm.Acknowledged = SAHPI_FALSE;
	NewAlarm.AlarmCond.Type = SAHPI_STATUS_COND_TYPE_USER;
	NewAlarm.AlarmCond.ResourceId = SAHPI_UNSPECIFIED_RESOURCE_ID;
	NewAlarm.AlarmCond.Data.Language = SAHPI_LANG_ENGLISH;
	NewAlarm.AlarmCond.Data.DataType = SAHPI_TL_TYPE_TEXT;
	NewAlarm.AlarmCond.Data.DataLength = 1;
	NewAlarm.AlarmCond.Data.Data[0] = 'a';
	NewAlarm.AlarmCond.DomainId = SAHPI_UNSPECIFIED_DOMAIN_ID;
	NewAlarm.AlarmCond.Name.Length = 4;
	NewAlarm.AlarmCond.Name.Value[0] = 'T';
	NewAlarm.AlarmCond.Name.Value[1] = 'e';
	NewAlarm.AlarmCond.Name.Value[2] = 's';
	NewAlarm.AlarmCond.Name.Value[3] = 't';

	status = saHpiAlarmAdd(session_id, &NewAlarm);

	if (status != SA_OK) {
		// Unable to create a new alarm
		e_print(saHpiAlarmAdd, SA_OK, status);
		retval = SAF_TEST_UNRESOLVED;
	} else {
		// When the Alarm is successfully set

		FirstAlarm = NewAlarm.AlarmId;

		// create the second alarm
		NewAlarm.AlarmId = 0;
		NewAlarm.Timestamp = 0;
		NewAlarm.AlarmCond.Name.Value[3] = '2';
		NewAlarm.AlarmCond.Data.Data[0] = 'b';
		status = saHpiAlarmAdd(session_id, &NewAlarm);

		if (status != SA_OK) {
			e_print(saHpiAlarmAdd, SA_OK, status);
			retval = SAF_TEST_UNRESOLVED;
		} else {
			SecondAlarm = NewAlarm.AlarmId;

			status = saHpiAlarmDelete(session_id,
						  SAHPI_ENTRY_UNSPECIFIED,
						  SAHPI_MINOR);
			if (status != SA_OK) {
				e_print(saHpiAlarmDelete, SA_OK, status);
				retval = SAF_TEST_FAIL;
			} else {
				// Check the alarms to see if they were deleted

				status =
				    saHpiAlarmGet(session_id, FirstAlarm,
						  &NewAlarm);
				if (status != SA_ERR_HPI_NOT_PRESENT) {
					if (status == SA_OK)
						AlarmFound = SAHPI_TRUE;
					else {
						// Unable to read an alarm
						m_print
						    ("Function \"saHpiAlarmGet\" failed to read an user alarm!");
						retval = SAF_TEST_UNRESOLVED;
					}
				} else {
					// Check the second alarm
					status = saHpiAlarmGet(session_id,
							       SecondAlarm,
							       &NewAlarm);
					if (status != SA_ERR_HPI_NOT_PRESENT) {
						if (status == SA_OK)
							AlarmFound = SAHPI_TRUE;
						else {
							// Unable to read an alarm
							m_print
							    ("Function \"saHpiAlarmGet\" failed to read an user alarm!");
							retval =
							    SAF_TEST_UNRESOLVED;
						}
					} else {
						// Both Alarms are not present
						// result from a proper delete
						retval = SAF_TEST_PASS;
					}
				}
				if (AlarmFound) {
					m_print
					    ("Function \"saHpiAlarmDelete\" failed to remove both of the alarms of the same severity!");
					retval = SAF_TEST_FAIL;

					// Clean up individually in hopes that this works
					status = saHpiAlarmDelete(session_id,
								  FirstAlarm,
								  0);
					status = saHpiAlarmDelete(session_id,
								  SecondAlarm,
								  0);
				}

			}
		}
	}
	return (retval);
}

/**********************************************************
*   Main Function
*      takes no arguments
*      
*       returns: SAF_TEST_PASS when successfull
*                SAF_TEST_FAIL when an unexpected error occurs
*************************************************************/
int main(int argc, char **argv)
{
	int retval = SAF_TEST_UNKNOWN;

	retval = process_all_domains(NULL, NULL, Test_Domain);

	return (retval);
}
