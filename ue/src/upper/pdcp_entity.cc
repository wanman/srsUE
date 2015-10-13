/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2015 The srsUE Developers. See the
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

#include "upper/pdcp_entity.h"
#include "liblte/hdr/liblte_pdcp.h"
#include "liblte/hdr/liblte_security.h"

using namespace srslte;

namespace srsue{

pdcp_entity::pdcp_entity()
  :active(false)
  ,tx_sn(0)
  ,rx_sn(0)
  ,do_security(false)
{
  pool = buffer_pool::get_instance();
}

void pdcp_entity::init(rlc_interface_pdcp *rlc_,
                       rrc_interface_pdcp *rrc_,
                       gw_interface_pdcp  *gw_,
                       srslte::log        *log_,
                       uint32_t            lcid_)
{
  rlc     = rlc_;
  rrc     = rrc_;
  gw      = gw_;
  log     = log_;
  lcid    = lcid_;
  active  = true;
}

bool pdcp_entity::is_active()
{
  return active;
}

// RRC interface
void pdcp_entity::write_sdu(srsue_byte_buffer_t *sdu)
{
  LIBLTE_PDCP_CONTROL_PDU_STRUCT  control;
  srsue_byte_buffer_t            *pdu;

  log->info_hex(sdu->msg, sdu->N_bytes, "UL %s SDU", srsue_rb_id_text[lcid]);

  // Handle SRB messages
  switch(lcid)
  {
  case SRSUE_RB_ID_SRB0:
    // Simply pass on to RLC
    rlc->write_sdu(lcid, sdu);
    break;
  case SRSUE_RB_ID_SRB1:  // Intentional fall-through
  case SRSUE_RB_ID_SRB2:
    // Pack SDU into a control PDU
    pdu = pool->allocate();
    control.count = tx_sn;
    if(do_security)
    {
      /*liblte_pdcp_pack_control_pdu(&control,
                                   (LIBLTE_BYTE_MSG_STRUCT*)sdu,
                                   user->get_k_rrc_int(),
                                   LIBLTE_SECURITY_DIRECTION_UPLINK,
                                   lcid-1,
                                   (LIBLTE_BYTE_MSG_STRUCT*)&pdu);*/
    }else{
      liblte_pdcp_pack_control_pdu(&control,
                                   (LIBLTE_BYTE_MSG_STRUCT*)sdu,
                                   (LIBLTE_BYTE_MSG_STRUCT*)&pdu);
    }
    tx_sn++;
    pool->deallocate(sdu);
    rlc->write_sdu(lcid, pdu);

    break;
  }

  // Handle DRB messages
  if(lcid >= SRSUE_RB_ID_DRB1)
  {

  }
}

// RLC interface
void pdcp_entity::write_pdu(srsue_byte_buffer_t *pdu)
{
  // Handle SRB messages
  switch(lcid)
  {
  case SRSUE_RB_ID_SRB0:
    // Simply pass on to RRC
    log->info_hex(pdu->msg, pdu->N_bytes, "DL %s PDU", srsue_rb_id_text[lcid]);
    rrc->write_pdu(SRSUE_RB_ID_SRB0, pdu);
    break;
  case SRSUE_RB_ID_SRB1:
  case SRSUE_RB_ID_SRB2:
    break;
  }

  // Handle DRB messages
  if(lcid >= SRSUE_RB_ID_DRB1)
  {

  }
}

}
