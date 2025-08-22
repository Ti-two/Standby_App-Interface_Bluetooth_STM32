#ifndef TYPES_H
#define TYPES_H

//#include "stm32f10x.h"

/*******************************************************************************
* Basic types definitions
*******************************************************************************/

typedef unsigned char   U8;
#define U8_CAL          const U8
#define U8_MIN          (0U)
#define U8_MAX          (255U)

typedef signed char     S8;
#define S8_CAL          const S8
#define S8_MIN          (-128)
#define S8_MAX          (127)

typedef unsigned short  U16;
#define U16_CAL         const U16
#define U16_MIN         (0U)
#define U16_MAX         (65535U)

typedef signed short    S16;
#define S16_CAL         const S16
#define S16_MIN         (-32768)
#define S16_MAX         (32767)

typedef unsigned int    U32;
#define U32_CAL         const U32
#define U32_MIN         (0UL)
#define U32_MAX         (4294967295UL)

typedef unsigned long long    U64;
#define U64_CAL         const U64
#define U64_MIN         (0UL)
#define U64_MAX         (4294967295UL)

typedef signed long long    S64;

typedef float           F64;
#define F64_MAX         3.4e38

typedef signed long  const SC32;  /* Read Only */
typedef signed short const SC16;  /* Read Only */
typedef signed char  const SC8;   /* Read Only */

typedef volatile signed long  VS32;
typedef volatile signed short VS16;
typedef volatile signed char  VS8;

typedef volatile signed long  const VSC32;  /* Read Only */
typedef volatile signed short const VSC16;  /* Read Only */
typedef volatile signed char  const VSC8;   /* Read Only */

typedef volatile unsigned long  VU32;
typedef volatile unsigned short VU16;
typedef volatile unsigned char  VU8;

typedef volatile unsigned long  const VUC32;  /* Read Only */
typedef volatile unsigned short const VUC16;  /* Read Only */
typedef volatile unsigned char  const VUC8;   /* Read Only */

typedef unsigned long  const UC32;  /* Read Only */
typedef unsigned short const UC16;  /* Read Only */
typedef unsigned char  const UC8;   /* Read Only */

typedef signed long       S32;
#define S32_CAL         const S32
#define S32_MIN         (-2147483647L -1)
#define S32_MAX         (2147483647L)
/*
#define s32 S32
#define s16 S16
#define s8  S8

#define sc32 SC32  // Read Only
#define sc16 SC16  // Read Only
#define sc8 SC8   // Read Only

#define vs32 VS32
#define vs16 VS16
#define vs8  VS8

#define vsc32 VSC32  // Read Only
#define vsc16 VSC16  // Read Only
#define vsc8 VSC8   // Read Only

#define u8 U8
#define uc8 UC8
#define vu8 VU8
#define u16 U16
#define uc16 UC16
#define vu16 VU16
#define u32 U32
#define uc32 UC32
#define vu32 VU32
#define vuc32 VUC32
*/
/*******************************************************************************
* Storage modifier
*******************************************************************************/

#define CAL             const



/*******************************************************************************
* Binary points definitions
*******************************************************************************/

#define BIN0            1
#define BIN1            2
#define BIN2            4
#define BIN3            8
#define BIN4            16
#define BIN5            32
#define BIN6            64
#define BIN7            128
#define BIN8            256
#define BIN9            512
#define BIN10           1024
#define BIN11           2048
#define BIN12           4096
#define BIN13           8192
#define BIN14           16384
#define BIN15           32768
#define BIN16           65536L
#define BIN17           131072L
#define BIN18           262144L
#define BIN19           524288L
#define BIN20           1048576L
#define BIN21           2097152L
#define BIN22           4194304L
#define BIN23           8388608L
#define BIN24           16777216L
#define BIN25           33554432L
#define BIN26           67108864L
#define BIN27           134217728L
#define BIN28           268435456L
#define BIN29           536870912L
#define BIN30           1073741824L
#define BIN31           2147483648L
#define BIN32           4294967296L



/*******************************************************************************
* Decimal points definitions
*******************************************************************************/

#define DEC0            1
#define DEC1            10
#define DEC2            100
#define DEC3            1000
#define DEC4            10000
#define DEC5            100000L
#define DEC6            1000000L
#define DEC7            10000000L
#define DEC8            100000000L
#define DEC9            1000000000L


/*******************************************************************************
* Boolean definitions
*******************************************************************************/
typedef enum
{
    FALSE = 0,
    TRUE
} BOOL_TYPE;

#define BOOL_TYPE_CAL const BOOL_TYPE
#define DISABLED        FALSE
#define ENABLED         TRUE

#define OFF             FALSE
#define ON              TRUE

#define LOW             FALSE
#define HIGH            TRUE

#define BP_RELEASED     FALSE
#define BP_PUSHED       TRUE

/*******************************************************************************
* Bit oriented definitions
*******************************************************************************/

typedef unsigned short  BIT_FIELD;

#define U16_FLAG        U16

typedef struct
{
    const U8            flag_pos;
    U16_FLAG *          const flag_carrier;
} BIT_BOOL_TYPE;

#define BIT_BOOL         const BIT_BOOL_TYPE
/*******************************************************************************
* Function oriented definitions
*******************************************************************************/
typedef  void (*pt2Function)(void); /* ptr to function returning void */

/*******************************************************************************
* Other definitions
*******************************************************************************/
/*
typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) ((STATE == DISABLE) || (STATE == ENABLE))

typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;
*/

#define NULL            (void *)(0)
#endif
