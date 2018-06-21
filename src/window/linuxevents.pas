unit LinuxEvents;

interface

uses BaseUnix;

const
  // Event types
  // TODO: those definitions belong ..elsewhere; ask michalis where
  EV_SYN = $00;
  EV_KEY = $01;
  EV_REL = $02;
  EV_ABS = $03;
  EV_MSC = $04;
  EV_SW  = $05;
  EV_LED = $11;
  EV_SND = $12;
  EV_REP = $14;
  EV_FF  = $15;
  EV_PWR = $16;
  EV_FF_STATUS  = $17;
  EV_MAX = $1f;
  EV_CNT =(EV_MAX+1);

  // Synchronization events
  SYN_REPORT    = 0;
  SYN_MT_REPORT = 2;

  // Absolute pointer input
  ABS_X              = $00;
  ABS_Y              = $01;
  ABS_Z              = $02;
  ABS_RX             = $03;
  ABS_RY             = $04;
  ABS_RZ             = $05;
  ABS_MT_SLOT        = $2f;
  ABS_MT_POSITION_X  = $35;
  ABS_MT_POSITION_Y  = $36;
  ABS_MT_TRACKING_ID = $39;

  // Relative pointer input
  REL_X              = $00;
  REL_Y              = $01;
  REL_Z              = $02;
  REL_RX             = $03;
  REL_RY             = $04;
  REL_RZ             = $05;
  REL_HWHEEL         = $06;
  REL_DIAL           = $07;
  REL_WHEEL          = $08;
  REL_MISC           = $09;
  REL_MAX            = $0f;
  REL_CNT            = (REL_MAX + 1);

type 

  TLinuxInputEvent = packed record
    time: timeval; //sec, nsec: UInt32;
    _type, code: UInt16;
    value: Int32;
  end;

implementation

end.