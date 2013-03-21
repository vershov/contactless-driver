



#include <linux/string.h>



#include "common.h"
#include "felica.h"
#include "pn512.h"
#include "pn512app.h"
#include "delay.h"
#include "picc.h"
#include "debug.h"

UINT16 feliSysCode = FELICA_APPLICATION_ALL;

const UINT8 gaFelRFCon[RF_FEL_CON_LEN] = {0x11, 0x85, 0x69, 0x3f, 0x11, 0x55, 0x69, 0x3f};

UINT8 CfgTbl_Pas106i[] =
{
    REG_MODE,        0x39,    // CRCPreset = 6363H
    REG_TXMODE,      0x80,    // ISO/IEC 14443A/MIFARE and 106 kbit, TxCRCEn On
    REG_RXMODE,      0x80,    // ISO/IEC 14443A/MIFARE and 106 kbit, RxCRCEn On   	
    REG_BITFRAMING,  0x00,
    REG_RFCFG,       0x59,
    REG_GSNON,       0xF4,
    REG_CWGSP,       0x3F,
    REG_MANUALRCV,   0x00,
    REG_TXAUTO,      0x43,
    REG_DEMOD,       0x4D,
    REG_RXTHRESHOLD, 0x55,
    REG_MODWIDTH,    0x26,
    REG_RXSEL,       0x84,
    0x00
};

UINT8 CfgTbl_Pas212i[] =
{
    REG_MODE,       0xB8,    // CRCPreset = 0000H
    REG_TXMODE,     0x92,
    REG_RXMODE,     0x92,
    REG_BITFRAMING, 0x00,
    REG_GSNON,      0xF4,
    REG_GSNOFF,     0x6F,
    REG_MANUALRCV,  0x10,
    REG_TXAUTO,     0x03,
    REG_DEMOD,      0x41,
    REG_TXBITPHASE, 0x8F,
    REG_RXSEL,      0x84,
    0x00
};

UINT8 CfgTbl_Pas424i[] =
{
    REG_MODE,       0xB8,    // CRCPreset = 0000H
    REG_TXMODE,     0xA2,
    REG_RXMODE,     0xA2,
    REG_BITFRAMING, 0x00,
    REG_GSNON,      0xFF,
    REG_GSNOFF,     0x6F,
    REG_MANUALRCV,  0x10,
    REG_TXAUTO,     0x03,
    REG_DEMOD,      0x41,
    REG_TXBITPHASE, 0x8F,
    0x00
};

UINT8 CfgTbl_Act106i[] =
{
    REG_MODE,        0x39,    // CRCPreset = 6363H
    REG_TXMODE,      0x81,    // ISO/IEC 14443A/MIFARE and 106 kbit, TxCRCEn On
    REG_RXMODE,      0x81,    // ISO/IEC 14443A/MIFARE and 106 kbit, RxCRCEn On 		
    REG_BITFRAMING,  0x00,
    REG_RFCFG,       0x59,
    REG_GSNON,       0xF4,
    REG_GSNOFF,      0x6F,
    REG_CWGSP,       0x3F,
    REG_MANUALRCV,   0x00,
    REG_TXAUTO,      0xCB,    //
    REG_DEMOD,       0x61,
    REG_RXTHRESHOLD, 0x55,
    REG_MODWIDTH,    0x26,
    REG_TXBITPHASE,  0x8F,
    0x00
};

UINT8 CfgTbl_Act212i[] =
{
    REG_MODE,        0xB8,    // CRCPreset = 0000H
    REG_TXMODE,      0x91,
    REG_RXMODE,      0x91,
    REG_BITFRAMING,  0x00,
    REG_RFCFG,       0x69,
    REG_GSNON,       0xFF,
    REG_GSNOFF,      0x6F,
    REG_CWGSP,       0x3F,
    REG_MANUALRCV,   0x10,
    REG_TXAUTO,      0x8B,
    REG_DEMOD,       0x61,
    REG_RXTHRESHOLD, 0x55,
    REG_MODGSP,      0x11,
    REG_TXBITPHASE,  0x8F,
    0x00
};

UINT8 CfgTbl_Act424i[] =
{
    REG_MODE,        0xB8,    // CRCPreset = 0000H
    REG_TXMODE,      0xA1,
    REG_RXMODE,      0xA1,
    REG_BITFRAMING,  0x00,
    REG_RFCFG,       0x69,
    REG_GSNON,       0xFF,
    REG_GSNOFF,      0x6F,
    REG_CWGSP,       0x3F,
    REG_MANUALRCV,   0x10,
    REG_TXAUTO,      0x8B,
    REG_DEMOD,       0x61,
    REG_RXTHRESHOLD, 0x55,
    REG_MODGSP,      0x11,
    REG_TXBITPHASE,  0x8F,
    0x00
};






/******************************************************************/
//       Configure NFC Type
/******************************************************************/
static void NFCInitiatorRegConfig(UINT8 NFCType)
{
    UINT8 Addr;
    UINT8 *pTable;
    UINT8 i;


    PrtMsg(DBGL1, "%s: start, NFCType = %02X\n", __FUNCTION__, NFCType);    

    switch(NFCType)
    {
        case PASSDEPI_106:
        case PASSDEPT_106:
            pTable = CfgTbl_Pas106i;
            break;
        case PASSDEPI_212:
        case PASSDEPT_212:
            RegWrite(REG_MODGSP, gaFelRFCon[FEL2_CON_ModGsP]);
            RegWrite(REG_RXTHRESHOLD, gaFelRFCon[FEL2_CON_RxThres]);
            RegWrite(REG_RFCFG, gaFelRFCon[FEL2_CON_RFCfg]);
            RegWrite(REG_CWGSP, gaFelRFCon[FEL2_CON_CWGsP]);
            pTable = CfgTbl_Pas212i;
            break;
        case PASSDEPI_424:
        case PASSDEPT_424:
            RegWrite(REG_MODGSP, gaFelRFCon[FEL4_CON_ModGsP]);
            RegWrite(REG_RXTHRESHOLD, gaFelRFCon[FEL4_CON_RxThres]);
            RegWrite(REG_RFCFG, gaFelRFCon[FEL4_CON_RFCfg]);
            RegWrite(REG_CWGSP, gaFelRFCon[FEL4_CON_CWGsP]);
            pTable = CfgTbl_Pas424i;
            break;
        case ACTDEPI_106:
        case ACTDEPT_106:
            pTable = CfgTbl_Act106i;
            break;
        case ACTDEPI_212:
        case ACTDEPT_212:
            pTable = CfgTbl_Act212i;
            break;
        default:
            pTable = CfgTbl_Act424i;
            break;
    }
    
    i = 0;
    Addr = pTable[i++];
    while(Addr) 
    {
        RegWrite(Addr,pTable[i++]);
        Addr = pTable[i++];
    }
}

/*****************************************************************/
//       START   A   PCD   transceive COMMAND 
/*****************************************************************/
static UINT8 FelicaBulkTransceive(UINT8 *senBuf, UINT8 senLen, UINT8 MaxFirstFIFOLen, UINT8 *recBuf, UINT8 *recLen, BOOL parityCheck)      
{
    UINT8 ret = ERROR_NO;
    UINT8 i = 0;
    UINT8 tempLen;
    UINT8 waitFor;
    UINT8 temp;


//    PrtMsg(DBGL5, "%s: start\n", __FUNCTION__);

    i = 0;
    if(senLen)
    {
        tempLen = (senLen < MaxFirstFIFOLen) ? senLen : MaxFirstFIFOLen;
        FIFOWrite(senBuf, tempLen);
        senLen -= tempLen;
        i += tempLen;
    }
    RegWrite(REG_COMMIRQ, 0x21);               // Clear the Rx Irq bit first
    RegWrite(REG_COMMAND, CMD_TRANSCEIVE);
    RegWrite(REG_BITFRAMING, 0x80);            // Start transmission

    while(senLen)
    {
        if(RegRead(REG_FIFOLEVEL) < 0x30)      // length < 48 ?
        {
            tempLen = (senLen < 0x0E) ? senLen : 0x0E;    // 0x0E ???
            FIFOWrite(senBuf + i, tempLen);
            senLen -= tempLen;
            i += tempLen;
        }
    }

    temp = 0;
    while((temp & 0x07) < 0x05)   //the value of the receicing is 110, Receving state
    {
        temp = RegRead(REG_STATUS2);
        PrtMsg(DBGL4, "%s: temp = %02X\n", __FUNCTION__, temp);
        if(RegRead(REG_STATUS2) == 0x01)
        {
            SetRegBit(REG_BITFRAMING, BIT_STARTSEND);
        }
    }

    tempLen = 0;
    i = 0;
    while(!(RegRead(REG_COMMIRQ) & 0x21))
    {
        tempLen = RegRead(REG_FIFOLEVEL);
        if(tempLen > 1)                            //  >10
        {
            FIFORead(recBuf + i, tempLen);
            i += tempLen;
            if(i > FELINFFIELDLEN)
            {
                return(ERROR_FSDLENTH);
            }
        }
    }
    tempLen = RegRead(REG_FIFOLEVEL);
    FIFORead(recBuf + i, tempLen);
    i += tempLen;
    if(i > FELINFFIELDLEN)
    {
        return(ERROR_FSDLENTH);
    }
    
    *recLen = i;
    if(RegRead(REG_COMMIRQ) & 0x01) 
    {
        ret = ERROR_NOTAG;         // Time Out Error
    }
   	else
   	{
        waitFor = RegRead(REG_ERROR) & 0x17;
        if(waitFor)  
        { 
            if(parityCheck)
            {
                if(waitFor & BIT_PARITYERR)
                {  
                    // parity error
                   	ret = ERROR_PARITY;
                }        
            }
            if(waitFor & BIT_PROTOCOLERR) 
            {  
                // protocol error
                ret = ERROR_PROTOCOL;
            }
            if(waitFor & BIT_BUFFEROVFL) 
            { 
                // FIFO overflow
                FIFOFlush();
                ret = ERROR_BUFOVFL;
            }
            if(waitFor & BIT_CRCERR) 
            {
                 ret = ERROR_CRC;
            }
        }
    }

//    PrtMsg(DBGL5, "%s: exit, ret = %02X\n", __FUNCTION__, ret);

    return(ret);
}



/******************************************************************/
//       Configure NFC Type
/******************************************************************/
static UINT8 FelREQC(UINT8 timeSlot, UINT16 SystemCode)
{
    UINT8 ret;
    UINT8 tempLen = 0;
    UINT8 tempBuf[20];


    PrtMsg(DBGL1, "%s: start, timeSlot = %02X, systemCode = %02X\n", __FUNCTION__, timeSlot, SystemCode);   

    SetRegBit(REG_TXAUTO, BIT_INIRFON);    // Enable RF initial on
    SetRegBit(REG_TXMODE, BIT_TXCRCEN);    // Enable TxCRC, 
    SetRegBit(REG_RXMODE, BIT_RXCRCEN);
    SetRegBit(REG_RXMODE, BIT_RXMULTIPLE);    // Enable RxCRC, Rx Mutiple
    ClearRegBit(REG_STATUS2, BIT_MFCRYPTO1ON);              // Disable crypto 1 unit

    RegRead(REG_TXAUTO);
    RegRead(REG_TXMODE);
    RegRead(REG_RXMODE);
    RegRead(REG_STATUS2);

    FIFOFlush();
    // REQC command format
    tempBuf[0] = 0x06;                 // 0x06
    tempBuf[1] = CMD_REQC;             // command code
    tempBuf[2] = SystemCode >> 8;        // system code high byte
    tempBuf[3] = SystemCode;           // system code low byte
    tempBuf[4] = 0x00;                 // reserved byte, 0x00
    tempBuf[5] = timeSlot;             // time slot number
    SetTimer100us(8000);            // timeout 8ms
    ret = FelicaBulkTransceive(tempBuf, 0x06, MAX_FIFO_LENGTH, tempBuf, &tempLen, FLAG_NOPARITYCHECK);
    ClearRegBit(REG_RXMODE, BIT_RXMULTIPLE);
    if((ret == ERROR_NO) || (ret == ERROR_NOTAG) || (ret == ERROR_CRC))
    {
        if(tempLen < 0x12)
        {
            return(ERROR_BYTECOUNT);
        }
        if(((tempBuf[0] != 0x12) && (tempBuf[0] != 0x14)) || (tempBuf[1] != RES_REQC))
        {
            return(ERROR_WRONGPARAM);
        }
        
        memcpy(picc.sn, tempBuf + 2, 8);    // NFCID2, 8 bytes

        PrtMsg(DBGL5, "%s: has found a felica tag\n", __FUNCTION__);
        PrtMsg(DBGL5, "NFCID: %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X\n", tempBuf[2], tempBuf[3], tempBuf[4], tempBuf[5], tempBuf[6], tempBuf[7], tempBuf[8], tempBuf[9]);
        picc.snLen = 8;
        memcpy(picc.PAD, tempBuf + 10, 8);
        if(tempBuf[0] == 0x14)
        {
            memcpy(picc.sysCode, tempBuf + 18, 2);
        }
    }
    
    return(ret);
}




void PollFeliCaTags(UINT8 feliType)
{
    PrtMsg(DBGL1, "%s: start\n", __FUNCTION__);

    NFCInitiatorRegConfig(feliType);

    if (FelREQC(CODE_TIMESLOTNUMBER_1, feliSysCode) == ERROR_NO)
    {
        if (feliType == PASSDEPI_212)
        {
            picc.type = PICC_FELICA212;
            pcd.curSpeed |= 0x09;        // ~ 212 kbit/s)
        }
        else
        {
            picc.type = PICC_FELICA424;
            pcd.curSpeed |= 0x12;        // ~ 424 kbit/s)
        }
    }
    else
    {
        picc.type = PICC_ABSENT;
    }
}

/*********************************************/
//    Set the PN512 timer clock for FeliCa use
/********************************************/
static void FelTimerSet(UINT8 *FelCommandBuf)
{
    UINT8 timeout;
    UINT8 TempN;
    UINT16  ReloadVal;
    

    RegWrite(REG_TMODE, 0x88);         //TAuto=1,TAutoRestart=0,TPrescaler=2047=7FFh
    RegWrite(REG_TPRESCALER, 0x00);    // Indicate 302us per timeslot

    if((FelCommandBuf[1]&0x01) || (FelCommandBuf[1]>0x17))
    {
        timeout = 0x30;
        TempN = FelCommandBuf[0];
    }
    else
    {
        if(FelCommandBuf[1] == CMD_REQRESPONSE)
        {
            timeout = picc.PAD[3];
            TempN = 0;
        }
        else if(FelCommandBuf[1] == CMD_REQSERVICE)
        {
            timeout = picc.PAD[2];
            TempN = FelCommandBuf[10];                 //Number of service
        }
        else if(FelCommandBuf[1] == CMD_READFROMSECURE)
        {
            timeout = picc.PAD[5];
            TempN = FelCommandBuf[10];                 // Number of service
        }
        else if(FelCommandBuf[1] == CMD_WRITETOSECURE)
        {
            timeout = picc.PAD[6];
            TempN = FelCommandBuf[10];                 // Number of service
        }
        else if(FelCommandBuf[1] == CMD_READ_NONEAUTH)
        {
            timeout = picc.PAD[5];
            TempN = FelCommandBuf[10+FelCommandBuf[10]*2];   //Number of Blocks
        }
        else if(FelCommandBuf[1] == CMD_WRITE_NONEAUTH)
        {
            timeout = picc.PAD[6];
            TempN = FelCommandBuf[10+FelCommandBuf[10]*2];   //Number of Blocks
        }
        else if(FelCommandBuf[1] == CMD_AUTH1)
        {
            timeout = picc.PAD[4];
            TempN = FelCommandBuf[10+FelCommandBuf[10]*2];  
            TempN += FelCommandBuf[10];
        }
        else if(FelCommandBuf[1] == CMD_AUTH2)
        {
            timeout = picc.PAD[4];
            TempN = 0;
        }
        else
        {
            timeout = 0xC0;
            TempN = 0;
        }
    }

    // response time = T * [(B + 1) * n + (A + 1)] * 4 ^ E
    ReloadVal  = (((timeout & 0x38) >> 3) + 1) * TempN;
    ReloadVal += (timeout & 0x07) + 1;
    if(timeout & 0xC0)
    {
        TempN = 4 << (timeout >> 5);
        TempN >>= 2;
    }
    else
    {
        TempN = 1;
    }
    ReloadVal *= TempN;
    RegWrite(REG_TRELOADVAL_HI, (UINT8)(ReloadVal >> 8));
    RegWrite(REG_TRELOADVAL_LO, (UINT8)ReloadVal);
    RegWrite(REG_COMMIRQ, 0x01);                           // Clear the TimerIrq bit
}


UINT8 FelReqResponse(void)
{
    UINT8 ret;
    UINT8 tempLen = 0;
    UINT8 tempBuf[10];
    

    FIFOFlush();

    // request response command
    tempBuf[1] = CMD_REQRESPONSE;                      // command code: 04
    memcpy(tempBuf + 2, picc.sn, picc.snLen);          // NFCID2: 8 bytes
    tempBuf[0] = picc.snLen + 2;
    FelTimerSet(tempBuf);
    ret = FelicaBulkTransceive(tempBuf, tempBuf[0], MAX_FIFO_LENGTH, tempBuf, &tempLen, FLAG_NOPARITYCHECK);
    if(ret == ERROR_NO)
    {
        if(tempLen != 0x0B)
        {
            return(ERROR_BYTECOUNT);
        }
        if((tempBuf[0]!=0x0B) || (tempBuf[1] != RES_REQRESPONSE))
        {
            ret = ERROR_WRONGPARAM;
        }
    }
    
    return(ret);
}



UINT8 FelXfrHandle(UINT8 *cmdBuf, UINT16 cmdLen, UINT8 *resBuf, UINT16 *resLen)
{
    UINT8 ret = SLOT_NO_ERROR;
    UINT8 tempLen = 0;


    FIFOFlush();
    if(cmdLen > FELINFFIELDLEN)
    {
        resBuf[0] = 0x67;
        resBuf[1] = 0x00;
        *resLen = 2;
        return(SLOT_NO_ERROR);
    }
    
    FelTimerSet(cmdBuf);
    ret = FelicaBulkTransceive(cmdBuf, (UINT8)cmdLen, MAX_FIFO_LENGTH, resBuf, &tempLen, FLAG_PARITYCHECK);
    if(ret == ERROR_NO)
    {
        *resLen = tempLen;
         ret = SLOT_NO_ERROR;
    }
    else
    {
        resBuf[0] = 0x64;
        resBuf[1] = 0x01;
        *resLen = 2;
        ret = SLOT_NO_ERROR;
    }
    
    return(ret);
}


UINT8 FelTransmisionHandle(UINT8 *cmdBuf, UINT16 cmdLen, UINT8 *resBuf, UINT16 *resLen)
{
    UINT8 ret = SLOT_NO_ERROR;
    UINT8 tempLen = 0;
    

    FIFOFlush();
    if(cmdLen > 254)
    {
        resBuf[0] = 0x67;
        resBuf[1] = 0x00;
        *resLen = 2;
        return(SLOT_NO_ERROR);
    }
    FelTimerSet(cmdBuf);
    ret = FelicaBulkTransceive(cmdBuf, (UINT8)cmdLen, MAX_FIFO_LENGTH, resBuf, &tempLen, FLAG_NOPARITYCHECK);
    if(ret == ERROR_NO)
    {
        *resLen = tempLen;
        ret = SLOT_NO_ERROR;
    }
    else
    {
        resBuf[0] = 0x64;
        resBuf[1] = 0x01;
        *resLen   = 2;
        ret       = SLOT_NO_ERROR;
    }
    
    return(ret);
}

