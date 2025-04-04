#include <types.h>
#include <lib.h>
#include <synchprobs.h>
#include <synch.h>
#include <opt-A1.h>

/*
 * This simple default synchronization mechanism allows only vehicle at a time
 * into the intersection.   The intersectionSem is used as a a lock.
 * We use a semaphore rather than a lock so that this code will work even
 * before locks are implemented.
 */

/*
 * Replace this default synchronization mechanism with your own (better) mechanism
 * needed for your solution.   Your mechanism may use any of the available synchronzation
 * primitives, e.g., semaphores, locks, condition variables.   You are also free to
 * declare other global variables if your solution requires them.
 */

static struct lock *NE, *NS, *NW,
                   *EN, *ES, *EW,
                   *SN, *SE, *SW,
                   *WN, *WE, *WS;


/*
 * The simulation driver will call this function once before starting
 * the simulation
 *
 * You can use it to initialize synchronization and other variables.
 *
 */
void
intersection_sync_init(void)
{
  NE = lock_create("North-to-East Lock");
  if (NE == NULL) {
    panic("could not create north-to-east lock");
  }

  NS = lock_create("North-to-South Lock");
  if (NS == NULL) {
    panic("could not create north-to-south lock");
  }

  NW = lock_create("North-to-West Lock");
  if (NW == NULL) {
    panic("could not create north-to-west lock");
  }

  EN = lock_create("East-to-North Lock");
  if (EN == NULL) {
    panic("could not create east-to-north lock");
  }

  ES = lock_create("East-to-South Lock");
  if (ES == NULL) {
    panic("could not create east-to-south lock");
  }

  EW = lock_create("East-to-West Lock");
  if (EW == NULL) {
    panic("could not create east-to-west lock");
  }

  SN = lock_create("South-to-North Lock");
  if (SN == NULL) {
    panic("could not create south-to-north lock");
  }

  SE = lock_create("South-to-East Lock");
  if (SE == NULL) {
    panic("could not create south-to-east lock");
  }

  SW = lock_create("South-to-West Lock");
  if (SW == NULL) {
    panic("could not create south-to-west lock");
  }

  WN = lock_create("West-to-North Lock");
  if (WN == NULL) {
    panic("could not create west-to-north lock");
  }

  WE = lock_create("West-to-East Lock");
  if (WE == NULL) {
    panic("could not create west-to-east lock");
  }

  WS = lock_create("West-to-South Lock");
  if (WS == NULL) {
    panic("could not create west-to-south lock");
  }

}

/*
 * The simulation driver will call this function once after
 * the simulation has finished
 *
 * You can use it to clean up any synchronization and other variables.
 *
 */
void
intersection_sync_cleanup(void)
{
  lock_destroy(NE);
  lock_destroy(NS);
  lock_destroy(NW);
  lock_destroy(EN);
  lock_destroy(ES);
  lock_destroy(EW);
  lock_destroy(SN);
  lock_destroy(SE);
  lock_destroy(SW);
  lock_destroy(WN);
  lock_destroy(WE);
  lock_destroy(WS);
}


/*
 * The simulation driver will call this function each time a vehicle
 * tries to enter the intersection, before it enters.
 * This function should cause the calling simulation thread
 * to block until it is OK for the vehicle to enter the intersection.
 *
 * parameters:
 *    * origin: the Direction from which the vehicle is arriving
 *    * destination: the Direction in which the vehicle is trying to go
 *
 * return value: none
 */

void
intersection_before_entry(Direction origin, Direction destination)
{
  if (origin == north && destination == east) {
    lock_acquire(ES);
    lock_acquire(EW);
    lock_acquire(SN);
    lock_acquire(SE);
    lock_acquire(SW);
    lock_acquire(WN);
    lock_acquire(WE);
  } else if (origin == north && destination == south) {
    lock_acquire(ES);
    lock_acquire(EW);
    lock_acquire(SW);
    lock_acquire(WN);
    lock_acquire(WE);
    lock_acquire(WS);
  } else if (origin == north && destination == west) {
    lock_acquire(EW);
    lock_acquire(SW);
  } else if (origin == east && destination == north) {
    lock_acquire(SN);
    lock_acquire(WN);
  } else if (origin == east && destination == south) {
    lock_acquire(NE);
    lock_acquire(NS);
    lock_acquire(SN);
    lock_acquire(SW);
    lock_acquire(WN);
    lock_acquire(WE);
    lock_acquire(WS);
  } else if (origin == east && destination == west) {
    lock_acquire(NE);
    lock_acquire(NS);
    lock_acquire(NW);
    lock_acquire(SN);
    lock_acquire(SW);
    lock_acquire(WN);
  } else if (origin == south && destination == north) {
    lock_acquire(NE);
    lock_acquire(EN);
    lock_acquire(ES);
    lock_acquire(EW);
    lock_acquire(WN);
    lock_acquire(WE);
  } else if (origin == south && destination == east) {
    lock_acquire(NE);
    lock_acquire(WE);
  } else if (origin == south && destination == west) {
    lock_acquire(NE);
    lock_acquire(NS);
    lock_acquire(NW);
    lock_acquire(ES);
    lock_acquire(EW);
    lock_acquire(WN);
    lock_acquire(WE);
  } else if (origin == west && destination == north) {
    lock_acquire(NE);
    lock_acquire(NS);
    lock_acquire(EN);
    lock_acquire(ES);
    lock_acquire(EW);
    lock_acquire(SN);
    lock_acquire(SW);
  } else if (origin == west && destination == east) {
    lock_acquire(NE);
    lock_acquire(NS);
    lock_acquire(ES);
    lock_acquire(SN);
    lock_acquire(SE);
    lock_acquire(SW);
  } else {
    lock_acquire(NS);
    lock_acquire(ES);
  }
}


/*
 * The simulation driver will call this function each time a vehicle
 * leaves the intersection.
 *
 * parameters:
 *    * origin: the Direction from which the vehicle arrived
 *    * destination: the Direction in which the vehicle is going
 *
 * return value: none
 */

void
intersection_after_exit(Direction origin, Direction destination)
{
  if (origin == north && destination == east) {
    lock_release(ES);
    lock_release(EW);
    lock_release(SN);
    lock_release(SE);
    lock_release(SW);
    lock_release(WN);
    lock_release(WE);
  } else if (origin == north && destination == south) {
    lock_release(ES);
    lock_release(EW);
    lock_release(SW);
    lock_release(WN);
    lock_release(WE);
    lock_release(WS);
  } else if (origin == north && destination == west) {
    lock_release(EW);
    lock_release(SW);
  } else if (origin == east && destination == north) {
    lock_release(SN);
    lock_release(WN);
  } else if (origin == east && destination == south) {
    lock_release(NE);
    lock_release(NS);
    lock_release(SN);
    lock_release(SW);
    lock_release(WN);
    lock_release(WE);
    lock_release(WS);
  } else if (origin == east && destination == west) {
    lock_release(NE);
    lock_release(NS);
    lock_release(NW);
    lock_release(SN);
    lock_release(SW);
    lock_release(WN);
  } else if (origin == south && destination == north) {
    lock_release(NE);
    lock_release(EN);
    lock_release(ES);
    lock_release(EW);
    lock_release(WN);
    lock_release(WE);
  } else if (origin == south && destination == east) {
    lock_release(NE);
    lock_release(WE);
  } else if (origin == south && destination == west) {
    lock_release(NE);
    lock_release(NS);
    lock_release(NW);
    lock_release(ES);
    lock_release(EW);
    lock_release(WN);
    lock_release(WE);
  } else if (origin == west && destination == north) {
    lock_release(NE);
    lock_release(NS);
    lock_release(EN);
    lock_release(ES);
    lock_release(EW);
    lock_release(SN);
    lock_release(SW);
  } else if (origin == west && destination == east) {
    lock_release(NE);
    lock_release(NS);
    lock_release(ES);
    lock_release(SN);
    lock_release(SE);
    lock_release(SW);
  } else {
    lock_release(NS);
    lock_release(ES);
  }
}
