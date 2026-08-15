#ifndef FLIGHTSTATE_H
#define FLIGHTSTATE_H

enum flight_state {
  INIT,
  IDLE,
  LAUNCH,
  LANDING,
  LOGGING,
  FINISH
};

#endif // FLIGHTSTATE_H