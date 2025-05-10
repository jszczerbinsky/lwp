#ifndef WLPINFO_H
#define WLPINFO_H

typedef struct {
	char* dir_path;
	char* name;
	char* author;
} WlpInfo;

int	 wlp_load_info(WlpInfo* info, const char* dir_path);
void wlp_free_info_data(WlpInfo* info);
void wlp_subscan(int* count, WlpInfo** infos, const char* dir_path);

#endif
