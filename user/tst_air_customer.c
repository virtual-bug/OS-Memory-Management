// Air reservation
// Customer program
#include <inc/lib.h>
#include <user/air.h>

void
_main(void)
{
	int32 parentenvID = sys_getparentenvid();

	// Get the shared variables from the main program ***********************************

	struct Customer * customers = sget(parentenvID, "customers");

	int* custCounter = sget(parentenvID, "custCounter");

	int* cust_ready_queue = sget(parentenvID, "cust_ready_queue");

	int* queue_in = sget(parentenvID, "queue_in");

	// *********************************************************************************

	int custId, flightType;
	sys_waitSemaphore(parentenvID, "custCounterCS");
	{
		custId = *custCounter;
		//cprintf("custCounter= %d\n", *custCounter);
		*custCounter = *custCounter +1;
	}
	sys_signalSemaphore(parentenvID, "custCounterCS");

	//wait on one of the clerks
	sys_waitSemaphore(parentenvID, "clerk");

	//enqueue the request
	flightType = customers[custId].flightType;
	sys_waitSemaphore(parentenvID, "custQueueCS");
	{
		cust_ready_queue[*queue_in] = custId;
		*queue_in = *queue_in +1;
	}
	sys_signalSemaphore(parentenvID, "custQueueCS");

	//signal ready
	sys_signalSemaphore(parentenvID, "cust_ready");

	//wait on finished
	char prefix[30]="cust_finished";
	char id[5]; char sname[50];
	ltostr(custId, id);
	strcconcat(prefix, id, sname);
	sys_waitSemaphore(parentenvID, sname);

	//print the customer status
	if(customers[custId].booked == 1)
	{
		cprintf("cust %d: finished (BOOKED flight %d) \n", custId, flightType);
	}
	else
	{
		cprintf("cust %d: finished (NOT BOOKED) \n", custId);
	}

	//customer is terminated
	sys_signalSemaphore(parentenvID, "custTerminated");

	return;
}
