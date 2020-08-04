/*
  2020 © Copyright (c) BiDaE Technology Inc. 
  Provided under BiDaE SHAREWARE LICENSE-1.0 in the LICENSE.

  Project Name:

     BeDIS library

  File Name:

     libEncrypt.h

  File Description:

     This file contains the definitions and declarations of BeDIS encryption 
     library.

  Version:

     2.0, 20200803

  Abstract:

     BeDIS uses LBeacons to deliver 3D coordinates and textual descriptions of
     their locations to users' devices. Basically, a LBeacon is an inexpensive,
     Bluetooth Smart Ready device. The 3D coordinates and location description
     of every LBeacon are retrieved from BeDIS (Building/environment Data and
     Information System) and stored locally during deployment and maintenance
     times. Once initialized, each LBeacon broadcasts its coordinates and
     location description to Bluetooth enabled user devices within its coverage
     area.

  Authors:
     Wayne Kang, biggkqq@gmail.com
     Chun Yu Lai   , chunyu1202@gmail.com
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
