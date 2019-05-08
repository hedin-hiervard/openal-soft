#include "stdafx.h"

struct entry {
	void *key;
	uint8 id;
};

iSimpleArray<entry> db;

unsigned int touchId(void* key) {
	for(uint32 xx=0; xx<db.GetSize(); xx++)
		if(db[xx].key == key) 
			return iMIN<unsigned int>(db[xx].id, MAX_MICE_COUNT - 1);
	sint8 top = -1;
	for(uint32 xx=0; xx<db.GetSize(); xx++)
		if(db[xx].id > top) top = db[xx].id;
	entry en;
	en.id = top + 1;
	en.key = key;
	db.Add(en);
	return iMIN(MAX_MICE_COUNT - 1, top+1);
}

void removeTouch(void* key)
{
 
	iSimpleArray<entry> copy;
	for(uint32 xx=0; xx<db.GetSize(); xx++)
		if(db[xx].key != key) copy.Add(db[xx]);
	db = copy;
}
