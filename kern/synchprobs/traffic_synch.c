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

static struct lock *NE, NS, NW,
                    EN, ES, EW,
                    SN, SE, SW,
                    WN, WE, WS;


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
  /* replace this default implementation with your own implementation */
  KASSERT(intersectionSem != NULL);
  sem_destroy(intersectionSem);
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
  /* replace this default implementation with your own implementation */
  (void)origin;  /* avoid compiler complaint about unused parameter */
  (void)destination; /* avoid compiler complaint about unused parameter */
  KASSERT(intersectionSem != NULL);
  P(intersectionSem);
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
  /* replace this default implementation with your own implementation */
  (void)origin;  /* avoid compiler complaint about unused parameter */
  (void)destination; /* avoid compiler complaint about unused parameter */
  KASSERT(intersectionSem != NULL);
  V(intersectionSem);
}
