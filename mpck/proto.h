/* bufio.c */
void init_cfile(CFILE *c);
int cfload(CFILE *c, long offset);
CFILE *cfopen(const char *filename, char *mode);
size_t cfread(char *outbuf, size_t size, CFILE *c);
int cfsetpos(CFILE *c, size_t pos);
int cfseek(CFILE *c, long offset, int whence);
size_t cftell(CFILE *c);
int cfeof(CFILE *c);
int cfclose(CFILE *c);
/* checkfile.c */
int set_first_n_bytes(int value);
int set_first_n_frames(int value);
int set_verbose(int value);
int set_quiet(int value);
int set_maxname(int value);
int set_badonly(int value);
int set_namecheck(int value);
void sigint(int signal);
void final_file_stats(file_info *file);
int check_end(file_info *file);
int check_begin(file_info *file);
int checkfile(file_info *file);
/* checkframe.c */
void parseframe(const file_info *file, frame_info *fi, char *buf);
void calcframestats(const file_info *file, frame_info *fi);
int checkvalidity(const file_info *file, const frame_info *fi);
int checkconsistency(const file_info *file, const frame_info *fi);
int parse_file_stats(file_info *file, const frame_info *fi);
int checkframe(file_info *file, char *buf);
/* crc.c */
int crc16(unsigned char *data, int length);
/* filename.c */
int extention_match(const char *filename, const char *extention);
void check_filename(file_info *file, int maxname, int namecheck);
/* id3.c */
int read_id3v1_tag(file_info *file);
int read_id3v2_tag(file_info *file);
/* main.c */
char *_getprogname(void);
int recursivecheck(const char *dirname);
int check_arg(char *filename);
int parse_args(char **argv);
int parse_options(int argc, char *argv[]);
int main(int argc, char *argv[]);
/* print.c */
void error(const char *format, ...);
void fileerror(const file_info *f, const char *format, ...);
void offseterror(const file_info *f, const char *format, ...);
void print_frame_info(const frame_info *fi, const file_info *file);
void print_scanning(const file_info *file);
void print_file_errors(const file_info *file);
void print_file_info(const file_info *file);
void print_total_info(const total_info *total);
void print_usage(void);
void print_version(void);
void print_terse_info(const file_info *file);
/* total.c */
void total_init(void);
int total_allgood(void);
