#ifndef _lub_xml2c_h
#define _lub_xml2c_h

#define XML2C_NULL ""
#define XML2C_STR(str) ( str ? str : XML2C_NULL )
#define XML2C_BOOL(val) ( val ? "BOOL_TRUE" : "BOOL_FALSE" )

const char *xml2c_enum(int value, const char *array[]);

#endif
