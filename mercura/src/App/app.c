/*******************************************************************************
*             MERCURA Departement SYNERGIE Ingénierie Industrielle             *
*                                                                              *
*                Ce document est la propriétée de la société                   *
*            MERCURA Département SYNERGIE Ingénierie Industrielle              *
*     Il ne doit pas être copié partiellement ou totalement sans l'accord      *
*                       écrit de la société MERCURA                            *
*******************************************************************************/
/*******************************************************************************
*  Library Include Files														  *
*******************************************************************************/
#include	"hwi.h" 
#include	"app.h"   

/*******************************************************************************
*                                                                              *
* ***                             ATTENTION !!!!!                          *** *
* ***           LA ZONE CI-DESSOUS EST UNE ZONE DE PARAMETRAGE             *** *
*                                                                              *
*  - SEULES LES VALEURS DES CONSTANTES PEUVENT ETRE MODIFIEES                  *
*  - TOUTE DONNEE SUPPLEMENTAIRE SERA AUTOMATIQUEMENT DETRUITE                 *
*                                                                              *
*------------------------------------------------------------------------------*/

// APP_SW_NAME ne doit pas dépasser 4 caractères HEXA
const U8 APP_SW_NAME[APP_SW_NAME_SIZE]			= "CBU2";

// APP_HW_VERSION TEXT doit absolument être formaté en 8 caractères
const U8 APP_HW_NAME[APP_HW_NAME_SIZE]			= "WIFI_BLT";

// APP_SW_VERSION TEXT doit absolument être formaté en xx.yy
const U8 APP_SW_VERSION_TEXT[APP_SW_VERSION_TEXT_SIZE]	= "01.06";


/******************************************************************************
*                                                                              *
* ***                             ATTENTION !!!!                           *** *
* ***    LA ZONE CI-DESSOUS EST INTERDITE A TOUTE MISE A JOUR MANUELLE     *** *
*                                                                              *
*            TOUTE DONNEE SUPPLEMENTAIRE INSCRITE DANS CETTE ZONE              *
*                      SERA AUTOMATIQUEMENT DETRUITE                           *
*                                                                              *
*------------------------------------------------------------------------------*/

S8_CAL		APP_PROM_ID_APV[APP_PROM_ID_SIZE_CPV]		= "CBU20106 du 31/01/2025 a 10:49:17";
const S8	APP_CODE_VERSION_CPV[APP_PROM_ID_SIZE_CPV]	= "CBU20106 du 31/01/2025 a 10:49:17";

S8_CAL		APP_USER_NAME_DATA_APV[APP_USER_NAME_DATA_SIZE_CPV] = "bdelporte";
const S8	APP_USER_NAME_CODE_CPV[APP_USER_NAME_CODE_SIZE_CPV] = "bdelporte";

U8_CAL		APP_SW_VERSION_DATA_APV[APP_SW_VERSION_SIZE_CPV] = {0x25, 0x5, 0x04}; /* year, week, day in the week */ 
const U8	APP_SW_VERSION_CODE_CPV[APP_SW_VERSION_SIZE_CPV] = {0x25, 0x5, 0x04}; /* year, week, day in the week */ 

