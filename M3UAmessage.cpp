/**
 *   Prototype VSCP
 *   Copyright (C) 2012, 2013  Kalle Aalto <kalle.aalto@vmail.me>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "M3UAmessage.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include "Common.h"



//#define LOG(x) \
    std::cout << __FILE__ << "::" << __FUNCTION__ << ":" << __LINE__ << ": " << x << std::endl;



M3UAmessage::M3UAmessage() : M3UAtype(UNKNOWN)
{
	return;
}


M3UAmessage::M3UAmessage(ByteStream &_incoming) : M3UAtype(UNKNOWN)
{
	unsigned char msgType, msgClass;

	LOG("Incoming M3UA message: " << _incoming.size() );


	if (_incoming.size() < 8)
	{
		LOG ("ERROR  -  M3UA: message too short");
		return;
	}
	if (_incoming[0] != 0x01 )
	{
		LOG("ERROR  -  M3UA: Invalid protocol version");
		return;
	}

	msgClass = _incoming[2];
	msgType  = _incoming[3];


	switch(msgClass)
	{
	case 0:			// MGMT
		if (msgType == 1) M3UAtype = MGMT_NTFY;
		break;
	case 3:			// ASPSM
		if (msgType == 4) M3UAtype = ASPSM_ASPUP_ACK;
		break;
	case 4:			// ASPTM
		if (msgType == 3) M3UAtype = ASPTM_ASPAC_ACK;
		break;
	case 1:			// Transfer Messages
		if (msgType == 1) M3UAtype = TM_DATA;
		break;
	default:
		LOG("Unsupported M3UAtype");
		break;
	}
	if (M3UAtype != UNKNOWN)
	{
		LOG("M3UAType : " << M3UAtype);
	}
	else {
		LOG("Class: " << msgClass << ", type: " << msgType);
		return;
	}

    int messageLength = 0;
    messageLength |= (_incoming[4] & 0xff000000);
    messageLength |= (_incoming[5] & 0x00ff0000);
    messageLength |= (_incoming[6] & 0x0000ff00);
    messageLength |= (_incoming[7] & 0x000000ff);

    LOG("M3UA message length: " << messageLength);
    m_msg = _incoming;

    return;

}


int M3UAmessage::aspUP (const SctpConnection &_sctpConn)
{

/*
	3.5.1. ASP Up

	      The ASP Up message is used to indicate to a remote M3UA peer that
	      the adaptation layer is ready to receive any ASPSM/ASPTM messages
	      for all Routing Keys that the ASP is configured to serve.

	      The ASP Up message contains the following parameters:

	         ASP Identifier                Optional
	         INFO String                   Optional

	      The format for ASP Up message parameters is as follows:

	        0                   1                   2                   3
	        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	       |         Tag = 0x0011          |           Length = 8          |
	       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	       |                         ASP Identifier                        |
	       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	       |         Tag = 0x0004          |             Length            |
	       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	       \                                                               \
	       /                          INFO String                          /
	       \                                                               \
	       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	   ASP Identifier: 32-bit unsigned integer

	      The optional ASP Identifier parameter contains a unique value that
	      is locally significant among the ASPs that support an AS.  The SGP
	      should save the ASP Identifier to be used, if necessary, with the
	      Notify message (see Section 3.8.2).

	      The format and description of the optional INFO String parameter
	      are the same as for the DUNA message (see Section 3.4.1).
*/

	std::vector <unsigned char> msg;

	int tlvLength = 0;

	msg = commonHeader(0x03, 0x01, tlvLength);
	LOG("sending ASP UP")
	_sctpConn.sendMsg(msg, SCTP_PID_M3UA, 0); // data, protocol ID, stream ID


	return 0;
}


int M3UAmessage::aspACTIVE(const SctpConnection &_scptConn)
{
	std::vector <unsigned char> msg;
	int tlvLength = 0;

/*
 * 3.7.1. ASP Active


   The ASP Active message is sent by an ASP to indicate to a remote M3UA
   peer that it is ready to process signalling traffic for a particular
   Application Server.  The ASP Active message affects only the ASP
   state for the Routing Keys identified by the Routing Contexts, if
   present.

   The ASP Active message contains the following parameters:

      Traffic Mode Type     Optional
      Routing Context       Optional
      INFO String           Optional

   The format for the ASP Active message is as follows:

       0                   1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |         Tag = 0x000b          |          Length = 8           |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                      Traffic Mode Type                        |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |         Tag = 0x0006          |            Length             |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      \                                                               \
      /                       Routing Context                         /
      \                                                               \
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |         Tag = 0x0004          |             Length            |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      \                                                               \
      /                          INFO String                          /
      \                                                               \
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	 */


	msg.push_back(0x00);
	msg.push_back(0x0B);	// tag
	msg.push_back(0x00);
	msg.push_back(0x08);	// length


	/*

   Traffic Mode Type: 32-bit (unsigned integer)

      The Traffic Mode Type parameter identifies the traffic mode of
      operation of the ASP within an AS.  The valid values for Traffic
      Mode Type are shown in the following table:

         1         Override
         2         Loadshare
         3         Broadcast

      Within a particular Routing Context, Override, Loadshare, and
      Broadcast SHOULD NOT be mixed.  The Override value indicates that
      the ASP is operating in Override mode, in which the ASP takes over
      all traffic in an Application Server (i.e., primary/backup
      operation), overriding any currently active ASPs in the AS.  In
      Loadshare mode, the ASP will share in the traffic distribution
      with any other currently active ASPs.  In Broadcast mode, the ASP
      will receive the same messages as any other currently active ASP.
      */

	msg.push_back(0x00);
	msg.push_back(0x00);
	msg.push_back(0x00);
	msg.push_back(0x02);	// Loadshare


	tlvLength = 8;

	ByteStream bsHeader = commonHeader(0x04, 0x01, tlvLength);	// ASPTM, ASPAC
	msg.insert( msg.begin(), bsHeader.begin(), bsHeader.end() );
	LOG("Sending ASP Active");
	_scptConn.sendMsg(msg, SCTP_PID_M3UA, 0); // data, protocol ID, stream ID

	//LOG("msg: "<< msg );
	LOG("SCTP_PID: "<< SCTP_PID_M3UA);

	return 0;

}

ByteStream M3UAmessage::commonHeader(unsigned char _msgClass, unsigned char _msgType, unsigned int _tlvLength)
{

	ByteStream bs;

/*	///////////////////////////////////////////////////////////////////////////////////
	3.1 Common Message Header

	   The protocol messages for MTP3-User Adaptation require a message
	   header which contains the adaptation layer version, the message type,
	   and message length.

	      0                   1                   2                   3
	      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	      |    Version    |   Reserved    | Message Class | Message Type  |
	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	      |                        Message Length                         |
	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	      \                                                               \
	      /                                                               /

	   All fields in an M3UA message MUST be transmitted in the network byte
	   order, unless otherwise stated.

	3.1.1 M3UA Protocol Version: 8 bits (unsigned integer)

	   The version field contains the version of the M3UA adaptation layer.

	   The supported versions are the following:

	         1      Release 1.0

*//////////////////////////////////////////////////////////////////////////////////////////

	bs.push_back(0x01);			// 0th byte, version
	bs.push_back(0x00);			// 1st byte, reserved, must be set to 0

/*/////////////////////////////////////////////////////////////////////////////////////////

	3.1.2  Message Classes and Types

	   The following list contains the valid Message Classes:

	   Message Class: 8 bits (unsigned integer)

	   The following list contains the valid Message Type Classes:

	        0     Management (MGMT) Messages
	        1     Transfer Messages
	        3     ASP State Maintenance (ASPSM) Messages
	        4     ASP Traffic Maintenance (ASPTM) Messages
	        5     Reserved for Other Sigtran Adaptation Layers
	        6     Reserved for Other Sigtran Adaptation Layers
	        7     Reserved for Other Sigtran Adaptation Layers
	        8     Reserved for Other Sigtran Adaptation Layers
	        9     Routing Key Management (RKM) Messages
	       10 to 127 Reserved by the IETF
	      128 to 255 Reserved for IETF-Defined Message Class extensions
	      Message Type: 8 bits (unsigned integer)

*//////////////////////////////////////////////////////////////////////////////////////////

	bs.push_back(_msgClass);		// 2nd byte, class

/*/////////////////////////////////////////////////////////////////////////////////////////

	           The following list contains the message types for the defined
	           messages.

	           Management (MGMT) Messages (See Section 3.8)

	             0        Error (ERR)
	             1        Notify (NTFY)
	          2 to 127    Reserved by the IETF
	        128 to 255    Reserved for IETF-Defined MGMT extensions

	           Transfer Messages (See Section 3.3)

	             0        Reserved
	             1        Payload Data (DATA)
	          2 to 127    Reserved by the IETF
	        128 to 255    Reserved for IETF-Defined Transfer extensions

	           SS7 Signalling Network Management (SSNM) Messages (See Section
	           3.4)

	             0        Reserved
	             1        Destination Unavailable (DUNA)
	             2        Destination Available (DAVA)
	             3        Destination State Audit (DAUD)
	             4        Signalling Congestion (SCON)
	             5        Destination User Part Unavailable (DUPU)
	             6        Destination Restricted (DRST)
	          7 to 127    Reserved by the IETF
	        128 to 255    Reserved for IETF-Defined SSNM extensions

	           ASP State Maintenance (ASPSM) Messages (See Section 3.5)

	             0        Reserved
	             1        ASP Up (ASPUP)
	             2        ASP Down (ASPDN)
	             3        Heartbeat (BEAT)
	             4        ASP Up Acknowledgement (ASPUP ACK)
	             5        ASP Down Acknowledgement (ASPDN ACK)
	             6        Heartbeat Acknowledgement (BEAT ACK)
	          7 to 127    Reserved by the IETF
	        128 to 255    Reserved for IETF-Defined ASPSM extensions

	        ASP Traffic Maintenance (ASPTM) Messages (See Section 3.7)

	              0        Reserved
	              1        ASP Active (ASPAC)
	              2        ASP Inactive (ASPIA)
	              3        ASP Active Acknowledgement (ASPAC ACK)
	              4        ASP Inactive Acknowledgement (ASPIA ACK)
	           5 to 127    Reserved by the IETF
	         128 to 255    Reserved for IETF-Defined ASPTM extensions

	            Routing Key Management (RKM) Messages (See Section 3.6)

	              0        Reserved
	              1        Registration Request (REG REQ)
	              2        Registration Response (REG RSP)
	              3        Deregistration Request (DEREG REQ)
	              4        Deregistration Response (DEREG RSP)
	           5 to 127    Reserved by the IETF
	         128 to 255    Reserved for IETF-Defined RKM extensions
*//////////////////////////////////////////////////////////////////////////////////////////


	bs.push_back(_msgType);		// 2rd byte, type



/*/////////////////////////////////////////////////////////////////////////////////////////

	      3.1.4  Message Length: 32-bits (unsigned integer)

	         The Message Length defines the length of the message in octets,
	         including the Common Header.  The Message Length MUST include
	         parameter padding bytes, if any.

	         Note: A receiver SHOULD accept the message whether or not the final
	         parameter padding is included in the message length.

*//////////////////////////////////////////////////////////////////////////////////////////




	unsigned int totLength = _tlvLength + 8;

	 bs.push_back((totLength >> 24) & 0xff);
	 bs.push_back((totLength >> 16) & 0xff);
	 bs.push_back((totLength >> 8) & 0xff);
	 bs.push_back(totLength & 0xff);

	return bs;

}

void M3UAmessage::decodePayload()

{

	LOG("decodePayload()");
	if(M3UAtype != TM_DATA)
	{
			LOG("DECODING FAILED - M3UA TYPE NOT DATA");
			return;
	}


/*
 *
 * 3.3.1.  Payload Data Message (DATA)


       0                   1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |        Tag = 0x0200           |          Length = 8           |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                       Network Appearance                      |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |        Tag = 0x0006           |          Length = 8           |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                        Routing Context                        |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |        Tag = 0x0210           |             Length            |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      \                                                               \
      /                        Protocol Data                          /
      \                                                               \
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |        Tag = 0x0013           |          Length = 8           |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                        Correlation Id                         |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Network Appearance: 32 bits (unsigned integer)

	Routing Context: 32 bits (unsigned integer)

	Protocol Data: variable length

	 */

	int dataLen = 0;

	for (size_t i = 0; i<m_msg.size(); i++ )
	{
		if (m_msg[i] == 0x02 && m_msg[i+1] == 0x10)
		{
			dataLen = m_msg[i+3];

/*			   The Protocol Data parameter is encoded as follows:
 *
 *			        0                   1                   2                   3
 *			        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *			       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *			       |                     Originating Point Code                    |
 *			       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *			       |                     Destination Point Code                    |
 *			       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *			       |       SI      |       NI      |      MP       |      SLS      |
 *			       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *			       \                                                               \
 *			       /                     User Protocol Data                        /
 *			       \                                                               \
 *			       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *		   Originating Point Code: 32 bits (unsigned integer)
 *		   Destination Point Code: 32 bits (unsigned integer)
 *		   Service Indicator: 8 bits (unsigned integer)
 *		   Network Indicator: 8 bits (unsigned integer)
 *		   Message Priority: 8 bits (unsigned integer)
 *		   Signalling Link Selection: 8 bits (unsigned integer)
 *		   User Protocol Data: variable-length octet string
 */

			for(int y = 4; y < 16 ; y++)
			{
				if(y < 8) m_opc.push_back(m_msg[y+i]);
				else if (y > 7 && y < 12) m_dpc.push_back(m_msg[y+i]);
				else if (y > 11 && y < 13) m_si.push_back(m_msg[y+i]);
				else if (y > 12 && y < 14) m_ni.push_back(m_msg[y+i]);
				else if (y > 13 && y < 15) m_mp.push_back(m_msg[y+i]);
				else if (y > 14 && y < 16) m_sls.push_back(m_msg[y+i]);
			}



			dataLen -= 16;		// skip 16 bytes
			i += 16;			//

			LOG("datalength: " << dataLen << " from: " << i);

			if(i + dataLen - 1 > m_msg.size())
			{
				LOG("ERROR - INVALID LENGTH TAG");
			}

			for(int x = 0; x < dataLen; x++)
			{
				payload.push_back(m_msg[x+i]);
			}
		}
	}
	if(dataLen == 0)
	{
		LOG("ERROR  -  NO DATA PAYLOAD FOUND IN M3UA");
	}

}

ByteStream M3UAmessage::getPayload() const
{

	LOG("getPayload");

	if (M3UAtype != TM_DATA)
	{
		ByteStream dummy;
		LOG("ERROR - CANNOT RETURN M3UA DATA - INVALID TYPE ") ;
		return dummy;
	}
	LOG ("Payload size: " << payload.size());
	return payload;
}

ByteStream M3UAmessage::encodeMsg(const ByteStream &_sccpMsg, const ByteStream &opc,
		const ByteStream &dpc, const ByteStream &si, const ByteStream &ni,const ByteStream &mp,
		const ByteStream &sls)
{

	ByteStream msg;
	int tlvLength = 0;


//	   The DATA message contains the SS7 MTP3-User protocol data, which is
//	   an MTP-TRANSFER primitive, including the complete MTP3 Routing Label.
//	   The DATA message contains the following variable-length parameters:
//
//	        Network Appearance       Optional
//	        Routing Context          Conditional
//	        Protocol Data            Mandatory
//	        Correlation Id           Optional
//
//	   The following format MUST be used for the Data Message:
//
//	       0                   1                   2                   3
//	       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	      |        Tag = 0x0200           |          Length = 8           |
//	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	      |                       Network Appearance                      |
//	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	      |        Tag = 0x0006           |          Length = 8           |
//	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	      |                        Routing Context                        |
//	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	      |        Tag = 0x0210           |             Length            |
//	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	      \                                                               \
//	      /                        Protocol Data                          /
//        \                                                               \
//	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	      |        Tag = 0x0013           |          Length = 8           |
//	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	      |                        Correlation Id                         |
//	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	//routing context tag

	msg.push_back(0x00);
	msg.push_back(0x06);

	msg.push_back(0x00);
	msg.push_back(0x08);

	//routing context

	msg.push_back(0x00);
	msg.push_back(0x00);
	msg.push_back(0x00);
	msg.push_back(0x02);

	// Protocol data tag

	msg.push_back(0x02);
	msg.push_back(0x10) ;

	// Protocol Data length

//	unsigned int dataLen = _sccpMsg.size();
	unsigned int dataLen = _sccpMsg.size() + opc.size() + dpc.size() + si.size() +
				ni.size() + mp.size() + sls.size();

	LOG("sccp message size: " << _sccpMsg.size());
	if(!dataLen)
	{
		ByteStream(empty);
		return empty;
	}

	msg.push_back(0x00);
	msg.push_back(dataLen + 4); // + size of tag + length fields


//	Protocol Data: variable length
//
//		----------------------------------------------------------------------------
//	   The Protocol Data parameter is encoded as follows:
//
//	       0                   1                   2                   3
//	       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	      |                     Originating Point Code                    |
//	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	      |                     Destination Point Code                    |
//	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	      |       SI      |       NI      |      MP       |      SLS      |
//	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	      \                                                               \
//	      /                     User Protocol Data                        /
//	      \                                                               \
//	      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//	   Originating Point Code: 32 bits (unsigned integer)
//	   Destination Point Code: 32 bits (unsigned integer)
//
//	   The Originating and Destination Point Code fields contains the OPC
//	   and DPC from the routing label of the original SS7 message in Network
//	   Byte Order, justified to the least significant bit.  Unused bits are
//	   coded `0'.
//
//	   Service Indicator: 8 bits (unsigned integer)
//
//	   The Service Indicator field contains the SI field from the original
//	   SS7 message justified to the least significant bit.  Unused bits are
//	   coded `0'.
//
//	   Network Indicator: 8-bits (unsigned integer)
//
//	   The Network Indicator contains the NI field from the original SS7
//	   message justified to the least significant bit.  Unused bits are
//	   coded `0'.
//
//	   Message Priority: 8 bits (unsigned integer)
//
//	   The Message Priority field contains the MP bits (if any) from the
//	   original SS7 message, both for ANSI-style and TTC-style [29] message
//	   priority bits. The MP bits are aligned to the least significant bit.
//	   Unused bits are coded `0'.
//
//
//	   Signalling Link Selection: 8 bits (unsigned integer)
//
//	   The Signalling Link Selection field contains the SLS bits from the
//	   routing label of the original SS7 message justified to the least
//	   significant bit and in Network Byte Order.  Unused bits are coded
//	   `0'.
//
//	   User Protocol Data: (byte string)
//
//	   The User Protocol Data field contains a byte string of MTP-User
//	   information from the original SS7 message starting with the first
//	   byte of the original SS7 message following the Routing Label.

	//OPC
	msg.insert(msg.end(), opc.begin(), opc.end());
	//DPC
	msg.insert(msg.end(), dpc.begin(), dpc.end());
	//SI
	msg.insert(msg.end(), si.begin(), si.end());
	//NI
	msg.insert(msg.end(), ni.begin(), ni.end());
	//MP
	msg.insert(msg.end(), mp.begin(), mp.end());
	//SLS
	msg.insert(msg.end(), sls.begin(), sls.end());

	//User Protocol Data
	msg.insert(msg.end(), _sccpMsg.begin(), _sccpMsg.end());
	LOG("bytes to M3UA: " << dataLen);




    // If we didn't supply bytes multiple of four, pad it to four:
    unsigned int padsToBeAdded = 4 - dataLen % 4;

    if (padsToBeAdded < 4) {
        for (unsigned int i=0; i < padsToBeAdded; i++)
            msg.push_back(0x00);

        std::cout << "Padding bytes to be added: "
                  << padsToBeAdded << "\n";
    }


// size on msg vector is total of TLV part size
	tlvLength = msg.size();


//
//       The following list contains the valid Message Type Classes:
//
//            1     Transfer Messages
//
//    Message Type: 8 bits (unsigned integer)
//
//          The following list contains the message types for the defined
//          messages.
//
//
//          Transfer Messages (see Section 3.3)
//
//               0        Reserved
//               1        Payload Data (DATA)
//            2 to 127    Reserved by the IETF
//          128 to 255    Reserved for IETF-Defined Transfer extensions



    ByteStream header = commonHeader(0x01, 0x01, tlvLength);	// class + type
    msg.insert(msg.begin(), header.begin(), header.end());

    LOG("M3UA out: " << msg)
    return msg;



}


