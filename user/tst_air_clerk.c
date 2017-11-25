// Air reservation
// Clerk program
#include <inc/lib.h>
#include <user/air.h>

void
_main(void)
{
	int parentenvID = sys_getparentenvid();

	// Get the shared variables from the main program ***********************************

	struct Customer * customers = sget(parentenvID, "customers");

	int* flight1Counter = sget(parentenvID, "flight1Counter");
	int* flight2Counter = sget(parentenvID, "flight2Counter");

	int* flight1BookedCounter = sget(parentenvID, "flightBooked1Counter");
	int* flight2BookedCounter = sget(parentenvID, "flightBooked2Counter");

	int* flight1BookedArr = sget(parentenvID, "flightBooked1Arr");
	int* flight2BookedArr = sget(parentenvID, "flightBooked2Arr");

	int* cust_ready_queue = sget(parentenvID, "cust_ready_queue");

	int* queue_out = sget(parentenvID, "queue_out");
	//cprintf("address of queue_out = %d\n", queue_out);
	// *********************************************************************************

	while(1==1)
	{
		int custId;
		//wait for a customer
		sys_waitSemaphore(parentenvID, "cust_ready");

		//dequeue the customer info
		sys_waitSemaphore(parentenvID, "custQueueCS");
		{
			//cprintf("*queue_out = %d\n", *queue_out);
			custId = cust_ready_queue[*queue_out];
			*queue_out = *queue_out +1;
		}
		sys_signalSemaphore(parentenvID, "custQueueCS");

		//try reserving on the required flight
		int custFlightType = customers[custId].flightType;
		//cprintf("custId dequeued = %d, ft = %d\n", custId, customers[custId].flightType);

		switch (custFlightType)
		{
		case 1:
		{
			//Check and update Flight1
			sys_waitSemaphore(parentenvID, "flight1CS");
			{
				if(*flight1Counter > 0)
				{
					*flight1Counter = *flight1Counter - 1;
					customers[custId].booked = 1;
					flight1BookedArr[*flight1BookedCounter] = custId;
					*flight1BookedCounter =*flight1BookedCounter+1;
				}
				else
				{

				}
			}
			sys_signalSemaphore(parentenvID, "flight1CS");
		}

		break;
		case 2:
		{
			//Check and update Flight2
			sys_waitSemaphore(parentenvID, "flight2CS");
			{
				if(*flight2Counter > 0)
				{
					*flight2Counter = *flight2Counter - 1;
					customers[custId].booked = 1;
					flight2BookedArr[*flight2BookedCounter] = custId;
					*flight2BookedCounter =*flight2BookedCounter+1;
				}
				else
				{

				}
			}
			sys_signalSemaphore(parentenvID, "flight2CS");
		}
		break;
		case 3:
		{
			//Check and update Both Flights
			sys_waitSemaphore(parentenvID, "flight1CS"); sys_waitSemaphore(parentenvID, "flight2CS");
			{
				if(*flight1Counter > 0 && *flight2Counter >0 )
				{
					*flight1Counter = *flight1Counter - 1;
					customers[custId].booked = 1;
					flight1BookedArr[*flight1BookedCounter] = custId;
					*flight1BookedCounter =*flight1BookedCounter+1;

					*flight2Counter = *flight2Counter - 1;
					customers[custId].booked = 1;
					flight2BookedArr[*flight2BookedCounter] = custId;
					*flight2BookedCounter =*flight2BookedCounter+1;

				}
				else
				{

				}
			}
			sys_signalSemaphore(parentenvID, "flight2CS"); sys_signalSemaphore(parentenvID, "flight1CS");
		}
		break;
		default:
			panic("customer must have flight type\n");
		}

		//signal finished
		char prefix[30]="cust_finished";
		char id[5]; char sname[50];
		ltostr(custId, id);
		strcconcat(prefix, id, sname);
		sys_signalSemaphore(parentenvID, sname);

		//signal the clerk
		sys_signalSemaphore(parentenvID, "clerk");
	}
}
