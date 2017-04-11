#ifndef CMD_PROCESS_H
#define CMD_PROCESS_H

typedef struct cmd_process
{
	char	*tag;
	int		(*cmd_handle)(SOCKET,char *); 
}CMD_PROCESS_T;


extern CMD_PROCESS_T g_cmd_list[];
int set_video_param(SOCKET c_sid,char *data);
int set_value(SOCKET c_sid,char *data);
//∞¥º¸≤‚ ‘
int key_test(SOCKET c_sid,char *data);
//SDø®≤‚ ‘
int sdcard_test(SOCKET c_sid,char *data);
//wifi≤‚ ‘
int wifi_test(SOCKET c_sid,char *data);
//‘∆Ã®≤‚ ‘
int ptz_test(SOCKET c_sid,char *data);
//IRCUT≤‚ ‘
int ircut_test(SOCKET c_sid,char *data);
//¬º“Ù£¨∑≈“Ù≤‚ ‘
int codec_test(SOCKET c_sid,char *data);

#endif
