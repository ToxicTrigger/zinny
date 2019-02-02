#include <stdio.h>
#include <string>

#define ARCHIVE_NAME ".pot"

class zinny {
public:
#pragma pack(push, 1)
  typedef struct _pot_header {
    __uint16_t magic;
    __uint16_t version;
  } pot_header, *ppot_header;

  typedef struct _pot_file {
    char name[256];
    __uint32_t size;
    __uint32_t data_offset;
  } pot_file, *ppot_file;

#pragma pack(pop)

  typedef struct _archive {
    pot_header header;
    FILE *fp;
  } archive, *parchive;

public:
  static __uint32_t get_file_size(FILE *fp);
  static auto append(parchive archive, std::string filename);
};