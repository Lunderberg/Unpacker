#ifndef _RINGITEMTYPES_H_
#define _RINGITEMTYPES_H_
#ifndef __CINT__

// From http://docs.nscl.msu.edu/daq/newsite/11.0/x312.html

enum class HeaderType {
  BEGIN_RUN            = 1,    // 0x0001
  END_RUN              = 2,    // 0x0002
  PAUSE_RUN            = 3,    // 0x0003
  RESUME_RUN           = 4,    // 0x0004
  // ABNORMAL_ENDRUN      = 4,    // 0x0004
  PACKET_TYPES         = 10,   // 0x000A
  MONITORED_VARIABLES  = 11,   // 0x000B
  RING_FORMAT          = 12,   // 0x000C
  PERIODIC_SCALERS     = 20,   // 0x0014
  PHYSICS_EVENT        = 30,   // 0x001E
  PHYSICS_EVENT_COUNT  = 31,   // 0x001F
  EVB_FRAGMENT         = 40,   // 0x0028
  EVB_UNKNOWN_PAYLOAD  = 41,   // 0x0029
  EVB_GLOM_INFO        = 42,   // 0x002A
  FIRST_USER_ITEM_CODE = 32768 // 0x8000
};

#endif
#endif /* _RINGITEMTYPES_H_ */
