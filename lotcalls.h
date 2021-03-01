#ifndef __LOTCALLS_H
#define __LOTCALLS_H

#pragma pack(1)

struct LOTUSFUNCS
{
  __int32 lfield_0;
  void far *(__pascal *AllocMem)(int vmr, int size, int);
  void (__pascal *lfield_8)(void far *ptr, int size);
  void far *(__pascal __far far *Allocate)(int size, int tag);
  int (__pascal __far *Free)(void far *addr, int unknown, int size);

  // I believe this maps generic ptr m onto a VMR for use with EMS.
  void far *(__pascal *MapMemVmr)(void far *m, int v);

  // Tell 123 that you're about to use VMR v, and so it better be mapped in.
  void far *(__pascal *LoadVmr)(int v);

  __int32 field_1C;
  __int32 field_20;
  __int32 field_24;
  __int32 field_28;
  __int32 field_2C;
  __int32 field_30;
  __int32 field_34;
  __int32 field_38;
  __int32 field_3C;
  __int32 field_40;
  __int32 field_44;
  __int32 field_48;
  __int32 field_4C;
  int (__pascal far *RegisterDevdataStruct)(struct DEVDATA far *, int far *);
  __int32 field_54;
  __int32 field_58;
  __int32 field_5C;
  void (__pascal __far far *UnregisterDevdata)(int hdl);
  __int32 field_64;
  __int32 field_68;
  __int32 field_6C;
  __int32 field_70;
  __int32 field_74;
  __int32 field_78;
  __int32 field_7C;
  __int32 field_80;
  __int32 field_84;
  __int32 field_88;
  __int32 field_8C;
  __int32 field_90;
  __int32 field_94;
  __int32 field_98;
  __int32 field_9C;
  __int32 field_A0;
  void (__pascal __far far *MaybeGetVramPtrs)(void far * far *, int unknown, int size);
  __int32 field_A8;
  __int32 field_AC;
  __int32 field_B0;
  __int32 field_B4;
  __int32 field_B8;
  __int32 field_BC;
  __int32 field_C0;
  __int32 field_C4;
  __int32 field_C8;
  __int32 field_CC;
  __int32 field_D0;
  __int32 field_D4;
  __int32 field_D8;
  void (__pascal __far far *MaybeFreeVramPtrs)(int seg);
  __int32 field_E0;
  __int32 field_E4;
};

extern struct LOTUSFUNCS far *callbacks;

#endif
