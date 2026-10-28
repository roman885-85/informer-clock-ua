#include "MD5.h"
#include "rom/md5_hash.h"

unsigned char *MD5::make_hash(char *arg) {
  struct MD5Context context;
  unsigned char *hash = (unsigned char *)malloc(16);
  MD5Init(&context);
  MD5Update(&context, (const unsigned char *)arg, strlen(arg));
  MD5Final(hash, &context);
  return hash;
}
char *MD5::make_digest(const unsigned char *digest, int len) {
  char *md5str = (char *)malloc(sizeof(char) * (len * 2 + 1));
  static const char hexits[17] = "0123456789abcdef";
  for (int i = 0; i < len; i++) {
    md5str[i * 2] = hexits[digest[i] >> 4];
    md5str[(i * 2) + 1] = hexits[digest[i] & 0x0F];
  }
  md5str[len * 2] = '\0';
  return md5str;
}
