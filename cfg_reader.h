#ifndef _HEAD_CFG_
#define _HEAD_CFG_

#include "master_header.h"

struct cfg_read
{
   char IP[15];
   char PORT[5];
};

struct cfg_read *cfg_reader(struct cfg_read *cf); /*читаем айпи и порт из текстового файлика если его нет то генерим по новой*/

#endif