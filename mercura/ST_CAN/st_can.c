/****************************************************************************
 *                      S T A N D B Y
 ****************************************************************************
 *              M O D U L E    I M P L E M E N T A T I O N
 ****************************************************************************
 *  @file       st_can.c
 *  @brief      Standby can protocol
 *  @author     hans-juergen.schmitz / STANDBY GmbH, Dinslaken
 *  @date       03.08.2017 10:33:44 - created
 *  @version    1.0 - 03.08.2017 10:33:44 - hans-juergen.schmitz - created
 *  @bug        none
 *  @warning    none
*****************************************************************************/

#define ST_CAN_C
#include "st_can.h"
#include <string.h>
#include "conf_device.h"
/*
*******************************************************************************
*				#DEFINES
*******************************************************************************
*/
#define STCAN_MAX_NODE          61

// timer
#define STCAN_TIME_CANMSG         5000
#define STCAN_TIME_PRIMARY_WAKEUP 5
#define STCAN_TIME_PRIMARY_ACTIVE 400
#define STCAN_TIME_ACTIVE         600
#define STCAN_TIME_WAKEUP         100

// register
#define STCAN_VALUE_SECONDARY_REQUEST   99

/*
*******************************************************************************
*				ENUM....
*******************************************************************************
*/
enum STCAN_STATE
{
  STCAN_STATE_BOOTUP,
  STCAN_STATE_ACTIVE,
  STCAN_STATE_WAKEUP,
  STCAN_STATE_POWERDOWN,
  STCAN_STATE_IDLE,
};

enum STCAN_TMR
{
  STCAN_TMR_WAKEUP,
  STCAN_TMR_PRIMARYDATA,
  STCAN_TMR_ACTIVE,
  STCAN_TMR_MSG,
  STCAN_TMR_NB,  
};

enum STCAN_STATUS
{
  STCAN_STATUS_WAKEUPEXT,
  STCAN_STATUS_PRIMARYUPDATE,
  STCAN_STATUS_MASTER_ENABLE,
};
//////////////////////////////////////////////////////////////////////////
// COMMON
typedef struct _StCanData_t
{
  uint32_t  tmr[STCAN_TMR_NB];
  uint32_t  tmrPrimaryReload;
  uint8_t   state;
  uint8_t   nodeAnswer;
}StCanData_t;

/*
*******************************************************************************
*				DECLARATION OF LOCAL VARIABLES
*******************************************************************************
*/
StCanData_t       m_StCanData;
uint8_t           m_StCanNode;
uint32_t          m_StCanStatus;
TCanMsg	COMPILER_ALIGNED(4)	m_StCanMsg;
	
/*
*******************************************************************************
*				DECLARATION OF LOCAL FUNCTIONS
*******************************************************************************
*/
void StCan_SetPMState( uint8_t newstate );
/*
*******************************************************************************
*				DEFINITION OF LOCAL FUNCTIONS
*******************************************************************************
*/
/*******************************************************************************/
/*@brief  set next power state
*	@param
*	@retval
********************************************************************************/
void StCan_SetPMState( uint8_t newstate )
{
  // last state
  switch ( m_StCanData.state )
  {
  case STCAN_STATE_WAKEUP:
    ClrBit( m_StCanStatus, STCAN_STATUS_WAKEUPEXT );
  	break;
  }
  switch( newstate )
  {
    case STCAN_STATE_IDLE:
      m_StCanData.tmrPrimaryReload = 0;
      m_StCanData.tmr[STCAN_TMR_PRIMARYDATA] = 0;
      m_StCanData.tmr[STCAN_TMR_MSG] = STCAN_TIME_CANMSG;
      break;
    case STCAN_STATE_ACTIVE:
      ClrBit( m_StCanStatus, STCAN_STATUS_WAKEUPEXT );
      m_StCanData.tmrPrimaryReload = STCAN_TIME_PRIMARY_ACTIVE;
      m_StCanData.tmr[STCAN_TMR_PRIMARYDATA] = 1;
      break;
    case STCAN_STATE_WAKEUP:
      m_StCanData.tmrPrimaryReload = STCAN_TIME_PRIMARY_WAKEUP;
      m_StCanData.tmr[STCAN_TMR_WAKEUP] =  STCAN_TIME_WAKEUP;
      m_StCanData.tmr[STCAN_TMR_PRIMARYDATA] = 1;
      break;
  }
  m_StCanData.state = newstate;
}

/*
*******************************************************************************
*				DEFINITION OF EXPORTED FUNCTIONS
*******************************************************************************
*/

/*******************************************************************************/
/*@brief
*	@param
*	@retval
********************************************************************************/
void StCan_Init(void)
{
  if ( GetBit( m_StCanStatus, STCAN_STATUS_MASTER_ENABLE ) )
  {
    // start nodes
    StCan_Master_Init();
  }
  StCan_Device_Init();
	StCan_Gateway_Init();
}

/*******************************************************************************/
/*@brief
*	@param
*	@retval
********************************************************************************/
void StCan_Start( void )
{
  memset( &m_StCanData, 0, sizeof( m_StCanData ) );
  m_StCanData.nodeAnswer = m_StCanNode;
  if ( GetBit( m_StCanStatus, STCAN_STATUS_MASTER_ENABLE ) )
  {
    // start nodes
    StCan_Master_Start();
  }
  else
  {
    StCan_Device_Start();
  }    
}

/*******************************************************************************/
/*@brief
*	@param
*	@retval
********************************************************************************/
void StCan_Stop(void)
{
  if ( GetBit( m_StCanStatus, STCAN_STATUS_MASTER_ENABLE ) )
  {
    StCan_Master_Stop();
  }
  else
  {
    StCan_SetPMState( STCAN_STATE_POWERDOWN );
    StCan_ClearData();
  }  
}

/*******************************************************************************/
/*@brief
*	@param
*	@retval
********************************************************************************/
void StCan_Task(void)
{
  // check nodes
  if ( GetBit( m_StCanStatus, STCAN_STATUS_MASTER_ENABLE ) )
  {
    StCan_Master_Task();
  }
  else
  {
    // serve timer
    for( uint_fast8_t i = 0; i < STCAN_TMR_NB; i++ )
    {
      if( m_StCanData.tmr[i] )
      {
        m_StCanData.tmr[i]--;
        if ( m_StCanData.tmr[i] == 0 )
        {
          switch( i )
          {
            case STCAN_TMR_PRIMARYDATA:
              {
                m_StCanMsg.id = StCan_CalcCanid( m_StCanData.nodeAnswer, STCAN_GROUP_PRIMARY_DATAIN, STCAN_TYPE_RESPONSE );
                m_StCanMsg.len = StCan_SetPrimaryData(&m_StCanMsg.data);
                StCan_SendMessage( &m_StCanMsg );
                //m_StCanData.tmr[STCAN_TMR_PRIMARYDATA] = m_StCanData.tmrPrimaryReload;
              }
              break;
            case STCAN_TMR_ACTIVE:
              StCan_ClearData();
          }
        }      
      }
    }
    switch ( m_StCanData.state)
    {
    case STCAN_STATE_BOOTUP:
      StCan_SetPMState( STCAN_STATE_IDLE );
    	break;
    case STCAN_STATE_IDLE:
      if ( m_StCanData.tmr[STCAN_TMR_ACTIVE] )
      {
        StCan_SetPMState( STCAN_STATE_ACTIVE );
      }
      else if ( GetBit( m_StCanStatus, STCAN_STATUS_WAKEUPEXT ) )
      {
        StCan_SetPMState( STCAN_STATE_WAKEUP );
      }
      else if ( m_StCanData.tmr[STCAN_TMR_MSG] == 0 )
      {
        StCan_SetPMState( STCAN_STATE_POWERDOWN );
      }
      break;
    case STCAN_STATE_ACTIVE:
      if ( m_StCanData.tmr[STCAN_TMR_ACTIVE] == 0 )
      {
        StCan_SetPMState( STCAN_STATE_IDLE );
      }
      else
      {
        if ( GetBit( m_StCanStatus, STCAN_STATUS_PRIMARYUPDATE ) )
        {
          ClrBit( m_StCanStatus, STCAN_STATUS_PRIMARYUPDATE );
          m_StCanData.tmr[STCAN_TMR_PRIMARYDATA] = 1;
        }
      }
      break;
    case STCAN_STATE_WAKEUP:
      if ( m_StCanData.tmr[STCAN_TMR_ACTIVE] )
      {
        StCan_SetPMState( STCAN_STATE_ACTIVE );
      }
      else if ( m_StCanData.tmr[STCAN_TMR_WAKEUP] == 0 )
      {
        StCan_SetPMState( STCAN_STATE_IDLE );
      }
      break;
    case STCAN_STATE_POWERDOWN:
      if ( GetBit( m_StCanStatus, STCAN_STATUS_WAKEUPEXT ) 
      ||   m_StCanData.tmr[STCAN_TMR_ACTIVE] 
      ||   m_StCanData.tmr[STCAN_TMR_MSG] )
      {
        StCan_Device_Init();
        StCan_Start();
      }
      break;
    }
  }
}

/*******************************************************************************/
/*@brief
* @param
* @retval
********************************************************************************/
void StCan_ProcessMessage( TCanMsg* pPacket)
{
  if ( GetBit( m_StCanStatus, STCAN_STATUS_MASTER_ENABLE ) )
  {
    StCan_Master_ProcessMessage( pPacket);
  }
  else
  {
    m_StCanData.tmr[STCAN_TMR_MSG] = STCAN_TIME_CANMSG;
  }
  uint8_t  node = StCanGetNode(pPacket->id);

  // data for node or broadcast
  if ( ( node == m_StCanNode )
       ||   ( node == STCAN_NODE_BROADCAST ) )
  {
    if ( StCanGetType(pPacket->id) == STCAN_TYPE_REQUEST )
    {
      switch( StCanGetGroup(pPacket->id) )
      {
      case STCAN_GROUP_PRIMARY_DATAOUT:
        m_StCanData.tmr[STCAN_TMR_ACTIVE] = STCAN_TIME_ACTIVE;
        StCan_ProcessPrimaryData( &pPacket->data );
        m_StCanData.tmr[STCAN_TMR_PRIMARYDATA] = 1;
        m_StCanData.nodeAnswer = node;
        break;
      case STCAN_GROUP_SECONDARYDATA:
        // process only if active
        if( pPacket->data.u16[0] == CPU_TO_BE16(STCAN_REG_REPORTDATA) )
        {
          pPacket->data.u16[0] =pPacket->data.u16[1];
          pPacket->len = StCan_SecondaryDataRead( &pPacket->data );
          if ( pPacket->len )
          {
            pPacket->id = StCan_CalcCanid( node, STCAN_GROUP_SECONDARYDATA, STCAN_TYPE_RESPONSE );
            pPacket->len += 2;
            StCan_SendMessage( pPacket );
          }
        }
        else
        {
          StCan_SecondaryDataWrite( &pPacket->data );
        }
        break;
      case STCAN_GROUP_CONFIGDATA:
        // process only if active
        if( pPacket->data.u16[0] == CPU_TO_BE16(STCAN_REG_REPORTDATA) )
        {
          pPacket->data.u16[0] = pPacket->data.u16[1];
          pPacket->len = StCan_ConfigDataRead( &pPacket->data );
          if ( pPacket->len )
          {
            pPacket->id = StCan_CalcCanid( node, STCAN_GROUP_CONFIGDATA, STCAN_TYPE_RESPONSE );
            pPacket->len += 2;
            StCan_SendMessage( pPacket );
          }
        }
        else
        {
          if( ( node == STCAN_NODE_BROADCAST )
              || (pPacket->data.u16[0] < CPU_TO_BE16(STCAN_CONFREG_TESTERONLY)))
          {
            StCan_ConfigDataWrite( &pPacket->data, pPacket->len);
          }
        }
        break;
      }
    }
  }
	else
	{
		StCan_Gateway_ProcessMessage( pPacket );
	}
}


/*******************************************************************************/
/*@brief  map internal data to can
*	@param
*	@retval
********************************************************************************/
void StCan_Update( void )
{
  if ( GetBit( m_StCanStatus, STCAN_STATUS_MASTER_ENABLE ) )
  {
    StCan_Master_Update();
  }
  else
  {
    StCan_Device_Update();
  }
}

/*******************************************************************************/
/*@brief  set new function state
*	@param  pFctData  : reference to function data ( id, parameter, option )
* @param  value     : 
*	@retval -
********************************************************************************/
void StCan_FctUpdate( TFctData fct, uint32_t value )
{
  if ( GetBit( m_StCanStatus, STCAN_STATUS_MASTER_ENABLE ) )
  {
    StCan_Master_FctUpdate( fct, value);
  }
  else
  {
    if( StCan_Device_FctUpdate( fct, value) )
    {
      SetBit( m_StCanStatus, STCAN_STATUS_PRIMARYUPDATE );
      if ( fct.Id == PROT_FCT_INPUT_ID )
      {
        SetBit( m_StCanStatus, STCAN_STATUS_WAKEUPEXT );
      }
    }
  }
}

/*******************************************************************************/
/*@brief
*	@param
*	@retval
********************************************************************************/
TByte StCan_GetBusStatus(void)
{
  if ( GetBit( m_StCanStatus, STCAN_STATUS_MASTER_ENABLE ) )
  {
    return StCan_Master_GetBusStatus();
  }
  else
  {
    TByte value = 0;
    if ( m_StCanData.tmr[STCAN_TMR_ACTIVE] )
    {
      value |= BUSY_STATUS_COMMUNICATION;
    }
    if ( m_StCanData.state < STCAN_STATE_POWERDOWN )
    {
      value |= BUSY_STATUS_NODEACTIVE;
    }
    return value;
  }
}


/*******************************************************************************/
/*@brief
* @param
* @retval
********************************************************************************/
void StCan_Config( void )
{
  if ( GetBit( m_StCanStatus, STCAN_STATUS_MASTER_ENABLE ) )
  {
    StCan_Master_Config();
  }
  else
  {
    StCan_Device_Config();
  }    
}

/*******************************************************************************/
/*@brief
* @param
* @retval
********************************************************************************/
void StCan_SetMapping( T_MAP_ITEM* pItem )
{
  if ( GetBit( m_StCanStatus, STCAN_STATUS_MASTER_ENABLE ) )
  {
    StCan_Master_SetMapping( pItem );
  }
  StCan_Device_SetMapping( pItem );
}

/*******************************************************************************/
/*  @brief
 *   @param
 *   @retval
 ********************************************************************************/
void StCan_SetBitValue(TByte id, TByte newvalue, TByte lastvalue, TByte bit)
{
  if ( GetBit( lastvalue, bit ) != GetBit(newvalue, bit))
  {
    StCan_SetFctValue( (TFctData) { .Id = id}, GetBit(newvalue, bit));
  }
}

/*******************************************************************************/
/*@brief
* @param
* @retval
********************************************************************************/
void StCan_WakeUp( uint32_t source )
{
  if ( source )
  {
    SetBit( m_StCanStatus, STCAN_STATUS_WAKEUPEXT );
  }
}

/**
 * @brief wrapper for Set_FctValue
 *
 * @param id    function id
 * @param value
 */
void StCan_SetValue( TByte id, uint32_t value)
{
  if ( id )
  {
    StCan_SetFctValue( (TFctData) {.Id=id}, value );
  }
}

/**
 * @brief set node id
 *
 * @param node id
 */
void StCan_SetNodeId( uint8_t node)
{
  m_StCanNode = node;
}

uint8_t StCan_GetNodeId( void )
{
  return m_StCanNode;
}

/**
 * @brief returns communication status of specified node
 *
 * @param node
 * @return
 */
uint8_t StCan_GetNodeStatus(uint8_t node)
{
  // ToDO
  return 0;
}

void StCan_MasterEnable( TBool enable)
{
  SetBitValue( m_StCanStatus, STCAN_STATUS_MASTER_ENABLE, enable );
}
//////////////////////////////////////////////////////////////////////////
// APP callback
__weak TByte StCan_SendMessage( TCanMsg* pPacket){ return FALSE;}
__weak void StCan_SetFctValue( TFctData fct, uint32_t value ){}
__weak void StCan_ProcessPrimaryData( Union64* pData ){}
__weak uint8_t StCan_SetPrimaryData( Union64* pData ){return 1;}
__weak uint8_t StCan_ConfigDataRead( Union64* pData ){ return 0; }
__weak void StCan_ConfigDataWrite( Union64* pData, uint8_t nbdata){}
__weak uint8_t StCan_SecondaryDataRead( Union64* pData ){ return 0; } 
__weak void StCan_SecondaryDataWrite( Union64* pData ){}
__weak void StCan_ClearData( void ){}
__weak void StCan_Device_ResetData(void){}
//////////////////////////////////////////////////////////////////////////
// SLAVE
__weak void  StCan_Device_Config(void){}
__weak void  StCan_Device_Start( void ){}
__weak void  StCan_Device_Init( void ){}
__weak void  StCan_Device_Update(void){}
__weak uint8_t StCan_Device_FctUpdate( TFctData fct, uint32_t value ){ return FALSE;}
__weak void  StCan_Device_SetMapping( T_MAP_ITEM* pItem ){}

//////////////////////////////////////////////////////////////////////////
// MASTER
__weak void  StCan_Master_Start( void ){}
__weak void  StCan_Master_Stop(void){}
__weak void  StCan_Master_Init( void ){}
__weak void  StCan_Master_Task( void ){}
__weak void  StCan_Master_Update(void){}
__weak void  StCan_Master_FctUpdate( TFctData fct, uint32_t value ){}
__weak void  StCan_Master_ProcessMessage( TCanMsg* pPacket){}
__weak uint8_t StCan_Master_GetBusStatus(void){return 0;}
__weak void  StCan_Master_SetMapping( T_MAP_ITEM* pItem ){}
__weak void  StCan_Master_Config( void ){}  
__weak void StCan_SetConfigRBC( const TConfigRBCStCan* pSrc ){}
//////////////////////////////////////////////////////////////////////////
// GATEWAY
__weak void StCan_Gateway_ProcessMessage( TCanMsg* pPacket){}
__weak void StCan_Gateway_Init(void){}
__weak void StCan_Gateway_SetData( uint8_t* pData, uint8_t len, eGatewaySource src){}
// end of file
