unit CastleInternalLinuxEvents;

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

  // MSC events ---------------------------------
  
  // Key codes ----------------------------------

  KEY_RESERVED      = 0;
  KEY_ESC           = 1;
  KEY_1             = 2;
  KEY_2             = 3;
  KEY_3             = 4;
  KEY_4             = 5;
  KEY_5             = 6;
  KEY_6             = 7;
  KEY_7             = 8;
  KEY_8             = 9;
  KEY_9             = 10;
  KEY_0             = 11;
  KEY_MINUS         = 12;
  KEY_EQUAL         = 13;
  KEY_BACKSPACE     = 14;
  KEY_TAB           = 15;
  KEY_Q             = 16;
  KEY_W             = 17;
  KEY_E             = 18;
  KEY_R             = 19;
  KEY_T             = 20;
  KEY_Y             = 21;
  KEY_U             = 22;
  KEY_I             = 23;
  KEY_O             = 24;
  KEY_P             = 25;
  KEY_LEFTBRACE     = 26;
  KEY_RIGHTBRACE    = 27;
  KEY_ENTER         = 28;
  KEY_LEFTCTRL      = 29;
  KEY_A             = 30;
  KEY_S             = 31;
  KEY_D             = 32;
  KEY_F             = 33;
  KEY_G             = 34;
  KEY_H             = 35;
  KEY_J             = 36;
  KEY_K             = 37;
  KEY_L             = 38;
  KEY_SEMICOLON     = 39;
  KEY_APOSTROPHE    = 40;
  KEY_GRAVE         = 41;
  KEY_LEFTSHIFT     = 42;
  KEY_BACKSLASH     = 43;
  KEY_Z             = 44;
  KEY_X             = 45;
  KEY_C             = 46;
  KEY_V             = 47;
  KEY_B             = 48;
  KEY_N             = 49;
  KEY_M             = 50;
  KEY_COMMA         = 51;
  KEY_DOT           = 52;
  KEY_SLASH         = 53;
  KEY_RIGHTSHIFT    = 54;
  KEY_KPASTERISK    = 55;
  KEY_LEFTALT       = 56;
  KEY_SPACE         = 57;
  KEY_CAPSLOCK      = 58;
  KEY_F1            = 59;
  KEY_F2            = 60;
  KEY_F3            = 61;
  KEY_F4            = 62;
  KEY_F5            = 63;
  KEY_F6            = 64;
  KEY_F7            = 65;
  KEY_F8            = 66;
  KEY_F9            = 67;
  KEY_F10           = 68;
  KEY_NUMLOCK       = 69;
  KEY_SCROLLLOCK    = 70;
  KEY_KP7           = 71;
  KEY_KP8           = 72;
  KEY_KP9           = 73;
  KEY_KPMINUS       = 74;
  KEY_KP4           = 75;
  KEY_KP5           = 76;
  KEY_KP6           = 77;
  KEY_KPPLUS        = 78;
  KEY_KP1           = 79;
  KEY_KP2           = 80;
  KEY_KP3           = 81;
  KEY_KP0           = 82;
  KEY_KPDOT         = 83;

  KEY_ZENKAKUHANKAKU  = 85;
  KEY_102ND           = 86;
  KEY_F11             = 87;
  KEY_F12             = 88;
  KEY_RO              = 89;
  KEY_KATAKANA        = 90;
  KEY_HIRAGANA        = 91;
  KEY_HENKAN          = 92;
  KEY_KATAKANAHIRAGANA  = 93;
  KEY_MUHENKAN        = 94;
  KEY_KPJPCOMMA       = 95;
  KEY_KPENTER         = 96;
  KEY_RIGHTCTRL       = 97;
  KEY_KPSLASH         = 98;
  KEY_SYSRQ           = 99;
  KEY_RIGHTALT        = 100;
  KEY_LINEFEED        = 101;
  KEY_HOME            = 102;
  KEY_UP              = 103;
  KEY_PAGEUP          = 104;
  KEY_LEFT            = 105;
  KEY_RIGHT           = 106;
  KEY_END             = 107;
  KEY_DOWN            = 108;
  KEY_PAGEDOWN        = 109;
  KEY_INSERT          = 110;
  KEY_DELETE          = 111;
  KEY_MACRO           = 112;
  KEY_MUTE            = 113;
  KEY_VOLUMEDOWN      = 114;
  KEY_VOLUMEUP        = 115;
  KEY_POWER           = 116 { SC System Power Down };
  KEY_KPEQUAL         = 117;
  KEY_KPPLUSMINUS     = 118;
  KEY_PAUSE           = 119;
  KEY_SCALE           = 120 { AL Compiz Scale (Expose) };

  KEY_KPCOMMA   = 121;
  KEY_HANGEUL   = 122;
  KEY_HANGUEL   = KEY_HANGEUL;
  KEY_HANJA     = 123;
  KEY_YEN       = 124;
  KEY_LEFTMETA  = 125;
  KEY_RIGHTMETA = 126;
  KEY_COMPOSE   = 127;

  KEY_STOP    = 128 { AC Stop };
  KEY_AGAIN   = 129;
  KEY_PROPS   = 130 { AC Properties };
  KEY_UNDO    = 131 { AC Undo };
  KEY_FRONT   = 132;
  KEY_COPY    = 133 { AC Copy };
  KEY_OPEN    = 134 { AC Open };
  KEY_PASTE   = 135 { AC Paste };
  KEY_FIND    = 136 { AC Search };
  KEY_CUT     = 137 { AC Cut };
  KEY_HELP    = 138 { AL Integrated Help Center };
  KEY_MENU    = 139 { Menu (show menu) };
  KEY_CALC    = 140 { AL Calculator };
  KEY_SETUP   = 141;
  KEY_SLEEP   = 142 { SC System Sleep };
  KEY_WAKEUP  = 143 { System Wake Up };
  KEY_FILE    = 144 { AL Local Machine Browser };
  KEY_SENDFILE = 145;
  KEY_DELETEFILE = 146;
  KEY_XFER    = 147;
  KEY_PROG1   = 148;
  KEY_PROG2   = 149;
  KEY_WWW     = 150 { AL Internet Browser };
  KEY_MSDOS   = 151;
  KEY_COFFEE  = 152 { AL Terminal Lock/Screensaver };
  KEY_SCREENLOCK   = KEY_COFFEE;
  KEY_ROTATE_DISPLAY = 153 { Display orientation for e.g. tablets };
  KEY_DIRECTION   = KEY_ROTATE_DISPLAY;
  KEY_CYCLEWINDOWS  = 154;
  KEY_MAIL        = 155;
  KEY_BOOKMARKS   = 156 { AC Bookmarks };
  KEY_COMPUTER    = 157;
  KEY_BACK        = 158 { AC Back };
  KEY_FORWARD     = 159 { AC Forward };
  KEY_CLOSECD     = 160;
  KEY_EJECTCD     = 161;
  KEY_EJECTCLOSECD  = 162;
  KEY_NEXTSONG    = 163;
  KEY_PLAYPAUSE   = 164;
  KEY_PREVIOUSSONG  = 165;
  KEY_STOPCD    = 166;
  KEY_RECORD    = 167;
  KEY_REWIND    = 168;
  KEY_PHONE   = 169 { Media Select Telephone };
  KEY_ISO     = 170;
  KEY_CONFIG    = 171 { AL Consumer Control Configuration };
  KEY_HOMEPAGE  = 172 { AC Home };
  KEY_REFRESH   = 173 { AC Refresh };
  KEY_EXIT    = 174 { AC Exit };
  KEY_MOVE    = 175;
  KEY_EDIT    = 176;
  KEY_SCROLLUP    = 177;
  KEY_SCROLLDOWN    = 178;
  KEY_KPLEFTPAREN   = 179;
  KEY_KPRIGHTPAREN  = 180;
  KEY_NEW     = 181 { AC New };
  KEY_REDO    = 182 { AC Redo/Repeat };

  KEY_F13     = 183;
  KEY_F14     = 184;
  KEY_F15     = 185;
  KEY_F16     = 186;
  KEY_F17     = 187;
  KEY_F18     = 188;
  KEY_F19     = 189;
  KEY_F20     = 190;
  KEY_F21     = 191;
  KEY_F22     = 192;
  KEY_F23     = 193;
  KEY_F24     = 194;

  KEY_PLAYCD    = 200;
  KEY_PAUSECD   = 201;
  KEY_PROG3     = 202;
  KEY_PROG4     = 203;
  KEY_DASHBOARD = 204 { AL Dashboard };
  KEY_SUSPEND   = 205;
  KEY_CLOSE     = 206 { AC Close };
  KEY_PLAY      = 207;
  KEY_FASTFORWARD = 208;
  KEY_BASSBOOST   = 209;
  KEY_PRINT     = 210 { AC Print };
  KEY_HP        = 211;
  KEY_CAMERA    = 212;
  KEY_SOUND     = 213;
  KEY_QUESTION  = 214;
  KEY_EMAIL     = 215;
  KEY_CHAT      = 216;
  KEY_SEARCH    = 217;
  KEY_CONNECT   = 218;
  KEY_FINANCE   = 219 { AL Checkbook/Finance };
  KEY_SPORT     = 220;
  KEY_SHOP      = 221;
  KEY_ALTERASE  = 222;
  KEY_CANCEL    = 223 { AC Cancel };
  KEY_BRIGHTNESSDOWN = 224;
  KEY_BRIGHTNESSUP   = 225;
  KEY_MEDIA     = 226;

  KEY_SWITCHVIDEOMODE = 227 { Cycle between available video
                              outputs (Monitor/LCD/TV-out/etc) };
  KEY_KBDILLUMTOGGLE  = 228;
  KEY_KBDILLUMDOWN  = 229;
  KEY_KBDILLUMUP    = 230;

  KEY_SEND      = 231 { AC Send };
  KEY_REPLY     = 232 { AC Reply };
  KEY_FORWARDMAIL = 233 { AC Forward Msg };
  KEY_SAVE      = 234 { AC Save };
  KEY_DOCUMENTS = 235;

  KEY_BATTERY   = 236;

  KEY_BLUETOOTH = 237;
  KEY_WLAN      = 238;
  KEY_UWB       = 239;

  KEY_UNKNOWN   = 240;

  KEY_VIDEO_NEXT = 241 { drive next video source };
  KEY_VIDEO_PREV = 242 { drive previous video source };
  KEY_BRIGHTNESS_CYCLE = 243 { brightness up, after max is min };
  KEY_BRIGHTNESS_AUTO  = 244 { Set Auto Brightness: manual 
                               brightness control is off,
                               rely on ambient };
  KEY_BRIGHTNESS_ZERO = KEY_BRIGHTNESS_AUTO;
  KEY_DISPLAY_OFF = 245 { display device to off state };

  KEY_WWAN    = 246 { Wireless WAN (LTE, UMTS, GSM, etc.) };
  KEY_WIMAX   = KEY_WWAN;
  KEY_RFKILL  = 247 { Key that controls all radios };

  KEY_MICMUTE = 248 { Mute / unmute the microphone };

  { Code 255 is reserved for special needs of AT keyboard driver }

  BTN_MISC    = $100;
  BTN_0       = $100;
  BTN_1       = $101;
  BTN_2       = $102;
  BTN_3       = $103;
  BTN_4       = $104;
  BTN_5       = $105;
  BTN_6       = $106;
  BTN_7       = $107;
  BTN_8       = $108;
  BTN_9       = $109;

  BTN_MOUSE   = $110;
  BTN_LEFT    = $110;
  BTN_RIGHT   = $111;
  BTN_MIDDLE  = $112;
  BTN_SIDE    = $113;
  BTN_EXTRA   = $114;
  BTN_FORWARD = $115;
  BTN_BACK    = $116;
  BTN_TASK    = $117;

  BTN_JOYSTICK= $120;
  BTN_TRIGGER = $120;
  BTN_THUMB   = $121;
  BTN_THUMB2  = $122;
  BTN_TOP     = $123;
  BTN_TOP2    = $124;
  BTN_PINKIE  = $125;
  BTN_BASE    = $126;
  BTN_BASE2   = $127;
  BTN_BASE3   = $128;
  BTN_BASE4   = $129;
  BTN_BASE5   = $12a;
  BTN_BASE6   = $12b;
  BTN_DEAD    = $12f;

  BTN_GAMEPAD = $130;
  BTN_SOUTH   = $130;
  BTN_A       = BTN_SOUTH;
  BTN_EAST    = $131;
  BTN_B       = BTN_EAST;
  BTN_C       = $132;
  BTN_NORTH   = $133;
  BTN_X       = BTN_NORTH;
  BTN_WEST    = $134;
  BTN_Y       = BTN_WEST;
  BTN_Z       = $135;
  BTN_TL      = $136;
  BTN_TR      = $137;
  BTN_TL2     = $138;
  BTN_TR2     = $139;
  BTN_SELECT  = $13a;
  BTN_START   = $13b;
  BTN_MODE    = $13c;
  BTN_THUMBL  = $13d;
  BTN_THUMBR  = $13e;

  BTN_DIGI          = $140;
  BTN_TOOL_PEN      = $140;
  BTN_TOOL_RUBBER   = $141;
  BTN_TOOL_BRUSH    = $142;
  BTN_TOOL_PENCIL   = $143;
  BTN_TOOL_AIRBRUSH = $144;
  BTN_TOOL_FINGER   = $145;
  BTN_TOOL_MOUSE    = $146;
  BTN_TOOL_LENS     = $147;
  BTN_TOOL_QUINTTAP = $148 { Five fingers on trackpad };
  BTN_STYLUS3       = $149;
  BTN_TOUCH         = $14a;
  BTN_STYLUS        = $14b;
  BTN_STYLUS2       = $14c;
  BTN_TOOL_DOUBLETAP = $14d;
  BTN_TOOL_TRIPLETAP = $14e;
  BTN_TOOL_QUADTAP  = $14f { Four fingers on trackpad };

  BTN_WHEEL     = $150;
  BTN_GEAR_DOWN = $150;
  BTN_GEAR_UP   = $151;

  KEY_OK      = $160;
  KEY_SELECT  = $161;
  KEY_GOTO    = $162;
  KEY_CLEAR   = $163;
  KEY_POWER2  = $164;
  KEY_OPTION  = $165;
  KEY_INFO    = $166 { AL OEM Features/Tips/Tutorial };
  KEY_TIME    = $167;
  KEY_VENDOR    = $168;
  KEY_ARCHIVE   = $169;
  KEY_PROGRAM   = $16a { Media Select Program Guide };
  KEY_CHANNEL   = $16b;
  KEY_FAVORITES = $16c;
  KEY_EPG     = $16d;
  KEY_PVR     = $16e { Media Select Home };
  KEY_MHP     = $16f;
  KEY_LANGUAGE= $170;
  KEY_TITLE   = $171;
  KEY_SUBTITLE= $172;
  KEY_ANGLE   = $173;
  KEY_ZOOM    = $174;
  KEY_MODE    = $175;
  KEY_KEYBOARD  = $176;
  KEY_SCREEN    = $177;
  KEY_PC      = $178 { Media Select Computer };
  KEY_TV      = $179 { Media Select TV };
  KEY_TV2     = $17a { Media Select Cable };
  KEY_VCR     = $17b { Media Select VCR };
  KEY_VCR2    = $17c { VCR Plus };
  KEY_SAT     = $17d { Media Select Satellite };
  KEY_SAT2    = $17e;
  KEY_CD      = $17f { Media Select CD };
  KEY_TAPE    = $180 { Media Select Tape };
  KEY_RADIO   = $181;
  KEY_TUNER   = $182 { Media Select Tuner };
  KEY_PLAYER  = $183;
  KEY_TEXT    = $184;
  KEY_DVD     = $185 { Media Select DVD };
  KEY_AUX     = $186;
  KEY_MP3     = $187;
  KEY_AUDIO   = $188 { AL Audio Browser };
  KEY_VIDEO   = $189 { AL Movie Browser };
  KEY_DIRECTORY = $18a;
  KEY_LIST    = $18b;
  KEY_MEMO    = $18c { Media Select Messages };
  KEY_CALENDAR = $18d;
  KEY_RED     = $18e;
  KEY_GREEN   = $18f;
  KEY_YELLOW  = $190;
  KEY_BLUE    = $191;
  KEY_CHANNELUP   = $192 { Channel Increment };
  KEY_CHANNELDOWN = $193 { Channel Decrement };
  KEY_FIRST   = $194;
  KEY_LAST    = $195 { Recall Last };
  KEY_AB      = $196;
  KEY_NEXT    = $197;
  KEY_RESTART = $198;
  KEY_SLOW    = $199;
  KEY_SHUFFLE = $19a;
  KEY_BREAK   = $19b;
  KEY_PREVIOUS = $19c;
  KEY_DIGITS  = $19d;
  KEY_TEEN    = $19e;
  KEY_TWEN    = $19f;
  KEY_VIDEOPHONE    = $1a0 { Media Select Video Phone };
  KEY_GAMES   = $1a1 { Media Select Games };
  KEY_ZOOMIN    = $1a2 { AC Zoom In };
  KEY_ZOOMOUT   = $1a3 { AC Zoom Out };
  KEY_ZOOMRESET   = $1a4 { AC Zoom };
  KEY_WORDPROCESSOR = $1a5 { AL Word Processor };
  KEY_EDITOR    = $1a6 { AL Text Editor };
  KEY_SPREADSHEET   = $1a7 { AL Spreadsheet };
  KEY_GRAPHICSEDITOR  = $1a8 { AL Graphics Editor };
  KEY_PRESENTATION  = $1a9 { AL Presentation App };
  KEY_DATABASE    = $1aa { AL Database App };
  KEY_NEWS        = $1ab { AL Newsreader };
  KEY_VOICEMAIL   = $1ac { AL Voicemail };
  KEY_ADDRESSBOOK = $1ad { AL Contacts/Address Book };
  KEY_MESSENGER   = $1ae { AL Instant Messaging };
  KEY_DISPLAYTOGGLE = $1af { Turn display (LCD) on and off };
  KEY_BRIGHTNESS_TOGGLE = KEY_DISPLAYTOGGLE;
  KEY_SPELLCHECK  = $1b0   { AL Spell Check };
  KEY_LOGOFF      = $1b1   { AL Logoff };

  KEY_DOLLAR    = $1b2;
  KEY_EURO    = $1b3;

  KEY_FRAMEBACK     = $1b4 { Consumer - transport controls };
  KEY_FRAMEFORWARD  = $1b5;
  KEY_CONTEXT_MENU  = $1b6 { GenDesc - system context menu };
  KEY_MEDIA_REPEAT  = $1b7 { Consumer - transport control };
  KEY_10CHANNELSUP  = $1b8 { 10 channels up (10+) };
  KEY_10CHANNELSDOWN  = $1b9 { 10 channels down (10-) };
  KEY_IMAGES    = $1ba { AL Image Browser };

  KEY_DEL_EOL   = $1c0;
  KEY_DEL_EOS   = $1c1;
  KEY_INS_LINE    = $1c2;
  KEY_DEL_LINE    = $1c3;

  KEY_FN      = $1d0;
  KEY_FN_ESC  = $1d1;
  KEY_FN_F1   = $1d2;
  KEY_FN_F2   = $1d3;
  KEY_FN_F3   = $1d4;
  KEY_FN_F4   = $1d5;
  KEY_FN_F5   = $1d6;
  KEY_FN_F6   = $1d7;
  KEY_FN_F7   = $1d8;
  KEY_FN_F8   = $1d9;
  KEY_FN_F9   = $1da;
  KEY_FN_F10  = $1db;
  KEY_FN_F11  = $1dc;
  KEY_FN_F12  = $1dd;
  KEY_FN_1    = $1de;
  KEY_FN_2    = $1df;
  KEY_FN_D    = $1e0;
  KEY_FN_E    = $1e1;
  KEY_FN_F    = $1e2;
  KEY_FN_S    = $1e3;
  KEY_FN_B    = $1e4;

  KEY_BRL_DOT1    = $1f1;
  KEY_BRL_DOT2    = $1f2;
  KEY_BRL_DOT3    = $1f3;
  KEY_BRL_DOT4    = $1f4;
  KEY_BRL_DOT5    = $1f5;
  KEY_BRL_DOT6    = $1f6;
  KEY_BRL_DOT7    = $1f7;
  KEY_BRL_DOT8    = $1f8;
  KEY_BRL_DOT9    = $1f9;
  KEY_BRL_DOT10   = $1fa;

  KEY_NUMERIC_0   = $200 { used by phones, remote controls, };
  KEY_NUMERIC_1   = $201 { and other keypads };
  KEY_NUMERIC_2   = $202;
  KEY_NUMERIC_3   = $203;
  KEY_NUMERIC_4   = $204;
  KEY_NUMERIC_5   = $205;
  KEY_NUMERIC_6   = $206;
  KEY_NUMERIC_7   = $207;
  KEY_NUMERIC_8   = $208;
  KEY_NUMERIC_9   = $209;
  KEY_NUMERIC_STAR  = $20a;
  KEY_NUMERIC_POUND = $20b;
  KEY_NUMERIC_A   = $20c { Phone key A - HUT Telephony $b9 };
  KEY_NUMERIC_B   = $20d;
  KEY_NUMERIC_C   = $20e;
  KEY_NUMERIC_D   = $20f;

  KEY_CAMERA_FOCUS  = $210;
  KEY_WPS_BUTTON    = $211 { WiFi Protected Setup key };

  KEY_TOUCHPAD_TOGGLE = $212 { Request switch touchpad on or off };
  KEY_TOUCHPAD_ON   = $213;
  KEY_TOUCHPAD_OFF  = $214;

  KEY_CAMERA_ZOOMIN = $215;
  KEY_CAMERA_ZOOMOUT = $216;
  KEY_CAMERA_UP     = $217;
  KEY_CAMERA_DOWN   = $218;
  KEY_CAMERA_LEFT   = $219;
  KEY_CAMERA_RIGHT  = $21a;

  KEY_ATTENDANT_ON  = $21b;
  KEY_ATTENDANT_OFF = $21c;
  KEY_ATTENDANT_TOGGLE = $21d { Attendant call on or off };
  KEY_LIGHTS_TOGGLE = $21e { Reading light on or off };

  BTN_DPAD_UP     = $220;
  BTN_DPAD_DOWN   = $221;
  BTN_DPAD_LEFT   = $222;
  BTN_DPAD_RIGHT  = $223;

  KEY_ALS_TOGGLE  = $230 { Ambient light sensor };
  KEY_ROTATE_LOCK_TOGGLE = $231 { Display rotation lock };

  KEY_BUTTONCONFIG  = $240 { AL Button Configuration };
  KEY_TASKMANAGER   = $241 { AL Task/Project Manager };
  KEY_JOURNAL       = $242 { AL Log/Journal/Timecard };
  KEY_CONTROLPANEL  = $243 { AL Control Panel };
  KEY_APPSELECT     = $244 { AL Select Task/Application };
  KEY_SCREENSAVER   = $245 { AL Screen Saver };
  KEY_VOICECOMMAND  = $246 { Listening Voice Command };
  KEY_ASSISTANT     = $247 { AL Context-aware desktop assistant };

  KEY_BRIGHTNESS_MIN = $250 { Set Brightness to Minimum };
  KEY_BRIGHTNESS_MAX = $251 { Set Brightness to Maximum };

  KEY_KBDINPUTASSIST_PREV   = $260;
  KEY_KBDINPUTASSIST_NEXT   = $261;
  KEY_KBDINPUTASSIST_PREVGROUP = $262;
  KEY_KBDINPUTASSIST_NEXTGROUP = $263;
  KEY_KBDINPUTASSIST_ACCEPT = $264;
  KEY_KBDINPUTASSIST_CANCEL = $265;

  { Diagonal movement keys }
  KEY_RIGHT_UP      = $266;
  KEY_RIGHT_DOWN    = $267;
  KEY_LEFT_UP       = $268;
  KEY_LEFT_DOWN     = $269;

  KEY_ROOT_MENU     = $26a { Show Device's Root Menu };
  { Show Top Menu of the Media (e.g. DVD) }
  KEY_MEDIA_TOP_MENU  = $26b;
  KEY_NUMERIC_11      = $26c;
  KEY_NUMERIC_12      = $26d;
  (*
   * Toggle Audio Description: refers to an audio service that helps blind and
   * visually impaired consumers understand the action in a program. Note: in
   * some countries this is referred to as "Video Description".
   *)
  KEY_AUDIO_DESC      = $26e;
  KEY_3D_MODE         = $26f;
  KEY_NEXT_FAVORITE   = $270;
  KEY_STOP_RECORD     = $271;
  KEY_PAUSE_RECORD    = $272;
  KEY_VOD             = $273 { Video on Demand };
  KEY_UNMUTE          = $274;
  KEY_FASTREVERSE     = $275;
  KEY_SLOWREVERSE     = $276;
  (*
   * Control a data application associated with the currently viewed channel,
   * e.g. teletext or data broadcast application (MHEG, MHP, HbbTV, etc.)
   *)
  KEY_DATA              = $277;
  KEY_ONSCREEN_KEYBOARD = $278;

  BTN_TRIGGER_HAPPY   = $2c0;
  BTN_TRIGGER_HAPPY1  = $2c0;
  BTN_TRIGGER_HAPPY2  = $2c1;
  BTN_TRIGGER_HAPPY3  = $2c2;
  BTN_TRIGGER_HAPPY4  = $2c3;
  BTN_TRIGGER_HAPPY5  = $2c4;
  BTN_TRIGGER_HAPPY6  = $2c5;
  BTN_TRIGGER_HAPPY7  = $2c6;
  BTN_TRIGGER_HAPPY8  = $2c7;
  BTN_TRIGGER_HAPPY9  = $2c8;
  BTN_TRIGGER_HAPPY10 = $2c9;
  BTN_TRIGGER_HAPPY11 = $2ca;
  BTN_TRIGGER_HAPPY12 = $2cb;
  BTN_TRIGGER_HAPPY13 = $2cc;
  BTN_TRIGGER_HAPPY14 = $2cd;
  BTN_TRIGGER_HAPPY15 = $2ce;
  BTN_TRIGGER_HAPPY16 = $2cf;
  BTN_TRIGGER_HAPPY17 = $2d0;
  BTN_TRIGGER_HAPPY18 = $2d1;
  BTN_TRIGGER_HAPPY19 = $2d2;
  BTN_TRIGGER_HAPPY20 = $2d3;
  BTN_TRIGGER_HAPPY21 = $2d4;
  BTN_TRIGGER_HAPPY22 = $2d5;
  BTN_TRIGGER_HAPPY23 = $2d6;
  BTN_TRIGGER_HAPPY24 = $2d7;
  BTN_TRIGGER_HAPPY25 = $2d8;
  BTN_TRIGGER_HAPPY26 = $2d9;
  BTN_TRIGGER_HAPPY27 = $2da;
  BTN_TRIGGER_HAPPY28 = $2db;
  BTN_TRIGGER_HAPPY29 = $2dc;
  BTN_TRIGGER_HAPPY30 = $2dd;
  BTN_TRIGGER_HAPPY31 = $2de;
  BTN_TRIGGER_HAPPY32 = $2df;
  BTN_TRIGGER_HAPPY33 = $2e0;
  BTN_TRIGGER_HAPPY34 = $2e1;
  BTN_TRIGGER_HAPPY35 = $2e2;
  BTN_TRIGGER_HAPPY36 = $2e3;
  BTN_TRIGGER_HAPPY37 = $2e4;
  BTN_TRIGGER_HAPPY38 = $2e5;
  BTN_TRIGGER_HAPPY39 = $2e6;
  BTN_TRIGGER_HAPPY40 = $2e7;

  { We avoid low common keys in module aliases so they don't get huge. }
  KEY_MIN_INTERESTING = KEY_MUTE;
  KEY_MAX     = $2ff;
  KEY_CNT     = (KEY_MAX+1);

type 

  TLinuxInputEvent = packed record
    time: timeval; //sec, nsec: UInt32;
    _type, code: UInt16;
    value: Int32;
  end;

implementation

end.