#include "./Base64.h"
#include "mbedtls/md.h"

char* id_pelanggan = "100000009001";
int T = 1645159560;

// JWT TOKEN
char header[] = R"=====({"alg":"HS256","typ":"JWT"})=====";
char ecHeader[128];
char payload[128];
char ecPayload[128];
char signature[128];

void removeInvalidChar(char *str){
  int strLen = strlen(str);
  for (int i=0; i<strLen; i++){
    if (str[i]=='+') {
      str[i]='-';
    } else if (str[i]=='/') {
      str[i]='_';
    } else if (str[i]=='=') {
      str[i]='\0';
    }
  }
}

void encodeHeader(){
  memset(ecHeader,0,sizeof(ecHeader)); // clear destination buffer
  Base64.encode(ecHeader, header, strlen(header)); 
  removeInvalidChar(ecHeader);
}

void generatePayload(){
  int E = random(0,20);
  int Wi = random(0,20);
  T+= 10;

  E = 17;
  Wi = 6;
  T = 1645159680;
  memset(ecPayload,0,sizeof(ecPayload)); // clear destination buffer
  
//  char payload[128];
  sprintf(payload, R"=====({"id":%s,"E":%d,"Wi":%d,"T":%d})=====", id_pelanggan, E, Wi, T);
  Base64.encode(ecPayload, payload, strlen(payload)); 
  removeInvalidChar(ecPayload);
}

void generateSignature(){
  Serial.println("Generate Signature");
  char *key = "s3Cr3T";
  char unsignedToken[128];
  sprintf(unsignedToken, "%s.%s", ecHeader, ecPayload);
  Serial.println(unsignedToken);
 
  byte hmacResult[32];
 
  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
 
  const size_t uTokenLength= strlen(unsignedToken);
  const size_t keyLength = strlen(key);            
 
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
  mbedtls_md_hmac_starts(&ctx, (const unsigned char *) key, keyLength);
  mbedtls_md_hmac_update(&ctx, (const unsigned char *) unsignedToken, uTokenLength);
  mbedtls_md_hmac_finish(&ctx, hmacResult);
  mbedtls_md_free(&ctx);

  // Serial.print(hmacResult);
  Serial.println("Hmac Result: ");
  char result[128];
 
  for(int i= 0; i< sizeof(hmacResult); i++){
      char str[3];
      sprintf(str, "%02x", (int)hmacResult[i]);
      strcpy(result, result);
      strcat(result, str);
  }
  //  Serial.print("result : ");
  Serial.print(result);
  //  Serial.println(strlen(result));
  //  Base64.encode(signature, result, strlen(result)); 
  //  removeInvalidChar(signature);
  //  Serial.println(signature);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  encodeHeader();
}

void loop() {  
//  Serial.println("Header: ");
//  Serial.println(header); 
//  Serial.println(ecHeader); 

  generatePayload();
//  Serial.println("Payload : "); 
//  Serial.println(payload);
//  Serial.println(ecPayload);

  generateSignature();  
  delay(10000);
}
