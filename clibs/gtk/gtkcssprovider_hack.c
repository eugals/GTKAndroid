// This is a hack to gtkcssprovider.c, so it would use Android application ASSETS
// instead of the local files system

#include "gtkandroidassets.h"

#define g_file_test g_aasset_test
#define g_file_new_for_path g_aasset_as_GFile_for_path

#include "gtkcssprovider.c"
