#ifndef __ISP_H
#define __ISP_H

void isp_Init(
	void (*status_func)(int type, int progdone, int error, int bytesRecorded),
		#define stsPROG_DONE	0
		#define stsPROGRESS		1
	void (*setSpeed_func)(u32 bd),
	int(*puts_func)(u8* buffer, u16 count),
	int(*getc_func)(u8* ch),
	int(*byte_available_func)(void),
	void(*flushRX_func)(void)
);
int isp_ProgramFlash(int deviceType, pchar addrCode, uint size);
	#define ispDEVICE_TYPE_WORKTEMP 0
	#define ispDEVICE_TYPE_PR111 	1
void isp_Stop(void);
int isp_Busy(void);
int isp_CodeError(void);
int isp_ProgDone(void);
void isp_Tick(void);
void isp_Process(void);

#endif
