#include "stdio.h"

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

void sendKeystroke(char c) {
  char buf[2] = {c, 0};
  Serial.print(buf);
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
    Serial.print("\n");
  }
  delay(1);
}
