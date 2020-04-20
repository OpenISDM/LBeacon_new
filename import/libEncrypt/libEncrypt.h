/*
	Compilation:
		Windows:
			make libEncrypt.dll
		Linux:
			make libEncrypt.so
*/
#ifndef LIBENCRYPT_H 
#define LIBENCRYPT_H 

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __unix__

#define DLL_EXPORT_ENCRYPT

#elif defined( _WIN32 ) || defined (_WIN32)

#ifdef _EXPORTING
#define DLL_EXPORT_ENCRYPT __declspec(dllexport)
#else
#define DLL_EXPORT_ENCRYPT __declspec(dllimport)
#endif

#endif

DLL_EXPORT_ENCRYPT int AES_ECB_Encoder(char in[], char out[], int maxOutSize);

DLL_EXPORT_ENCRYPT int AES_ECB_Decoder(char in[], char out[], int maxOutSize);

DLL_EXPORT_ENCRYPT int AES_ECB_Encoder_With_Token_Prefix(char in[], char out[], int maxOutSize);

DLL_EXPORT_ENCRYPT int AES_ECB_Decoder_With_Token_Prefix(char in[], char out[], int maxOutSize);

DLL_EXPORT_ENCRYPT int SHA_256_Hash(char in[], char out[], int maxOutSize);

#ifdef __cplusplus
}
#endif

#endif
