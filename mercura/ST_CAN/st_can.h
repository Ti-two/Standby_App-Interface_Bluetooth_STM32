/****************************************************************************
 *                      S T A N D B Y
 ****************************************************************************
 *              M O D U L E    S P E C I F I C A T I O N
 ****************************************************************************
 *  @file       st_can.h
 *  @brief      Standby can protocol
 *  @author     hans-juergen.schmitz / STANDBY GmbH, Dinslaken
 *  @date       03.08.2017 10:34:04 - created
 *  @version    1.0 - 03.08.2017 10:34:04 - hans-juergen.schmitz - created
 *  @bug        none
 *  @warning    none
*****************************************************************************/

#ifndef ST_CAN_INC
#define ST_CAN_INC

#ifdef __cplusplus
extern "C" {
#endif

#include "common_types.h"

/*
*******************************************************************************
*				#DEFINES
*******************************************************************************
*/
#define STCAN_TIMEOUT_HEARTBEAT  600
#define STCAN_TMR_REPEAT         400

#define STCAN_CANID_POS_TYPE    0
#define STCAN_CANID_POS_NODE    3
#define STCAN_CANID_POS_GROUP   9

#define STCAN_CANID_MASK_TYPE   0x07
#define STCAN_CANID_MASK_NODE   0x3F
#define STCAN_CANID_MASK_GROUP  0x03

// node address
#define STCAN_NODE_BROADCAST           0

// common data
#define STCAN_RESTORE_PARAMETER         0xAAAA

// scaling
#define STCAN_VOLUME_PA_TO_CAN				( 100.0 / 255.0 )
/*
*******************************************************************************
*				MACRO....
*******************************************************************************
*/
#define StCan_CalcCanid( node, group, type)   ( (( group & STCAN_CANID_MASK_GROUP ) << STCAN_CANID_POS_GROUP ) | (( node & STCAN_CANID_MASK_NODE) << STCAN_CANID_POS_NODE ) | (( type & STCAN_CANID_MASK_TYPE )) )
#define StCanGetNode( id )                    ((id >> STCAN_CANID_POS_NODE) & STCAN_CANID_MASK_NODE)
#define StCanGetGroup( id )                   ((id >> STCAN_CANID_POS_GROUP) & STCAN_CANID_MASK_GROUP)
#define StCanGetType( id )                    ((id >> STCAN_CANID_POS_TYPE) & STCAN_CANID_MASK_TYPE)
/*
*******************************************************************************
*				ENUM....
*******************************************************************************
*/
#define STCAN_MASK_WBMODE               0x03

typedef enum
{
  // common register
  STCAN_REG_NODETYPE = 30,
  STCAN_REG_SWVERSION = 31,
  STCAN_REG_ARTICLENB = 32,
  STCAN_REG_SERIALNB = 33,
  STCAN_REG_NODEADDR = 34,
  STCAN_REG_FACTORYDEFAULT = 38,
  STCAN_REG_HARDWARE_ID = 40,
  STCAN_REG_REPORTDATA = 99,
  // boot loader
  STCAN_REG_BLVERSION = 85,
  STCAN_REG_FIRMWARE_CMD = 401,
  STCAN_REG_FIRMWARE_STATUS = 402,
  STCAN_REG_FIRMWARE_DATA = 403,
  STCAN_REG_FIRMWARE_TEXTSTRING_DATA = 404,
}eStCanRegConfig;

enum E_STCAN_CGW_OPTION
{
  STCAN_OPTION_CGW_WBMODE1,
  STCAN_OPTION_CGW_WBMODE2,
  STCAN_OPTION_CGW_SYNCOFF,
  STCAN_OPTION_CGW_MASTER,
  STCAN_OPTION_CGW_S09,
  STCAN_OPTION_CGW_BOBW3,
  STCAN_OPTION_CGW_RESERVED,
};

enum E_STCAN_GROUP
{
  STCAN_GROUP_PRIMARY_DATAOUT,
  STCAN_GROUP_PRIMARY_DATAIN,
  STCAN_GROUP_SECONDARYDATA,
  STCAN_GROUP_CONFIGDATA,
};

enum E_STCAN_TYPE
{
  STCAN_TYPE_REQUEST,
  STCAN_TYPE_RESPONSE,
  STCAN_TYPE_GLOBALDATAOUT,
  STCAN_TYPE_GLOBALDATAIN,
  STCAN_TYPE_FLASHSYNC = 7,
};

enum E_STCAN_STATUS
{
  STCAN_STATUS_BUSACTIVE,
  STCAN_STATUS_BUSY,
  STCAN_STATUS_ROTATE_PRIO1,
  STCAN_STATUS_UPDATE,
  STCAN_STATUS_SHUTDOWN,
  STCAN_STATUS_BEACON_FLASHER_ACTIVE,
};

typedef enum E_STCAN_PRIMARY_PACKET
{
  STCAN_PRIMARY_REQUEST,
  STCAN_PRIMARY_RESPONSE,
  STCAN_PRIMARY_PACKET_COUNT,
} eSTCanPacket;

typedef enum
{
	STCAN_LOADER_CMD_NOACTION = 0,
	STCAN_LOADER_CMD_ERASE = 1,
	STCAN_LOADER_CMD_DATASTART = 2,
	STCAN_LOADER_CMD_DATAEND = 3,
	STCAN_LOADER_CMD_VERIFY = 4,
	STCAN_LOADER_CMD_RESET = 5,
	STCAN_LOADER_CMD_CLEARFLAGS = 6,
	STCAN_LOADER_CMD_GET_FIRMWAREID = 7,
	STCAN_LOADER_CMD_ENCRYPTED_DATASTART = 8,
	STCAN_LOADER_CMD_ENCRYPTED_DATAEND = 9,
	STCAN_LOADER_CMD_CONFIG_DATASTART = 10,
	STCAN_LOADER_CMD_CONFIG_DATAEND = 11,
} eStCanLoaderCmd;

typedef enum
{
	STCAN_LOADER_STATUSFLAG_ERASE_SUCCESS = 0,
	STCAN_LOADER_STATUSFLAG_ERASE_FAILED = 1,
	STCAN_LOADER_STATUSFLAG_CHECKSUM_OK = 2,
	STCAN_LOADER_STATUSFLAG_CHECKSUM_FAILED = 3,
	STCAN_LOADER_STATUSFLAG_WRITEDATA_FAIL = 4,
	STCAN_LOADER_STATUSFLAG_ILLEGAL_CMD = 5,
	STCAN_LOADER_STATUSFLAG_DATATRANSFER_PENDING = 6,
	STCAN_LOADER_STATUSFLAG_FIRMWAREID_MISMATCH = 7,
	STCAN_LOADER_STATUSFLAG_DATAPROCESS_PENDING = 8,
} eStCanLoaderStatus;

/*
*******************************************************************************
*				STRUCT....
*******************************************************************************
*/

/*
*******************************************************************************
*				TYPEDEF....
*******************************************************************************
*/
typedef struct _TStCanCGW
{
  TByte Protocol;
  TByte res1;
  TByte NodeAddrDeviceWB1;
  TByte NodeAddrWB2;
  TByte NodeAddrD16_1;
  TByte NodeAddrLS1;
  TByte NodeAddrHandyButton;
  TByte NodeAddrS09;
  TByte NodeAddrD16_2;
  TByte NodeAddrD16_3;
  TByte NodeAddrSL;
  TByte Option;
  TByte res2;
  TByte res3;
  TByte res4;
  TByte res5;
  TByte BaudrateId;
  TByte NodeAddrP4;
  TByte NodeAddrT4_1;
  TByte NodeAddrT4_2;
  TByte NodeAddrMSG_1;
  TByte NodeAddrMSG_2;
  TByte NodeAddrMSG_3;
  TByte NodeAddrMSG_4;
	TByte NodeAddrGatewayKeypad;
} TStCanCGW;

typedef struct _TStCanPacketConfig
{
  TByte group;
  TByte type;
  TByte len;
} TStCanPacketConfig;

typedef struct __TStCanMap
{
  TByte   Id;
  TWord   Para;
  TWord   Option;
}TStCanMap;

typedef struct
{
  TByte   Function;
  TByte   Parameter;
  TWord   Option;
}TConfigSTCANOut;

#define PROT_STCAN_WBOUT_NUM    34
#define PROT_STCAN_WBINPUT_NUM  10

enum E_STCAN_HBKEY
{
  STCAN_HBKEY_PRESS,
  STCAN_HBKEY_LONGPRESS_TOGGLE,
  STCAN_HBKEY_TOGGLE,
	STCAN_HBKEY_LONGPRESS_OFF,
  STCAN_HBKEY_NB,
};

typedef struct
{
  TByte               Option;
  TConfigSTCANOut     Output[PROT_STCAN_WBOUT_NUM];
  TProtKeypadItem3    Input[PROT_STCAN_WBINPUT_NUM];
  TProtKeyItem        KeyItem[STCAN_HBKEY_NB];
}__packed TConfigRBCStCan;

typedef struct
{
  const char* pString;
  uint32_t  size;
  uint32_t  index;
}StcanStringTransfer_t;

/*
*******************************************************************************
*				DECLARATION OF EXTERNAL VARIABLES
*******************************************************************************
*/

/*
*******************************************************************************
*				DECLARATION OF EXPORTED VARIABLES
*******************************************************************************
*/
#ifdef ST_CAN_C
#define extern
#endif

#ifdef extern
#undef extern
#endif


/*******************************************************************************
*				DECLARATION OF EXPORTED FUNCTIONS
*******************************************************************************/
void    StCan_Init(void);
void    StCan_Start( void );
void    StCan_Stop(void);
void    StCan_Task(void);
void    StCan_ProcessMessage( TCanMsg* pPacket);
void    StCan_Update( void );
TByte   StCan_GetBusStatus(void);
void    StCan_Config( void );
void    StCan_SetConfigRBC( const TConfigRBCStCan* pSrc );
void    StCan_SetMapping( T_MAP_ITEM* pItem );
void    StCan_SetBitValue( TByte id, TByte newvalue, TByte lastvalue, TByte bit );
void    StCan_WakeUp( uint32_t source );
void  	StCan_SetNodeId(uint8_t node);
uint8_t StCan_GetNodeId(void);
void 		StCan_SetNodeType( uint16_t type );
uint8_t StCan_GetNodeStatus(uint8_t node);
void    StCan_SetValue( TByte id, uint32_t value);
void    StCan_FctUpdate( TFctData fct, uint32_t value );
void		StCan_MasterEnable( TBool enable);
//////////////////////////////////////////////////////////////////////////
/// external callback function
uint8_t StCan_SendMessage( TCanMsg* pMsg);
void  StCan_SetFctValue( TFctData fct, uint32_t value );
void  StCan_ClearData( void );

//////////////////////////////////////////////////////////////////////////
/// STCAN Device
void  StCan_ProcessPrimaryData( Union64* pData );
uint8_t  StCan_SecondaryDataRead( Union64* pData );
void  StCan_SecondaryDataWrite( Union64* pData );
uint8_t StCan_ConfigDataRead( Union64* pData );
void StCan_ConfigDataWrite( Union64* pData, uint8_t nbdata);
void  StCan_Device_Init( void );
void StCan_Device_Start( void );
void  StCan_Device_Update(void);
uint8_t  StCan_Device_FctUpdate( TFctData fct, uint32_t value );
void  StCan_Device_Config( void );
void  StCan_Device_SetMapping( T_MAP_ITEM* pItem );
uint8_t StCan_SetPrimaryData( Union64* pData );
// callback function
void StCan_Device_ResetData(void);

//////////////////////////////////////////////////////////////////////////
/// STCAN Master
void  StCan_Master_Start( void );
void  StCan_Master_Init( void );
void  StCan_Master_Stop(void);
void  StCan_Master_Task( void );
void  StCan_Master_Update(void);
void  StCan_Master_FctUpdate( TFctData fct, uint32_t value );
void  StCan_Master_Config( void );
void  StCan_Master_ProcessMessage( TCanMsg* pPacket);
TByte StCan_Master_GetBusStatus(void);
void  StCan_Master_SetMapping( T_MAP_ITEM* pItem );
//////////////////////////////////////////////////////////////////////////
/// STCAN GATEWAY
void StCan_Gateway_Init(void);
void StCan_Gateway_ProcessMessage( TCanMsg* pPacket);
void StCan_Gateway_NewData( uint8_t* pData, uint8_t length, eGatewaySource src);
void StCan_Gateway_SetData( uint8_t* pData, uint8_t length, eGatewaySource src);

#ifdef __cplusplus
}
#endif

#endif /* ST_CAN_INC */
