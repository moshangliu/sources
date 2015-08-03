#include <string.h>       // Keil: Line added

// NXP: Include some header files that diifers from the origin
#include "lpc177x_8x_libcfg.h"
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_gpio.h"
#include "lpc177x_8x_nvic.h"
#include "lpc177x_8x_adc.h"
#include "tcpip.h"
#include "EMAC.h"         // Keil: Line added


TTCPStateMachine TCPStateMachine;         // perhaps the most important var at all ;-)
TLastFrameSent LastFrameSent;             // retransmission type

unsigned short ISNGenHigh;                // upper word of our Initial Sequence Number
unsigned long TCPSeqNr;                   // next sequence number to send
unsigned long TCPUNASeqNr;                // last unaknowledged sequence number
// incremented AFTER sending data
unsigned long TCPAckNr;                   // next seq to receive and ack to send
// incremented AFTER receiving data
unsigned char TCPTimer;                   // inc'd each 262ms
unsigned char RetryCounter;               // nr. of retransmissions

// properties of the just received frame
unsigned short RecdFrameLength;           // EMAC reported frame length
unsigned short RecdFrameMAC[3];           // 48 bit MAC
unsigned short RecdFrameIP[2];            // 32 bit IP
unsigned short RecdIPFrameLength;         // 16 bit IP packet length

// the next 3 buffers must be word-aligned!
// (here the 'RecdIPFrameLength' above does that)
unsigned short _TxFrame1[ ( ETH_HEADER_SIZE + IP_HEADER_SIZE + TCP_HEADER_SIZE + MAX_TCP_TX_DATA_SIZE ) /2];
#pragma data_alignment=4
unsigned short _TxFrame2[ ( ETH_HEADER_SIZE + MAX_ETH_TX_DATA_SIZE ) /2];
#pragma data_alignment=4
unsigned short _RxTCPBuffer[MAX_TCP_RX_DATA_SIZE/2]; // space for incoming TCP-data

#define TxFrame1      ((unsigned char *)_TxFrame1)
#define TxFrame2      ((unsigned char *)_TxFrame2)
#define RxTCPBuffer   ((unsigned char *)_RxTCPBuffer)

unsigned short TxFrame1Size;              // bytes to send in TxFrame1
unsigned char TxFrame2Size;               // bytes to send in TxFrame2

unsigned char TransmitControl;
unsigned char TCPFlags;

// API global vars and flags
unsigned short TCPRxDataCount;            // nr. of bytes rec'd
unsigned short TCPTxDataCount;            // nr. of bytes to send

unsigned short TCPLocalPort;              // TCP ports
unsigned short TCPRemotePort;

unsigned short RemoteMAC[3];              // MAC and IP of current TCP-session
unsigned short RemoteIP[2];

unsigned short MyIP[2];
unsigned short SubnetMask[2];
unsigned short GatewayIP[2];
unsigned char MyMAC[6];


unsigned char SocketStatus;

const unsigned short MyIP_init[] =                  // "MYIP1.MYIP2.MYIP3.MYIP4"
{
    MYIP_1 + ( MYIP_2 << 8 ),                             // use 'unsigned int' to
    MYIP_3 + ( MYIP_4 << 8 )                             // achieve word alignment
};

const unsigned short SubnetMask_init[] =// "SUBMASK1.SUBMASK2.SUBMASK3.SUBMASK4"
{
    SUBMASK_1 + ( SUBMASK_2 << 8 ),                       // use 'unsigned int' to
    SUBMASK_3 + ( SUBMASK_4 << 8 )                       // achieve word alignment
};

const unsigned short GatewayIP_init[] =             // "GWIP1.GWIP2.GWIP3.GWIP4"
{
    GWIP_1 + ( GWIP_2 << 8 ),                             // use 'unsigned int' to
    GWIP_3 + ( GWIP_4 << 8 )                             // achieve word alignment
};

const unsigned char MyMAC_init[6] =                       // "M1-M2-M3-M4-M5-M6"
{
    MYMAC_1, MYMAC_2, MYMAC_3,
    MYMAC_4, MYMAC_5, MYMAC_6
};

// NXP LowLevel initializing implementation
// initalizes the LAN-controller, reset flags, starts timer-ISR

void TCPLowLevelInit ( void )
{
    Init_EMAC();
    TransmitControl = 0;
    TCPFlags = 0;
    TCPStateMachine = CLOSED;
    SocketStatus = 0;
}

// does a passive open (listen on 'MyIP:TCPLocalPort' for an incoming
// connection)

void TCPPassiveOpen ( void )
{
    if ( TCPStateMachine == CLOSED )
    {
        TCPFlags &= ~TCP_ACTIVE_OPEN;                // let's do a passive open!
        TCPStateMachine = LISTENING;
        SocketStatus = SOCK_ACTIVE;                  // reset, socket now active
    }
}

// does an active open (tries to establish a connection between
// 'MyIP:TCPLocalPort' and 'RemoteIP:TCPRemotePort')

void TCPActiveOpen ( void )
{
    if ( ( TCPStateMachine == CLOSED ) || ( TCPStateMachine == LISTENING ) )
    {
        TCPFlags |= TCP_ACTIVE_OPEN;                 // let's do an active open!
        TCPFlags &= ~IP_ADDR_RESOLVED;               // we haven't opponents MAC yet

        PrepareARP_REQUEST();                        // ask for MAC by sending a broadcast
        LastFrameSent = ARP_REQUEST;
        TCPStartRetryTimer();
        SocketStatus = SOCK_ACTIVE;                  // reset, socket now active
    }
}

// closes an open connection

void TCPClose ( void )
{
    switch ( TCPStateMachine )
    {
        case LISTENING :
        case SYN_SENT :
        {
            TCPStateMachine = CLOSED;
            TCPFlags = 0;
            SocketStatus = 0;
            break;
        }
        case SYN_RECD :
        case ESTABLISHED :
        {
            TCPFlags |= TCP_CLOSE_REQUESTED;
            break;
        }
    }
}

// releases the receive-buffer and allows to store new data
// NOTE: rx-buffer MUST be released periodically, else the other TCP
//       get no ACKs for the data it sent

void TCPReleaseRxBuffer ( void )
{
    SocketStatus &= ~SOCK_DATA_AVAILABLE;
}

// transmitts data stored in 'TCP_TX_BUF'
// NOTE: * number of bytes to transmit must have been written to 'TCPTxDataCount'
//       * data-count MUST NOT exceed 'MAX_TCP_TX_DATA_SIZE'

//void TCPTransmitTxBuffer(uint8_t *dp,uint16_t cnt)

void TCPTransmitTxBuffer ( void )
{
    if ( ( TCPStateMachine == ESTABLISHED ) || ( TCPStateMachine == CLOSE_WAIT ) )
        if ( SocketStatus & SOCK_TX_BUF_RELEASED )
        {
            //      memcpy(TCP_TX_BUF, dp, cnt);
            //      TCPTxDataCount = cnt;

            SocketStatus &= ~SOCK_TX_BUF_RELEASED;               // occupy tx-buffer
            TCPUNASeqNr += TCPTxDataCount;                       // advance UNA

            TxFrame1Size = ETH_HEADER_SIZE + IP_HEADER_SIZE + TCP_HEADER_SIZE + TCPTxDataCount;
            TransmitControl |= SEND_FRAME1;

            LastFrameSent = TCP_DATA_FRAME;
            TCPStartRetryTimer();
        }
}

// Reads the length of the received ethernet frame and checks if the
// destination address is a broadcast message or not
unsigned int IsBroadcast ( void )
{
    unsigned short RecdDestMAC[3];         // 48 bit MAC

    RecdFrameLength = StartReadFrame();

    CopyFromFrame_EMAC ( &RecdDestMAC,  6 );        // receive DA to see if it was a broadcast
    CopyFromFrame_EMAC ( &RecdFrameMAC, 6 );        // store SA (for our answer)

    if ( ( RecdDestMAC[0] == 0xFFFF ) &&
            ( RecdDestMAC[1] == 0xFFFF ) &&
            ( RecdDestMAC[2] == 0xFFFF ) )
    {
        return ( 1 );
    }
    else
    {
        return ( 0 );
    }
}


// 'main()'-function
// must be called from user program periodically (the often - the better)
// handles network, TCP/IP-stack and user events
uint8_t debugcnt;
void DoNetworkStuff ( void )
{
    if ( CheckFrameReceived() )                    // Packet received
    {
        if ( IsBroadcast() )
        {
            ProcessEthBroadcastFrame();
        }
        else
        {
            ProcessEthIAFrame();
        }

        EndReadFrame();                              // release buffer in ethernet controller
    }
    debugcnt++;
if (debugcnt>18)
{
    debugcnt=0;
}
    if ( TCPFlags & TCP_TIMER_RUNNING )
        if ( TCPFlags & TIMER_TYPE_RETRY )
        {
            if ( TCPTimer > RETRY_TIMEOUT )
            {
                TCPRestartTimer();                       // set a new timeout

                if ( RetryCounter )
                {
                    TCPHandleRetransmission();             // resend last frame
                    RetryCounter--;
                }
                else
                {
                    TCPStopTimer();
                    TCPHandleTimeout();
                }
            }
        }
        else if ( TCPTimer > FIN_TIMEOUT )
        {
            TCPStateMachine = CLOSED;
            TCPFlags = 0;                              // reset all flags, stop retransmission...
            SocketStatus &= SOCK_DATA_AVAILABLE;       // clear all flags but data available
        }

    switch ( TCPStateMachine )
    {
        case CLOSED :
        case LISTENING :
        {
            if ( TCPFlags & TCP_ACTIVE_OPEN )          // stack has to open a connection?
                if ( TCPFlags & IP_ADDR_RESOLVED )       // IP resolved?
                    if ( ! ( TransmitControl & SEND_FRAME2 ) ) // buffer free?
                    {
                        TCPSeqNr = ( ( unsigned long ) ISNGenHigh << 16 ) | ( SysTick->VAL & 0xFFFF ); // NXP: changed from T0TC to SysTick->VAL;
                        // set local ISN
                        TCPUNASeqNr = TCPSeqNr;
                        TCPAckNr = 0;                                       // we don't know what to ACK!
                        TCPUNASeqNr++;                                      // count SYN as a byte
                        PrepareTCP_FRAME ( TCP_CODE_SYN );                  // send SYN frame
                        LastFrameSent = TCP_SYN_FRAME;
                        TCPStartRetryTimer();                               // we NEED a retry-timeout
                        TCPStateMachine = SYN_SENT;
                    }

            break;
        }
        case SYN_RECD :
        case ESTABLISHED :
        {
            if ( TCPFlags & TCP_CLOSE_REQUESTED )                // user has user initated a close?
                if ( ! ( TransmitControl & ( SEND_FRAME2 | SEND_FRAME1 ) ) ) // buffers free?
                    if ( TCPSeqNr == TCPUNASeqNr )                        // all data ACKed?
                    {
                        TCPUNASeqNr++;
                        PrepareTCP_FRAME ( TCP_CODE_FIN | TCP_CODE_ACK );
                        LastFrameSent = TCP_FIN_FRAME;
                        TCPStartRetryTimer();
                        TCPStateMachine = FIN_WAIT_1;
                    }

            break;
        }
        case CLOSE_WAIT :
        {
            if ( ! ( TransmitControl & ( SEND_FRAME2 | SEND_FRAME1 ) ) ) // buffers free?
                if ( TCPSeqNr == TCPUNASeqNr )                          // all data ACKed?
                {
                    TCPUNASeqNr++;                                        // count FIN as a byte
                    PrepareTCP_FRAME ( TCP_CODE_FIN | TCP_CODE_ACK );     // we NEED a retry-timeout
                    LastFrameSent = TCP_FIN_FRAME;                        // time to say goodbye...
                    TCPStartRetryTimer();
                    TCPStateMachine = LAST_ACK;
                }

            break;
        }
    }

    if ( TransmitControl & SEND_FRAME2 )
    {
        RequestSend ( TxFrame2Size );

        if ( Rdy4Tx() )                               // NOTE: when using a very fast MCU, maybe
        {
            SendFrame2();                              // the EMAC isn't ready yet, include
        }
        else                                           // a kind of timer or counter here
        {
            TCPStateMachine = CLOSED;
            SocketStatus = SOCK_ERR_ETHERNET;          // indicate an error to user
            TCPFlags = 0;                              // clear all flags, stop timers etc.
        }

        TransmitControl &= ~SEND_FRAME2;             // clear tx-flag
    }

    if ( TransmitControl & SEND_FRAME1 )
    {
        PrepareTCP_DATA_FRAME();                     // build frame w/ actual SEQ, ACK....
        RequestSend ( TxFrame1Size );

        if ( Rdy4Tx() )                               // EMAC ready to accept our frame?
        {
            SendFrame1();                              // (see note above)
        }
        else
        {
            TCPStateMachine = CLOSED;
            SocketStatus = SOCK_ERR_ETHERNET;          // indicate an error to user
            TCPFlags = 0;                              // clear all flags, stop timers etc.
        }

        TransmitControl &= ~SEND_FRAME1;             // clear tx-flag
    }
}

// handles an incoming broadcast frame

void ProcessEthBroadcastFrame ( void )
{
    unsigned short TargetIP[2];

    if ( ReadFrameBE_EMAC() == FRAME_ARP )         // get frame type, check for ARP
        if ( ReadFrameBE_EMAC() == HARDW_ETH10 )     // Ethernet frame
            if ( ReadFrameBE_EMAC() == FRAME_IP )      // check protocol
                if ( ReadFrameBE_EMAC() == IP_HLEN_PLEN ) // check HLEN, PLEN
                    if ( ReadFrameBE_EMAC() == OP_ARP_REQUEST )
                    {
                        DummyReadFrame_EMAC ( 6 );           // ignore sender's hardware address
                        CopyFromFrame_EMAC ( &RecdFrameIP, 4 ); // read sender's protocol address
                        DummyReadFrame_EMAC ( 6 );           // ignore target's hardware address
                        CopyFromFrame_EMAC ( &TargetIP, 4 ); // read target's protocol address

                        if ( !memcmp ( &MyIP, &TargetIP, 4 ) ) // is it for us?
                        {
                            PrepareARP_ANSWER();    // yes->create ARP_ANSWER frame
                        }
                    }
}

// handles an incoming frame that passed EMAC's address filter
// (individual addressed = IA)

void ProcessEthIAFrame ( void )
{
    unsigned short TargetIP[2];
    unsigned char ProtocolType;

    switch ( ReadFrameBE_EMAC() )                   // get frame type
    {
        case FRAME_ARP :                             // check for ARP
        {
            if ( ( TCPFlags & ( TCP_ACTIVE_OPEN | IP_ADDR_RESOLVED ) ) == TCP_ACTIVE_OPEN )
                if ( ReadFrameBE_EMAC() == HARDW_ETH10 )       // check for the right prot. etc.
                    if ( ReadFrameBE_EMAC() == FRAME_IP )
                        if ( ReadFrameBE_EMAC() == IP_HLEN_PLEN )
                            if ( ReadFrameBE_EMAC() == OP_ARP_ANSWER )
                            {
                                TCPStopTimer();                       // OK, now we've the MAC we wanted ;-)
                                CopyFromFrame_EMAC ( &RemoteMAC, 6 ); // extract opponents MAC
                                TCPFlags |= IP_ADDR_RESOLVED;
                            }

            break;
        }
        case FRAME_IP :                                        // check for IP-type
        {
            if ( ( ReadFrameBE_EMAC() & 0xFF00 ) == IP_VER_IHL )  // IPv4, IHL=5 (20 Bytes Header)
            {
                // ignore Type Of Service
                RecdIPFrameLength = ReadFrameBE_EMAC();             // get IP frame's length
                ReadFrameBE_EMAC();                                 // ignore identification

                if ( ! ( ReadFrameBE_EMAC() & ( IP_FLAG_MOREFRAG | IP_FRAGOFS_MASK ) ) ) // only unfragm. frames
                {
                    ProtocolType = ReadFrameBE_EMAC() & 0xFF;         // get protocol, ignore TTL
                    ReadFrameBE_EMAC();                               // ignore checksum
                    CopyFromFrame_EMAC ( &RecdFrameIP, 4 );           // get source IP
                    CopyFromFrame_EMAC ( &TargetIP, 4 );              // get destination IP

                    if ( !memcmp ( &MyIP, &TargetIP, 4 ) )           // is it for us?
                        switch ( ProtocolType )
                        {
                            case PROT_ICMP :
                            {
                                ProcessICMPFrame();
                                break;
                            }
                            case PROT_TCP  :
                            {
                                ProcessTCPFrame();
                                break;
                            }
                            case PROT_UDP  :
                                break;                      // not implemented!
                        }
                }
            }

            break;
        }
    }
}

// we've just rec'd an ICMP-frame (Internet Control Message Protocol)
// check what to do and branch to the appropriate sub-function

void ProcessICMPFrame ( void )
{
    unsigned short ICMPTypeAndCode;

    ICMPTypeAndCode = ReadFrameBE_EMAC();           // get Message Type and Code
    ReadFrameBE_EMAC();                             // ignore ICMP checksum

    switch ( ICMPTypeAndCode >> 8 )                // check type
    {
        case ICMP_ECHO :                             // is echo request?
        {
            PrepareICMP_ECHO_REPLY();                  // echo as much as we can...
            break;
        }
    }
}

// we've just rec'd an TCP-frame (Transmission Control Protocol)
// this function mainly implements the TCP state machine according to RFC793

void ProcessTCPFrame ( void )
{
    unsigned short TCPSegSourcePort;               // segment's source port
    unsigned short TCPSegDestPort;                 // segment's destination port
    unsigned long TCPSegSeq;                       // segment's sequence number
    unsigned long TCPSegAck;                       // segment's acknowledge number
    unsigned short TCPCode;                        // TCP code and header length
    unsigned char TCPHeaderSize;                   // real TCP header length
    unsigned short NrOfDataBytes;                  // real number of data

    TCPSegSourcePort = ReadFrameBE_EMAC();                    // get ports
    TCPSegDestPort = ReadFrameBE_EMAC();

    if ( TCPSegDestPort != TCPLocalPort )
    {
        return;    // drop segment if port doesn't match
    }

    TCPSegSeq = ( unsigned long ) ReadFrameBE_EMAC() << 16;   // get segment sequence nr.
    TCPSegSeq |= ReadFrameBE_EMAC();

    TCPSegAck = ( unsigned long ) ReadFrameBE_EMAC() << 16;   // get segment acknowledge nr.
    TCPSegAck |= ReadFrameBE_EMAC();

    TCPCode = ReadFrameBE_EMAC();                             // get control bits, header length...

    TCPHeaderSize = ( TCPCode & DATA_OFS_MASK ) >> 10;       // header length in bytes
    NrOfDataBytes = RecdIPFrameLength - IP_HEADER_SIZE - TCPHeaderSize;     // seg. text length

    if ( NrOfDataBytes > MAX_TCP_RX_DATA_SIZE )
    {
        return;    // packet too large for us :...-(
    }

    if ( TCPHeaderSize > TCP_HEADER_SIZE )                   // ignore options if any
    {
        DummyReadFrame_EMAC ( TCPHeaderSize - TCP_HEADER_SIZE );
    }

    switch ( TCPStateMachine )                               // implement the TCP state machine
    {
        case CLOSED :
        {
            if ( ! ( TCPCode & TCP_CODE_RST ) )
            {
                TCPRemotePort = TCPSegSourcePort;
                memcpy ( &RemoteMAC, &RecdFrameMAC, 6 );           // save opponents MAC and IP
                memcpy ( &RemoteIP, &RecdFrameIP, 4 );             // for later use

                if ( TCPCode & TCP_CODE_ACK )                      // make the reset sequence
                {
                    // acceptable to the other
                    TCPSeqNr = TCPSegAck;                            // TCP
                    PrepareTCP_FRAME ( TCP_CODE_RST );
                }
                else
                {
                    TCPSeqNr = 0;
                    TCPAckNr = TCPSegSeq + NrOfDataBytes;

                    if ( TCPCode & ( TCP_CODE_SYN | TCP_CODE_FIN ) )
                    {
                        TCPAckNr++;
                    }

                    PrepareTCP_FRAME ( TCP_CODE_RST | TCP_CODE_ACK );
                }
            }

            break;
        }
        case LISTENING :
        {
            if ( ! ( TCPCode & TCP_CODE_RST ) )                  // ignore segment containing RST
            {
                TCPRemotePort = TCPSegSourcePort;
                memcpy ( &RemoteMAC, &RecdFrameMAC, 6 );           // save opponents MAC and IP
                memcpy ( &RemoteIP, &RecdFrameIP, 4 );             // for later use

                if ( TCPCode & TCP_CODE_ACK )                      // reset a bad
                {
                    // acknowledgement
                    TCPSeqNr = TCPSegAck;
                    PrepareTCP_FRAME ( TCP_CODE_RST );
                }
                else if ( TCPCode & TCP_CODE_SYN )
                {
                    TCPAckNr = TCPSegSeq + 1;                           // get remote ISN, next byte we expect
                    TCPSeqNr = ( ( unsigned long ) ISNGenHigh << 16 ) | ( SysTick->VAL & 0xFFFF ); // Keil: changed from TAR to T0TC;
                    // set local ISN
                    TCPUNASeqNr = TCPSeqNr + 1;                         // one byte out -> increase by one
                    PrepareTCP_FRAME ( TCP_CODE_SYN | TCP_CODE_ACK );
                    LastFrameSent = TCP_SYN_ACK_FRAME;
                    TCPStartRetryTimer();
                    TCPStateMachine = SYN_RECD;
                }
            }

            break;
        }
        case SYN_SENT :
        {
            if ( memcmp ( &RemoteIP, &RecdFrameIP, 4 ) )
            {
                break;    // drop segment if its IP doesn't belong
            }

            // to current session

            if ( TCPSegSourcePort != TCPRemotePort )
            {
                break;    // drop segment if port doesn't match
            }

            if ( TCPCode & TCP_CODE_ACK )              // ACK field significant?
                if ( TCPSegAck != TCPUNASeqNr )          // is our ISN ACKed?
                {
                    if ( ! ( TCPCode & TCP_CODE_RST ) )
                    {
                        TCPSeqNr = TCPSegAck;
                        PrepareTCP_FRAME ( TCP_CODE_RST );
                    }

                    break;                                 // drop segment
                }

            if ( TCPCode & TCP_CODE_RST )              // RST??
            {
                if ( TCPCode & TCP_CODE_ACK )            // if ACK was acceptable, reset
                {
                    // connection
                    TCPStateMachine = CLOSED;
                    TCPFlags = 0;                          // reset all flags, stop retransmission...
                    SocketStatus = SOCK_ERR_CONN_RESET;
                }

                break;                                   // drop segment
            }

            if ( TCPCode & TCP_CODE_SYN )              // SYN??
            {
                TCPAckNr = TCPSegSeq;                    // get opponents ISN
                TCPAckNr++;                              // inc. by one...

                if ( TCPCode & TCP_CODE_ACK )
                {
                    TCPStopTimer();                        // stop retransmission, other TCP got our SYN
                    TCPSeqNr = TCPUNASeqNr;                // advance our sequence number

                    PrepareTCP_FRAME ( TCP_CODE_ACK );     // ACK this ISN
                    TCPStateMachine = ESTABLISHED;
                    SocketStatus |= SOCK_CONNECTED;
                    SocketStatus |= SOCK_TX_BUF_RELEASED;  // user may send data now :-)
                }
                else
                {
                    TCPStopTimer();
                    PrepareTCP_FRAME ( TCP_CODE_SYN | TCP_CODE_ACK ); // our SYN isn't ACKed yet,
                    LastFrameSent = TCP_SYN_ACK_FRAME;               // now continue with sending
                    TCPStartRetryTimer();                            // SYN_ACK frames
                    TCPStateMachine = SYN_RECD;
                }
            }

            break;
        }
        default :
        {
            if ( memcmp ( &RemoteIP, &RecdFrameIP, 4 ) )
            {
                break;    // drop segment if IP doesn't belong
            }

            // to current session

            if ( TCPSegSourcePort != TCPRemotePort )
            {
                break;    // drop segment if port doesn't match
            }

            if ( TCPSegSeq != TCPAckNr )
            {
                break;    // drop if it's not the segment we expect
            }

            if ( TCPCode & TCP_CODE_RST )              // RST??
            {
                TCPStateMachine = CLOSED;                // close the state machine
                TCPFlags = 0;                            // reset all flags, stop retransmission...
                SocketStatus = SOCK_ERR_CONN_RESET;      // indicate an error to user
                break;
            }

            if ( TCPCode & TCP_CODE_SYN )              // SYN??
            {
                PrepareTCP_FRAME ( TCP_CODE_RST );       // is NOT allowed here! send a reset,
                TCPStateMachine = CLOSED;                // close connection...
                TCPFlags = 0;                            // reset all flags, stop retransmission...
                SocketStatus = SOCK_ERR_REMOTE;          // fatal error!
                break;                                   // ...and drop the frame
            }

            if ( ! ( TCPCode & TCP_CODE_ACK ) )
            {
                break;    // drop segment if the ACK bit is off
            }

            if ( TCPSegAck == TCPUNASeqNr )            // is our last data sent ACKed?
            {
                TCPStopTimer();                          // stop retransmission
                TCPSeqNr = TCPUNASeqNr;                  // advance our sequence number

                switch ( TCPStateMachine )               // change state if necessary
                {
                    case SYN_RECD :                        // ACK of our SYN?
                    {
                        TCPStateMachine = ESTABLISHED;       // user may send data now :-)
                        SocketStatus |= SOCK_CONNECTED;
                        break;
                    }
                    case FIN_WAIT_1 :
                    {
                        TCPStateMachine = FIN_WAIT_2;    // ACK of our FIN?
                        break;
                    }
                    case CLOSING :
                    {
                        TCPStateMachine = TIME_WAIT;    // ACK of our FIN?
                        break;
                    }
                    case LAST_ACK :                                            // ACK of our FIN?
                    {
                        TCPStateMachine = CLOSED;
                        TCPFlags = 0;                        // reset all flags, stop retransmission...
                        SocketStatus &= SOCK_DATA_AVAILABLE; // clear all flags but data available
                        break;
                    }
                    case TIME_WAIT :
                    {
                        PrepareTCP_FRAME ( TCP_CODE_ACK );   // ACK a retransmission of remote FIN
                        TCPRestartTimer();                   // restart TIME_WAIT timeout
                        break;
                    }
                }

                if ( TCPStateMachine == ESTABLISHED )    // if true, give the frame buffer back
                {
                    SocketStatus |= SOCK_TX_BUF_RELEASED;    // to user
                }
            }

            if ( ( TCPStateMachine == ESTABLISHED ) || ( TCPStateMachine == FIN_WAIT_1 ) || ( TCPStateMachine == FIN_WAIT_2 ) )
                if ( NrOfDataBytes )                               // data available?
                    if ( ! ( SocketStatus & SOCK_DATA_AVAILABLE ) )  // rx data-buffer empty?
                    {
                        DummyReadFrame_EMAC ( 6 );                     // ignore window, checksum, urgent pointer
                        CopyFromFrame_EMAC ( RxTCPBuffer, NrOfDataBytes ); // fetch data and
                        TCPRxDataCount = NrOfDataBytes;                // ...tell the user...
                        SocketStatus |= SOCK_DATA_AVAILABLE;           // indicate the new data to user
                        TCPAckNr += NrOfDataBytes;
                        PrepareTCP_FRAME ( TCP_CODE_ACK );             // ACK rec'd data
                    }

            if ( TCPCode & TCP_CODE_FIN )              // FIN??
            {
                switch ( TCPStateMachine )
                {
                    case SYN_RECD :
                    case ESTABLISHED :
                    {
                        TCPStateMachine = CLOSE_WAIT;
                        break;
                    }
                    case FIN_WAIT_1 :
                    {
                        // if our FIN was ACKed, we automatically
                        TCPStateMachine = CLOSING;           // enter FIN_WAIT_2 (look above) and therefore
                        SocketStatus &= ~SOCK_CONNECTED;     // TIME_WAIT
                        break;
                    }
                    case FIN_WAIT_2 :
                    {
                        TCPStartTimeWaitTimer();
                        TCPStateMachine = TIME_WAIT;
                        SocketStatus &= ~SOCK_CONNECTED;
                        break;
                    }
                    case TIME_WAIT :
                    {
                        TCPRestartTimer();
                        break;
                    }
                }

                TCPAckNr++;                              // ACK remote's FIN flag
                PrepareTCP_FRAME ( TCP_CODE_ACK );
            }
        }
    }
}

// prepares the TxFrame2-buffer to send an ARP-request

void PrepareARP_REQUEST ( void )
{
    // Ethernet
    memset ( &TxFrame2[ETH_DA_OFS], ( char ) 0xFF, 6 );            // we don't know opposites MAC!
    memcpy ( &TxFrame2[ETH_SA_OFS], &MyMAC, 6 );
    * ( unsigned short * ) &TxFrame2[ETH_TYPE_OFS] = SWAPB ( FRAME_ARP );

    // ARP
    * ( unsigned short * ) &TxFrame2[ARP_HARDW_OFS] = SWAPB ( HARDW_ETH10 );
    * ( unsigned short * ) &TxFrame2[ARP_PROT_OFS] = SWAPB ( FRAME_IP );
    * ( unsigned short * ) &TxFrame2[ARP_HLEN_PLEN_OFS] = SWAPB ( IP_HLEN_PLEN );
    * ( unsigned short * ) &TxFrame2[ARP_OPCODE_OFS] = SWAPB ( OP_ARP_REQUEST );
    memcpy ( &TxFrame2[ARP_SENDER_HA_OFS], &MyMAC, 6 );
    memcpy ( &TxFrame2[ARP_SENDER_IP_OFS], &MyIP, 4 );
    memset ( &TxFrame2[ARP_TARGET_HA_OFS], 0x00, 6 );        // we don't know opposites MAC!

    if ( ( ( RemoteIP[0] ^ MyIP[0] ) & SubnetMask[0] ) || ( ( RemoteIP[1] ^ MyIP[1] ) & SubnetMask[1] ) )
    {
        memcpy ( &TxFrame2[ARP_TARGET_IP_OFS], &GatewayIP, 4 );    // IP not in subnet, use gateway
    }
    else
    {
        memcpy ( &TxFrame2[ARP_TARGET_IP_OFS], &RemoteIP, 4 );    // other IP is next to us...
    }

    TxFrame2Size = ETH_HEADER_SIZE + ARP_FRAME_SIZE;
    TransmitControl |= SEND_FRAME2;
}

// prepares the TxFrame2-buffer to send an ARP-answer (reply)

void PrepareARP_ANSWER ( void )
{
    // Ethernet
    memcpy ( &TxFrame2[ETH_DA_OFS], &RecdFrameMAC, 6 );
    memcpy ( &TxFrame2[ETH_SA_OFS], &MyMAC, 6 );
    * ( unsigned short * ) &TxFrame2[ETH_TYPE_OFS] = SWAPB ( FRAME_ARP );

    // ARP
    * ( unsigned short * ) &TxFrame2[ARP_HARDW_OFS] = SWAPB ( HARDW_ETH10 );
    * ( unsigned short * ) &TxFrame2[ARP_PROT_OFS] = SWAPB ( FRAME_IP );
    * ( unsigned short * ) &TxFrame2[ARP_HLEN_PLEN_OFS] = SWAPB ( IP_HLEN_PLEN );
    * ( unsigned short * ) &TxFrame2[ARP_OPCODE_OFS] = SWAPB ( OP_ARP_ANSWER );
    memcpy ( &TxFrame2[ARP_SENDER_HA_OFS], &MyMAC, 6 );
    memcpy ( &TxFrame2[ARP_SENDER_IP_OFS], &MyIP, 4 );
    memcpy ( &TxFrame2[ARP_TARGET_HA_OFS], &RecdFrameMAC, 6 );
    memcpy ( &TxFrame2[ARP_TARGET_IP_OFS], &RecdFrameIP, 4 );

    TxFrame2Size = ETH_HEADER_SIZE + ARP_FRAME_SIZE;
    TransmitControl |= SEND_FRAME2;
}

// prepares the TxFrame2-buffer to send an ICMP-echo-reply

void PrepareICMP_ECHO_REPLY ( void )
{
    unsigned short ICMPDataCount;

    if ( RecdIPFrameLength > MAX_ETH_TX_DATA_SIZE )                    // don't overload TX-buffer
    {
        ICMPDataCount = MAX_ETH_TX_DATA_SIZE - IP_HEADER_SIZE - ICMP_HEADER_SIZE;
    }
    else
    {
        ICMPDataCount = RecdIPFrameLength - IP_HEADER_SIZE - ICMP_HEADER_SIZE;
    }

    // Ethernet
    memcpy ( &TxFrame2[ETH_DA_OFS], &RecdFrameMAC, 6 );
    memcpy ( &TxFrame2[ETH_SA_OFS], &MyMAC, 6 );
    * ( unsigned short * ) &TxFrame2[ETH_TYPE_OFS] = SWAPB ( FRAME_IP );

    // IP
    * ( unsigned short * ) &TxFrame2[IP_VER_IHL_TOS_OFS] = SWAPB ( IP_VER_IHL );
    WriteWBE ( &TxFrame2[IP_TOTAL_LENGTH_OFS], IP_HEADER_SIZE + ICMP_HEADER_SIZE + ICMPDataCount );
    * ( unsigned short * ) &TxFrame2[IP_IDENT_OFS] = 0;
    * ( unsigned short * ) &TxFrame2[IP_FLAGS_FRAG_OFS] = 0;
    * ( unsigned short * ) &TxFrame2[IP_TTL_PROT_OFS] = SWAPB ( ( DEFAULT_TTL << 8 ) | PROT_ICMP );
    * ( unsigned short * ) &TxFrame2[IP_HEAD_CHKSUM_OFS] = 0;
    memcpy ( &TxFrame2[IP_SOURCE_OFS], &MyIP, 4 );
    memcpy ( &TxFrame2[IP_DESTINATION_OFS], &RecdFrameIP, 4 );
    * ( unsigned short * ) &TxFrame2[IP_HEAD_CHKSUM_OFS] = CalcChecksum ( &TxFrame2[IP_VER_IHL_TOS_OFS], IP_HEADER_SIZE, 0 );

    // ICMP
    * ( unsigned short * ) &TxFrame2[ICMP_TYPE_CODE_OFS] = SWAPB ( ICMP_ECHO_REPLY << 8 );
    * ( unsigned short * ) &TxFrame2[ICMP_CHKSUM_OFS] = 0;               // initialize checksum field

    CopyFromFrame_EMAC ( &TxFrame2[ICMP_DATA_OFS], ICMPDataCount );     // get data to echo...
    * ( unsigned short * ) &TxFrame2[ICMP_CHKSUM_OFS] = CalcChecksum ( &TxFrame2[IP_DATA_OFS], ICMPDataCount + ICMP_HEADER_SIZE, 0 );

    TxFrame2Size = ETH_HEADER_SIZE + IP_HEADER_SIZE + ICMP_HEADER_SIZE + ICMPDataCount;
    TransmitControl |= SEND_FRAME2;
}

// prepares the TxFrame2-buffer to send a general TCP frame
// the TCPCode-field is passed as an argument

void PrepareTCP_FRAME ( unsigned short TCPCode )
{
    // Ethernet
    memcpy ( &TxFrame2[ETH_DA_OFS], &RemoteMAC, 6 );
    memcpy ( &TxFrame2[ETH_SA_OFS], &MyMAC, 6 );
    * ( unsigned short * ) &TxFrame2[ETH_TYPE_OFS] = SWAPB ( FRAME_IP );

    // IP
    * ( unsigned short * ) &TxFrame2[IP_VER_IHL_TOS_OFS] = SWAPB ( IP_VER_IHL | IP_TOS_D );

    if ( TCPCode & TCP_CODE_SYN )                  // if SYN, we want to use the MSS option
    {
        * ( unsigned short * ) &TxFrame2[IP_TOTAL_LENGTH_OFS] = SWAPB ( IP_HEADER_SIZE + TCP_HEADER_SIZE + TCP_OPT_MSS_SIZE );
    }
    else
    {
        * ( unsigned short * ) &TxFrame2[IP_TOTAL_LENGTH_OFS] = SWAPB ( IP_HEADER_SIZE + TCP_HEADER_SIZE );
    }

    * ( unsigned short * ) &TxFrame2[IP_IDENT_OFS] = 0;
    * ( unsigned short * ) &TxFrame2[IP_FLAGS_FRAG_OFS] = SWAPB ( IP_FLAG_DONTFRAG );
    * ( unsigned short * ) &TxFrame2[IP_TTL_PROT_OFS] = SWAPB ( ( DEFAULT_TTL << 8 ) | PROT_TCP );
    * ( unsigned short * ) &TxFrame2[IP_HEAD_CHKSUM_OFS] = 0;
    memcpy ( &TxFrame2[IP_SOURCE_OFS], &MyIP, 4 );
    memcpy ( &TxFrame2[IP_DESTINATION_OFS], &RemoteIP, 4 );
    * ( unsigned short * ) &TxFrame2[IP_HEAD_CHKSUM_OFS] = CalcChecksum ( &TxFrame2[IP_VER_IHL_TOS_OFS], IP_HEADER_SIZE, 0 );

    // TCP
    WriteWBE ( &TxFrame2[TCP_SRCPORT_OFS], TCPLocalPort );
    WriteWBE ( &TxFrame2[TCP_DESTPORT_OFS], TCPRemotePort );

    WriteDWBE ( &TxFrame2[TCP_SEQNR_OFS], TCPSeqNr );
    WriteDWBE ( &TxFrame2[TCP_ACKNR_OFS], TCPAckNr );

    * ( unsigned short * ) &TxFrame2[TCP_WINDOW_OFS] = SWAPB ( MAX_TCP_RX_DATA_SIZE ); // data bytes to accept
    * ( unsigned short * ) &TxFrame2[TCP_CHKSUM_OFS] = 0;         // initalize checksum
    * ( unsigned short * ) &TxFrame2[TCP_URGENT_OFS] = 0;

    if ( TCPCode & TCP_CODE_SYN )                  // if SYN, we want to use the MSS option
    {
        * ( unsigned short * ) &TxFrame2[TCP_DATA_CODE_OFS] = SWAPB ( 0x6000 | TCPCode ); // TCP header length = 24
        * ( unsigned short * ) &TxFrame2[TCP_DATA_OFS] = SWAPB ( TCP_OPT_MSS );      // MSS option
        * ( unsigned short * ) &TxFrame2[TCP_DATA_OFS + 2] = SWAPB ( MAX_TCP_RX_DATA_SIZE ); // max. length of TCP-data we accept
        * ( unsigned short * ) &TxFrame2[TCP_CHKSUM_OFS] = CalcChecksum ( &TxFrame2[TCP_SRCPORT_OFS], TCP_HEADER_SIZE + TCP_OPT_MSS_SIZE, 1 );
        TxFrame2Size = ETH_HEADER_SIZE + IP_HEADER_SIZE + TCP_HEADER_SIZE + TCP_OPT_MSS_SIZE;
    }
    else
    {
        * ( unsigned short * ) &TxFrame2[TCP_DATA_CODE_OFS] = SWAPB ( 0x5000 | TCPCode ); // TCP header length = 20
        * ( unsigned short * ) &TxFrame2[TCP_CHKSUM_OFS] = CalcChecksum ( &TxFrame2[TCP_SRCPORT_OFS], TCP_HEADER_SIZE, 1 );
        TxFrame2Size = ETH_HEADER_SIZE + IP_HEADER_SIZE + TCP_HEADER_SIZE;
    }

    TransmitControl |= SEND_FRAME2;
}

// prepares the TxFrame1-buffer to send a payload-packet

void PrepareTCP_DATA_FRAME ( void )
{
    // Ethernet
    memcpy ( &TxFrame1[ETH_DA_OFS], &RemoteMAC, 6 );
    memcpy ( &TxFrame1[ETH_SA_OFS], &MyMAC, 6 );
    * ( unsigned short * ) &TxFrame1[ETH_TYPE_OFS] = SWAPB ( FRAME_IP );

    // IP
    * ( unsigned short * ) &TxFrame1[IP_VER_IHL_TOS_OFS] = SWAPB ( IP_VER_IHL | IP_TOS_D );
    WriteWBE ( &TxFrame1[IP_TOTAL_LENGTH_OFS], IP_HEADER_SIZE + TCP_HEADER_SIZE + TCPTxDataCount );
    * ( unsigned short * ) &TxFrame1[IP_IDENT_OFS] = 0;
    * ( unsigned short * ) &TxFrame1[IP_FLAGS_FRAG_OFS] = SWAPB ( IP_FLAG_DONTFRAG );
    * ( unsigned short * ) &TxFrame1[IP_TTL_PROT_OFS] = SWAPB ( ( DEFAULT_TTL << 8 ) | PROT_TCP );
    * ( unsigned short * ) &TxFrame1[IP_HEAD_CHKSUM_OFS] = 0;
    memcpy ( &TxFrame1[IP_SOURCE_OFS], &MyIP, 4 );
    memcpy ( &TxFrame1[IP_DESTINATION_OFS], &RemoteIP, 4 );
    * ( unsigned short * ) &TxFrame1[IP_HEAD_CHKSUM_OFS] = CalcChecksum ( &TxFrame1[IP_VER_IHL_TOS_OFS], IP_HEADER_SIZE, 0 );

    // TCP
    WriteWBE ( &TxFrame1[TCP_SRCPORT_OFS], TCPLocalPort );
    WriteWBE ( &TxFrame1[TCP_DESTPORT_OFS], TCPRemotePort );

    WriteDWBE ( &TxFrame1[TCP_SEQNR_OFS], TCPSeqNr );
    WriteDWBE ( &TxFrame1[TCP_ACKNR_OFS], TCPAckNr );
    * ( unsigned short * ) &TxFrame1[TCP_DATA_CODE_OFS] = SWAPB ( 0x5000 | TCP_CODE_ACK ); // TCP header length = 20
    * ( unsigned short * ) &TxFrame1[TCP_WINDOW_OFS] = SWAPB ( MAX_TCP_RX_DATA_SIZE ); // data bytes to accept
    * ( unsigned short * ) &TxFrame1[TCP_CHKSUM_OFS] = 0;
    * ( unsigned short * ) &TxFrame1[TCP_URGENT_OFS] = 0;
    * ( unsigned short * ) &TxFrame1[TCP_CHKSUM_OFS] = CalcChecksum ( &TxFrame1[TCP_SRCPORT_OFS], TCP_HEADER_SIZE + TCPTxDataCount, 1 );
}

// calculates the TCP/IP checksum. if 'IsTCP != 0', the TCP pseudo-header
// will be included.

unsigned short CalcChecksum ( void *Start, unsigned short Count, unsigned char IsTCP )
{
    unsigned long Sum = 0;
    unsigned short *piStart;                         // Keil: Pointer added to correct expression

    if ( IsTCP )                                   // if we've a TCP frame...
    {
        Sum += MyIP[0];                              // ...include TCP pseudo-header
        Sum += MyIP[1];
        Sum += RemoteIP[0];
        Sum += RemoteIP[1];
        Sum += SwapBytes ( Count );                  // TCP header length plus data length
        Sum += SWAPB ( PROT_TCP );
    }

    piStart = Start;                               // Keil: Line added

    while ( Count > 1 )                            // sum words
    {
        //  Sum += *((unsigned short *)Start)++;		     // Keil: Line replaced with following line
        Sum += *piStart++;
        Count -= 2;
    }

    if ( Count )                                   // add left-over byte, if any
        //  Sum += *(unsigned char *)Start; 	         // Keil: Line replaced with following line
    {
        Sum += * ( unsigned char * ) piStart;
    }

    while ( Sum >> 16 )                            // fold 32-bit sum to 16 bits
    {
        Sum = ( Sum & 0xFFFF ) + ( Sum >> 16 );
    }

    return ~Sum;
}

// starts the timer as a retry-timer (used for retransmission-timeout)

void TCPStartRetryTimer ( void )
{
    TCPTimer = 0;
    RetryCounter = MAX_RETRYS;
    TCPFlags |= TCP_TIMER_RUNNING;
    TCPFlags |= TIMER_TYPE_RETRY;
}

// starts the timer as a 'TIME_WAIT'-timer (used to finish a TCP-session)

void TCPStartTimeWaitTimer ( void )
{
    TCPTimer = 0;
    TCPFlags |= TCP_TIMER_RUNNING;
    TCPFlags &= ~TIMER_TYPE_RETRY;
}

// restarts the timer

void TCPRestartTimer ( void )
{
    TCPTimer = 0;
}

// stopps the timer

void TCPStopTimer ( void )
{
    TCPFlags &= ~TCP_TIMER_RUNNING;
}

// if a retransmission-timeout occured, check which packet
// to resend.

void TCPHandleRetransmission ( void )
{
    switch ( LastFrameSent )
    {
        case ARP_REQUEST :
        {
            PrepareARP_REQUEST();
            break;
        }
        case TCP_SYN_FRAME :
        {
            PrepareTCP_FRAME ( TCP_CODE_SYN );
            break;
        }
        case TCP_SYN_ACK_FRAME :
        {
            PrepareTCP_FRAME ( TCP_CODE_SYN | TCP_CODE_ACK );
            break;
        }
        case TCP_FIN_FRAME :
        {
            PrepareTCP_FRAME ( TCP_CODE_FIN | TCP_CODE_ACK );
            break;
        }
        case TCP_DATA_FRAME :
        {
            TransmitControl |= SEND_FRAME1;
            break;
        }
    }
}

// if all retransmissions failed, close connection and indicate an error

void TCPHandleTimeout ( void )
{
    TCPStateMachine = CLOSED;

    if ( ( TCPFlags & ( TCP_ACTIVE_OPEN | IP_ADDR_RESOLVED ) ) == TCP_ACTIVE_OPEN )
    {
        SocketStatus = SOCK_ERR_ARP_TIMEOUT;    // indicate an error to user
    }
    else
    {
        SocketStatus = SOCK_ERR_TCP_TIMEOUT;
    }

    TCPFlags = 0;                                  // clear all flags
}


// transfers the contents of 'TxFrame1'-Buffer to the EMAC

void SendFrame1 ( void )
{
    CopyToFrame_EMAC ( TxFrame1, TxFrame1Size );
}

// transfers the contents of 'TxFrame2'-Buffer to the EMAC

void SendFrame2 ( void )
{
    CopyToFrame_EMAC ( TxFrame2, TxFrame2Size );
}

// help function to write a WORD in big-endian byte-order
// to MCU-memory

void WriteWBE ( unsigned char *Add, unsigned short Data )
{
    *Add++ = Data >> 8;
    *Add = ( char ) Data;
}

// help function to write a DWORD in big-endian byte-order
// to MCU-memory

void WriteDWBE ( unsigned char *Add, unsigned long Data )
{
    *Add++ = Data >> 24;
    *Add++ = Data >> 16;
    *Add++ = Data >> 8;
    *Add = ( char ) Data;
}

// help function to swap the byte order of a WORD

unsigned short SwapBytes ( unsigned short Data )
{
    return ( Data >> 8 ) | ( Data << 8 );
}

//#endif