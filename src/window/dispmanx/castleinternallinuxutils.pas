unit CastleInternalLinuxUtils;

interface

uses Classes, CastleKeysMouse;


procedure KeyLinuxToCastle(const Key: Int32; const Shift: TShiftState;
  out MyKey: TKey; out MyCharKey: char);

implementation

uses CastleStringUtils, CastleInternalLinuxEvents, CastleLog;

procedure KeyLinuxToCastle(const Key: Int32; const Shift: TShiftState;
  out MyKey: TKey; out MyCharKey: char);
const
  { Ctrl key on most systems, Command key on macOS. }
  // TODO: this definition may be of use elsewhere too
  ssCtrlOrCommand = {$ifdef DARWIN} ssMeta {$else} ssCtrl {$endif};

  procedure SetLetter(OurKey: TKey; Letter: Char);
  begin
    MyKey := OurKey; // TKey(Ord(keyA)  + Ord(Key) - Ord('A'));
    if ssCtrlOrCommand in Shift then
      MyCharKey := Chr(Ord(CtrlA) + Ord(Letter) - Ord('A')) else
    begin
      MyCharKey := Letter;
      if not (ssShift in Shift) then
        MyCharKey := LoCase(Letter);
    end;
  end;

begin
  MyKey := keyNone;
  MyCharKey := #0;

  case Key of
    KEY_BACK:       begin MyKey := keyBackSpace;       MyCharKey := CharBackSpace; end;
    KEY_TAB:        begin MyKey := keyTab;             MyCharKey := CharTab;       end;
    KEY_ENTER:      begin MyKey := keyEnter;           MyCharKey := CharEnter;     end;
    KEY_LEFTSHIFT:        MyKey := keyShift;
    KEY_RIGHTSHIFT:       MyKey := keyShift;
    KEY_LEFTCTRL:         MyKey := keyCtrl;
    KEY_RIGHTCTRL:        MyKey := keyCtrl;
    KEY_LEFTALT:          MyKey := keyAlt;
    KEY_RIGHTALT:         MyKey := keyAlt;
    KEY_ESC:        begin MyKey := keyEscape;          MyCharKey := CharEscape;    end;
    KEY_SPACE:      begin MyKey := keySpace;           MyCharKey := ' ';           end;
    KEY_PAGEUP:           MyKey := keyPageUp;
    KEY_PAGEDOWN:         MyKey := keyPageDown;
    KEY_END:              MyKey := keyEnd;
    KEY_HOME:             MyKey := keyHome;
    KEY_LEFT:             MyKey := keyLeft;
    KEY_UP:               MyKey := keyUp;
    KEY_RIGHT:            MyKey := keyRight;
    KEY_DOWN:             MyKey := keyDown;
    KEY_INSERT:           MyKey := keyInsert;
    KEY_DELETE:     begin MyKey := keyDelete;          MyCharKey := CharDelete; end;
    KEY_KPPLUS:     begin MyKey := keyNumpadPlus;      MyCharKey := '+';        end;
    KEY_KPMINUS:    begin MyKey := keyNumpadMinus;     MyCharKey := '-';        end;
    KEY_SYSRQ:            MyKey := keyPrintScreen;
    KEY_NUMLOCK:          MyKey := keyNumLock;
    KEY_SCROLLLOCK:       MyKey := keyScrollLock;
    KEY_CAPSLOCK:         MyKey := keyCapsLock;
    KEY_PAUSE:            MyKey := keyPause;
    KEY_COMMA:      begin MyKey := keyComma;           MyCharKey := ','; end;
    KEY_DOT:        begin MyKey := keyPeriod;          MyCharKey := '.'; end;
    KEY_KP0:        begin MyKey := keyNumpad0;         MyCharKey := '0'; end;
    KEY_KP1:        begin MyKey := keyNumpad1;         MyCharKey := '1'; end;
    KEY_KP2:        begin MyKey := keyNumpad2;         MyCharKey := '2'; end;
    KEY_KP3:        begin MyKey := keyNumpad3;         MyCharKey := '3'; end;
    KEY_KP4:        begin MyKey := keyNumpad4;         MyCharKey := '4'; end;
    KEY_KP5:        begin MyKey := keyNumpad5;         MyCharKey := '5'; end;
    KEY_KP6:        begin MyKey := keyNumpad6;         MyCharKey := '6'; end;
    KEY_KP7:        begin MyKey := keyNumpad7;         MyCharKey := '7'; end;
    KEY_KP8:        begin MyKey := keyNumpad8;         MyCharKey := '8'; end;
    KEY_KP9:        begin MyKey := keyNumpad9;         MyCharKey := '9'; end;
    KEY_CLEAR:            MyKey := keyNumpadBegin;
    KEY_KPASTERISK: begin MyKey := keyNumpadMultiply;  MyCharKey := '*'; end;
    KEY_KPSLASH:    begin MyKey := keyNumpadDivide;    MyCharKey := '/'; end;
    KEY_MINUS:      begin MyKey := keyMinus;           MyCharKey := '-'; end;
    KEY_EQUAL:
      if ssShift in Shift then
      begin
        MyKey := keyPlus ; MyCharKey := '+';
      end else
      begin
        MyKey := keyEqual; MyCharKey := '=';
      end;

    KEY_0:
      begin
        MyKey := key0;
        MyCharKey := '0';
      end;

    KEY_1 .. KEY_9:
      begin
        MyKey := TKey(Ord(key1)  + (Key - KEY_1));
        MyCharKey := Chr(Ord('1') + (Key - KEY_1));
      end;

    // Unfortunately, Linux keycodes for letters are not in alphabetical order!
    // (the order is in fact US QWERTY)
    KEY_A : SetLetter(keyA, 'a');
    KEY_B : SetLetter(keyB, 'b');
    KEY_C : SetLetter(keyC, 'c');
    KEY_D : SetLetter(keyD, 'd');
    KEY_E : SetLetter(keyE, 'e');
    KEY_F : SetLetter(keyF, 'f');
    KEY_G : SetLetter(keyG, 'g');
    KEY_H : SetLetter(keyH, 'h');
    KEY_I : SetLetter(keyI, 'i');
    KEY_J : SetLetter(keyJ, 'j');
    KEY_K : SetLetter(keyK, 'k');
    KEY_L : SetLetter(keyL, 'l');
    KEY_M : SetLetter(keyM, 'm');
    KEY_N : SetLetter(keyN, 'n');
    KEY_O : SetLetter(keyO, 'o');
    KEY_P : SetLetter(keyP, 'p');
    KEY_R : SetLetter(keyR, 'r');
    KEY_S : SetLetter(keyS, 's');
    KEY_T : SetLetter(keyT, 't');
    KEY_U : SetLetter(keyU, 'u');
    KEY_V : SetLetter(keyV, 'v');
    KEY_W : SetLetter(keyW, 'w');
    KEY_X : SetLetter(keyX, 'x');
    KEY_Y : SetLetter(keyY, 'y');
    KEY_Z : SetLetter(keyZ, 'z');

    KEY_F1 .. KEY_F10: MyKey := TKey(Ord(keyF1 ) + (Key - KEY_F1 ));
    KEY_F11, KEY_F12 : MyKey := TKey(Ord(keyF11) + (Key - KEY_F11));
  end;

  if (MyKey = keyNone) and (MyCharKey = #0) then
    WritelnLog('CastleLinuxUtils', 'Cannot translate Linux KEY_xxx key code %d to Castle Game Engine key', [Key]);
end;

end.
