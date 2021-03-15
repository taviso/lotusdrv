#if      !defined(LEP_H)
#define  LEP_H
/****************************************************************************/
/**   Module Name: LEP.H                                                   **/
/**   123 for OS/2 External Programming include file                       **/
/**   Copyright (c) 1991, Lotus Development Corporation                    **/
/****************************************************************************/

/****************************************************************************/
/**   Constants                                                            **/
/****************************************************************************/

/****************************************************************************/
/**   messages sent to the Library Manager function                        **/
/****************************************************************************/
#define  LEPC_MSG_BASE                    0
#define  LEPC_MSG_ATTACH                  (LEPC_MSG_BASE+0)
#define  LEPC_MSG_DETACH                  (LEPC_MSG_BASE+1)
#define  LEPC_MSG_BREAK                   (LEPC_MSG_BASE+2)

/****************************************************************************/
/**   @function registration options                                       **/
/****************************************************************************/
#define  LEPC_OPT_REG_NONE                0
#define  LEPC_OPT_REG_ALWAYS_DIRTY        (1<<0)
#define  LEPC_OPT_REG_NO_ARGUMENTS        (1<<1)

/****************************************************************************/
/**   @function iteration options                                          **/
/****************************************************************************/
#define  LEPC_OPT_LOOP_NONE               0
#define  LEPC_OPT_LOOP_EMPTY_CELLS        (1<<0)

/****************************************************************************/
/**   range coordinate constants                                           **/
/****************************************************************************/
#define  LEPC_COORD_DIMEN                 3
#define  LEPC_COORD_SHEET                 0
#define  LEPC_COORD_COLUMN                1
#define  LEPC_COORD_ROW                   2

/****************************************************************************/
/**   macro command argument prototype values                              **/
/****************************************************************************/
#define  LEPC_PROT_TYPE_MASK              0x00FF
#define  LEPC_PROT_TYPE_BASE              0
#define  LEPC_PROT_TYPE_STRING            (LEPC_PROT_TYPE_BASE+0)
#define  LEPC_PROT_TYPE_NUMBER            (LEPC_PROT_TYPE_BASE+1)
#define  LEPC_PROT_TYPE_CONDITION         (LEPC_PROT_TYPE_BASE+2)
#define  LEPC_PROT_TYPE_VALUE             (LEPC_PROT_TYPE_BASE+3)
#define  LEPC_PROT_TYPE_RANGE             (LEPC_PROT_TYPE_BASE+4)

/****************************************************************************/
/**   macro command argument prototype options                             **/
/****************************************************************************/
#define  LEPC_PROT_OPT_MASK               0xFF00
#define  LEPC_PROT_OPT_OPTIONAL           (1<<8)

/****************************************************************************/
/**   @function and macro command argument and cell value types            **/
/****************************************************************************/
#define  LEPC_TYPE_BASE                   0
#define  LEPC_TYPE_VALUE                  (LEPC_TYPE_BASE+0)
#define  LEPC_TYPE_TINY                   (LEPC_TYPE_BASE+1)
#define  LEPC_TYPE_SHORT                  (LEPC_TYPE_BASE+2)
#define  LEPC_TYPE_LONG                   (LEPC_TYPE_BASE+3)
#define  LEPC_TYPE_FLOAT                  (LEPC_TYPE_BASE+4)
#define  LEPC_TYPE_DOUBLE                 (LEPC_TYPE_BASE+5)
#define  LEPC_TYPE_TREAL                  (LEPC_TYPE_BASE+6)
#define  LEPC_TYPE_ERR                    (LEPC_TYPE_BASE+7)
#define  LEPC_TYPE_NA                     (LEPC_TYPE_BASE+8)
#define  LEPC_TYPE_EMPTY                  (LEPC_TYPE_BASE+9)
#define  LEPC_TYPE_STRING                 (LEPC_TYPE_BASE+10)
#define  LEPC_TYPE_RANGE                  (LEPC_TYPE_BASE+11)
#define  LEPC_TYPE_COLLECTION             (LEPC_TYPE_BASE+12)
#define  LEPC_TYPE_MISSING                (LEPC_TYPE_BASE+13)

#define  LEPC_MAX_STRING                  513

/****************************************************************************/
/**   External Programming Manager return codes                            **/
/****************************************************************************/
#define  LEPC_RET_BASE                    0
#define  LEPC_RET_SUCCESS                 (LEPC_RET_BASE+0)
#define  LEPC_RET_LOOP_DONE               (LEPC_RET_BASE+1)
#define  LEPC_RET_DUPLICATE_NAME          (LEPC_RET_BASE+2)
#define  LEPC_RET_ARGUMENT_OVERFLOW       (LEPC_RET_BASE+3)
#define  LEPC_RET_NUMERIC_OVERFLOW        (LEPC_RET_BASE+4)
#define  LEPC_RET_TRUNCATED_NUMBER        (LEPC_RET_BASE+5)
#define  LEPC_RET_TRUNCATED_STRING        (LEPC_RET_BASE+6)
#define  LEPC_RET_INVALID_ACTION          (LEPC_RET_BASE+7)
#define  LEPC_RET_INVALID_ARGUMENT        (LEPC_RET_BASE+8)
#define  LEPC_RET_INVALID_CELL            (LEPC_RET_BASE+9)
#define  LEPC_RET_INVALID_INDEX           (LEPC_RET_BASE+10)
#define  LEPC_RET_INVALID_LOOP            (LEPC_RET_BASE+11)
#define  LEPC_RET_INVALID_NAME            (LEPC_RET_BASE+12)
#define  LEPC_RET_INVALID_PROTOTYPE       (LEPC_RET_BASE+13)
#define  LEPC_RET_INVALID_RANGE           (LEPC_RET_BASE+14)
#define  LEPC_RET_INVALID_SIZE            (LEPC_RET_BASE+15)
#define  LEPC_RET_INVALID_TYPE            (LEPC_RET_BASE+16)
#define  LEPC_RET_MEMORY_ERROR            (LEPC_RET_BASE+17)

/****************************************************************************/
/**   data type for external programming API and callback functions        **/
/****************************************************************************/
#define  LEPC_API                         _System

/****************************************************************************/
/**   Data Types                                                           **/
/****************************************************************************/

/****************************************************************************/
/**   callback function prototypes for the Library Manager,                **/
/**   and the @function and macro command implementation functions         **/
/****************************************************************************/
typedef  PVOID (* LEPC_API LEPTPFNMANAGER)(PVOID, USHORT);
typedef  VOID  (* LEPC_API LEPTPFNFUNCTION)(PVOID, USHORT);
typedef  VOID  (* LEPC_API LEPTPFNCOMMAND)(PVOID, USHORT);

/****************************************************************************/
/**   Function Prototypes                                                  **/
/****************************************************************************/

/****************************************************************************/
/**   Customized @Function API                                             **/
/****************************************************************************/

/****************************************************************************/
/**   register an @function                                                **/
/****************************************************************************/
USHORT LEPC_API LepAfRegisterFunction( PSZ               pszName,
                                       LEPTPFNFUNCTION   pfnFunction,
                                       USHORT            fsOptions,
                                       PVOID             pData,
                                       PSZ               pszHelp  );

/****************************************************************************/
/**   get the type and size of an @function argument                       **/
/****************************************************************************/
USHORT LEPC_API LepAfGetArgType( USHORT   iArgument,
                                 PUSHORT  pusType,
                                 PUSHORT  pusSize  );

/****************************************************************************/
/**   get the value of an @function argument                               **/
/****************************************************************************/
USHORT LEPC_API LepAfGetArgValue(   USHORT   iArgument,
                                    USHORT   usType,
                                    USHORT   usSize,
                                    PVOID    pValue   );

/****************************************************************************/
/**   begin iterating through the cells in a range or collection           **/
/****************************************************************************/
USHORT LEPC_API LepAfBeginRangeLoop(   PVOID       pRange,
                                       USHORT      fsOptions,
                                       PVOID       *ppLoop  );
   
/****************************************************************************/
/**   move to the next cell in a range or collection                       **/
/****************************************************************************/
USHORT LEPC_API LepAfNextLoopCell(  PVOID    pLoop );
   
/****************************************************************************/
/**   get the type and size of the current cell in a range or collection   **/
/****************************************************************************/
USHORT LEPC_API LepAfGetLoopType(   PVOID    pLoop,
                                    PUSHORT  pusType,
                                    PUSHORT  pusSize  );
   
/****************************************************************************/
/**   get the value of the current cell in a range or collection           **/
/****************************************************************************/
USHORT LEPC_API LepAfGetLoopValue(  PVOID    pLoop,
                                    USHORT   usType,
                                    USHORT   usSize,
                                    PVOID    pValue   );
   
/****************************************************************************/
/**   finish iterating through the cells in a range or collection          **/
/****************************************************************************/
USHORT LEPC_API LepAfEndRangeLoop(  PVOID pLoop );
   
/****************************************************************************/
/**   get the dimensions of a range                                        **/
/****************************************************************************/
USHORT LEPC_API LepAfGetRangeDimen( PVOID       pRange,
                                    USHORT      ausDimen[LEPC_COORD_DIMEN] );
   
/****************************************************************************/
/**   get the type and size of a cell in a range specified by coordinates  **/
/****************************************************************************/
USHORT LEPC_API LepAfGetCellType(   PVOID       pRange,
                                    USHORT      ausCoord[LEPC_COORD_DIMEN],
                                    PUSHORT     pusType,
                                    PUSHORT     pusSize  );
   
/****************************************************************************/
/**   get the value of a cell in a range specified by coordinates          **/
/****************************************************************************/
USHORT LEPC_API LepAfGetCellValue(  PVOID       pRange,
                                    USHORT      ausCoord[LEPC_COORD_DIMEN],
                                    USHORT      usType,
                                    USHORT      usSize,
                                    PVOID       pValue   );
   
/****************************************************************************/
/**   return a value to the cell or formula containing the @function       **/
/****************************************************************************/
USHORT LEPC_API LepAfReturnValue(   USHORT   usType,  
                                    USHORT   usSize,
                                    PVOID    pValue   );

/****************************************************************************/
/**   Customized Macro Commmand API                                        **/
/****************************************************************************/

/****************************************************************************/
/**   register a  macro commmand                                           **/
/****************************************************************************/
USHORT LEPC_API LepMcRegisterCommand(  PSZ               pszName,
                                       LEPTPFNCOMMAND    pfnCommand,
                                       USHORT            fsOptions,
                                       USHORT            cusArguments,
                                       USHORT            ausPrototype[],
                                       PVOID             pData,
                                       PSZ               pszHelp  );

/****************************************************************************/
/**   get the type and size of a macro commmand argument                   **/
/****************************************************************************/
USHORT LEPC_API LepMcGetArgType( USHORT   iArgument,
                                 PUSHORT  pusType,
                                 PUSHORT  pusSize  );

/****************************************************************************/
/**   get the value of a macro commmand argument                           **/
/****************************************************************************/
USHORT LEPC_API LepMcGetArgValue(   USHORT   iArgument,
                                    USHORT   usType,
                                    USHORT   usSize,
                                    PVOID    pValue   );

/****************************************************************************/
/**   get the dimensions of a range                                        **/
/****************************************************************************/
USHORT LEPC_API LepMcGetRangeDimen( PVOID       pRange,
                                    USHORT      ausDimen[LEPC_COORD_DIMEN] );
   
/****************************************************************************/
/**   get the type and size of a cell in a range specified by coordinates  **/
/****************************************************************************/
USHORT LEPC_API LepMcGetCellType(   PVOID       pRange,
                                    USHORT      ausCoord[LEPC_COORD_DIMEN],
                                    PUSHORT     pusType,
                                    PUSHORT     pusSize  );
   
/****************************************************************************/
/**   get the value of a cell in a range specified by coordinates          **/
/****************************************************************************/
USHORT LEPC_API LepMcGetCellValue(  PVOID       pRange,
                                    USHORT      ausCoord[LEPC_COORD_DIMEN],
                                    USHORT      usType,
                                    USHORT      usSize,
                                    PVOID       pValue   );
   
/****************************************************************************/
/**   set the value of a cell in a range specified by coordinates          **/
/****************************************************************************/
USHORT LEPC_API LepMcSetCellValue(  PVOID       pRange,
                                    USHORT      ausCoord[LEPC_COORD_DIMEN],
                                    USHORT      usType,
                                    USHORT      usSize,
                                    PVOID       pValue   );

/****************************************************************************/
/**   display an error message in a macro program                          **/
/****************************************************************************/
USHORT LEPC_API LepMcDisplayError(  PUCHAR   puchError   );

#endif
