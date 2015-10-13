/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsUE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <iostream>
#include "upper/rlc_am.h"

// Simple status PDU
uint8_t pdu1[] = {0x00, 0x78};
uint32_t PDU1_LEN = 2;

// Status PDU with 4 NACKs
uint8_t pdu2[] = {0x00 ,0x22 ,0x00 ,0x40 ,0x0C ,0x01 ,0xC0 ,0x20};
uint32_t PDU2_LEN = 8;

int main(int argc, char **argv) {
  srsue::rlc_status_pdu_t s;
  srsue::srsue_byte_buffer_t b1,b2;

  memcpy(b1.msg, &pdu1[0], PDU1_LEN);
  b1.N_bytes = PDU1_LEN;
  rlc_am_read_status_pdu(&b1, &s);
  assert(s.ack_sn == 30);
  assert(s.N_nack == 0);
  rlc_am_write_status_pdu(&s, &b2);
  assert(b2.N_bytes == PDU1_LEN);
  for(int i=0;i<b2.N_bytes;i++)
    assert(b2.msg[i] == b1.msg[i]);

  b1.reset();
  b2.reset();
  memset(&s, 0, sizeof(srsue::rlc_status_pdu_t));

  memcpy(b1.msg, &pdu2[0], PDU2_LEN);
  b1.N_bytes = PDU2_LEN;
  rlc_am_read_status_pdu(&b1, &s);
  assert(s.ack_sn == 8);
  assert(s.N_nack == 4);
  assert(s.nack_sn[0] == 0);
  assert(s.nack_sn[1] == 1);
  assert(s.nack_sn[2] == 3);
  assert(s.nack_sn[3] == 4);
  rlc_am_write_status_pdu(&s, &b2);
  assert(b2.N_bytes == PDU2_LEN);
  for(int i=0;i<b2.N_bytes;i++)
    assert(b2.msg[i] == b1.msg[i]);
}
