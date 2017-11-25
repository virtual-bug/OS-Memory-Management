// Air reservation problem
// Master program
//TODOTA MAKE SURE THAT MAX_SEMAPHORES macro equals at least 100
#include <inc/lib.h>
#include <user/air.h>
int find(int* arr, int size, int val);

void
_main(void)
{
	int envID = sys_getenvid();

	// *************************************************************************************************
	/// Shared Variables Region ************************************************************************
	// *************************************************************************************************

	int numOfCustomers = 15;
	int flight1Customers = 3;
	int flight2Customers = 8;
	int flight3Customers = 4;

	int flight1NumOfTickets = 8;
	int flight2NumOfTickets = 15;

	struct Customer * custs;
	custs = smalloc("customers", sizeof(struct Customer)*numOfCustomers, 1);
	//sys_createSharedObject("customers", sizeof(struct Customer)*numOfCustomers, 1, (void**)&custs);


	{
		int f1 = 0;
		for(;f1<flight1Customers; ++f1)
		{
			custs[f1].booked = 0;
			custs[f1].flightType = 1;
		}

		int f2=f1;
		for(;f2<f1+flight2Customers; ++f2)
		{
			custs[f2].booked = 0;
			custs[f2].flightType = 2;
		}

		int f3=f2;
		for(;f3<f2+flight3Customers; ++f3)
		{
			custs[f3].booked = 0;
			custs[f3].flightType = 3;
		}
	}

	int* custCounter = smalloc("custCounter", sizeof(int), 1);
	*custCounter = 0;

	int* flight1Counter = smalloc("flight1Counter", sizeof(int), 1);
	*flight1Counter = flight1NumOfTickets;

	int* flight2Counter = smalloc("flight2Counter", sizeof(int), 1);
	*flight2Counter = flight2NumOfTickets;

	int* flight1BookedCounter = smalloc("flightBooked1Counter", sizeof(int), 1);
	*flight1BookedCounter = 0;

	int* flight2BookedCounter = smalloc("flightBooked2Counter", sizeof(int), 1);
	*flight2BookedCounter = 0;

	int* flight1BookedArr = smalloc("flightBooked1Arr", sizeof(int)*flight1NumOfTickets, 1);
	int* flight2BookedArr = smalloc("flightBooked2Arr", sizeof(int)*flight2NumOfTickets, 1);

	int* cust_ready_queue = smalloc("cust_ready_queue", sizeof(int)*numOfCustomers, 1);

	int* queue_in = smalloc("queue_in", sizeof(int), 1);
	*queue_in = 0;

	int* queue_out = smalloc("queue_out", sizeof(int), 1);
	*queue_out = 0;

	// *************************************************************************************************
	/// Semaphores Region ******************************************************************************
	// *************************************************************************************************
	sys_createSemaphore("flight1CS", 1);
	sys_createSemaphore("flight2CS", 1);

	sys_createSemaphore("custCounterCS", 1);
	sys_createSemaphore("custQueueCS", 1);

	sys_createSemaphore("clerk", 3);

	sys_createSemaphore("cust_ready", 0);

	sys_createSemaphore("custTerminated", 0);

	int s=0;
	for(s=0; s<numOfCustomers; ++s)
	{
		char prefix[30]="cust_finished";
		char id[5]; char sname[50];
		ltostr(s, id);
		strcconcat(prefix, id, sname);
		sys_createSemaphore(sname, 0);
	}

	// *************************************************************************************************
	// start all clerks and customers ******************************************************************
	// *************************************************************************************************

	//3 clerks
	uint32 envId;
	envId = sys_create_env("taircl", (myEnv->page_WS_max_size));
	sys_run_env(envId);

	envId = sys_create_env("taircl", (myEnv->page_WS_max_size));
	sys_run_env(envId);

	envId = sys_create_env("taircl", (myEnv->page_WS_max_size));
	sys_run_env(envId);

	//customers
	int c;
	for(c=0; c< numOfCustomers;++c)
	{
		envId = sys_create_env("taircu", (myEnv->page_WS_max_size));
		sys_run_env(envId);
	}

	//wait until all customers terminated
	for(c=0; c< numOfCustomers;++c)
	{
		sys_waitSemaphore(envID, "custTerminated");
	}

	env_sleep(1500);

	//print out the results
	int b;
	for(b=0; b< (*flight1BookedCounter);++b)
	{
		cprintf("cust %d booked flight 1, originally ordered %d\n", flight1BookedArr[b], custs[flight1BookedArr[b]].flightType);
	}

	for(b=0; b< (*flight2BookedCounter);++b)
	{
		cprintf("cust %d booked flight 2, originally ordered %d\n", flight2BookedArr[b], custs[flight2BookedArr[b]].flightType);
	}

	//check out the final results and semaphores
	{
		int f1 = 0;
		for(;f1<flight1Customers; ++f1)
		{
			if(find(flight1BookedArr, flight1NumOfTickets, f1) != 1)
			{
				panic("Error, wrong booking for user %d\n", f1);
			}
		}

		int f2=f1;
		for(;f2<f1+flight2Customers; ++f2)
		{
			if(find(flight2BookedArr, flight2NumOfTickets, f2) != 1)
			{
				panic("Error, wrong booking for user %d\n", f2);
			}
		}

		int f3=f2;
		for(;f3<f2+flight3Customers; ++f3)
		{
			if(find(flight1BookedArr, flight1NumOfTickets, f3) != 1 || find(flight2BookedArr, flight2NumOfTickets, f3) != 1)
			{
				panic("Error, wrong booking for user %d\n", f3);
			}
		}

		assert(sys_getSemaphoreValue(envID, "flight1CS") == 1);
		assert(sys_getSemaphoreValue(envID, "flight2CS") == 1);

		assert(sys_getSemaphoreValue(envID, "custCounterCS") ==  1);
		assert(sys_getSemaphoreValue(envID, "custQueueCS") ==  1);

		assert(sys_getSemaphoreValue(envID, "clerk") == 3);

		assert(sys_getSemaphoreValue(envID, "cust_ready") == -3);

		assert(sys_getSemaphoreValue(envID, "custTerminated") ==  0);

		int s=0;
		for(s=0; s<numOfCustomers; ++s)
		{
			char prefix[30]="cust_finished";
			char id[5]; char cust_finishedSemaphoreName[50];
			ltostr(s, id);
			strcconcat(prefix, id, cust_finishedSemaphoreName);
			assert(sys_getSemaphoreValue(envID, cust_finishedSemaphoreName) ==  0);
		}

		cprintf("Congratulations, All reservations are successfully done... have a nice flight :)\n");
	}

}


int find(int* arr, int size, int val)
{

	int result = 0;

	int i;
	for(i=0; i<size;++i )
	{
		if(arr[i] == val)
		{
			result = 1;
			break;
		}
	}

	return result;
}
