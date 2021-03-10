#ifndef __LOTCALLS_H
#define __LOTCALLS_H

#pragma pack(1)

struct LOTUSFUNCS
{
  __int32 lfield_0;
  void far *(__pascal __far *AllocMem)(int vmr, int size, int);
  void      (__pascal __far *lfield_8)(void far *ptr, int size);
  void far *(__pascal __far *Allocate)(int size, int tag);
  int       (__pascal __far *Free)(void far *addr, int unknown, int size);
  // I believe this maps generic ptr m onto a VMR for use with EMS.
  void far *(__pascal __far *MapMemVmr)(void far *m, int v);

  // Tell 123 that you're about to use VMR v, and so it better be mapped in.
  void far *(__pascal __far *LoadVmr)(int v);

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
  int       (__pascal __far *RegisterDevdataStruct)(struct DEVDATA far *, int far *);
  __int32 field_54;
  void      (__pascal __far *field_58)(int, int);
  int       (__pascal __far *field_5C)(int, void far *ptr);
  void      (__pascal __far *UnregisterDevdata)(int hdl);
  int       (__pascal __far *field_64)(int, void far *ptr);
  int       (__pascal __far *field_68)(int, int, void far *ptr1, void far *ptr2);
  int       (__pascal __far *field_6C)(int, int, void *a, int, void far *b, void far *c);
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
  unsigned (__pascal __far far *GetDescriptor)(unsigned far *selector, int base, int limit);
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
  void (__pascal __far far *FreeDescriptor)(int seg);
  __int32 field_E0;
  __int32 field_E4;
};

extern struct LOTUSFUNCS far *callbacks;

extern void __pascal ClearRegionForeground(unsigned cols,
                                           unsigned lines,
                                           unsigned char attrs);
extern void __pascal MoveCursor(int col, int line);

#endif
