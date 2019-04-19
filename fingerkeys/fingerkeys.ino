#include "stdio.h"

// #define SERIALCONSOLE_DEBUG

void setup() {
  pinMode(4, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}
#define middle_ring_pinky LOW
#define thumb_index HIGH

#define thumb  0x01
#define index  0x02
#define middle 0x04
#define ring   0x08
#define pinky  0x10

typedef uint8_t keyMdfyers;
#define left_shiftkey 0x2

bool sendKeystroke_inRange( char c, char rbgn, char rend
                          , uint8_t tgtbgn, keyMdfyers mdfy=0 ) {
  if (c>=rbgn && c<=rend) {
    uint8_t buf[8] = {mdfy,0,tgtbgn+(c-rbgn),0,0,0,0};
    Serial.write(buf, 8);
    buf[2] = 0;
    Serial.write(buf, 8);
    return true;
  }
  return false;
}

bool sendKeystroke_inRange( char c, char rbgn, char rend
                           , const uint8_t* tgts, const uint8_t* mdfyers ) {
  if (c>=rbgn && c<=rend) {
    uint8_t buf[8] = {mdfyers[c-rbgn],0,tgts[c-rbgn],0,0,0,0};
    Serial.write(buf, 8);
    buf[2] = 0;
    Serial.write(buf, 8);
    return true;
  }
  return false;
}

void sendKeystroke(char c, keyMdfyers mdfy=0) {
#ifdef SERIALCONSOLE_DEBUG
  char buf[2] = {c,0};
  Serial.print(buf);
#else
 if (sendKeystroke_inRange(c, 'a','z', 0x4               )
  || sendKeystroke_inRange(c, 'A','Z', 0x4, left_shiftkey)
  || sendKeystroke_inRange(c, '1','9', 0x1e              )
  || sendKeystroke_inRange(c, '0','0', 0x27              ))
   return;
 uint8_t buf[8] = {0};
 if (sendKeystroke_inRange(c, '\t','\n'
           , (const uint8_t[]){0x2b, 0x28}
           , (const uint8_t[]){0x0 , 0x0 })
  || sendKeystroke_inRange(c, ' ','/'
           , (const uint8_t[]){44,30,52,32,50,33,34,36,52,38,39,37,46,54,45,55,56}
           , (const uint8_t[]){ 0, 2, 0, 2, 0, 2, 2, 2, 0, 2, 2, 2, 2, 0, 0, 0, 0})
  || sendKeystroke_inRange(c, ':','@'
           , (const uint8_t[]){51,51,54,46,55,56,31}
           , (const uint8_t[]){ 2, 0, 2, 0, 2, 2, 2})
  || sendKeystroke_inRange(c, '[','`'
           , (const uint8_t[]){47,49,48,35,45,53}
           , (const uint8_t[]){ 0, 0, 0, 2, 2, 0})
  || sendKeystroke_inRange(c, '{','\b'
           , (const uint8_t[]){47,49,48,50,42}
           , (const uint8_t[]){ 2, 2, 2, 2, 0}))
   return;
#endif
}

void loop() {

  // In one iteration check thumb and index, otherwise the other fingers.
  static boolean handpart;

  // Which fingers are currently pressed.
  static int state = 0x0, oldstate = state;

  // Hypothesis: which finger _appeared_ to be pressed last time we checked?
  // Because the analog input isn't very fast, and perhaps due to switch bounce,
  // sometimes a press gives a wrong readout. Therefore, we only want to trigger
  // if the result is also replicated in the next cycle.
  static int hypothesis_t = state, hypothesis_m = state;

  // Toggle between the hand parts.
  handpart = handpart==middle_ring_pinky
              ? thumb_index
              : middle_ring_pinky;

  digitalWrite(4, handpart);
  digitalWrite(7, !handpart);

  // Read out the finger switches.
  int newstate = state;
  oldstate = state;
  delay(1);
  int lvl = analogRead(0);
  switch (handpart) {
   case thumb_index:
    if (lvl<580) {
      newstate &= ~thumb;
      if (lvl<443) {
//  th_ix: 398
        newstate &= ~index;
      } else {
//  th_IX: 487
        newstate |= index;
      }
    } else {
      newstate |= thumb;
      if (lvl<823) {
//  TH_ix: 672
        newstate &= ~index;
      } else {
//  TH_IX: 973
        newstate |= index;
      }
    }
    if (newstate == hypothesis_t) {
      state = newstate;
    } else {
      hypothesis_t = newstate;
    }
    break;
   case middle_ring_pinky:
    if (lvl<484) {
      newstate |= pinky;
      if (lvl<282) {
        newstate |= ring;
        if (lvl<133) {
//  MD_RG_PK: 49
          newstate |= middle;
        } else {
//  md_RG_PK: 216
          newstate &= ~middle;
        }
      } else {
        newstate &= ~ring;
        if (lvl<391) {
//  MD_rg_PK: 347
          newstate |= middle;
        } else {
//  md_rg_PK: 435
          newstate &= ~middle;
        }
      }
    } else {
      newstate &= ~pinky;
      if (lvl<599) {
        newstate |= ring;
        if (lvl<556) {
//  MD_RG_pk: 532
          newstate |= middle;
        } else {
//  md_RG_pk: 579
          newstate &= ~middle;
        }
      } else {
        newstate &= ~ring;
        if (lvl<636) {
//  MD_rg_pk: 618
          newstate |= middle;
        } else {
//  md_rg_pk: 653
          newstate &= ~middle;
        }
      }
    }
    if (newstate == hypothesis_m) {
      state = newstate;
    } else {
      hypothesis_m = newstate;
    }
    break;
  }

  // If something has changed, print to terminal.
  if (state != oldstate) {
    sendKeystroke(state&thumb? 'T': 't');
    sendKeystroke(state&index? 'I': 'i');
    sendKeystroke(state&middle?'M': 'm');
    sendKeystroke(state&ring ? 'R': 'r');
    sendKeystroke(state&pinky? 'P': 'p');
    sendKeystroke('\n');
  }
  delay(1);
}
