#ifndef FLIGHTSTATE_H
#define FLIGHTSTATE_H

enum flight_state {
  INIT,
  IDLE,
  LAUNCH,
  LANDING,
  LOGGINGUP,
  LOGGINGDOWN,
  FINISH
};

#endif // FLIGHTSTATE_H